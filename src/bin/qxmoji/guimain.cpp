#include "guimain.h"

#include "qxmoji.h"

#include <QLocale>
#include <QTranslator>

int guimain(int argc, char **argv, void (*started)())
{
    QCoreApplication::setApplicationName("qXmoji");
    QCoreApplication::setApplicationVersion(QXMOJIVERSTR);

    QXmoji qxmoji(argc, argv, started);
    return qxmoji.exec();
}

