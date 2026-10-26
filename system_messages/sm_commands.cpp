#include "sm_commands.h"

using namespace Qt::Literals::StringLiterals;

// ==================== Сервер -> Клиент ====================

event_message::event_message(const QString& eventName, const QCborMap& payload)
    : system_message(SystemMessageType::event)
{
    this->event = eventName;
    this->payload = payload;
}

QByteArray event_message::toCbor() const
{
    QCborMap map = sysBaseMap();
    map.insert("event"_L1, this->event);
    for (auto it = this->payload.constBegin(); it != this->payload.constEnd(); ++it)
        map.insert(it.key(), it.value());
    return QCborValue(map).toCbor();
}

error_message::error_message(const QString& text)
    : system_message(SystemMessageType::Error)
{
    this->text = text;
}

QByteArray error_message::toCbor() const
{
    QCborMap map = sysBaseMap();
    map.insert("text"_L1, this->text);
    return QCborValue(map).toCbor();
}

// ==================== Клиент -> Сервер ====================

get_server_info_message::get_server_info_message(const QByteArray& bytes)
    : system_message(SystemMessageType::GetServerInfo)
{
    QCborMap map;
    this->valid = bytesToMap(bytes, map);
}

login_message::login_message(const QByteArray& bytes)
    : system_message(SystemMessageType::Login)
{
    QCborMap map;
    if (!bytesToMap(bytes, map))
        return;

    this->name = map.value("name"_L1).toString().trimmed();
    this->valid = !this->name.isEmpty();
}

join_voice_message::join_voice_message(const QByteArray& bytes)
    : system_message(SystemMessageType::JoinVoice)
{
    QCborMap map;
    if (!bytesToMap(bytes, map))
        return;

    this->chanel_id = quint64(qMax<qint64>(0, map.value("chanel_id"_L1).toInteger()));
    this->valid = true;
}

leave_voice_message::leave_voice_message(const QByteArray& bytes)
    : system_message(SystemMessageType::LeaveVoice)
{
    QCborMap map;
    this->valid = bytesToMap(bytes, map);
}

create_chanel_message::create_chanel_message(const QByteArray& bytes)
    : system_message(SystemMessageType::CreateChanel)
{
    QCborMap map;
    if (!bytesToMap(bytes, map))
        return;

    this->name = map.value("name"_L1).toString().trimmed();
    this->is_voice = map.value("is_voice"_L1).toBool();
    this->valid = !this->name.isEmpty();
}
