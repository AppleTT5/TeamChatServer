#include "system_message.h"

using namespace Qt::Literals::StringLiterals;

QByteArray system_message::toCbor() const
{
    return QCborValue(sysBaseMap()).toCbor();
}

QCborMap system_message::sysBaseMap() const
{
    QCborMap map = baseMap();
    map.insert("sys_type"_L1, int(this->sys_type));
    return map;
}
