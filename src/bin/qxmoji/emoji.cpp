#include "emoji.h"

#include <string.h>

#include <QCoreApplication>
#include <QString>

struct Emoji
{
    const EmojiGroup *const group;
    const char32_t *const codepoints;
    const QString qstr;
    const char *const name;
};

struct EmojiGroup
{
    const size_t size;
    const char *const name;
    const Emoji *const emojis;
};

typedef struct EmojiBucket
{
    const size_t size;
    const Emoji *const *const emojis;
} EmojiBucket;

#include "emojidata.cpp"

static int hashstr(const QString &str)
{
    size_t h = 5381;
    const char16_t *c = reinterpret_cast<const char16_t *>(str.constData());
    qsizetype sz = str.size();
    for (qsizetype i = 0; i < sz; ++i) h+= (h<<5) + c[i];
    return h & 0x3ffU;
}

SOLOCAL const EmojiGroup *Emoji_group(const Emoji *self)
{
    return self->group;
}

SOLOCAL const char32_t *Emoji_codepoints(const Emoji *self)
{
    return self->codepoints;
}

SOLOCAL const QString &Emoji_qstr(const Emoji *self)
{
    return self->qstr;
}

SOLOCAL const char *Emoji_name(const Emoji *self)
{
    return self->name;
}

SOLOCAL QString Emoji_localname(const Emoji *self)
{
    return QCoreApplication::translate("EmojiData", self->name);
}

SOLOCAL void Emoji_passlocalname(const Emoji *self,
	void (*recv)(const char *, void *), void *ctx)
{
    recv(qPrintable(Emoji_localname(self)), ctx);
}

SOLOCAL const Emoji *Emoji_byQstr(const QString &str)
{
    int hash = hashstr(str);
    const Emoji *emoji = 0;
    for (size_t i = 0; i < emojiint_hashtable[hash].size; ++i)
    {
	if (emojiint_hashtable[hash].emojis[i]->qstr == str)
	{
	    emoji = emojiint_hashtable[hash].emojis[i];
	    break;
	}
    }
    return emoji;
}

SOLOCAL size_t Emoji_count(void)
{
    return sizeof emojiint_emojis / sizeof *emojiint_emojis;
}

SOLOCAL const Emoji *Emoji_at(size_t i)
{
    if (i > sizeof emojiint_emojis / sizeof *emojiint_emojis) return 0;
    return emojiint_emojis + i;
}

SOLOCAL const char *EmojiGroup_name(const EmojiGroup *self)
{
    return self->name;
}

SOLOCAL size_t EmojiGroup_countEmojis(const EmojiGroup *self)
{
    return self->size;
}

SOLOCAL const Emoji *EmojiGroup_emoji(const EmojiGroup *self, size_t i)
{
    if (i >= self->size) return 0;
    return self->emojis + i;
}

SOLOCAL size_t EmojiGroup_count(void)
{
    return sizeof emojiint_groups / sizeof *emojiint_groups;
}

SOLOCAL const EmojiGroup *EmojiGroup_at(size_t i)
{
    if (i > sizeof emojiint_groups / sizeof *emojiint_groups) return 0;
    return emojiint_groups + i;
}

