#include "sm_get_chanel_history.h"

#include <QtCore/>
#include <QtCore/QString>
#include <QtCore/QStringLiteral>

using namespace Qt::Literals::StringLiterals;
QT_USE_NAMESPACE;
get_chanel_history_message::get_chanel_history_message(QByteArray Bytes) {
	QCborValue cborValue = QCborValue::fromCbor(Bytes);
	QCborMap map = cborValue.toMap();
	uint id= map.value("id"_L1).toInteger();
	this->data = s_GetChanelHistiry{id};
}