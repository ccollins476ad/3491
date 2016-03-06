#include <assert.h>
#include "gen/glog.h"
#include "gen/gen_dbg.h"

void
gen_dbg_log(const char *fmt, ...)
{
    static struct glog_desc_t desc = {
        .filename = "gen_dbg.txt",
    };

    va_list ap;

    va_start(ap, fmt);
    vglog(&desc, fmt, ap);
    va_end(ap);
}

