#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QString>
#include "chanel.h"
#include "voice_message.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

// Состояние одного подключения
struct ClientSession
{
    QWebSocket* socket = nullptr;
    QString nickname;
    bool loggedIn = false;
    quint64 voiceChanelId = 0; // 0 -> не в голосовом канале
};

// TeamChat-сервер: текстовые каналы (с историей и бинарными файлами)
// и голосовые каналы (релей аудио) поверх WebSocket + CBOR.
//
// Поток работы клиента:
//   1. Подключение -> сервер сразу шлёт ServerInfo + ChanelsInfo.
//   2. SystemMessage(Login, {name}) -> event login_ok + event user_connected всем.
//   3. Текст/файлы: UserMessage {chanel, text, [file_name, file_data]} ->
//      рассылается всем клиентам, попадает в историю канала.
//   4. История: SystemMessage(GetChanelHistory, {id}) -> ChanelHistory.
//   5. Голос: SystemMessage(JoinVoice, {chanel_id}) ->
//      дальше VoiceData-пакеты релеятся участникам этого канала.
//      SystemMessage(LeaveVoice) -> выход.
class ChatServer : public QObject
{
    Q_OBJECT
public:
    explicit ChatServer(quint16 port, QObject* parent = nullptr);
    ~ChatServer() override;

    // Ограничения (защита от мусора/переполнения памяти)
    static constexpr qint64 MaxFileSize = 32 * 1024 * 1024;   // макс. размер файла: 32 МБ
    static constexpr int MaxVoicePacketSize = 64 * 1024;      // макс. размер голосового пакета: 64 КБ
    static constexpr int MaxNicknameLength = 32;              // макс. длина никнейма

private slots:
    void onNewConnection();
    void processBinaryMessage(const QByteArray& data);
    void socketDisconnected();

private:
    // ===== Разбор и обработка пакетов =====
    void handleUserMessage(ClientSession* session, user_message& msg);
    void handleSystemPacket(ClientSession* session, system_message& sys);
    void handleVoicePacket(ClientSession* session, voice_message& voice);

    // ===== Команды клиента =====
    void handleLogin(ClientSession* session, const QString& nick);
    void handleJoinVoice(ClientSession* session, quint64 chanelId);
    void handleLeaveVoice(ClientSession* session);
    void handleCreateChanel(ClientSession* session, const QString& name, bool isVoice);

    // ===== Отправка конкретному клиенту =====
    void sendTo(QWebSocket* client, const QByteArray& data);
    void sendSystem(QWebSocket* client, const system_message& msg);
    void sendError(QWebSocket* client, const QString& text);
    void sendServerInfo(QWebSocket* client);
    void sendChanelsInfo(QWebSocket* client);
    void sendHistory(QWebSocket* client, quint64 chanelId);

    // ===== Рассылка =====
    void broadcast(const QByteArray& data, QWebSocket* except = nullptr);
    void broadcastSystem(const system_message& msg, QWebSocket* except = nullptr);
    void broadcastChanelsInfo();
    void broadcastVoiceMembersList(quint64 chanelId);
    void relayVoice(const voice_message& voice, QWebSocket* sender);
    QList<ClientSession*> voiceMembers(quint64 chanelId);

    // ===== Служебное =====
    ClientSession* sessionFor(QWebSocket* socket);
    chanel* findChanel(quint64 id);
    bool nicknameTaken(const QString& nick) const;
    quint64 takeChanelId();

    QWebSocketServer* m_pWebSocketServer = nullptr;
    QHash<QWebSocket*, ClientSession> m_sessions;
    QList<chanel> m_chanels;
    quint64 m_nextChanelId = 1;

    QString m_serverName = QStringLiteral("TeamChat Server");
    QString m_serverDescription = QStringLiteral("TeamChat voice & text server");
};

#endif // CHATSERVER_H
