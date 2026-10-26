#include <QtCore/QCborArray>
#include "sm_chanel_history.h"

using namespace Qt::Literals::StringLiterals;

chanel_history_message::chanel_history_message(const s_ChanelHistory& data)
    : system_message(SystemMessageType::ChanelHistory)
{
    this->data = data;
}

QByteArray chanel_history_message::toCbor() const
{
    QCborMap map = sysBaseMap();
    map.insert("chanel_id"_L1, qint64(this->data.chanel_id));

    QCborArray messages;
    for (const user_message& um : this->data.messages) {
        QCborMap m;
        m.insert("type"_L1, int(MessageType::UserMessage));
        m.insert("name"_L1, um.data.NikName);
        m.insert("text"_L1, um.data.text);
        m.insert("ts"_L1, um.data.timestamp);
        if (um.hasFile()) {
            m.insert("file_name"_L1, um.data.fileName);
            m.insert("file_size"_L1, qint64(um.data.fileData.size()));
            m.insert("file_data"_L1, QCborValue(um.data.fileData));
        }
        messages.append(m);
    }

    map.insert("messages"_L1, messages);
    return QCborValue(map).toCbor();
}
