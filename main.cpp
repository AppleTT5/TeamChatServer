// TeamChat Server — аналог TeamSpeak: текстовые каналы (с историей и
// бинарными файлами) + голосовые каналы (релей аудио) поверх WebSocket + CBOR.
//
// Запуск:
//   TeamChatServer                — слушать порт 1234
//   TeamChatServer --port 5000    — свой порт
//
// Протокол описан в комментариях message.h / user_message.h /
// voice_message.h / system_message.h / system_messages/*.
#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>
#include <QtCore/QTextStream>
#include "ChatServer.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("TeamChatServer"));
    QCoreApplication::setApplicationVersion(QStringLiteral("1.0.0.0"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("TeamChat: voice & text chat server (WebSocket + CBOR)"));
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption portOption({QStringLiteral("p"), QStringLiteral("port")},
                                  QStringLiteral("TCP port to listen on"),
                                  QStringLiteral("port"), QStringLiteral("1234"));
    parser.addOption(portOption);
    parser.process(a);

    bool ok = false;
    const quint16 port = parser.value(portOption).toUShort(&ok);
    if (!ok || port == 0) {
        QTextStream(stderr) << "Invalid port: " << parser.value(portOption) << "\n";
        return 1;
    }

    ChatServer server(port);

    return a.exec();
}
