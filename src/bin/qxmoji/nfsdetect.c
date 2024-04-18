#include "nfsdetect.h"

#undef statnfs

#ifdef __linux__
#  include <linux/magic.h>
#  include <sys/vfs.h>
#  define statnfs(s) ((s).f_type == NFS_SUPER_MAGIC)
#else
#  include <sys/param.h>
#  ifdef BSD4_4
#    include <string.h>
#    include <sys/mount.h>
#    define statnfs(s) (!strcmp("nfs", (s).f_fstypename))
#  endif
#endif

SOLOCAL int isNfs(const char *path)
{
#ifdef statnfs
    struct statfs s;
    if (statfs(path, &s) < 0) return 0;
    return statnfs(s);
#else
    (void)path;
    return 0;
#endif
}

