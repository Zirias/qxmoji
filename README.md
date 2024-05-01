# qXmoji – X11 emoji keyboard

qXmoji is a very simple emoji keyboard designed to work with any X11
application. It uses Qt to show its GUI, but "types" emojis using plain X11
keyboard events.

The GUI itself displays the emojis as "text", and prefers the `Noto Color
Emoji` font for that, falling back to `Noto Emoji` and then to whatever is
configured as the default font. So, for best visual results, install `Noto
Color Emoji`.

![qXmoji main window](.github/screenshots/mainwindow.png?raw=true)

## Features

* Works with "plain" X11 events, so it can send emojis to any window,
  independent of toolkits like GTK or Qt, or XIM (input method) extensions.
* Search (as you type) for an emoji by its (english) name. The search box
  temporarily grabs the keyboard to allow typing and immediately releases it
  again when the mouse pointer leaves it.
* Maintains a history of the 100 least recently used emojis, this list is
  automatically persisted.
* Offers single-instance (per user and machine) and systray modes.
* Keeps settings in sync between different instances, even across machines
  when using NFS.

## How it works

X11 has no concept of "Unicode keyboard events" like e.g. Windows has.
Keyboard events in X11 always use a "key code" in the range 8-255 which are
meant to identify an actual key on the keyboard. Therefore the only way to
inject Unicode characters as keyboard events is to temporarily modify the
keyboard mapping.

When qXmoji "types" an emoji, it first determines the number of Unicode
codepoints needed, remaps the first `n` keycodes to these codepoints and then
sends key-press and key-release events for these in sequence, using the
`Xtest` extension. Without the extension, only "synthetic" events can be sent,
and many X11 clients would just ignore them.

When the sequence is complete, it sleeps for a short time before restoring
the original keyboard mapping. Waiting is necessary because otherwise, the X11
client receiving the keyboard events could update its own mapping before it is
used to map the keyboard events still queued.

### The settings file

Settings are automatically stored in `~/.config/qxmoji.ini`. This file is also
watched for external changes which are applied automatically. This means for
example if you have multiple instances running and switch to single-instance
mode, all but one instance will immediately exit.

For watching the file, the system's notification mechanisms (e.g. `inotify` on
Linux, `kqueue` on FreeBSD) are used, so there's no cost attached, with one
exception: qXmoji detects whether the file is stored on NFS and in this case,
falls back to checking the modification time once per second. That's still
very little overhead, but if you're worried about that, just make sure the
file is stored on a local filesystem. Currently, there's no way to use a
different location, but just putting a symbolic link will work fine.

The emoji history is also stored in the settings file, as a simple "string of
emojis". There's a difference between Qt5 and Qt6 though, Qt5 keeps settings
entries in plain ASCII, therefore using escape sequences for the emojis, while
Qt6 uses UTF-8, so you will see actual emoji characters in the settings file.
Although Qt5 can be configured to use UTF-8 as well, it doesn't work reliably.

Therefore, switching from qXmoji using Qt6 to Qt5, you will lose your history.
The other way around works without issues, Qt6 can read the strings with
escape sequences written by Qt5 just fine.

## Limitations

* The default time (50 ms) waiting before restoring the original keyboard
  mapping is a wild guess that seems to work while testing. It could also be
  too short (e.g.  with a slower connection to the X server), so it's
  configurable in the range 0 - 500 ms.  There's unfortunately no way to know
  when a client processed all its keyboard events, so waiting is the only
  option.

* Modifier keys (like shift, alt, meta, ...) could interfer with the faked key
  press and release events. It might be possible to add code temporarily
  disabling other keyboards and setting modifiers to a known state, but this
  would add a lot of complexity. The simpler workaround is not to touch any
  other keyboards while clicking on an emoji in qXmoji.

* Many emojis consist of a sequence of Unicode codepoints. Whether they are
  displayed correctly as a single emoji depends on the X11 client receiving
  the fake keyboard events. While testing, `xterm` couldn't display any
  multi-codepoint emoji, while `chromium` got it right for most of them, but
  with a few exceptions.

## Building

To obtain the source from git, make sure to include submodules, e.g. with the
`--recurse-submodules` option to `git clone`. Release tarballs will include
everything needed for building.

Dependencies:

* C and C++ compilers understanding GNU commandline options and C11/C++17
  language standards (GNU GCC and LLVM clang work fine)
* GNU make
* Qt's `moc`, `rcc` and `lrelease` utilities
* Qt libs: `Core`, `Gui`, `Network` and `Widgets`, plus `X11Extras` for Qt5
* xcb libs: `libxcb-xtest`

To build qXmoji with Qt6, just type

    make

To build with Qt5, type

    make QT_VERSION=5

Typical make variables like `prefix` (defaults to `/usr/local`) and `DESTDIR`
(defaults to empty) are supported, as well as variables to override individual
tools like `CC`, `CXX`, `MOC`, etc.

There's also

    make install

to install qXmoji to the selected prefix and DESTDIR.

If your default `make` tool is not GNU make, the latter is probably installed
using the name `gmake` (e.g. on BSD systems), so type that instead of `make`.

### Building on Debian / Ubuntu Linux

Install the needed dependencies.

    sudo apt install qtbase5-dev libqt5x11extras5-dev libxcb-xtest0-dev -y
    sudo apt install build-essential -y

After that you can continue with the build instructions given above.  

This was tested on Debian 12 amd64 with GCC 12.2.0 (Debian 12.2.0-14).
