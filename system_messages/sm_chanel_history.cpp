#include "sm_chanel_history.h"

#include <QtCore>

QT_USE_NAMESPACE;
using namespace  Qt::Literals::StringLiterals;

chanel_history_message::chanel_history_message(s_ChanelHistory data)
{
	this->sys_type = SystemMessageType::ChanelHistory;
	QCborMap map;
	map.insert("type"_L1, this->type);
	map.insert("v_api"_L1, this->version_api);
	map.insert("sys_type"_L1, this->sys_type);
	map.insert("chanel_id"_L1, data.chanel_id);
	QCborArray cborArray;
	for (user_message tci : data.messages) {
		QCborMap t_map;
		t_map.insert("type"_L1, tci.type);
	    t_map.insert("name"_L1, tci.name);
		t_map.insert("text"_L1, tci.text);
		cborArray.append(t_map);
	}

	map.insert("messages"_L1, cborArray);
	QByteArray binaryData = QCborValue(map).toCbor();
	this->bytes = binaryData;
}
