#include "qxmoji.h"

#include "emojibutton.h"
#include "qxmojiwin.h"
#include "xkeyinjector.h"

class QXmojiPrivate {
    Q_DISABLE_COPY(QXmojiPrivate)
    Q_DECLARE_PUBLIC(QXmoji)
    QXmoji *const q_ptr;

    QIcon appIcon;
    QXmojiWin win;
    
    QXmojiPrivate(QXmoji *);
};

QXmojiPrivate::QXmojiPrivate(QXmoji *app) : q_ptr(app)
{
    appIcon.addPixmap(QPixmap(":/icon_48.png"));
    appIcon.addPixmap(QPixmap(":/icon_32.png"));
    appIcon.addPixmap(QPixmap(":/icon_16.png"));
    win.setWindowIcon(appIcon);
}

QXmoji::QXmoji(int &argc, char **argv) :
    QApplication(argc, argv),
    d_ptr(new QXmojiPrivate(this))
{
    connect(&d_ptr->win, &QXmojiWin::clicked, [](const EmojiButton *b){
	    XKeyInjector_inject(b->emoji());});
}

void QXmoji::show()
{
    Q_D(QXmoji);
    d->win.show();
}

QXmoji::~QXmoji() {}
