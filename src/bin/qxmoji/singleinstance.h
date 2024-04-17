#ifndef QXMOJI_SINGLEINSTANCE_H
#define QXMOJI_SINGLEINSTANCE_H

#include <QObject>
#include <QScopedPointer>

class SingleInstancePrivate;
class SingleInstance: public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(SingleInstance)
	QScopedPointer<SingleInstancePrivate> const d_ptr;

    public:
	SingleInstance();
	~SingleInstance();

	bool startInstance();
	void stopInstance();

    signals:
	void secondaryInstance();
};

#endif
