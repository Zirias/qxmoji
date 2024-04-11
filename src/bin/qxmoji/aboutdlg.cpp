#include "aboutdlg.h"

#include "decl.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

AboutDlg::AboutDlg(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("About qXmoji");
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    QLabel *logo = new QLabel(this);
    logo->setPixmap(QPixmap(":/icon_256.png"));
    layout->addWidget(logo);
    QWidget *content = new QWidget(this);
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    QLabel *text = new QLabel(content);
    text->setText("<h2>qXmoji v" QXMOJIVERSTR "</h2>"
	    "<p>X11 emoji keyboard with a Qt GUI</p>"
	    "<p><table><tr><td>License:</td><td>BSD 2-clause</td></tr>"
	    "<tr><td>Author:</td><td>Felix Palmen <a href=\"mailto:"
	    "felix@palmen-it.de\">&lt;felix@palmen-it.de&gt;</a></td></tr>"
	    "<tr><td>WWW:</td><td><a "
	    "href=\"https://github.com/Zirias/qxmoji\">"
	    "https://github.com/Zirias/qxmoji</a></td></tr></table></p>");
    text->setTextFormat(Qt::RichText);
    text->setTextInteractionFlags(Qt::TextBrowserInteraction);
    text->setOpenExternalLinks(true);
    contentLayout->addWidget(text);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttons, &QDialogButtonBox::accepted, this, &QWidget::hide);
    contentLayout->addWidget(buttons);
    content->setLayout(contentLayout);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addWidget(content);
    setLayout(layout);
}
