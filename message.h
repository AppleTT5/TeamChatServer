#ifndef MESSAGE_H
#define MESSAGE_H

#include <QtCore/QtGlobal>
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QCborMap>
#include <QtCore/QCborValue>
#include <memory>

// Версия API, которая кладётся в каждый пакет (ключ "v_api")
constexpr char API_VERSION[] = "1.0.0.0";

// Верхнеуровневые типы пакетов (ключ "type")
enum MessageType : quint8 {
    UserMessage = 1,    // текст и/или файл от пользователя
    ServerMessage = 2,  // зарезервировано для сообщений сервера
    SystemMessage = 3,  // служебные пакеты (ключ "sys_type")
    VoiceData = 4       // голосовой пакет (релей между участниками канала)
};

// Базовый класс всех сетевых пакетов. Сериализация — CBOR.
class message
{
public:
    MessageType type = UserMessage;
    QString version_api = QString::fromLatin1(API_VERSION);

    message() = default;
    explicit message(MessageType t) : type(t) {}
    virtual ~message() = default;

    // Сериализация пакета в CBOR
    virtual QByteArray toCbor() const = 0;

    // Фабрика: разбирает сырые байты и создаёт объект нужного подкласса.
    // Возвращает nullptr, если пакет повреждён или тип неизвестен.
    static std::unique_ptr<message> parse(const QByteArray& bytes, QString* errorOut = nullptr);

protected:
    // Общий заголовок пакета: type + v_api
    QCborMap baseMap() const;

    // CBOR-байты -> карта. false, если данные повреждены / это не карта.
    static bool bytesToMap(const QByteArray& bytes, QCborMap& out);
};

#endif // MESSAGE_H
