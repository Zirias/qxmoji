#include "xkeyinjector.h"

#include "emoji.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xcb/xtest.h>

static xcb_connection_t *c;
static const xcb_setup_t *s;

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

void XKeyInjector_inject(const Emoji *emoji)
{
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

    if ((error = xcb_request_check(c, xcb_change_keyboard_mapping(
		c, len, s->min_keycode, kmap->keysyms_per_keycode,
		syms))))
    {
	fputs("Error changing X11 keyboard mapping.\n", stderr);
	free(error);
	goto done;
    }
    usleep(1000);

    for (size_t x = 0; x < len; ++x)
    {
	if ((error = xcb_request_check(c, xcb_test_fake_input(c,
			    XCB_KEY_PRESS, s->min_keycode + x,
			    XCB_CURRENT_TIME, XCB_NONE, 0, 0, 0))))
	{
	    fputs("Error sending key press event.\n", stderr);
	    free(error);
	    goto done;
	}
    }
    usleep(6000);
    for (size_t x = 0; x < len; ++x)
    {
	if ((error = xcb_request_check(c, xcb_test_fake_input(c,
			    XCB_KEY_RELEASE, s->min_keycode + x,
			    XCB_CURRENT_TIME, XCB_NONE, 0, 0, 0))))
	{
	    fputs("Error sending key release event.\n", stderr);
	    free(error);
	    goto done;
	}
    }

    usleep(100000);
    if ((error = xcb_request_check(c, xcb_change_keyboard_mapping(
		c, len, s->min_keycode, kmap->keysyms_per_keycode,
		(xcb_keysym_t*)(kmap + 1)))))
    {
	fputs("Error restoring keyboard mapping.\n", stderr);
	free(error);
	goto done;
    }

done:
    free(syms);
    free(kmap);
}
