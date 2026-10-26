#ifndef SYSTEM_MESSAGE_H
#define SYSTEM_MESSAGE_H

#include "message.h"
#include "user_message.h"

// Подтипы служебных пакетов (ключ "sys_type")
enum SystemMessageType {
    ServerInfo = 0,        // S->C: имя и описание сервера
    ChanelsInfo = 1,       // S->C: список каналов (текстовые и голосовые)
    ChanelHistory = 2,     // S->C: история текстового канала
    GetChanelHistory = 3,  // C->S: запрос истории канала
    event = 4,             // S->C: событие (login_ok, user_connected, voice_*)
    GetServerInfo = 5,     // C->S: запрос ServerInfo + ChanelsInfo
    JoinVoice = 6,         // C->S: войти в голосовой канал
    LeaveVoice = 7,        // C->S: выйти из голосового канала
    CreateChanel = 8,      // C->S: создать канал
    Login = 9,             // C->S: вход по никнейму
    Error = 10             // S->C: ошибка обработки запроса
};

// ===== Структуры данных служебных пакетов =====

struct s_ServerInfo
{
    QString Name;
    QString Description;
};

// Описание одного канала (для ChanelsInfo)
struct ChanelInfo
{
    QString Name;
    quint64 id = 0;
    bool isVoice = false;  // true -> голосовой, false -> текстовый
};

struct s_ChanelsInfo
{
    QList<ChanelInfo> chanels; // все каналы: и текстовые, и голосовые
};

struct s_ChanelHistory
{
    quint64 chanel_id = 0;
    QList<user_message> messages; // user_message объявлен в message.h -> user_message.h
};

struct s_GetChanelHistory
{
    quint64 id = 0;
};

// Базовый класс служебных пакетов.
// Конкретные пакеты наследуются и добавляют свои поля в CBOR-карту.
class system_message : public message
{
public:
    SystemMessageType sys_type = ServerInfo;

    system_message()
    {
        this->type = MessageType::SystemMessage;
    }
    explicit system_message(SystemMessageType st) : sys_type(st)
    {
        this->type = MessageType::SystemMessage;
    }

    // Базовая сериализация: только type + v_api + sys_type.
    // Подклассы переопределяют toCbor() и добавляют payload.
    QByteArray toCbor() const override;

protected:
    // Карта с заголовком type/v_api/sys_type
    QCborMap sysBaseMap() const;
};

#endif // SYSTEM_MESSAGE_H
