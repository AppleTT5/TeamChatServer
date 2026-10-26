#include "sm_server_info.h"

using namespace Qt::Literals::StringLiterals;

server_info_message::server_info_message(const s_ServerInfo& data)
    : system_message(SystemMessageType::ServerInfo)
{
    this->data = data;
}

QByteArray server_info_message::toCbor() const
{
    QCborMap map = sysBaseMap();
    map.insert("name"_L1, this->data.Name);
    map.insert("description"_L1, this->data.Description);
    return QCborValue(map).toCbor();
}
