#include "life/zone.h"

int zone_beingx;
int zone_beingy;
int zone_being_dimx;
int zone_being_dimy;

int zone_bulletx;
int zone_bullety;
int zone_bullet_dimx;
int zone_bullet_dimy;

int
zone_contains_being(int beingx, int beingy)
{
    return beingx >= zone_beingx &&
           beingy >= zone_beingy &&
           beingx < zone_beingx + zone_being_dimx &&
           beingy < zone_beingy + zone_being_dimy;
}

int
zone_contains_bullet(int bulletx, int bullety)
{
    return bulletx >= zone_bulletx &&
           bullety >= zone_bullety &&
           bulletx < zone_bulletx + zone_bullet_dimx &&
           bullety < zone_bullety + zone_bullet_dimy;
}
