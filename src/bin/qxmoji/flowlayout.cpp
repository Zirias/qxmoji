#include "flowlayout.h"

#include <QList>

class FlowLayoutPrivate
{
    Q_DISABLE_COPY(FlowLayoutPrivate)
    Q_DECLARE_PUBLIC(FlowLayout)
    FlowLayout *const q_ptr;

    QList<QLayoutItem *> items;

    FlowLayoutPrivate(FlowLayout *);
    int doLayout(const QRect &rect, bool testOnly) const;
};

FlowLayoutPrivate::FlowLayoutPrivate(FlowLayout *layout) :
    q_ptr(layout)
{}

int FlowLayoutPrivate::doLayout(const QRect &rect, bool testOnly) const
{
    int x = rect.x();
    int y = rect.y();
    int lineHeight = 0;

    for (QLayoutItem *item : qAsConst(items))
    {
	int nextX = x + item->sizeHint().width();
	if (nextX > rect.right() && lineHeight > 0)
	{
	    x = rect.x();
	    y = y + lineHeight;
	    nextX = x + item->sizeHint().width();
	    lineHeight = 0;
	}
	if (!testOnly)
	{
	    item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
	}

	x = nextX;
	lineHeight = qMax(lineHeight, item->sizeHint().height());
    }

    return y + lineHeight - rect.y();
}

FlowLayout::FlowLayout(QWidget *parent) :
    QLayout(parent),
    d_ptr(new FlowLayoutPrivate(this))
{
    setContentsMargins(0, 0, 0, 0);
}

FlowLayout::~FlowLayout()
{
    QLayoutItem *item;
    while ((item = takeAt(0))) delete item;
}

void FlowLayout::addItem(QLayoutItem *item)
{
    Q_D(FlowLayout);
    d->items.append(item);
}

int FlowLayout::count() const
{
    Q_D(const FlowLayout);
    return d->items.size();
}

QLayoutItem *FlowLayout::itemAt(int index) const
{
    Q_D(const FlowLayout);
    return d->items.value(index);
}

QLayoutItem *FlowLayout::takeAt(int index)
{
    Q_D(FlowLayout);
    if (index >= 0 && index < d->items.size()) return d->items.takeAt(index);
    return nullptr;
}

Qt::Orientations FlowLayout::expandingDirections() const
{
    return {};
}

bool FlowLayout::hasHeightForWidth() const
{
    return true;
}

int FlowLayout::heightForWidth(int width) const
{
    Q_D(const FlowLayout);
    return d->doLayout(QRect(0, 0, width, 0), true);
}

void FlowLayout::setGeometry(const QRect &rect)
{
    Q_D(FlowLayout);
    QLayout::setGeometry(rect);
    d->doLayout(rect, false);
}

QSize FlowLayout::sizeHint() const
{
    QSize hintSize = minimumSize();
    hintSize.setWidth(hintSize.width()*16+1);
    return hintSize;
}

QSize FlowLayout::minimumSize() const
{
    Q_D(const FlowLayout);
    QSize size;
    for (const QLayoutItem *item : qAsConst(d->items))
    {
	size = size.expandedTo(item->minimumSize());
    }
    return size;
}

