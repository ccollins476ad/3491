#ifndef BLOOD_H_
#define BLOOD_H_

#include "life/phys.h"
#include "gen/ticker.h"
#include "gfx/animation.h"
#include "life/debris.h"

#define BLOOD_TYPE_NONE                 (-1)
#define BLOOD_TYPE_DROP                 0
#define BLOOD_TYPE_MAX                  1

#define BLOOD_STAIN_TICK                256
#define BLOOD_STAIN_SECOND              (BLOOD_STAIN_TICK * LIFE_FPS)
    
struct blood_properties_t {
    struct particle_properties_t particle_properties;
};

struct blood_t {
    struct blood_properties_t *properties;
    struct particle_t particle;
};

struct spray_t {
    struct erupt_t erupt;
    int blood_type;
    int frame_dur_base;
    int frame_dur_rand;
    int count_base;
    int count_rand;
};

struct stain_t {
    int x;
    int y;
    int alpha;
    struct ticker_t ticker;
};

struct stain_star_info_t {
    int length_base;
    int length_rand;
    int angle_base;
    int angle_rand;
    int count_base;
    int count_rand;
    int frame_dur_base;
    int frame_dur_rand;
};

struct blood_t *blood_add(int type, int frame_duration, int x, int y);
struct stain_t *blood_stain_add(int x, int y, int frame_duration);
void blood_update_all(void);
void blood_draw_all_top(void);
void blood_draw_all_bottom(void);
void blood_clear(void);
void blood_init(void);

#endif

