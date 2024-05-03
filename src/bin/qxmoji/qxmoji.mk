qxmoji_VERSION=		0.7
qxmoji_MODULES=		emojisearch \
			main \
			nfsdetect \
			xcbadapter \
			xkeyinjector
qxmoji_CXXMODULES=	aboutdlg \
			emoji \
			flowlayout \
			guimain \
			settingsdlg
qxmoji_MOCMODULES=	emojibutton \
			emojifont \
			emojihistory \
			searchfield \
			singleinstance \
			qxmoji \
			qxmojisettings \
			qxmojiwin
qxmoji_USE_QT=		Gui Network Widgets
qxmoji_USE_QT5=		X11Extras
qxmoji_QRC=		icon
qxmoji_MOCMODE=		included
qxmoji_QT_LANGUAGES=	de \
			fr
qxmoji_QT_TRANSLATIONS=	qxmoji emojidata
qxmoji_translationsdir=	$(qxmoji_datadir)$(PSEP)translations
qxmoji_PKGDEPS=		xcb \
			xcb-xtest
qxmoji_DEFINES=		-DTRANSLATIONSDIR=\"$(qxmoji_translationsdir)\"
qxmoji_SUB_FILES=	decl.h \
			qxmoji.desktop
qxmoji_SUB_LIST=	bindir=$(bindir)
qxmoji_GEN=		EGEN
qxmoji_EGEN_FILES=	emojidata.cpp:contrib
qxmoji_ICONSIZES=	16x16 32x32 48x48 256x256
qxmoji_DESKTOPFILE=	qxmoji
qxmoji_DOCS=		README.md \
			TRANSLATE.md

qxmoji_GENTS=		$(foreach l,$(qxmoji_QT_LANGUAGES),\
			$(qxmoji_SRCDIR)$(PSEP)emojidata_$l.ts)

GEN_EGEN_tool=		$(EMOJIGEN_TARGET)
GEN_EGEN_args=		$2 $(qxmoji_SRCDIR)$(PSEP)emojidata \
			$(qxmoji_QT_LANGUAGES) >$1

$(call binrules,qxmoji)

$(qxmoji_GENTS): $(qxmoji_SRCDIR)$(PSEP)emojidata.cpp
CLEAN += $(qxmoji_GENTS)
