#ifndef QXMOJI_EMOJISEARCH_H
#define QXMOJI_EMOJISEARCH_H

#include "decl.h"

#define EMOJISEARCH_MAXRESULTS 200

C_CLASS_DECL(Emoji);

DECLEXPORT const Emoji **Emoji_search(const char *pattern);

#endif
