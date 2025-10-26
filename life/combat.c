#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include "gen/gen_dbg.h"
#include "gen/gen_num.h"
#include "gfx/balleg.h"
#include "gfx/gfx.h"
#include "life/being.h"
#include "life/blast.h"
#include "life/blood.h"
#include "life/bullet.h"
#include "life/bullet_defs.h"
#include "life/carnage.h"
#include "life/combat.h"
#include "life/data.h"
#include "life/hit_box.h"
#include "life/life_defs.h"
#include "life/object.h"
#include "life/terr.h"
#include "life/violence.h"
#include "life/world.h"

#define COMBAT_IGNORE_MAX   (10 * 1024)
#define PROXY_ATTACK_MAX    1024

#define COMBAT_CARNAGE_FADE_DELAY (2 * LIFE_FPS)

struct combat_ignore_entry_t {
    struct combat_ignore_entry_t *next;
    unsigned int id;
};

static struct {
    struct slab_t ignore_slab;
    struct slab_t proxy_attack_slab;
} combat_data;

/*****************************************************************************
 * @ignore                                                                   *
 *****************************************************************************/

int
combat_ignore(struct combat_ignore_entry_t **list, unsigned int id)
{
    struct combat_ignore_entry_t *entry;

    entry = slab_insert(&combat_data.ignore_slab);
    if (entry == NULL) {
        return -1;
    }

    entry->id = id;
    entry->next = *list;
    *list = entry;

    return 0;
}

static int
combat_ignoring(struct combat_ignore_entry_t *list, unsigned int id)
{
    struct combat_ignore_entry_t *entry;

    for (entry = list; entry != NULL; entry = entry->next) {
        if (entry->id == id) {
            return 1;
        }
    }

    return 0;
}

void
combat_free_ignore_list(struct combat_ignore_entry_t *list)
{
    struct combat_ignore_entry_t *entry;
    struct combat_ignore_entry_t *next;

    entry = list;
    while (entry != NULL) {
        next = entry->next;
        slab_erase_elem(&combat_data.ignore_slab, entry);
        entry = next;
    }
}

/*****************************************************************************
 * @misc                                                                     *
 *****************************************************************************/

int
combat_attack_damage(struct attack_t *attack,
                     struct defense_properties_t *defense)
{
    if (attack->flags & defense->damaged_by) {
        return attack->damage;
    } else {
        return 0;
    }
}

int
combat_is_strike(attack_flags_t attack_flags, defense_flags_t defense_flags)
{
    return !(defense_flags & DEFENSE_F_ETHEREAL) ||
           attack_flags & ATTACK_F_ETHEREAL;
}

static struct attack_t *
combat_copy_temp_attack(struct attack_t *src)
{
    static struct attack_t temp_attack;

    assert(src != &temp_attack);

    temp_attack = *src;

    return &temp_attack;
}

static void
combat_scale_attack(struct attack_t *attack, double scale)
{
    attack->damage *= scale;
    attack->stun *= scale;
    attack->reel *= scale;
}

/*****************************************************************************
 * @blood                                                                    *
 *****************************************************************************/

static void
combat_blood_spray(struct spray_t *spray, int x, int y, int angle)
{
    struct blood_t *blood;
    struct erupt_t erupt;
    int frame_duration;
    int num_blood;
    int i;

    erupt = spray->erupt;
    erupt.angle_base += angle;
    num_blood = spray->count_base + rand_up_to(spray->count_rand);
    for (i = 0; i < num_blood; ++i) {
        frame_duration = spray->frame_dur_base +
                         rand_up_to(spray->frame_dur_rand);
        blood = blood_add(spray->blood_type, frame_duration, x, y);
        if (blood == NULL) {
            break;
        }
        erupt_apply(&erupt, &blood->particle.phys);
    }
}

static void
combat_add_sprays(int x, int y, struct spray_t *sprays, int angle)
{
    struct spray_t *spray;

    for (spray = sprays;
         spray->blood_type != BLOOD_TYPE_NONE;
         spray++) {

        combat_blood_spray(spray, x, y, angle);
    }
}

struct combat_add_stain_star_arg {
    int x;
    int y;
    struct stain_star_info_t *info;
};

static int
combat_add_stain_star_cb(int x, int y, void *arg)
{
    struct combat_add_stain_star_arg *cb_arg;
    int tiledx;
    int tiledy;
    int frame_dur;

    cb_arg = arg;

    frame_dur = cb_arg->info->frame_dur_base +
                rand_up_to(cb_arg->info->frame_dur_rand);
    tiledx = cb_arg->x + x * WORLD_TILE_DIM;
    tiledy = cb_arg->y + y * WORLD_TILE_DIM;
    blood_stain_add(tiledx, tiledy, frame_dur);

    return 0;
}

static void
combat_add_stain_star(int x, int y, struct stain_star_info_t *info)
{
    struct combat_add_stain_star_arg arg;
    int angle;
    int len;
    int x2;
    int y2;
    int i;

    arg.x = x;
    arg.y = y;
    arg.info = info;

    //arg.frame_dur_rand = BLOOD_STAIN_SECOND / 50;

    for (i = 0; i < 4; i++) {
        angle = rand_up_to(359);
        len = info->length_base + rand_up_to(info->length_rand);
        angle_to_vect(angle, len, &x2, &y2);
        line_foreach(0, 0, x2, y2, combat_add_stain_star_cb, &arg);
    }
}

static int
combat_add_carnage_bits(struct carnage_info_t *info, struct being_t *being)
{
    struct carnage_t *carnages[4];
    struct carnage_t *carnage;
    struct icon_t icon;
    double rads;
    int num_carnages;
    int speed;
    int mass;
    int xoff;
    int yoff;
    int i;

    num_carnages = sizeof carnages / sizeof carnages[0];
    being_icon(being, &icon);
    xoff = rand_up_to(TILE_DIM - 4) + 2;
    yoff = rand_up_to(TILE_DIM - 4) + 2;

    carnage_add_bits(&icon, xoff, yoff, carnages, &num_carnages);
    if (num_carnages == 0) {
        return -1;
    }

    for (i = 0; i < num_carnages; i++) {
        carnage = carnages[i];
        carnage->phys.x += being->phys.x;
        carnage->phys.y += being->phys.y;

        rads = rads_normalize(carnage->rads_off +
                                    info->rads_off_base +
                                    rand_up_to(info->rads_off_rand));

        speed = info->speed_base + rand_up_to(info->speed_rand);

        angle_to_vect(rads, speed,
                      &carnage->phys.xspeed, &carnage->phys.yspeed);
        carnage->angle_speed = info->rotate_speed_base +
                               rand_up_to(info->rotate_speed_rand);
        mass = carnage_mass(carnage);
        carnage->phys.xspeed /= mass;
        carnage->phys.yspeed /= mass;
        carnage->angle_speed /= mass;
        carnage->expiry = world_tick + COMBAT_CARNAGE_FADE_DELAY;

        entity_add_signal(&being->entity, &carnage->entity,
                          ENTITY_SIGNAL_TRASH);
    }

    return 0;
}

static int
combat_add_carnage_slice(struct carnage_info_t *info, struct being_t *being,
                         int centerx, int centery, int attack_angle)
{
    struct carnage_t *carnages[4];
    struct carnage_t *carnage;
    struct icon_t icon;
    struct line_t line;
    double rads;
    int num_carnages;
    int speed;
    int mass;
    int i;

    /* Convert from world coordinates to pixel offset. */
    centerx = (centerx - being->phys.x) / WORLD_FACTOR;
    centery = (centery - being->phys.y) / WORLD_FACTOR;

    num_carnages = sizeof carnages / sizeof carnages[0];
    being_icon(being, &icon);
    line_from_point_angle(&line, centerx, centery, attack_angle);

    carnage_slice_add(&icon, &line, carnages, &num_carnages);
    if (num_carnages == 0) {
        return -1;
    }

    for (i = 0; i < num_carnages; i++) {
        carnage = carnages[i];
        carnage->phys.x += being->phys.x;
        carnage->phys.y += being->phys.y;

        rads = rads_normalize(carnage->rads_off +
                              info->rads_off_base +
                              rand_up_to(info->rads_off_rand));

        speed = info->speed_base + rand_up_to(info->speed_rand);

        angle_to_vect(rads, speed,
                      &carnage->phys.xspeed, &carnage->phys.yspeed);
        carnage->angle_speed = info->rotate_speed_base +
                               rand_up_to(info->rotate_speed_rand);
        mass = carnage_mass(carnage);
        carnage->phys.xspeed /= mass;
        carnage->phys.yspeed /= mass;
        carnage->angle_speed /= mass;
        carnage->expiry = world_tick + COMBAT_CARNAGE_FADE_DELAY;

        entity_add_signal(&being->entity, &carnage->entity,
                          ENTITY_SIGNAL_TRASH);
    }

    return 0;
}

static int
combat_add_carnage(struct being_t *being, struct carnage_info_t *carnage,
                   int centerx, int centery, int attack_angle)
{
    int rc;

    switch (carnage->type) {
    case CARNAGE_TYPE_BITS:
        rc = combat_add_carnage_bits(carnage, being);
        return rc;

    case CARNAGE_TYPE_SLICE:
        rc = combat_add_carnage_slice(carnage, being, centerx, centery,
                                      attack_angle);
        return rc;

    default:
        assert(0);
        return -1;
    }
}

static struct violence_t *
combat_select_violence_and_add_carnage(struct being_t *being,
                                       struct attack_t *attack,
                                       int centerx,
                                       int centery,
                                       int attack_angle)
{
    struct violence_t *violence;
    int rc;

    if (attack->violence_seq == NULL) {
        return NULL;
    }

    /* Select the most appropriate violence entry based on the particulars of
     * this strike.
     */
    violence = violence_select(attack->violence_seq, attack->damage,
                               being->hit_points);

    /* Starting with the selected entry, loop through the violence entries
     * until carnage processing is successful.  Carnage processing may fail if
     * the angle and position of the attack are unsuitable, for example.
     */
    while (1) {
        if (violence->carnage_info == NULL) {
            /* No carnage needed. */
            break;
        }

        rc = combat_add_carnage(being, violence->carnage_info,
                                centerx, centery,
                                attack_angle);
        if (rc == 0) {
            break;
        }

#if 0
        if (violence + 1 == NULL) {
            /* Too bad; no non-carnage violence entries.  Being will just
             * vanish.
             */
            break;
        }
#endif

        violence++;
    }

    return violence;
}

/*****************************************************************************
 * @unidirectional                                                           *
 *****************************************************************************/

/**
 * Returns  0 if the bullet is still alive.
 *          1 if the bullet should be trashed.
 */
int
combat_bullet_vs_wall(int scapex, int scapey, struct bullet_t *bullet)
{
    struct attack_t attack;
    int rc;

    attack = bullet->properties->attack;
    while (1) {
        rc = terr_apply_attack(scapex, scapey, &attack);
        if (rc == -1) {
            return 0;
        }

        attack.damage -= rc;
        if (attack.damage <= 0) {
            return 1;
        }
    }
}

static void
combat_apply_attack_to_being(struct attack_t *attack, double scale,
                             struct being_t *being,
                             int centerx, int centery, int attack_angle,
                             int *out_was_dead, int *out_is_dead)
{
    struct violence_t *violence;
    int being_centerx;
    int being_centery;
    int was_dead;
    int is_dead;

    if (scale != 1.0) {
        attack = combat_copy_temp_attack(attack);
        combat_scale_attack(attack, scale);
    }

    /* XXX: Check defense properties. */
    was_dead = being->hit_points <= 0;
    being->hit_points -= attack->damage;
    is_dead = being->hit_points <= 0;

    /* Hit stun. */
    being_traits_replace(being, BEING_TRAITS_FRAME_DUR, attack->stun);

    /* Reel. */
    being_traits_replace(being, BEING_TRAITS_ANGLE, attack_angle);
    being_traits_replace(being, BEING_TRAITS_SPEED, attack->reel);

    /* Crawl duration. */
    being_traits_replace(being, BEING_TRAITS_CRAWL_DUR, attack->stun * 2);

    /* Violence. */
    violence = combat_select_violence_and_add_carnage(being, attack,
                                                      centerx, centery,
                                                      attack_angle);
    if (violence != NULL) {
        if (scale != 1.0) {
            violence = violence_temp_copy(violence);
            violence_scale(violence, scale);
        }
        if (violence->sprays != NULL) {
            combat_add_sprays(centerx, centery, violence->sprays,
                              attack_angle);
        }
        if (violence->star_info != NULL) {
            being_center(being, &being_centerx, &being_centery);
            combat_add_stain_star(being_centerx, being_centery,
                                  violence->star_info);
        }
        being_traits_replace(being, BEING_TRAITS_BLEED_CHANCE,
                             violence->bleed_chance);
        being_traits_replace(being, BEING_TRAITS_BLEED_DEC,
                             violence->bleed_dec);

        /* Tell the victim he is being hit. */
        fsm_ctxt_push_signal(&being->fsm_ctxt, violence->signal);
        //being_ai_sig_strike(being);

        /* HACK: don't allow this being to get hit again, since it no longer
         * exists.  This is necessary because the being doesn't get trashed
         * until it processes the signal from this attack.  We don't want
         * other simultaneous strikes generating additional carnage.
         */
        if (violence->signal == FSM_SIG_ANNIHILATE) {
            being->flags |= BEING_F_NO_HIT;
        }
    }

    /* Write output parameters. */
    if (out_was_dead != NULL) {
        *out_was_dead = was_dead;
    }
    if (out_is_dead != NULL) {
        *out_is_dead = is_dead;
    }
}

/**
 * @return              0 if the attack was successfully applied;
 *                      nonzero if the attack was ignored (output parameters
 *                          should be disregarded).
 */
static int
combat_apply_attack_to_object(struct attack_t *attack, double scale,
                              struct object_t *object,
                              int *out_was_dead, int *out_is_dead)
{
    struct defense_properties_t *defense;
    int was_dead;
    int is_dead;
    int damage;

    defense = &object->properties->defense;

    /* Don't do anything if the bullet should pass through this object. */
    if (!(combat_is_strike(attack->flags, defense->flags))) {
        return -1;
    }

    damage = combat_attack_damage(attack, defense);

    if (scale != 1.0) {
        attack = combat_copy_temp_attack(attack);
        combat_scale_attack(attack, scale);
    }

    was_dead = object->hit_points <= 0;
    object->hit_points -= damage;
    is_dead = object->hit_points <= 0;

    /* Write output parameters. */
    if (out_was_dead != NULL) {
        *out_was_dead = was_dead;
    }
    if (out_is_dead != NULL) {
        *out_is_dead = is_dead;
    }

    return 0;
}

static double
combat_calc_scale(struct attack_scale_info_t *scale_info, int dist)
{
    struct attack_scale_thresh_t *thresh;
    double scale;
    int i;

    scale = 1.0;
    if (scale_info->threshes != NULL) {
        for (i = 0; ; i++) {
            thresh = scale_info->threshes + i;
            if (dist <= thresh->max_distance) {
                scale = thresh->scale;
                break;
            }
        }
    }

    scale -= dist * scale_info->minus_per_dist;

    if (scale < 0.0) {
        scale = 0.0;
    }

    return scale;
}

static double
combat_bullet_calc_scale(struct bullet_t *bullet)
{
    struct attack_scale_info_t *scale_info;
    int bullet_age;
    int dist;

    scale_info = bullet->properties->scale_info;
    if (scale_info == NULL) {
        return 1.0;
    }

    bullet_age = world_tick - bullet->birth_tick;
    dist = bullet_age * (abs(bullet->phys.xspeed) + abs(bullet->phys.yspeed));

    return combat_calc_scale(scale_info, dist);
}

static void
combat_bullet_vs_being(struct hit_box_t *ahb, struct hit_box_t *dhb)
{
    struct defense_properties_t *defense;
    struct attack_t *attack;
    struct bullet_t *bullet;
    struct being_t *being;
    struct point_t center;
    double scale;
    int bullet_angle;
    int survives;
    int was_dead;
    int is_dead;
    int rc;

    bullet = entity_to_bullet(ahb->owner);
    being = entity_to_being(dhb->owner);
    attack = &bullet->properties->attack;
    defense = &being->properties->defense;

    /* If the bullet or being got trashed during this frame, ignore the
     * strike.
     */
    if (bullet->flags & BULLET_F_TRASH || being->flags & BEING_F_TRASH ||
        being->flags & BEING_F_NO_HIT) {
        return;
    }

    /* Don't do anything if the bullet should pass through this being. */
    if (!(combat_is_strike(attack->flags, defense->flags))) {
        return;
    }

    /* Don't let a bullet hit the same target twice. */
    if (combat_ignoring(bullet->ignore_list, being->entity.id)) {
        return;
    }

    /* No friendly fire. */
    if (bullet->team_id == being->team_id) {
        return;
    }

    /* Calculate attack's physical properties. */
    bullet_angle = angle_from_vect(bullet->phys.xspeed, bullet->phys.yspeed);
    rc = shape_intersection_center(&ahb->shape, &dhb->shape, &center);
    assert(rc == 0);

    scale = combat_bullet_calc_scale(bullet);

    /* Process the strike. */
    combat_apply_attack_to_being(attack, scale, being, center.x, center.y,
                                 bullet_angle, &was_dead, &is_dead);

    /* Trash the bullet if it doesn't survive this strike.  Make it ignore this
     * target in the future if it does survive.
     */
    survives = bullet_survives_strike(bullet->properties->survive_type,
                                      was_dead, is_dead);
    if (!survives) {
        bullet_impact(bullet, center.x, center.y);
    } else {
        combat_ignore(&bullet->ignore_list, being->entity.id);
    }
}

static void
combat_bullet_vs_object(struct hit_box_t *ahb, struct hit_box_t *dhb)
{
    struct attack_t *attack;
    struct bullet_t *bullet;
    struct object_t *object;
    struct point_t center;
    double scale;
    int survives;
    int was_dead;
    int is_dead;
    int rc;

    bullet = entity_to_bullet(ahb->owner);
    object = entity_to_object(dhb->owner);
    attack = &bullet->properties->attack;

    /* If the bullet or object got trashed during this frame, ignore the
     * strike.
     */
    if (bullet->flags & BULLET_F_TRASH || object->flags & OBJECT_F_TRASH) {
        return;
    }

    /* Don't let a bullet hit the same target twice. */
    if (combat_ignoring(bullet->ignore_list, object->entity.id)) {
        return;
    }

    /* Process the strike. */
    scale = combat_bullet_calc_scale(bullet);
    rc = combat_apply_attack_to_object(attack, scale, object,
                                       &was_dead, &is_dead);
    if (rc != 0) {
        /* Attack not applied. */
        return;
    }

    if (is_dead) {
        object_die(object);
    }

    /* Calculate attack's physical properties. */
    rc = shape_intersection_center(&ahb->shape, &dhb->shape, &center);
    assert(rc == 0);

    /* Trash the bullet if it doesn't survive this strike.  Make it ignore this
     * target in the future if it does survive.
     */
    survives = bullet_survives_strike(bullet->properties->survive_type,
                                      was_dead, is_dead);
    if (!survives) {
        bullet_impact(bullet, center.x, center.y);
    } else {
        combat_ignore(&bullet->ignore_list, object->entity.id);
    }
}

/*****************************************************************************
 * @proxy                                                                    *
 *****************************************************************************/

static void
combat_proxy_attack_vs_being(struct hit_box_t *atk, struct hit_box_t *def)
{
    struct proxy_attack_properties_t *properties;
    struct proxy_attack_t *proxy_attack;
    struct attack_t *attack;
    struct being_t *being;
    double scale;
    int centerx;
    int centery;
    int angle;
    int dist;

    assert(atk->shape.type == SHAPE_TYPE_CIRCLE);
    assert(def->shape.type == SHAPE_TYPE_RECTANGLE);

    proxy_attack = entity_to_proxy_attack(atk->owner);
    properties = proxy_attack->properties;
    attack = &properties->attack;
    being = entity_to_being(def->owner);

    being_center(being, &centerx, &centery);

    dist = distance(proxy_attack->x, proxy_attack->y, centerx, centery);
    scale = combat_calc_scale(&properties->scale_info, dist);
    angle = angle_from_vect(centerx - proxy_attack->x,
                            centery - proxy_attack->y);
    combat_apply_attack_to_being(attack, scale, being, centerx, centery,
                                 angle, NULL, NULL);
}

static void
combat_proxy_attack_vs_object(struct hit_box_t *atk, struct hit_box_t *def)
{
    struct proxy_attack_properties_t *properties;
    struct defense_properties_t *defense;
    struct proxy_attack_t *proxy_attack;
    struct attack_t *attack;
    struct object_t *object;
    double scale;
    int was_dead;
    int is_dead;
    int centerx;
    int centery;
    int dist;
    int rc;

    assert(atk->shape.type == SHAPE_TYPE_CIRCLE);
    assert(def->shape.type == SHAPE_TYPE_RECTANGLE);

    proxy_attack = entity_to_proxy_attack(atk->owner);
    properties = proxy_attack->properties;
    attack = &properties->attack;
    object = entity_to_object(def->owner);
    defense = &object->properties->defense;

    /* Don't do anything if the attack should pass through this object. */
    if (!(combat_is_strike(attack->flags, defense->flags))) {
        return;
    }

    object_center(object, &centerx, &centery);

    /* Process the strike. */
    dist = distance(proxy_attack->x, proxy_attack->y, centerx, centery);
    scale = combat_calc_scale(&properties->scale_info, dist);
    rc = combat_apply_attack_to_object(attack, scale, object,
                                       &was_dead, &is_dead);
    if (rc == 0 && is_dead) {
        object_die(object);
    }

    /* XXX: Ignore list. */
}

struct combat_proxy_attack_terr_cb_arg {
    int scapex;
    int scapey;
    struct proxy_attack_t *prox;
    struct hit_box_t *hit_box;
};

static int
combat_proxy_attack_terr_cb(int x, int y, void *arg)
{
    struct combat_proxy_attack_terr_cb_arg *cb_arg;
    struct proxy_attack_properties_t *properties;
    struct attack_t *attack;
    double scale;
    int dist;
    int rc;

    cb_arg = arg;
    properties = cb_arg->prox->properties;

    dist = distance(x * WORLD_TILE_DIM, y * WORLD_TILE_DIM, 0, 0);
    scale = combat_calc_scale(&properties->scale_info, dist);

    attack = combat_copy_temp_attack(&properties->attack);
    combat_scale_attack(attack, scale);

    while (1) {
        rc = terr_apply_attack(cb_arg->scapex + x, cb_arg->scapey + y, attack);
        if (rc == -1) {
            break;
        }

        attack->damage -= rc;
        if (attack->damage <= 0) {
            break;
        }
    }

    return 0;
}

static void
combat_proxy_attack_vs_terr(struct proxy_attack_t *proxy_attack,
                            struct hit_box_t *ahb)
{
    struct combat_proxy_attack_terr_cb_arg terr_arg;

    assert(ahb->shape.type == SHAPE_TYPE_CIRCLE);

    terr_arg.scapex = proxy_attack->x / WORLD_TILE_DIM;
    terr_arg.scapey = proxy_attack->y / WORLD_TILE_DIM;
    terr_arg.prox = proxy_attack;
    terr_arg.hit_box = ahb;

    circlefill_foreach(0, 0, ahb->shape.circle.radius / WORLD_TILE_DIM,
                  combat_proxy_attack_terr_cb, &terr_arg);
}

struct proxy_attack_t *
combat_proxy_attack_add(void)
{
    struct proxy_attack_t *proxy_attack;

    proxy_attack = slab_insert(&combat_data.proxy_attack_slab);
    if (proxy_attack != NULL) {
        memset(proxy_attack, 0, sizeof *proxy_attack);
        proxy_attack->entity.id = entity_next_id();
        proxy_attack->entity.type = ENTITY_TYPE_PROXY_ATTACK;
    }

    return proxy_attack;
}

static void
combat_proxy_attack_erase(struct proxy_attack_t *proxy_attack)
{
    entity_trashed(&proxy_attack->entity);
    slab_erase_elem(&combat_data.proxy_attack_slab, proxy_attack);
}

static void
combat_proxy_attack_update(struct proxy_attack_t *proxy_attack)
{
    struct hit_box_t *hit_box_perm;
    struct hit_box_t hit_box;

    if (proxy_attack->age >= proxy_attack->properties->duration) {
        proxy_attack->flags |= PROXY_ATTACK_F_TRASH;
    }

    if (proxy_attack->flags & PROXY_ATTACK_F_TRASH) {
        combat_proxy_attack_erase(proxy_attack);
        return;
    }

    proxy_attack->age++;

    hit_box_from_desc(&hit_box, &proxy_attack->properties->hit_box,
                      proxy_attack->x, proxy_attack->y);
    hit_box.owner = &proxy_attack->entity;

    hit_box_perm = hit_box_add(&hit_box);
    if (hit_box_perm != NULL) {
        combat_proxy_attack_vs_terr(proxy_attack, hit_box_perm);
    }
}

static void
combat_proxy_attack_update_all(void)
{
    struct proxy_attack_t *proxy_attack;

    SLAB_FOREACH(&combat_data.proxy_attack_slab, proxy_attack) {
        combat_proxy_attack_update(proxy_attack);
    }
}

void
combat_process_hit_boxes(void)
{
    struct hit_box_intersection_t *intersection;
    int i;

    hit_box_process_atk();
    for (i = 0; i < hit_box_intersections.size; ++i) {
        intersection = pslab_at(&hit_box_intersections, i);
        combat_process_attack(intersection->a, intersection->b);
    }
}

void
combat_shoot(struct being_t *being)
{
    struct gun_properties_t *properties;
    struct bullet_t *bullet;
    struct erupt_t erupt;
    struct gun_t *gun;
    int num_bullets;
    int bulletcx;
    int bulletcy;
    int bulletx;
    int bullety;
    int rc;
    int i;

    gun = &being->gun;
    properties = gun->properties;
    assert(properties != NULL);

    rc = gun_shoot(gun, world_tick);
    if (rc != 0) {
        return;
    }

    /* Center bullets; assume first hit box is centered. */
    bullet_center_offset(properties->bullet_properties, &bulletcx, &bulletcy);
    bulletx = being->phys.x + WORLD_TILE_DIM / 2 - bulletcx;
    bullety = being->phys.y + WORLD_TILE_DIM / 2 - bulletcy;

    erupt = properties->erupt;
    erupt.angle_base += being->angle;
    num_bullets = properties->count_base + rand_up_to(properties->count_rand);
    for (i = 0; i < num_bullets; ++i) {
        bullet = bullet_add(properties->bullet_properties);
        if (bullet == NULL) {
            return;
        }

        bullet->owner_id = being->entity.id;
        bullet->team_id = being->team_id;
        bullet->phys.x = bulletx;
        bullet->phys.y = bullety;
        erupt_apply(&erupt, &bullet->phys);

        if (bullet->properties->flags & BULLET_PROPERTIES_F_IGNORE_OWNER) {
            combat_ignore(&bullet->ignore_list, being->entity.id);
        }
    }
}

struct combat_process_matrix_elem_t {
    int atk_type;
    int def_type;
    void (*cb)(struct hit_box_t *atk, struct hit_box_t *def);
};

static struct combat_process_matrix_elem_t combat_process_matrix[] = { {
    .atk_type = ENTITY_TYPE_BULLET,
    .def_type = ENTITY_TYPE_BEING,
    .cb = combat_bullet_vs_being,
}, {
    .atk_type = ENTITY_TYPE_BULLET,
    .def_type = ENTITY_TYPE_OBJECT,
    .cb = combat_bullet_vs_object,
}, {
    .atk_type = ENTITY_TYPE_PROXY_ATTACK,
    .def_type = ENTITY_TYPE_BEING,
    .cb = combat_proxy_attack_vs_being,
}, {
    .atk_type = ENTITY_TYPE_PROXY_ATTACK,
    .def_type = ENTITY_TYPE_OBJECT,
    .cb = combat_proxy_attack_vs_object,
} };
#define COMBAT_PROCESS_MATRIX_SZ \
    (sizeof combat_process_matrix / sizeof combat_process_matrix[0])

void
combat_process_attack(struct hit_box_t *atk, struct hit_box_t *def)
{
    struct combat_process_matrix_elem_t *elem;
    int i;

    for (i = 0; i < COMBAT_PROCESS_MATRIX_SZ; i++) {
        elem = combat_process_matrix + i;
        if (elem->atk_type == atk->owner->type &&
            elem->def_type == def->owner->type) {

            elem->cb(atk, def);
            return;
        }
    }

    assert(0);
}

void
combat_update(void)
{
    combat_proxy_attack_update_all();
}

void
combat_init(void)
{
    int rc;

    slab_destroy(&combat_data.ignore_slab);
    rc = slab_create(&combat_data.ignore_slab,
                     sizeof (struct combat_ignore_entry_t),
                     COMBAT_IGNORE_MAX);
    assert(rc == 0);

    slab_destroy(&combat_data.proxy_attack_slab);
    rc = slab_create(&combat_data.proxy_attack_slab,
                     sizeof (struct proxy_attack_t),
                     PROXY_ATTACK_MAX);
    assert(rc == 0);
}

