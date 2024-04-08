#ifndef QXMOJI_EMOJI_H
#define QXMOJI_EMOJI_H

#include "decl.h"

#include <uchar.h>

C_CLASS_DECL(Emoji);
C_CLASS_DECL(EmojiGroup);

extern const EmojiGroup *emojigroups;

DECLEXPORT const char *Emoji_name(const Emoji *self)
    CMETHOD ATTR_PURE;
DECLEXPORT const char32_t *Emoji_codepoints(const Emoji *self)
    CMETHOD ATTR_PURE;
DECLEXPORT const Emoji *Emoji_next(const Emoji *self)
    CMETHOD ATTR_PURE;

DECLEXPORT const char *EmojiGroup_name(const EmojiGroup *self)
    CMETHOD ATTR_PURE;
DECLEXPORT const Emoji *EmojiGroup_emojis(const EmojiGroup *self)
    CMETHOD ATTR_PURE;
DECLEXPORT const EmojiGroup *EmojiGroup_next(const EmojiGroup *self)
    CMETHOD ATTR_PURE;

#endif
