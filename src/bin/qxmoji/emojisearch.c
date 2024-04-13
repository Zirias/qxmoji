#include "emojisearch.h"

#include "emoji.h"

#include <ctype.h>
#include <string.h>

#define MAXLEN 80

static const Emoji *results[EMOJISEARCH_MAXRESULTS+1];
static char patbuf[MAXLEN+1];
static char nmbuf[MAXLEN+1];

static void takelower(char *buf, const char *str)
{
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

    takelower(patbuf, pattern);
    const EmojiGroup *group = emojigroups;
    while (group)
    {
	const Emoji *emoji = EmojiGroup_emojis(group);
	while (emoji)
	{
	    takelower(nmbuf, Emoji_name(emoji));
	    if (strstr(nmbuf, patbuf))
	    {
		results[nresults++] = emoji;
		if (nresults == EMOJISEARCH_MAXRESULTS) goto done;
	    }
	    emoji = Emoji_next(emoji);
	}
	group = EmojiGroup_next(group);
    }

done:
    results[nresults] = 0;
    return results;
}

