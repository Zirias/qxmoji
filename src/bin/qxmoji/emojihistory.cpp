#include "emojihistory.h"

#include "emoji.h"
#include "emojistring.h"

#include <cstring>
#include <QString>
#include <QStringList>

class EmojiHistoryPrivate
{
    Q_DISABLE_COPY(EmojiHistoryPrivate)
    Q_DECLARE_PUBLIC(EmojiHistory)
    EmojiHistory *const q_ptr;

    const Emoji *history[EMOJIHISTORY_MAXLEN];
    QString hstr;
    int len;

    EmojiHistoryPrivate(EmojiHistory *, const QStringList &);
};

EmojiHistoryPrivate::EmojiHistoryPrivate(
	EmojiHistory *h, const QStringList &l):
    q_ptr(h),
    len(0)
{
    if (!l.count()) return;
    for (auto e = l.cbegin(); len < EMOJIHISTORY_MAXLEN && e != l.cend(); ++e)
    {
	const Emoji *emoji = Emoji_byQstr(*e);
	if (emoji) history[len++] = emoji;
    }
}

EmojiHistory::EmojiHistory(const QString &str) :
    d_ptr(new EmojiHistoryPrivate(this, str.split(' ')))
{}

EmojiHistory::~EmojiHistory() {}

int EmojiHistory::len()
{
    Q_D(EmojiHistory);
    return d->len;
}

const Emoji *EmojiHistory::at(int i)
{
    Q_D(EmojiHistory);
    if (i < 0 || i >= d->len) return nullptr;
    return d->history[i];
}

const QString &EmojiHistory::serialize()
{
    Q_D(EmojiHistory);
    QStringList entries;
    for (int i = 0; i < d->len; ++i) entries.append(Emoji_qstr(d->history[i]));
    d->hstr = entries.join(' ');
    return d->hstr;
}

void EmojiHistory::record(const Emoji *emoji)
{
    Q_D(EmojiHistory);
    int oldpos = -1;
    for (int i = 0; i < d->len; ++i)
    {
	if (d->history[i] == emoji)
	{
	    oldpos = i;
	    break;
	}
    }
    if (!oldpos) return;
    int movelen = oldpos > 0 ? oldpos : d->len++;
    memmove(d->history+1, d->history, movelen * sizeof *d->history);
    d->history[0] = emoji;
    emit changed(movelen+1);
}

