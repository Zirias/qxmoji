#ifndef QXMOJI_EMOJISTRING_H
#define QXMOJI_EMOJISTRING_H

#include "decl.h"

#include <QString>

C_CLASS_DECL(Emoji);

const QString &Emoji_qstr(const Emoji *self)
    CMETHOD ATTR_PURE;
const Emoji *Emoji_byQstr(const QString &str)
    ATTR_PURE;

#endif
