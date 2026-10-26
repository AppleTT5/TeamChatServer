#ifndef USER_MESSAGE_H
#define USER_MESSAGE_H
#include "message.h"

struct user_message_data
{
	bool rightStructure;
	QString NikName;
	int channel;
	QString text;
};

class user_message : public message
{
private:
	

	

public:
	enum type_input_data {
		none = 0,
		file = 1,
		structure = 2,
	};
	user_message(type_input_data type = type_input_data::none,QByteArray bytes = NULL,user_message_data  message_data = user_message_data{});
	QByteArray Write();
	user_message_data Read();
	QString name;
	int channel;
	QString text;
	user_message_data data;
	QByteArray bytes;

	

};

#endif