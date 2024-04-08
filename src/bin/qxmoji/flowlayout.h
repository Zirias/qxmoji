#ifndef QXMOJI_FLOWLAYOUT_H
#define QXMOJI_FLOWLAYOUT_H

#include <QLayout>
#include <QScopedPointer>

class FlowLayoutPrivate;
class FlowLayout: public QLayout
{
	Q_DECLARE_PRIVATE(FlowLayout)
	QScopedPointer<FlowLayoutPrivate> d_ptr;

    public:
	FlowLayout(QWidget *parent);
	~FlowLayout();

	void addItem(QLayoutItem *item) override;
	Qt::Orientations expandingDirections() const override;
	bool hasHeightForWidth() const override;
	int heightForWidth(int) const override;
	int count() const override;
	QLayoutItem *itemAt(int index) const override;
	QSize minimumSize() const override;
	void setGeometry(const QRect &rect) override;
	QSize sizeHint() const override;
	QLayoutItem *takeAt(int index) override;
};

#endif
