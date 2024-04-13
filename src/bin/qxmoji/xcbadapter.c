#include "xcbadapter.h"

#include <stdlib.h>
#include <string.h>

#define _STR(x) #x
#define STR(x) _STR(x)
#define X_SZNM(a) { sizeof STR(a) - 1, STR(a) },

#define A(X) (self->atoms[X])

struct XAtomNames
{
    int len;
    char *nm;
};

static const struct XAtomNames atomnm[] = {
    X_ATOMS(X_SZNM)
};

struct XcbAdapter
{
    xcb_connection_t *c;
    xcb_atom_t atoms[NATOMS];
};

SOLOCAL XcbAdapter *XcbAdapter_create(xcb_connection_t *conn)
{
    XcbAdapter *self = malloc(sizeof *self);
    if (!self) abort();
    self->c = conn;
    
    xcb_intern_atom_reply_t *atom;
    for (int i = 0; i < NATOMS; ++i)
    {
	atom = xcb_intern_atom_reply(conn, xcb_intern_atom(conn, 0,
		    atomnm[i].len, atomnm[i].nm), 0);
	if (atom)
	{
	    self->atoms[i] = atom->atom;
	    free(atom);
	}
	else self->atoms[i] = 0;
    }

    return self;
}

SOLOCAL xcb_connection_t *XcbAdapter_connection(const XcbAdapter *self)
{
    return self->c;
}

SOLOCAL xcb_atom_t XcbAdapter_atom(const XcbAdapter *self, XAtom id)
{
    if (id < 0 || id >= NATOMS) return 0;
    return A(id);
}

SOLOCAL int XcbAdapter_grabKeyboard(XcbAdapter *self, xcb_window_t w)
{
    if (!w) return -1;

    xcb_grab_keyboard_reply_t *kr = xcb_grab_keyboard_reply(self->c,
	    xcb_grab_keyboard(self->c, 1, w, XCB_CURRENT_TIME,
		XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC), 0);
    if (kr)
    {
	free(kr);
	return 0;
    }
    return -1;
}

SOLOCAL int XcbAdapter_ungrabKeyboard(XcbAdapter *self)
{
    xcb_generic_error_t *error = xcb_request_check(self->c,
	    xcb_ungrab_keyboard_checked(self->c, XCB_CURRENT_TIME));
    if (error)
    {
	free(error);
	return -1;
    }
    return 0;
}

SOLOCAL int XcbAdapter_setSkipTaskbar(
	XcbAdapter *self, xcb_window_t w, int skip)
{
    if (!w) return -1;

    if (!A(WM_STATE) || !A(_NET_WM_STATE) || !A(_NET_WM_STATE_SKIP_TASKBAR))
    {
	return -1;
    }

    int rc = -1;
    int isMapped = 0;
    xcb_generic_error_t *error = 0;

    xcb_get_property_reply_t *prop = xcb_get_property_reply(self->c,
	    xcb_get_property(self->c, 0, w, A(WM_STATE), A(WM_STATE),
		0, 2), 0);
    if (prop)
    {
	if (prop->type == A(WM_STATE))
	{
	    uint32_t state = *(uint32_t *)xcb_get_property_value(prop);
	    if (state) isMapped = 1;
	}
	free(prop);
    }

    if (isMapped)
    {
	xcb_query_tree_reply_t *tree = xcb_query_tree_reply(self->c,
		xcb_query_tree(self->c, w), 0);
	if (tree)
	{
	    xcb_window_t root = tree->root;
	    free(tree);
	    xcb_client_message_event_t ev = {
		.response_type = XCB_CLIENT_MESSAGE,
		.format = 32,
		.sequence = 0,
		.window = w,
		.type = A(_NET_WM_STATE),
		.data = {
		    .data32 = {
			skip, A(_NET_WM_STATE_SKIP_TASKBAR),
			0, 0, 0
		    }
		}
	    };
	    error = xcb_request_check(self->c, xcb_send_event(self->c, 0, root,
			XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
			XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
			(const char *)&ev));
	    if (error) goto done;
	}
	return 0;
    }

    prop = xcb_get_property_reply(self->c, xcb_get_property(self->c, 0, w,
		A(_NET_WM_STATE), XCB_ATOM_ATOM, 0, 32), 0);
    int wasSkipped = 0;
    int natoms = 0;
    xcb_atom_t *atoms = 0;
    if (prop && prop->type == XCB_ATOM_ATOM)
    {
	atoms = (xcb_atom_t *)xcb_get_property_value(prop);
	natoms = xcb_get_property_value_length(prop) / sizeof *atoms;
	for (int i = 0; i < natoms; ++i)
	{
	    if (atoms[i] == A(_NET_WM_STATE_SKIP_TASKBAR))
	    {
		if (i < natoms-1) memmove(atoms+i, atoms+i+1, natoms-i-1);
		wasSkipped = 1;
		--natoms;
		break;
	    }
	}
    }
    if (wasSkipped == skip)
    {
	free(prop);
	return 0;
    }
    if (skip)
    {
	error = xcb_request_check(self->c, xcb_change_property_checked(self->c,
		    XCB_PROP_MODE_APPEND, w, A(_NET_WM_STATE), XCB_ATOM_ATOM,
		    32, 1, &A(_NET_WM_STATE_SKIP_TASKBAR)));
	if (error) goto done;
    }
    else if (natoms)
    {
	error = xcb_request_check(self->c, xcb_change_property_checked(self->c,
		    XCB_PROP_MODE_REPLACE, w, A(_NET_WM_STATE), XCB_ATOM_ATOM,
		    32, natoms, atoms));
	if (error) goto done;
    }

    rc = 0;
done:
    free(error);
    free(prop);
    return rc;
}

SOLOCAL int XcbAdapter_blockMinimizeEvent(
	XcbAdapter *self, xcb_window_t w, xcb_generic_event_t *ev)
{
    xcb_get_property_reply_t *prop = 0;
    xcb_generic_error_t *error = 0;
    int blocked = 0;

    if (ev->response_type != XCB_PROPERTY_NOTIFY) goto done;
    if (!A(_NET_WM_STATE) || !A(_NET_WM_STATE_HIDDEN)) goto done;

    xcb_property_notify_event_t *nev = (xcb_property_notify_event_t *)ev;
    if (nev->atom != A(_NET_WM_STATE)) goto done;

    prop = xcb_get_property_reply(self->c, xcb_get_property(self->c, 1, w,
		A(_NET_WM_STATE), XCB_ATOM_ATOM, 0, 32), 0);
    if (!prop) goto done;
    if (prop->type != XCB_ATOM_ATOM) goto done;
    xcb_atom_t *atoms = (xcb_atom_t *)xcb_get_property_value(prop);
    int natoms = xcb_get_property_value_length(prop) / sizeof *atoms;
    if (!natoms) goto done;
    int minimized = 0;
    for (int i = 0; i < natoms; ++i)
    {
	if (atoms[i] == A(_NET_WM_STATE_HIDDEN))
	{
	    if (i < natoms-1) memmove(atoms+i, atoms+i+1, natoms-i-1);
	    minimized = 1;
	    --natoms;
	    break;
	}
    }
    if (!minimized) goto done;
    if (natoms)
    {
	error = xcb_request_check(self->c, xcb_change_property_checked(self->c,
		    XCB_PROP_MODE_REPLACE, w, A(_NET_WM_STATE), XCB_ATOM_ATOM,
		    32, natoms, atoms));
	if (error) goto done;
    }
    blocked = 1;

done:
    free(error);
    free(prop);
    return blocked;
}

SOLOCAL void XcbAdapter_destroy(XcbAdapter *self)
{
    free(self);
}
