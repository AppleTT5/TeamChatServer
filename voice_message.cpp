#include "voice_message.h"

using namespace Qt::Literals::StringLiterals;

voice_message::voice_message(const QByteArray& bytes)
{
    this->type = MessageType::VoiceData;
    this->version_api = QString::fromLatin1(API_VERSION);

    QCborMap map;
    if (!bytesToMap(bytes, map))
        return; // data.valid остаётся false

    this->data.valid  = true;
    this->data.user   = map.value("user"_L1).toString();
    this->data.chanel = quint64(qMax<qint64>(0, map.value("chanel"_L1).toInteger()));
    this->data.seq    = quint32(qMax<qint64>(0, map.value("seq"_L1).toInteger()));
    this->data.codec  = map.value("codec"_L1).toString();
    this->data.data   = map.value("data"_L1).toByteArray();
}

voice_message::voice_message(const QString& user, quint64 chanel, quint32 seq,
                             const QString& codec, const QByteArray& data)
{
    this->type = MessageType::VoiceData;
    this->version_api = QString::fromLatin1(API_VERSION);
    this->data.valid  = true;
    this->data.user   = user;
    this->data.chanel = chanel;
    this->data.seq    = seq;
    this->data.codec  = codec;
    this->data.data   = data;
}

QByteArray voice_message::toCbor() const
{
    QCborMap map = baseMap();
    map.insert("user"_L1, this->data.user);
    map.insert("chanel"_L1, qint64(this->data.chanel));
    map.insert("seq"_L1, qint64(this->data.seq));
    map.insert("codec"_L1, this->data.codec);
    map.insert("data"_L1, QCborValue(this->data.data));
    return QCborValue(map).toCbor();
}
