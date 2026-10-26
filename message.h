#ifndef MESSAGE_H
#define MESSAGE_H


#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>

#include "user_message.h"
#include "system_message.h"

enum MessageType : quint8 {
	UserMessage = 1,
	ServerMessage = 2,
	SystemMessage = 3
};



class message {

public:
	MessageType type;
	QString version_api;
	message();
	message(QByteArray bytes);
	std::variant<user_message_data, s_GetChanelHistiry> value;

};



#endif