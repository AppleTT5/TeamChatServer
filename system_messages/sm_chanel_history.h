#ifndef SM_CHANEL_HISTORY_H
#define SM_CHANEL_HISTORY_H

#include "../system_message.h"

// S->C: история текстового канала.
// Ключи: type=3, v_api, sys_type=2, chanel_id,
//        messages = [ {name, text, ts, [file_name, file_size, file_data]}, ... ]
class chanel_history_message : public system_message
{
public:
    s_ChanelHistory data;

    explicit chanel_history_message(const s_ChanelHistory& data);

    QByteArray toCbor() const override;
};

#endif // SM_CHANEL_HISTORY_H
