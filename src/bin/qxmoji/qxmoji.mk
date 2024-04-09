qxmoji_VERSION=		0.1
qxmoji_MODULES=		emoji \
			xkeyinjector
qxmoji_CXXMODULES=	flowlayout \
			main \
			$(qxmoji_PREPROCMODULES)
qxmoji_PREPROCMODULES=	emojibutton \
			emojifont \
			settingsdlg \
			qxmoji \
			qxmojiwin
qxmoji_PREPROC=		MOC
qxmoji_PREPROCFLAGS=	-p.
qxmoji_QRC=		icon
qxmoji_INCLUDES=	-I$(qxmoji_PPSRCDIR)
qxmoji_CXXFLAGS=	-fPIC
qxmoji_PKGDEPS=		xcb \
			xcb-xtest
qxmoji_SUB_FILES=	decl.h \
			qxmoji.desktop
qxmoji_SUB_LIST=	bindir=$(bindir)
qxmoji_GEN=		E2C
qxmoji_E2C_FILES=	emoji.c:contrib/emoji-test.txt
qxmoji_ICONSIZES=	16x16 32x32 48x48 256x256
qxmoji_DESKTOPFILE=	qxmoji
qxmoji_DOCS=		README.md

ifeq ($(QT_VERSION),5)
qxmoji_PKGDEPS+=	Qt5Core \
			Qt5Gui \
			Qt5Widgets \
			Qt5X11Extras
else
qxmoji_PKGDEPS+=	Qt6Core \
			Qt6Gui \
			Qt6Widgets
endif

GEN_E2C_tool=		$(HOSTSH) scripts/emoji2cdef.sh

$(call binrules,qxmoji)
