#ifndef H_CARNAGE_
#define H_CARNAGE_

#include "life/phys.h"
#include "gfx/fader.h"
#include "life/entity.h"
struct icon_t;
struct line_t;

#define CARNAGE_TYPE_NONE       (-1)
#define CARNAGE_TYPE_BITS       0
#define CARNAGE_TYPE_SLICE      1
#define CARNAGE_TYPE_MAX        2

struct carnage_info_t {
    int type;
    double rads_off_base;
    double rads_off_rand;
    int speed_base;
    int speed_rand;
    int rotate_speed_base;
    int rotate_speed_rand;
};

struct carnage_t {
    struct entity_t entity;
    struct phys_t phys;
    struct fader_t fader;
    double angle;
    double rads_off;
    int angle_speed;
    int expiry;

    struct BITMAP *bmp;
};

void carnage_draw_all(void);
void carnage_update_all(void);
void carnage_clear(void);
void carnage_signal(struct carnage_t *carnage, struct entity_t *sender,
                    uint8_t signal);
void carnage_add_bits(struct icon_t *icon, int x, int y,
                      struct carnage_t **dst, int *dst_len);
void carnage_slice_add(struct icon_t *icon, struct line_t *line,
                       struct carnage_t **dst, int *dst_len);
int carnage_mass(struct carnage_t *carnage);
void carnage_init(void);

#endif

