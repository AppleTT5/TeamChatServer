#include "text_chanel.h"
//#include <>

QT_USE_NAMESPACE;

text_chanel::text_chanel(QString name,uint id)
{
	this->name = name;
	this->id = id;
}

void text_chanel::add_message(message message)
{
	if (message.type == MessageType::UserMessage or message.type == MessageType::ServerMessage) 
	{
		this->messages << message;
	}
}
