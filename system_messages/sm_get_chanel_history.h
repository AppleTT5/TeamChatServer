#ifndef SM_GET_CHANEL_HISTORY_H
#define SM_GET_CHANEL_HISTORY_H

#include "..\system_message.h"

class get_chanel_history_message : public system_message {
public:
	s_GetChanelHistiry data;
	get_chanel_history_message(QByteArray Bytes);
};


#endif // !SM_GET_CHANEL_HISTORY_H
