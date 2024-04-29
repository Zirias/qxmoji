# QT_VERSION:	Qt major version, 6 or 5, defaults to 6

DISTCLEANDIRS=		tools/bin
SUBBUILD=		EMOJIGEN
EMOJIGEN_TARGET=	tools/bin/emojigen
EMOJIGEN_SRCDIR=	tools/emojigen
EMOJIGEN_MAKEARGS=	DESTDIR=../bin HOSTBUILD=1 STATIC=0 prefix= bindir=
EMOJIGEN_MAKEGOAL=	install
EMOJIGEN_CLEANGOAL=	distclean

USES=	fdofiles gen qt sub

include zimk/zimk.mk

$(call zinc,src/bin/qxmoji/qxmoji.mk)
