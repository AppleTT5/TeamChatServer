#include <QtCore/QCborArray>
#include "sm_chanels_info.h"

using namespace Qt::Literals::StringLiterals;

chanels_info_message::chanels_info_message(const s_ChanelsInfo& data)
    : system_message(SystemMessageType::ChanelsInfo)
{
    this->data = data;
}

QByteArray chanels_info_message::toCbor() const
{
    QCborMap map = sysBaseMap();

    QCborArray textChanels;
    QCborArray voiceChanels;
    for (const ChanelInfo& ci : this->data.chanels) {
        QCborMap ch;
        ch.insert("name"_L1, ci.Name);
        ch.insert("id"_L1, qint64(ci.id));
        if (ci.isVoice)
            voiceChanels.append(ch);
        else
            textChanels.append(ch);
    }

    map.insert("text_chanels"_L1, textChanels);
    map.insert("voice_chanels"_L1, voiceChanels);
    return QCborValue(map).toCbor();
}
