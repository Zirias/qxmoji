#ifndef QXMOJI_XCBADAPTER_H
#define QXMOJI_XCBADAPTER_H

#include "decl.h"

#include <xcb/xproto.h>

#define X_ENUM(a) a,

#define X_ATOMS(X) \
    X(WM_STATE) \
    X(_NET_WM_STATE) \
    X(_NET_WM_STATE_HIDDEN) \
    X(_NET_WM_STATE_SKIP_TASKBAR)

typedef enum XAtom
{
    X_ATOMS(X_ENUM)
    NATOMS
} XAtom;

C_CLASS_DECL(XcbAdapter);

DECLEXPORT XcbAdapter *XcbAdapter_create(xcb_connection_t *conn)
    ATTR_RETNONNULL ATTR_NONNULL((1));
DECLEXPORT xcb_connection_t *XcbAdapter_connection(
	const XcbAdapter *self)
    CMETHOD ATTR_PURE;
DECLEXPORT xcb_atom_t XcbAdapter_atom(const XcbAdapter *self, XAtom id)
    CMETHOD ATTR_PURE;

DECLEXPORT int XcbAdapter_grabKeyboard(XcbAdapter *self, xcb_window_t w)
    CMETHOD;
DECLEXPORT int XcbAdapter_ungrabKeyboard(XcbAdapter *self)
    CMETHOD;
DECLEXPORT int XcbAdapter_setSkipTaskbar(
	XcbAdapter *self, xcb_window_t w, int skip)
    CMETHOD;
DECLEXPORT int XcbAdapter_blockMinimizeEvent(
	XcbAdapter *self, xcb_window_t w, xcb_generic_event_t *ev)
    CMETHOD ATTR_NONNULL((3));

DECLEXPORT void XcbAdapter_destroy(XcbAdapter *self);

#endif
