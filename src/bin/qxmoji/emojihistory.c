#include "emojihistory.h"

#include "emoji.h"

#include <string.h>

#define SAVEBUFSZ (11 * 4 * EMOJIHISTORY_MAXLEN)

static const Emoji *history[EMOJIHISTORY_MAXLEN];
static unsigned char buffer[SAVEBUFSZ];

const Emoji **Emoji_history(void)
{
    return history;
}

void Emoji_use(const Emoji *emoji)
{
    int oldpos = -1;
    for (int i = 0; i < EMOJIHISTORY_MAXLEN; ++i)
    {
	if (history[i] == emoji)
	{
	    oldpos = i;
	    break;
	}
    }
    if (!oldpos) return;
    int movelen = oldpos > 0 ? oldpos : EMOJIHISTORY_MAXLEN-1;
    memmove(history+1, history, movelen * sizeof *history);
    *history = emoji;
}

const void *Emoji_saveHistory(size_t *sz)
{
    *sz = 0;
    const Emoji **h = history;
    while (*h)
    {
	const char32_t *codepoints = Emoji_codepoints(*h);
	for (;;)
	{
	    char32_t uc = *codepoints;
	    buffer[(*sz)++] = uc & 0xff;
	    uc >>= 8;
	    buffer[(*sz)++] = uc & 0xff;
	    uc >>= 8;
	    buffer[(*sz)++] = uc & 0xff;
	    uc >>= 8;
	    buffer[(*sz)++] = uc & 0xff;
	    if (!*codepoints) break;
	    ++codepoints;
	}
	++h;
    }
    return buffer;
}

void Emoji_loadHistory(size_t sz, const void *data)
{
    if (sz & 3) return;
    const unsigned char *buf = data;
    char32_t pat[11];
    int patpos = 0;

    memset(history, 0, sizeof history);
    int len = 0;
    while (sz)
    {
	char32_t uc = buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
	buf += 4;
	sz -= 4;
	pat[patpos++] = uc;
	if (uc && patpos > 10) return;
	if (!uc)
	{
	    const Emoji *entry = 0;
	    for (const EmojiGroup *group = emojigroups;
		    group && !entry; group = EmojiGroup_next(group))
	    {
		for (const Emoji *emoji = EmojiGroup_emojis(group);
			emoji && !entry; emoji = Emoji_next(emoji))
		{
		    const char32_t *a = pat;
		    const char32_t *b = Emoji_codepoints(emoji);
		    while (*a == *b)
		    {
			if (!*a)
			{
			    entry = emoji;
			    break;
			}
			++a;
			++b;
		    }
		}
	    }
	    if (entry)
	    {
		history[len++] = entry;
		if (len == EMOJIHISTORY_MAXLEN) return;
	    }
	    patpos = 0;
	}
    }
}

