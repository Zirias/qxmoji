# QT_VERSION:	Qt major version, 6 or 5, defaults to 6

SINGLECONFVARS=		QT_VERSION
DEFAULT_CXXFLAGS=	-std=c++17 -Wall -Wextra -pedantic

include zimk/zimk.mk

QT_VERSION:=	$(or $(filter 5,$(QT_VERSION)),6)

$(call zinc,src/bin/qxmoji/qxmoji.mk)
