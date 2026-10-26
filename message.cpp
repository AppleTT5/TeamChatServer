#include "message.h"

#include <QtCore>
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QStringLiteral>

using namespace Qt::Literals::StringLiterals;

message::message()
{

}

message::message(QByteArray Bytes)
{
	QCborValue cborValue = QCborValue::fromCbor(Bytes);
	if (!cborValue.isMap()) return;
	QCborMap map = cborValue.toMap();
	int type_int = map.value("type"_L1).toInteger();
	MessageType type = MessageType(type_int);
	this->type = type;
	QString v_api = map.value("v_api"_L1).toString();
	this->version_api = v_api;
	switch (this->type) {
	case MessageType::UserMessage:
		this->value = user_message(user_message::type_input_data::file, Bytes).data;
		break;
	case MessageType::SystemMessage:
		this->value = system_message(Bytes).data;
	}
}