#include "guimain.h"

#include "qxmoji.h"
#include "xkeyinjector.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QX11Info>
#endif

int guimain(int argc, char **argv, void (*started)())
{
    QCoreApplication::setApplicationName("qXmoji");
    QCoreApplication::setApplicationVersion(QXMOJIVERSTR);

    QXmoji qxmoji(argc, argv);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (!QX11Info::isPlatformX11()) return 1;
    XKeyInjector_setConnection(QX11Info::connection());
#else
    auto x11app = qxmoji.nativeInterface<QNativeInterface::QX11Application>();
    if (!x11app) return 1;
    XKeyInjector_setConnection(x11app->connection());
#endif

    started();
    qxmoji.show();
    return qxmoji.exec();
}

