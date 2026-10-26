#include "sm_server_info.h"

#include <QtCore/QByteArray>


QT_USE_NAMESPACE;
using namespace  Qt::Literals::StringLiterals;

server_info_message::server_info_message(s_ServerInfo data)
{
	sys_type = SystemMessageType::ServerInfo;
	QCborMap map;
	map.insert("type"_L1, this->type);
	map.insert("v_api"_L1, this->version_api);
	map.insert("sys_type"_L1, this->sys_type);
	map.insert("name"_L1, data.Name);
	map.insert("description"_L1,data.Description );
	QByteArray binaryData = QCborValue(map).toCbor();
	this->bytes = binaryData;
}