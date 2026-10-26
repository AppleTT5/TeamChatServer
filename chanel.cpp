#include "chanel.h"

chanel::chanel(const QString& name, quint64 id, ChanelType type)
{
    this->name = name;
    this->id = id;
    this->type = type;
}

void chanel::add_message(const user_message& m)
{
    if (this->type != ChanelType::Text)
        return;
    if (m.type != MessageType::UserMessage || !m.isValid())
        return;

    this->messages << m;

    // Ограничиваем историю, чтобы память сервера не росла бесконечно
    while (this->messages.size() > MaxHistorySize)
        this->messages.removeFirst();
}

ChanelInfo chanel::info() const
{
    ChanelInfo info;
    info.Name = this->name;
    info.id = this->id;
    info.isVoice = (this->type == ChanelType::Voice);
    return info;
}
