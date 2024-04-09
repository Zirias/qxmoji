qxmoji_VERSION=		0.1
qxmoji_MODULES=		emoji \
			xkeyinjector
qxmoji_CXXMODULES=	flowlayout \
			main \
			$(qxmoji_PREPROCMODULES)
qxmoji_PREPROCMODULES=	emojibutton \
			qxmoji \
			qxmojiwin
qxmoji_PREPROC=		MOC
qxmoji_PREPROCFLAGS=	-p.
qxmoji_INCLUDES=	-I$(qxmoji_PPSRCDIR)
qxmoji_CXXFLAGS=	-fPIC
qxmoji_PKGDEPS=		Qt5Core \
			Qt5Gui \
			Qt5Widgets \
			Qt5X11Extras \
			xcb \
			xcb-xtest
qxmoji_SUB_FILES=	decl.h
qxmoji_GEN=		E2C
qxmoji_E2C_FILES:=	emoji.c:../../../contrib/emoji-test.txt

GEN_E2C_tool=		$(HOSTSH)
GEN_E2C_args=		scripts/emoji2cdef.sh >$1 <$2

$(call binrules,qxmoji)
