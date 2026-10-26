#include "sm_chanels_info.h"

#include <QtCore>

QT_USE_NAMESPACE;
using namespace  Qt::Literals::StringLiterals;

chanels_info_message::chanels_info_message(s_ChanelsInfo data)
{
	this->sys_type = SystemMessageType::ChanelsInfo;
	QCborMap map;
	map.insert("type"_L1, this->type);
	map.insert("v_api"_L1, this->version_api);
	map.insert("sys_type"_L1, this->sys_type);
	QCborArray cborArray;
	for (TextChanelInfo tci : data.TextChanels) {
		QCborMap t_map;
		t_map.insert("name"_L1, tci.Name);
		t_map.insert("id"_L1, tci.id);
		cborArray.append(t_map);
	}
	
	map.insert("text_chanels"_L1,cborArray);
	QByteArray binaryData = QCborValue(map).toCbor();
	this->bytes = binaryData;
}