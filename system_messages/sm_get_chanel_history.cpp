#include "sm_get_chanel_history.h"

using namespace Qt::Literals::StringLiterals;

get_chanel_history_message::get_chanel_history_message(const QByteArray& bytes)
    : system_message(SystemMessageType::GetChanelHistory)
{
    QCborMap map;
    if (!bytesToMap(bytes, map))
        return;

    this->id = quint64(qMax<qint64>(0, map.value("id"_L1).toInteger()));
    this->valid = true;
}
