qxmoji_VERSION=		0.5
qxmoji_USES=		qt
qxmoji_USE_QT=		Core Gui Network Widgets
qxmoji_USE_QT5=		X11Extras
qxmoji_MODULES=		emojisearch \
			main \
			xcbadapter \
			xkeyinjector
qxmoji_CXXMODULES=	aboutdlg \
			emoji \
			flowlayout \
			guimain
qxmoji_MOCMODULES=	emojibutton \
			emojifont \
			emojihistory \
			searchfield \
			settingsdlg \
			singleinstance \
			qxmoji \
			qxmojiwin
qxmoji_QRC=		icon
qxmoji_CFLAGS=		-fPIC
qxmoji_CXXFLAGS=	-fPIC
qxmoji_PKGDEPS=		xcb \
			xcb-xtest
qxmoji_SUB_FILES=	decl.h \
			qxmoji.desktop
qxmoji_SUB_LIST=	bindir=$(bindir)
qxmoji_GEN=		EGEN
qxmoji_EGEN_FILES=	emojidata.cpp:contrib/emoji-test.txt
qxmoji_ICONSIZES=	16x16 32x32 48x48 256x256
qxmoji_DESKTOPFILE=	qxmoji
qxmoji_DOCS=		README.md

GEN_EGEN_tool=		$(EMOJIGEN_TARGET)
GEN_EGEN_args=		$2 >$1

$(call binrules,qxmoji)
