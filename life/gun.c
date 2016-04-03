#include <assert.h>
#include <inttypes.h>
#include "life/entity.h"
#include "life/gun.h"

/**
 * Tells you if the specified gun can be fired.  A gun an only be fired if:
 *    o enough time has passed since it was last fired.
 *
 * @param gun The gun to shoot.
 * @param now The current tick.
 *
 * @return 1 if the gun can be fired; else 0.
 */
int
gun_can_shoot(struct gun_t *gun, uint32_t now)
{
    if (gun->flags & GUN_F_TRIGGER_HELD &&
        !(gun->properties->flags & GUN_PROPERTIES_F_AUTO)) {

        return 0;
    }

    if (now - gun->prev_shoot_time < gun->properties->shoot_itvl) {
        return 0;
    }

    return 1;
}

/**
 * Attempts to shoot the specified gun.  A gun can only be fired if:
 *    o enough time has passed since it was last fired.
 *
 * If successfully fired, the gun's previous shoot time is updated.
 *
 * @param gun The gun to shoot.
 * @param now The current tick.
 *
 * @return 0 if successfully fired; else nonzero.
 */
int
gun_shoot(struct gun_t *gun, uint32_t now)
{
    int can_shoot;

    can_shoot = gun_can_shoot(gun, now);

    gun->flags |= GUN_F_TRIGGER_HELD;
    if (!can_shoot) {
        return -1;
    }

    gun->prev_shoot_time = now;

    return 0;
}

int
gun_avg_speed(struct gun_properties_t *gun_properties)
{
    return gun_properties->erupt.speed_base +
           gun_properties->erupt.speed_rand / 2;
}
