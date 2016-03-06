#ifndef H_LOG_ 
#define H_LOG_

#include <stdio.h>
#include <stdarg.h>

struct glog_desc_t {
    char *filename;
    FILE *fp;
};

void glog(struct glog_desc_t *entry, const char *fmt, ...);
void vglog(struct glog_desc_t *entry, const char *fmt, va_list ap);
void glog_if(int do_log, struct glog_desc_t *entry, const char *fmt,
             ...);

#endif

