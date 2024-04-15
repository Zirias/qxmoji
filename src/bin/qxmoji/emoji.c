#include "emoji.h"

#include <string.h>

struct Emoji
{
    const EmojiGroup *const group;
    const char32_t *const codepoints;
    const char16_t *const utf16;
    const char *const utf8;
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

#include "emojidata.c"

static int hashstr(const char *s)
{
    size_t h = 5381;
    while (*s) h += (h<<5) + ((unsigned char)*s++);
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

SOLOCAL const char16_t *Emoji_utf16(const Emoji *self)
{
    return self->utf16;
}

SOLOCAL const char *Emoji_utf8(const Emoji *self)
{
    return self->utf8;
}

SOLOCAL const char *Emoji_name(const Emoji *self)
{
    return self->name;
}

SOLOCAL const Emoji *Emoji_byUtf8(const char *utf8)
{
    int hash = hashstr(utf8);
    const Emoji *emoji = 0;
    for (size_t i = 0; i < emojiint_hashtable[hash].size; ++i)
    {
	if (!strcmp(utf8, emojiint_hashtable[hash].emojis[i]->utf8))
	{
	    emoji = emojiint_hashtable[hash].emojis[i];
	    break;
	}
    }
    return emoji;
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

