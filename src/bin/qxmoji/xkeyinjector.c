#include "xkeyinjector.h"

#include "emoji.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <xcb/xtest.h>

static xcb_connection_t *c;
static const xcb_setup_t *s;
static xcb_window_t w;
static int waitms = 50;
static int kbgrabbed = 0;

static void *xmalloc(size_t n)
{
    void *x = malloc(n);
    if (!x) abort();
    return x;
}

void XKeyInjector_setConnection(xcb_connection_t *conn)
{
    c = conn;
    s = xcb_get_setup(c);
}

void XKeyInjector_setGrabWindow(unsigned id)
{
    w = id;
}

void XKeyInjector_setWaitMs(int ms)
{
    if (ms < 0) ms = 0;
    if (ms > 500) ms = 500;
    waitms = ms;
}

void XKeyInjector_inject(const Emoji *emoji)
{
    if (!c) return;
    if (kbgrabbed) XKeyInjector_ungrabKeyboard();
    xcb_generic_error_t *error;
    xcb_keysym_t *syms = 0;
    xcb_get_keyboard_mapping_reply_t *kmap =
	xcb_get_keyboard_mapping_reply(c,
		xcb_get_keyboard_mapping(
		    c, s->min_keycode,
		    s->max_keycode - s->min_keycode + 1), 0);

    const char32_t *codepoints = Emoji_codepoints(emoji);
    size_t len = 0;
    while (codepoints[len]) ++len;

    syms = xmalloc(len * kmap->keysyms_per_keycode * sizeof *syms);
    size_t z = 0;
    for (size_t x = 0; x < len; ++x)
    {
	char32_t codepoint = codepoints[x];
	for (size_t y = 0; y < kmap->keysyms_per_keycode; ++y)
	{
	    syms[z++] = 0x1000000U + codepoint;
	}
    }

    if ((error = xcb_request_check(c, xcb_change_keyboard_mapping_checked(
		c, len, s->min_keycode, kmap->keysyms_per_keycode,
		syms))))
    {
	fputs("Error changing X11 keyboard mapping.\n", stderr);
	free(error);
	goto done;
    }

    for (size_t x = 0; x < len; ++x)
    {
	if ((error = xcb_request_check(c, xcb_test_fake_input_checked(c,
			    XCB_KEY_PRESS, s->min_keycode + x,
			    XCB_CURRENT_TIME, XCB_NONE, 0, 0, 0))))
	{
	    fputs("Error sending X11 key press event.\n", stderr);
	    free(error);
	    goto done;
	}
	if ((error = xcb_request_check(c, xcb_test_fake_input_checked(c,
			    XCB_KEY_RELEASE, s->min_keycode + x,
			    XCB_CURRENT_TIME, XCB_NONE, 0, 0, 0))))
	{
	    fputs("Error sending X11 key release event.\n", stderr);
	    free(error);
	    goto done;
	}
    }

    if (waitms)
    {
	struct timespec ts = {
	    .tv_sec = 0,
	    .tv_nsec = 1000000 * waitms
	};
	nanosleep(&ts, &ts);
    }

    if ((error = xcb_request_check(c, xcb_change_keyboard_mapping_checked(
		c, len, s->min_keycode, kmap->keysyms_per_keycode,
		(xcb_keysym_t*)(kmap + 1)))))
    {
	fputs("Error restoring X11 keyboard mapping.\n", stderr);
	free(error);
	goto done;
    }

done:
    free(syms);
    free(kmap);
}

void XKeyInjector_grabKeyboard(void)
{
    if (!c || !w || kbgrabbed) return;

    xcb_grab_keyboard_reply_t *kr = xcb_grab_keyboard_reply(c,
	    xcb_grab_keyboard(c, 1, w, XCB_CURRENT_TIME,
		XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC), 0);
    if (kr)
    {
	if (kr->status == XCB_GRAB_STATUS_SUCCESS) kbgrabbed = 1;
	free(kr);
    }
}

void XKeyInjector_ungrabKeyboard(void)
{
    if (!c || !kbgrabbed) return;

    xcb_generic_error_t *error = xcb_request_check(c,
	    xcb_ungrab_keyboard_checked(c, XCB_CURRENT_TIME));
    if (error) free(error);
    else kbgrabbed = 0;
}
