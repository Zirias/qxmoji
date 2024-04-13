#ifndef QXMOJI_XKEYINJECTOR_H
#define QXMOJI_XKEYINJECTOR_H

#include "decl.h"

C_CLASS_DECL(Emoji);
C_CLASS_DECL(XcbAdapter);

DECLEXPORT void XKeyInjector_inject(
	XcbAdapter *xcb, const Emoji *emoji, int waitms)
    ATTR_NONNULL((1)) ATTR_NONNULL((2));

#endif
