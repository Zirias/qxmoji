#ifndef QXMOJI_GUIMAIN_H
#define QXMOJI_GUIMAIN_H

#include "decl.h"

DECLEXPORT int guimain(int argc, char **argv, void (*started)(void))
    ATTR_NONNULL((2)) ATTR_NONNULL((3));

#endif
