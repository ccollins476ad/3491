#include <assert.h>
#include "gen/glog.h"

void
vglog(struct glog_desc_t *entry, const char *fmt, va_list ap)
{
    if (entry->fp == NULL) {
        entry->fp = fopen(entry->filename, "w");
        assert(entry->fp != NULL);
    }

    vfprintf(entry->fp, fmt, ap);
    fflush(entry->fp);
}

void
glog(struct glog_desc_t *entry, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vglog(entry, fmt, ap);
    va_end(ap);
}


void
glog_if(int do_log, struct glog_desc_t *entry, const char *fmt, ...)
{
    va_list ap;

    if (do_log) {
        va_start(ap, fmt);
        vglog(entry, fmt, ap);
        va_end(ap);
    }
}

