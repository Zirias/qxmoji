#include "searchfield.h"

#include <QMouseEvent>

SearchField::SearchField() : QLineEdit()
{
    setPlaceholderText(tr("Click to type and search ..."));
    setClearButtonEnabled(true);
}

void SearchField::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) emit activated();
    QLineEdit::mousePressEvent(ev);
}

void SearchField::leaveEvent(QEvent *ev)
{
    emit left();
    QLineEdit::leaveEvent(ev);
}

#include "moc_searchfield.cpp"
