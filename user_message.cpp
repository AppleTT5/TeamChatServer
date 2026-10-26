#include "user_message.h"

#include <QtCore/QCborParserError>

using namespace Qt::Literals::StringLiterals;

user_message::user_message(const QByteArray& bytes)
{
    this->type = MessageType::UserMessage;
    this->version_api = QString::fromLatin1(API_VERSION);

    QCborMap map;
    if (!bytesToMap(bytes, map))
        return; // data.rightStructure остаётся false

    this->version_api      = map.value("v_api"_L1).toString(this->version_api);
    this->data.rightStructure = true;
    this->data.NikName     = map.value("name"_L1).toString();
    this->data.channel     = quint64(qMax<qint64>(0, map.value("chanel"_L1).toInteger()));
    this->data.text        = map.value("text"_L1).toString();
    this->data.fileName    = map.value("file_name"_L1).toString();
    this->data.fileData    = map.value("file_data"_L1).toByteArray();
    this->data.timestamp   = map.value("ts"_L1).toInteger();
}

user_message::user_message(const QString& nick, quint64 channel, const QString& text,
                           const QString& fileName, const QByteArray& fileData)
{
    this->type = MessageType::UserMessage;
    this->version_api = QString::fromLatin1(API_VERSION);
    this->data.rightStructure = true;
    this->data.NikName  = nick;
    this->data.channel  = channel;
    this->data.text     = text;
    this->data.fileName = fileName;
    this->data.fileData = fileData;
}

QByteArray user_message::toCbor() const
{
    QCborMap map = baseMap();
    map.insert("name"_L1, this->data.NikName);
    map.insert("chanel"_L1, qint64(this->data.channel));
    map.insert("text"_L1, this->data.text);
    map.insert("ts"_L1, this->data.timestamp);

    if (!this->data.fileName.isEmpty() || !this->data.fileData.isEmpty()) {
        map.insert("file_name"_L1, this->data.fileName);
        map.insert("file_data"_L1, QCborValue(this->data.fileData));
    }

    return QCborValue(map).toCbor();
}
