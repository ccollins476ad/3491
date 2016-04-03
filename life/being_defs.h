#ifndef H_BEING_DEFS_
#define H_BEING_DEFS_

#include <inttypes.h>

#define BEING_F_TRASH                   0x01 /* Being is subject to erasure. */
#define BEING_F_PLAYER                  0x02 /* Being is player. */
#define BEING_F_DEATH_SCHED             0x04 /* Death fade has been sched. */
#define BEING_F_DOT                     0x08
#define BEING_F_PERIPH                  0x10
#define BEING_F_FRICTION                0x20
#define BEING_F_VISIBLE                 0x40
#define BEING_F_BLEED                   0x80
#define BEING_F_NO_HIT                  0x100 /* Being is immune to strikes. */
#define BEING_F_CRAWL                   0x200

#define BEING_F_PER_TICK                0
#define BEING_F_PER_FRAME               0

#define BEING_PURPOSE_NONE              (-1)
#define BEING_PURPOSE_TARGET            1
#define BEING_PURPOSE_MAX               2

#define BEING_TRAITS_FRAME_DUR          0
#define BEING_TRAITS_PERIPH_RADIUS      1
#define BEING_TRAITS_SPEED              2
#define BEING_TRAITS_ANGLE              3
#define BEING_TRAITS_BLEED_CHANCE       4
#define BEING_TRAITS_BLEED_DEC          5
#define BEING_TRAITS_CRAWL_DUR          6
#define BEING_TRAITS_CRAWL_END          7

#define BEING_TYPE_NONE                 0
#define BEING_TYPE_RALF                 1
#define BEING_TYPE_CIV1                 2
#define BEING_TYPE_MAX                  3

#define BEING_SPEED_SLOW                0
#define BEING_SPEED_FAST                1
#define BEING_SPEED_MAX                 2

#endif
