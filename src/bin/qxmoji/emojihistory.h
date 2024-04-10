#ifndef QXMOJI_EMOJIHISTORY_H
#define QXMOJI_EMOJIHISTORY_H

#include "decl.h"

#include <stddef.h>

#define EMOJIHISTORY_MAXLEN 100

C_CLASS_DECL(Emoji);

DECLEXPORT const Emoji **Emoji_history(void);
DECLEXPORT void Emoji_use(const Emoji *emoji);
DECLEXPORT const void *Emoji_saveHistory(size_t *sz);
DECLEXPORT void Emoji_loadHistory(size_t sz, const void *data);

#endif
