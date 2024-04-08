#include "qxmoji.h"

#include "emojibutton.h"
#include "qxmojiwin.h"
#include "xkeyinjector.h"

class QXmojiPrivate {
    Q_DISABLE_COPY(QXmojiPrivate)
    Q_DECLARE_PUBLIC(QXmoji)
    QXmoji *const q_ptr;

    QXmojiWin win;
    
    QXmojiPrivate(QXmoji *);
};

QXmojiPrivate::QXmojiPrivate(QXmoji *app) : q_ptr(app) {}

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
