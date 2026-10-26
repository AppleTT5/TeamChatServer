#ifndef SM_CHANELS_INFO_H
#define SM_CHANELS_INFO_H

#include "..\message.h"
#include "..\system_message.h"

class chanels_info_message : public system_message
{
	s_ChanelsInfo data;
	QByteArray bytes;
	chanels_info_message(s_ChanelsInfo data);
};

#endif