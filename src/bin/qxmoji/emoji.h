#ifndef QXMOJI_EMOJI_H
#define QXMOJI_EMOJI_H

#include "decl.h"

#include <stdlib.h>
#include <uchar.h>

C_CLASS_DECL(Emoji);
C_CLASS_DECL(EmojiGroup);

DECLEXPORT const EmojiGroup *Emoji_group(const Emoji *self)
    CMETHOD ATTR_PURE;
DECLEXPORT const char32_t *Emoji_codepoints(const Emoji *self)
    CMETHOD ATTR_PURE ATTR_RETNONNULL;
DECLEXPORT const char16_t *Emoji_utf16(const Emoji *self)
    CMETHOD ATTR_PURE ATTR_RETNONNULL;
DECLEXPORT const char *Emoji_utf8(const Emoji *self)
    CMETHOD ATTR_PURE ATTR_RETNONNULL;
DECLEXPORT const char *Emoji_name(const Emoji *self)
    CMETHOD ATTR_PURE ATTR_RETNONNULL;

DECLEXPORT const Emoji *Emoji_byUtf8(const char *utf8)
    ATTR_PURE;

DECLEXPORT const char *EmojiGroup_name(const EmojiGroup *self)
    CMETHOD ATTR_PURE;
DECLEXPORT size_t EmojiGroup_countEmojis(const EmojiGroup *self)
    CMETHOD ATTR_PURE;
DECLEXPORT const Emoji *EmojiGroup_emoji(const EmojiGroup *self, size_t i)
    CMETHOD ATTR_PURE;

DECLEXPORT size_t EmojiGroup_count(void)
    ATTR_CONST;
DECLEXPORT const EmojiGroup *EmojiGroup_at(size_t i)
    ATTR_PURE;

#endif
