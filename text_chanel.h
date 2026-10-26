#ifndef TEXT_CHANEL_H
#define TEXT_CHANEL_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QString>
#include "message.h"

struct TextChanelInfo
{
	QString Name;
	uint id;
};

class text_chanel
{
public:
	text_chanel(QString name,uint id);
	QList<message> messages;
	void add_message(message message);
	QString name;
private:
	uint id;
};

#endif