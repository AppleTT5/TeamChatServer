#include "user_message.h"
#include <QtCore>
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QStringLiteral>

using namespace std;
using namespace Qt::Literals::StringLiterals;
QT_USE_NAMESPACE

user_message::user_message(type_input_data type , QByteArray bytes , user_message_data  message_data ) {
	this->type = MessageType::UserMessage;
	this->version_api = "1.0.0.0";

	switch (type)
	{	
	case user_message::none:
		break;
	case user_message::file:
		this->bytes = bytes;
		this->data =  this->Read();
		break;
	case user_message::structure:
		this->name = message_data.NikName;
		this->channel = message_data.channel;
		this->text = message_data.text;
		this->Write();
		break;
	}

}

QByteArray user_message::Write()
{
	QCborMap map;
	map.insert("type"_L1, this->type);
	map.insert("v_api"_L1, this->version_api);
	map.insert("name"_L1, this->name);
	map.insert("chanel"_L1, this->channel);
	map.insert("text"_L1, this->text);
	QByteArray binaryData = QCborValue(map).toCbor();
	return binaryData;

}

user_message_data user_message::Read() 
{
	QCborValue cborValue = QCborValue::fromCbor(this->bytes);
	if (!cborValue.isMap()) return user_message_data{ false,NULL, NULL, NULL };
	QCborMap map = cborValue.toMap();
	int type_int = map.value("type"_L1).toInteger();
	MessageType type = MessageType(type_int);
	if (type != MessageType::UserMessage)return user_message_data{ false,NULL, NULL, NULL };
	QString v_api = map.value("v_api"_L1).toString();
	QString name = map.value("name"_L1).toString();
	int chanel = map.value("chanel"_L1).toInteger();
	QString text = map.value("text"_L1).toString();
	return user_message_data{ true,name,chanel,text };
}
