#ifndef SM_SERVER_INFO_H
#define SM_SERVER_INFO_H

#include "../system_message.h"

// S->C: информация о сервере.
// Ключи: type=3, v_api, sys_type=0, name, description
class server_info_message : public system_message
{
public:
    s_ServerInfo data;

    explicit server_info_message(const s_ServerInfo& data);

    QByteArray toCbor() const override;
};

#endif // SM_SERVER_INFO_H
