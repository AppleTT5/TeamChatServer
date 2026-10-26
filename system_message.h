#ifndef SYSTEM_MESSAGE_H
#define SYSTEM_MESSAGE_H

#include "message.h"
#include "user_message.h"
#include "text_chanel.h"
enum SystemMessageType {
	ServerInfo = 0,
	ChanelsInfo = 1,
	ChanelHistory = 2,
	GetChanelHistory = 3,
	event = 4,
};

struct s_ServerInfo
{
	QString Name;
	QString Description;
};

struct s_ChanelsInfo
{
	QList<TextChanelInfo> TextChanels;
};

struct s_ChanelHistory
{
	uint chanel_id;
	QList<user_message> messages;
};

struct s_GetChanelHistiry
{
	uint id;
};

class system_message : public  message
{
public:
	SystemMessageType sys_type;
	std::variant<s_GetChanelHistiry> data;
	system_message();
	system_message(QByteArray Bytes);

};

#endif 
