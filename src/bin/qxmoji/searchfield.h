#ifndef QXMOJI_SEARCHFIELD_H
#define QXMOJI_SEARCHFIELD_H

#include <QLineEdit>

class QEvent;
class QMouseEvent;

class SearchField: public QLineEdit
{
	Q_OBJECT
    
    public:
	SearchField();

    protected:
	virtual void leaveEvent(QEvent *ev) override;
	virtual void mousePressEvent(QMouseEvent *ev) override;

    signals:
	void activated();
	void left();
};

#endif
