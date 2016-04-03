#ifndef H_BEING_DBG_
#define H_BEING_DBG_

#include "gen/glog.h"

extern struct glog_desc_t being_dbg_log_desc_fsm;
extern struct glog_desc_t being_dbg_log_desc_ai;

#define BEING_DBG_LOG_FSM(beingp, fmt, args...)                     \
    glog_if((beingp)->log_ctxt.fsm, &being_dbg_log_desc_fsm,        \
            (fmt), args)

#define BEING_DBG_LOG_AI(beingp, fmt, args...)                      \
    glog_if((beingp)->log_ctxt.ai, &being_dbg_log_desc_ai,          \
            (fmt), args)

#endif

