#include "emojisearch.h"

#include "emoji.h"

#include <ctype.h>
#include <string.h>

#define MAXLEN 80

static const Emoji *results[EMOJISEARCH_MAXRESULTS+1];
static char patbuf[MAXLEN+1];
static char nmbuf[MAXLEN+1];

static void takelower(const char *str, void *bufptr)
{
    char *buf = bufptr;
    int i = 0;
    for (; i < MAXLEN && *str; ++i)
    {
	buf[i] = tolower(str[i]);
    }
    buf[i] = 0;
}

SOLOCAL const Emoji **Emoji_search(const char *pattern)
{
    int nresults = 0;

    takelower(pattern, patbuf);

    for (size_t i = 0; i < EmojiGroup_count(); ++i)
    {
	const EmojiGroup *group = EmojiGroup_at(i);
	for (size_t j = 0; j < EmojiGroup_countEmojis(group); ++j)
	{
	    const Emoji *emoji = EmojiGroup_emoji(group, j);
	    Emoji_passlocalname(emoji, takelower, nmbuf);
	    if (strstr(nmbuf, patbuf))
	    {
		results[nresults++] = emoji;
		if (nresults == EMOJISEARCH_MAXRESULTS) goto done;
	    }
	}
    }

done:
    results[nresults] = 0;
    return results;
}

