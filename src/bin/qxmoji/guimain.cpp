#include "guimain.h"

#include "qxmoji.h"

int guimain(int argc, char **argv, void (*started)())
{
    QCoreApplication::setApplicationName("qXmoji");
    QCoreApplication::setApplicationVersion(QXMOJIVERSTR);

    QXmoji qxmoji(argc, argv);
    started();
    if (!qxmoji.startOk()) return 0;
    qxmoji.show();
    return qxmoji.exec();
}

