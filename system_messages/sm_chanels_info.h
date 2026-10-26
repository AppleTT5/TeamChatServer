#ifndef SM_CHANELS_INFO_H
#define SM_CHANELS_INFO_H

#include "../system_message.h"

// S->C: список каналов сервера.
// Ключи: type=3, v_api, sys_type=1,
//        text_chanels  = [{name, id}, ...],
//        voice_chanels = [{name, id}, ...]
class chanels_info_message : public system_message
{
public:
    s_ChanelsInfo data;

    explicit chanels_info_message(const s_ChanelsInfo& data);

    QByteArray toCbor() const override;
};

#endif // SM_CHANELS_INFO_H
