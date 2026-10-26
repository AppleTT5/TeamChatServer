#ifndef CHANEL_HISTORY_H
#define CHANEL_HISTORY_H

#include "../system_message.h"


class chanel_history_message : public system_message
{
public:
	s_ChanelHistory data;
	QByteArray bytes;
	chanel_history_message(s_ChanelHistory data);
};

#endif // !CHANEL_HISTORY_H
