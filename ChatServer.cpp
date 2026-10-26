#include "ChatServer.h"

#include <QtWebSockets>
#include <QtCore>

#include <cstdio>

using namespace std;

QT_USE_NAMESPACE

static QString getIdentifier(QWebSocket* peer)
{
    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
        QString::number(peer->peerPort()));
}

ChatServer::ChatServer(quint16 port, QObject* parent) :
    QObject(parent),
    m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Chat Server"),
        QWebSocketServer::NonSecureMode,
        this))
{   
    this->text_chanels << text_chanel(QString("General"));
    if (m_pWebSocketServer->listen(QHostAddress::Any, port))
    {
        QTextStream(stdout) << "Chat Server listening on port " << port << '\n';
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
            this, &ChatServer::onNewConnection);
    }
}

ChatServer::~ChatServer()
{
    m_pWebSocketServer->close();
}

void ChatServer::onNewConnection()
{
    auto pSocket = m_pWebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(pSocket) << " connected!\n";
    pSocket->setParent(this);

    connect(pSocket, &QWebSocket::binaryMessageReceived,
        this, &ChatServer::processMessage);
    connect(pSocket, &QWebSocket::disconnected,
        this, &ChatServer::socketDisconnected);

    
    m_clients << pSocket;
}

void ChatServer::processMessage(const QByteArray& message)
{
    QWebSocket* pSender = qobject_cast<QWebSocket*>(sender());
    this->m_messages << message;
    for (QWebSocket* pClient : std::as_const(m_clients)) {
        if (pClient != pSender) //don't echo message back to sender
            pClient->sendBinaryMessage(message);
    }
}

void ChatServer::socketDisconnected()
{
    QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
    QTextStream(stdout) << getIdentifier(pClient) << " disconnected!\n";
    if (pClient)
    {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}