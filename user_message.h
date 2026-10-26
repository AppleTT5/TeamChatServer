#ifndef USER_MESSAGE_H
#define USER_MESSAGE_H

#include "message.h"

// Данные пользовательского сообщения
struct user_message_data
{
    bool rightStructure = false;
    QString NikName;
    quint64 channel = 0;
    QString text;
    QString fileName;      // имя файла; пусто -> обычное текстовое сообщение
    QByteArray fileData;   // бинарное содержимое файла; пусто -> текстовое сообщение
    qint64 timestamp = 0;  // время сервера (мс с эпохи), выставляется сервером
};

// Сообщение пользователя: текст и/или бинарный файл.
//
// Ключи пакета (CBOR-карта):
//   type      = 1 (UserMessage)
//   v_api     — версия API
//   name      — никнейм автора (сервер подменяет на авторитетный из Login)
//   chanel    — id канала
//   text      — текст сообщения (может быть пустым, если отправляется файл)
//   file_name — имя файла (необязательное)
//   file_data — байты файла   (необязательное)
//   ts        — время сервера, мс с эпохи (добавляется сервером)
class user_message : public message
{
public:
    user_message() = default;

    // Разбор принятого пакета
    explicit user_message(const QByteArray& bytes);

    // Создание сообщения для отправки
    user_message(const QString& nick, quint64 channel, const QString& text,
                 const QString& fileName = QString(),
                 const QByteArray& fileData = QByteArray());

    QByteArray toCbor() const override;

    bool isValid() const { return data.rightStructure; }
    bool hasFile() const { return data.rightStructure && !data.fileData.isEmpty(); }

    user_message_data data;
};

#endif // USER_MESSAGE_H
