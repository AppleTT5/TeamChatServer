#include "system_message.h"

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QStringLiteral>
#include "system_messages/sm_get_chanel_history.h"
using namespace Qt::Literals::StringLiterals;

system_message::system_message()
{      
	this->type= MessageType::SystemMessage;
}

system_message::system_message(QByteArray Bytes)
{
	QCborValue cborValue = QCborValue::fromCbor(Bytes);
	QCborMap map = cborValue.toMap();
	int int_sys_type= map.value("sys_type"_L1).toInteger();
	this->sys_type = SystemMessageType(int_sys_type);
	switch (this->sys_type) {
	case SystemMessageType::GetChanelHistory:
		this->data = get_chanel_history_message(Bytes).data;
	}
}