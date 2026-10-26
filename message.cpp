#include "message.h"

#include "user_message.h"
#include "voice_message.h"
#include "system_message.h"
#include "system_messages/sm_get_chanel_history.h"
#include "system_messages/sm_commands.h"

#include <QtCore/QCborParserError>

using namespace Qt::Literals::StringLiterals;

QCborMap message::baseMap() const
{
    QCborMap map;
    map.insert("type"_L1, int(this->type));
    map.insert("v_api"_L1, this->version_api);
    return map;
}

bool message::bytesToMap(const QByteArray& bytes, QCborMap& out)
{
    QCborParserError parserError;
    QCborValue value = QCborValue::fromCbor(bytes, &parserError);
    if (parserError.error != QCborError::NoError || !value.isMap())
        return false;
    out = value.toMap();
    return true;
}

std::unique_ptr<message> message::parse(const QByteArray& bytes, QString* errorOut)
{
    auto fail = [&](const QString& why) -> std::unique_ptr<message> {
        if (errorOut)
            *errorOut = why;
        return nullptr;
    };

    QCborMap map;
    if (!bytesToMap(bytes, map))
        return fail(QStringLiteral("packet is not a valid CBOR map"));

    const qint64 typeInt = map.value("type"_L1).toInteger();

    switch (typeInt) {
    case MessageType::UserMessage:
        return std::make_unique<user_message>(bytes);

    case MessageType::SystemMessage: {
        const qint64 sysTypeInt = map.value("sys_type"_L1).toInteger();
        switch (sysTypeInt) {
        case SystemMessageType::GetChanelHistory:
            return std::make_unique<get_chanel_history_message>(bytes);
        case SystemMessageType::GetServerInfo:
            return std::make_unique<get_server_info_message>(bytes);
        case SystemMessageType::JoinVoice:
            return std::make_unique<join_voice_message>(bytes);
        case SystemMessageType::LeaveVoice:
            return std::make_unique<leave_voice_message>(bytes);
        case SystemMessageType::CreateChanel:
            return std::make_unique<create_chanel_message>(bytes);
        case SystemMessageType::Login:
            return std::make_unique<login_message>(bytes);
        default:
            return fail(QStringLiteral("unknown system message type"));
        }
    }

    case MessageType::VoiceData:
        return std::make_unique<voice_message>(bytes);

    default:
        return fail(QStringLiteral("unknown message type"));
    }
}
