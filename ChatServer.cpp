#include "ChatServer.h"

#include "message.h"
#include "user_message.h"
#include "voice_message.h"
#include "system_message.h"
#include "system_messages/sm_server_info.h"
#include "system_messages/sm_chanels_info.h"
#include "system_messages/sm_chanel_history.h"
#include "system_messages/sm_get_chanel_history.h"
#include "system_messages/sm_commands.h"

#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QHostAddress>
#include <QtCore/QCborArray>
#include <QtCore/QDateTime>
#include <QtCore/QTextStream>
#include <memory>

using namespace Qt::Literals::StringLiterals;
QT_USE_NAMESPACE

static QString getIdentifier(QWebSocket* peer)
{
    if (!peer)
        return QStringLiteral("null");
    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
                                       QString::number(peer->peerPort()));
}

ChatServer::ChatServer(quint16 port, QObject* parent) :
    QObject(parent),
    m_pWebSocketServer(new QWebSocketServer(QStringLiteral("TeamChat Server"),
                                            QWebSocketServer::NonSecureMode,
                                            this))
{
    // Каналы по умолчанию: один текстовый и один голосовой
    m_chanels << chanel(QStringLiteral("General"), takeChanelId(), ChanelType::Text);
    m_chanels << chanel(QStringLiteral("Lobby"), takeChanelId(), ChanelType::Voice);

    // Лимит входящих сообщений у QWebSocket по умолчанию 128 МБ —
    // этого хватает для файлов до 32 МБ (проверяем сами в handleUserMessage)

    if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        QTextStream(stdout) << "TeamChat Server listening on port " << port << "\n"
                            << "Channels: General (text), Lobby (voice)\n" << Qt::flush;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &ChatServer::onNewConnection);
    } else {
        QTextStream(stderr) << "FATAL: cannot listen on port " << port << ": "
                            << m_pWebSocketServer->errorString() << "\n" << Qt::flush;
    }
}

ChatServer::~ChatServer()
{
    m_pWebSocketServer->close();
}

// ==================== Подключения ====================

void ChatServer::onNewConnection()
{
    QWebSocket* pSocket = m_pWebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(pSocket) << " connected\n" << Qt::flush;

    connect(pSocket, &QWebSocket::binaryMessageReceived,
            this, &ChatServer::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected,
            this, &ChatServer::socketDisconnected);

    m_sessions.insert(pSocket, ClientSession{pSocket, QString(), false, 0});

    // Сразу рассказываем новому клиенту про сервер и каналы
    sendServerInfo(pSocket);
    sendChanelsInfo(pSocket);
}

void ChatServer::socketDisconnected()
{
    QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
    if (!pClient)
        return;

    auto it = m_sessions.find(pClient);
    if (it == m_sessions.end())
        return;

    const ClientSession session = it.value();
    m_sessions.erase(it);
    pClient->deleteLater();

    if (session.nickname.isEmpty())
        QTextStream(stdout) << getIdentifier(pClient) << " disconnected\n" << Qt::flush;
    else
        QTextStream(stdout) << "User '" << session.nickname << "' disconnected\n" << Qt::flush;

    if (session.voiceChanelId != 0) {
        // Участник уже удалён из m_sessions, поэтому voiceMembers() его больше не вернёт
        event_message left(QStringLiteral("voice_left"),
                           QCborMap{{"chanel_id"_L1, qint64(session.voiceChanelId)},
                                    {"name"_L1, session.nickname}});
        broadcastSystem(left);
        broadcastVoiceMembersList(session.voiceChanelId);
    }

    if (session.loggedIn) {
        event_message ev(QStringLiteral("user_disconnected"),
                         QCborMap{{"name"_L1, session.nickname}});
        broadcastSystem(ev);
    }
}

// ==================== Входящие пакеты ====================

void ChatServer::processBinaryMessage(const QByteArray& data)
{
    QWebSocket* pSender = qobject_cast<QWebSocket*>(sender());
    if (!pSender)
        return;

    ClientSession* session = sessionFor(pSender);
    if (!session)
        return;

    QString error;
    std::unique_ptr<message> msg = message::parse(data, &error);
    if (!msg) {
        sendError(pSender, QStringLiteral("Failed to parse packet: %1").arg(error));
        return;
    }

    switch (msg->type) {
    case MessageType::UserMessage:
        handleUserMessage(session, *static_cast<user_message*>(msg.get()));
        break;
    case MessageType::SystemMessage:
        handleSystemPacket(session, *static_cast<system_message*>(msg.get()));
        break;
    case MessageType::VoiceData:
        handleVoicePacket(session, *static_cast<voice_message*>(msg.get()));
        break;
    default:
        sendError(pSender, QStringLiteral("Unsupported message type"));
        break;
    }
}

void ChatServer::handleUserMessage(ClientSession* session, user_message& msg)
{
    if (!session->loggedIn) {
        sendError(session->socket, QStringLiteral("Login required: send system message Login (sys_type=9) with your nickname first"));
        return;
    }
    if (!msg.isValid()) {
        sendError(session->socket, QStringLiteral("Malformed user message"));
        return;
    }
    if (!msg.data.text.isEmpty() && msg.data.text.size() > 8192) {
        sendError(session->socket, QStringLiteral("Text too long (max 8192 chars)"));
        return;
    }
    if (msg.data.fileData.size() > MaxFileSize) {
        sendError(session->socket, QStringLiteral("File too large (max %1 MB)").arg(MaxFileSize / (1024 * 1024)));
        return;
    }

    chanel* ch = findChanel(msg.data.channel);
    if (!ch) {
        sendError(session->socket, QStringLiteral("Unknown channel id %1").arg(msg.data.channel));
        return;
    }

    // Сервер выставляет авторитетные данные
    msg.data.NikName = session->nickname;
    msg.data.timestamp = QDateTime::currentMSecsSinceEpoch();

    // Текстовые сообщения и файлы хранятся в истории текстового канала
    ch->add_message(msg);

    // Рассылаем всем клиентам, кроме отправителя
    // (клиент фильтрует сообщения по полю "chanel")
    broadcast(msg.toCbor(), session->socket);

    QTextStream(stdout) << "[" << ch->name << "] " << msg.data.NikName << ": "
                        << (msg.hasFile()
                                ? QStringLiteral("<file '%1', %2 bytes>").arg(msg.data.fileName).arg(msg.data.fileData.size())
                                : msg.data.text)
                        << "\n" << Qt::flush;
}

void ChatServer::handleSystemPacket(ClientSession* session, system_message& sys)
{
    QWebSocket* client = session->socket;

    switch (sys.sys_type) {
    case SystemMessageType::GetServerInfo: {
        auto* req = static_cast<get_server_info_message*>(&sys);
        if (!req->valid) { sendError(client, QStringLiteral("Malformed GetServerInfo request")); break; }
        sendServerInfo(client);
        sendChanelsInfo(client);
        break;
    }

    case SystemMessageType::GetChanelHistory: {
        auto* req = static_cast<get_chanel_history_message*>(&sys);
        if (!req->valid) { sendError(client, QStringLiteral("Malformed history request")); break; }
        sendHistory(client, req->id);
        break;
    }

    case SystemMessageType::Login: {
        auto* req = static_cast<login_message*>(&sys);
        handleLogin(session, req->name);
        break;
    }

    case SystemMessageType::JoinVoice: {
        auto* req = static_cast<join_voice_message*>(&sys);
        if (!req->valid) { sendError(client, QStringLiteral("Malformed join voice request")); break; }
        handleJoinVoice(session, req->chanel_id);
        break;
    }

    case SystemMessageType::LeaveVoice: {
        auto* req = static_cast<leave_voice_message*>(&sys);
        if (!req->valid) { sendError(client, QStringLiteral("Malformed leave voice request")); break; }
        handleLeaveVoice(session);
        break;
    }

    case SystemMessageType::CreateChanel: {
        auto* req = static_cast<create_chanel_message*>(&sys);
        if (!req->valid) { sendError(client, QStringLiteral("Malformed create channel request")); break; }
        handleCreateChanel(session, req->name, req->is_voice);
        break;
    }

    default:
        // ServerInfo/ChanelsInfo/ChanelHistory/event/Error — только исходящие
        sendError(client, QStringLiteral("Unexpected system message from client"));
        break;
    }
}

void ChatServer::handleVoicePacket(ClientSession* session, voice_message& voice)
{
    if (!session->loggedIn || !voice.data.valid)
        return; // голос без логина молча игнорируем
    if (voice.data.data.size() > MaxVoicePacketSize)
        return; // слишком большой "аудио" пакет — подозрительно

    // Говорить можно только в канал, к которому присоединился
    if (session->voiceChanelId == 0 || session->voiceChanelId != voice.data.chanel)
        return;

    // Сервер подтверждает авторство — клиентский "user" не доверенный
    voice.data.user = session->nickname;
    relayVoice(voice, session->socket);
}

// ==================== Команды ====================

void ChatServer::handleLogin(ClientSession* session, const QString& nick)
{
    QString name = nick.trimmed();
    if (name.isEmpty()) {
        sendError(session->socket, QStringLiteral("Nickname must not be empty"));
        return;
    }
    if (name.size() > MaxNicknameLength)
        name.truncate(MaxNicknameLength);

    if (nicknameTaken(name)) {
        sendError(session->socket, QStringLiteral("Nickname '%1' is already in use").arg(name));
        return;
    }

    session->nickname = name;
    session->loggedIn = true;

    // Подтверждение автору
    event_message ok(QStringLiteral("login_ok"), QCborMap{{"name"_L1, name}});
    sendSystem(session->socket, ok);

    // Уведомляем остальных
    event_message ev(QStringLiteral("user_connected"), QCborMap{{"name"_L1, name}});
    broadcastSystem(ev, session->socket);

    QTextStream(stdout) << "User logged in: " << name << "\n" << Qt::flush;
}

void ChatServer::handleJoinVoice(ClientSession* session, quint64 chanelId)
{
    if (!session->loggedIn) {
        sendError(session->socket, QStringLiteral("Login required"));
        return;
    }

    chanel* ch = findChanel(chanelId);
    if (!ch || ch->type != ChanelType::Voice) {
        sendError(session->socket, QStringLiteral("Voice channel not found"));
        return;
    }

    // При смене канала сначала выходим из предыдущего
    if (session->voiceChanelId != 0 && session->voiceChanelId != chanelId)
        handleLeaveVoice(session);

    session->voiceChanelId = chanelId;

    event_message joined(QStringLiteral("voice_joined"),
                         QCborMap{{"chanel_id"_L1, qint64(chanelId)},
                                  {"name"_L1, session->nickname}});
    broadcastSystem(joined);
    broadcastVoiceMembersList(chanelId);

    QTextStream(stdout) << "User '" << session->nickname << "' joined voice channel '"
                        << ch->name << "'\n" << Qt::flush;
}

void ChatServer::handleLeaveVoice(ClientSession* session)
{
    if (session->voiceChanelId == 0)
        return;

    const quint64 oldId = session->voiceChanelId;
    session->voiceChanelId = 0;

    event_message left(QStringLiteral("voice_left"),
                       QCborMap{{"chanel_id"_L1, qint64(oldId)},
                                {"name"_L1, session->nickname}});
    broadcastSystem(left);
    broadcastVoiceMembersList(oldId);
}

void ChatServer::handleCreateChanel(ClientSession* session, const QString& name, bool isVoice)
{
    if (!session->loggedIn) {
        sendError(session->socket, QStringLiteral("Login required"));
        return;
    }

    QString trimmed = name.trimmed();
    if (trimmed.isEmpty() || trimmed.size() > 64) {
        sendError(session->socket, QStringLiteral("Invalid channel name (1..64 chars)"));
        return;
    }
    for (const chanel& c : std::as_const(m_chanels)) {
        if (c.name.compare(trimmed, Qt::CaseInsensitive) == 0) {
            sendError(session->socket, QStringLiteral("Channel '%1' already exists").arg(trimmed));
            return;
        }
    }

    const quint64 id = takeChanelId();
    m_chanels << chanel(trimmed, id, isVoice ? ChanelType::Voice : ChanelType::Text);

    QTextStream(stdout) << "User '" << session->nickname << "' created "
                        << (isVoice ? "voice" : "text") << " channel '" << trimmed
                        << "' (id=" << id << ")\n" << Qt::flush;

    // Обновляем список каналов у всех клиентов
    broadcastChanelsInfo();
}

// ==================== Отправка ====================

void ChatServer::sendTo(QWebSocket* client, const QByteArray& data)
{
    if (client)
        client->sendBinaryMessage(data);
}

void ChatServer::sendSystem(QWebSocket* client, const system_message& msg)
{
    sendTo(client, msg.toCbor());
}

void ChatServer::sendError(QWebSocket* client, const QString& text)
{
    QTextStream(stderr) << "Error to " << getIdentifier(client) << ": " << text << "\n" << Qt::flush;
    sendSystem(client, error_message(text));
}

void ChatServer::sendServerInfo(QWebSocket* client)
{
    s_ServerInfo info;
    info.Name = m_serverName;
    info.Description = m_serverDescription;
    sendSystem(client, server_info_message(info));
}

void ChatServer::sendChanelsInfo(QWebSocket* client)
{
    s_ChanelsInfo info;
    for (const chanel& c : std::as_const(m_chanels))
        info.chanels << c.info();
    sendSystem(client, chanels_info_message(info));
}

void ChatServer::sendHistory(QWebSocket* client, quint64 chanelId)
{
    chanel* ch = findChanel(chanelId);
    if (!ch) {
        sendError(client, QStringLiteral("Unknown channel id %1").arg(chanelId));
        return;
    }
    if (ch->type != ChanelType::Text) {
        sendError(client, QStringLiteral("Channel '%1' is a voice channel and has no text history").arg(ch->name));
        return;
    }

    s_ChanelHistory h;
    h.chanel_id = chanelId;
    h.messages = ch->messages;
    sendSystem(client, chanel_history_message(h));
}

// ==================== Рассылка ====================

void ChatServer::broadcast(const QByteArray& data, QWebSocket* except)
{
    for (auto it = m_sessions.constBegin(); it != m_sessions.constEnd(); ++it) {
        QWebSocket* client = it.key();
        if (client && client != except && client->isValid())
            client->sendBinaryMessage(data);
    }
}

void ChatServer::broadcastSystem(const system_message& msg, QWebSocket* except)
{
    broadcast(msg.toCbor(), except);
}

void ChatServer::broadcastChanelsInfo()
{
    s_ChanelsInfo info;
    for (const chanel& c : std::as_const(m_chanels))
        info.chanels << c.info();

    const QByteArray packet = chanels_info_message(info).toCbor();
    for (auto it = m_sessions.constBegin(); it != m_sessions.constEnd(); ++it)
        if (it.key() && it.key()->isValid())
            it.key()->sendBinaryMessage(packet);
}

void ChatServer::relayVoice(const voice_message& voice, QWebSocket* sender)
{
    const QByteArray packet = voice.toCbor();
    for (ClientSession* member : voiceMembers(voice.data.chanel)) {
        if (member->socket != sender && member->socket->isValid())
            member->socket->sendBinaryMessage(packet);
    }
}

QList<ClientSession*> ChatServer::voiceMembers(quint64 chanelId)
{
    QList<ClientSession*> result;
    for (auto it = m_sessions.begin(); it != m_sessions.end(); ++it) {
        if (it->voiceChanelId == chanelId)
            result << &it.value();
    }
    return result;
}

void ChatServer::broadcastVoiceMembersList(quint64 chanelId)
{
    // Список участников канала собираем "вручную", т.к. voiceMembers не const
    QStringList members;
    for (auto it = m_sessions.constBegin(); it != m_sessions.constEnd(); ++it) {
        if (it->voiceChanelId == chanelId)
            members << it->nickname;
    }

    QCborArray arr;
    for (const QString& name : members)
        arr.append(name);

    event_message ev(QStringLiteral("voice_members"),
                     QCborMap{{"chanel_id"_L1, qint64(chanelId)},
                              {"members"_L1, arr}});
    broadcastSystem(ev);
}

// ==================== Служебное ====================

ClientSession* ChatServer::sessionFor(QWebSocket* socket)
{
    auto it = m_sessions.find(socket);
    if (it == m_sessions.end())
        return nullptr;
    return &it.value();
}

chanel* ChatServer::findChanel(quint64 id)
{
    for (chanel& c : m_chanels)
        if (c.id == id)
            return &c;
    return nullptr;
}

bool ChatServer::nicknameTaken(const QString& nick) const
{
    for (auto it = m_sessions.constBegin(); it != m_sessions.constEnd(); ++it)
        if (it->loggedIn && it->nickname.compare(nick, Qt::CaseInsensitive) == 0)
            return true;
    return false;
}

quint64 ChatServer::takeChanelId()
{
    return m_nextChanelId++;
}
