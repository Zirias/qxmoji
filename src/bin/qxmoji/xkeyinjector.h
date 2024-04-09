#ifndef QXMOJI_XKEYINJECTOR_H
#define QXMOJI_XKEYINJECTOR_H

#include "decl.h"

#include <xcb/xcb.h>

C_CLASS_DECL(Emoji);

DECLEXPORT void XKeyInjector_setConnection(xcb_connection_t *conn);
DECLEXPORT void XKeyInjector_setWaitMs(int ms);
DECLEXPORT void XKeyInjector_inject(const Emoji *emoji) ATTR_NONNULL((1));

#endif
