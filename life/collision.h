#ifndef COLLISION_H_
#define COLLISION_H_

struct hit_box_t;
struct terr_properties_t;

#define COLLISION_TYPE_NONE             (-1)
#define COLLISION_TYPE_IMPACT           0
#define COLLISION_TYPE_SLIP             1
#define COLLISION_TYPE_MAX              2

int collision_terr_solid_box(struct hit_box_t *hit_box, int *x1, int *y1,
                             int *width, int *height, int all_solids);
int collision_terr_escape(struct hit_box_t *hit_box, uint8_t allowed_dirs,
                          int *out_deltax, int *out_deltay);
int collision_terr_hit_box_once(struct hit_box_t *hit_box,
                                int *diffx, int *diffy);

int collision_terr_impact(int xspeed, int yspeed, int diffx, int diffy);

#endif

