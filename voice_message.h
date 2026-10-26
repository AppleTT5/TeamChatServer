#ifndef VOICE_MESSAGE_H
#define VOICE_MESSAGE_H

#include "message.h"

// Данные голосового пакета
struct voice_packet_data
{
    bool valid = false;
    QString user;          // ник говорящего (сервер выставляет авторитетно по Login)
    quint64 chanel = 0;    // id голосового канала
    quint32 seq = 0;       // порядковый номер пакета (для джиттер-буфера клиента)
    QString codec;         // кодек, например "opus"
    QByteArray data;       // закодированное аудио (например, фрейм Opus)
};

// Голосовой пакет: клиент -> сервер -> всем остальным участникам голосового канала.
// Сервер является релеем: проверяет, что отправитель состоит в канале,
// подменяет поле "user" на никнейм из сессии и рассылает пакет участникам.
//
// Ключи пакета (CBOR-карта):
//   type = 4 (VoiceData), v_api, user, chanel, seq, codec, data
class voice_message : public message
{
public:
    voice_message() = default;

    // Разбор принятого пакета
    explicit voice_message(const QByteArray& bytes);

    // Создание пакета для отправки
    voice_message(const QString& user, quint64 chanel, quint32 seq,
                  const QString& codec, const QByteArray& data);

    QByteArray toCbor() const override;

    voice_packet_data data;
};

#endif // VOICE_MESSAGE_H
