#ifndef SM_GET_CHANEL_HISTORY_H
#define SM_GET_CHANEL_HISTORY_H

#include "../system_message.h"

// C->S: запрос истории текстового канала.
// Ключи: type=3, v_api, sys_type=3, id
class get_chanel_history_message : public system_message
{
public:
    bool valid = false;
    quint64 id = 0;

    explicit get_chanel_history_message(const QByteArray& bytes);
};

#endif // SM_GET_CHANEL_HISTORY_H
