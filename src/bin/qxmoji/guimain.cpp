#include "guimain.h"

#include "qxmoji.h"

#include <QLocale>
#include <QTranslator>

int guimain(int argc, char **argv, void (*started)())
{
    QCoreApplication::setApplicationName("qXmoji");
    QCoreApplication::setApplicationVersion(QXMOJIVERSTR);

    QXmoji qxmoji(argc, argv, started);

    QTranslator emojitranslator;
    if (emojitranslator.load(QLocale(), "emojidata", "_", TRANSLATIONSDIR))
    {
	QCoreApplication::installTranslator(&emojitranslator);
    }

    return qxmoji.exec();
}

