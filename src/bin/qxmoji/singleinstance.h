#ifndef QXMOJI_SINGLEINSTANCE_H
#define QXMOJI_SINGLEINSTANCE_H

#include <QObject>

class SingleInstance: public QObject
{
	Q_OBJECT

    public:
	SingleInstance();

	bool startInstance();

    signals:
	void secondaryInstance();
};

#endif
