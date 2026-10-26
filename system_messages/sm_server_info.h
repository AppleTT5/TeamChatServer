#ifndef SM_SERVER_INFO_H
#define SM_SERVER_INFO_H

#include "../system_message.h"
#include <QtCore>
#include <QtCore/QByteArray>

class server_info_message : public system_message {

public:
	s_ServerInfo data;
	QByteArray bytes;
	server_info_message(s_ServerInfo data);
};

#endif // !SM_SERVER_INFO
