#ifndef SM_COMMANDS_H
#define SM_COMMANDS_H

#include "../system_message.h"
#include <QtCore/QCborMap>

// ===== Сервер -> Клиент =====

// Событие (sys_type = 4).
// Ключи: type=3, v_api, sys_type=4, event, ...поля события
class event_message : public system_message
{
public:
    QString event;
    QCborMap payload;

    event_message(const QString& eventName, const QCborMap& payload = QCborMap());

    QByteArray toCbor() const override;
};

// Ошибка обработки запроса (sys_type = 10).
// Ключи: type=3, v_api, sys_type=10, text
class error_message : public system_message
{
public:
    QString text;

    explicit error_message(const QString& text);

    QByteArray toCbor() const override;
};

// ===== Клиент -> Сервер =====

// Запрос информации о сервере (sys_type = 5). Полей не требует.
class get_server_info_message : public system_message
{
public:
    bool valid = false;

    explicit get_server_info_message(const QByteArray& bytes);
};

// Вход по никнейму (sys_type = 9).
// Ключи: type=3, v_api, sys_type=9, name
class login_message : public system_message
{
public:
    bool valid = false;
    QString name;

    explicit login_message(const QByteArray& bytes);
};

// Вход в голосовой канал (sys_type = 6).
// Ключи: type=3, v_api, sys_type=6, chanel_id
class join_voice_message : public system_message
{
public:
    bool valid = false;
    quint64 chanel_id = 0;

    explicit join_voice_message(const QByteArray& bytes);
};

// Выход из голосового канала (sys_type = 7). Полей не требует.
class leave_voice_message : public system_message
{
public:
    bool valid = false;

    explicit leave_voice_message(const QByteArray& bytes);
};

// Создание канала (sys_type = 8).
// Ключи: type=3, v_api, sys_type=8, name, is_voice
class create_chanel_message : public system_message
{
public:
    bool valid = false;
    QString name;
    bool is_voice = false;

    explicit create_chanel_message(const QByteArray& bytes);
};

#endif // SM_COMMANDS_H
