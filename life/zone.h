#ifndef ZONE_H_
#define ZONE_H_

extern int zone_beingx;
extern int zone_beingy;
extern int zone_being_dimx;
extern int zone_being_dimy;

extern int zone_bulletx;
extern int zone_bullety;
extern int zone_bullet_dimx;
extern int zone_bullet_dimy;

int
zone_contains_being(int beingx, int beingy);

int
zone_contains_bullet(int bulletx, int bullety);

#endif
