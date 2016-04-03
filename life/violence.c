#include <assert.h>
#include <stddef.h>
#include "gen/gen_num.h"
#include "life/blood.h"
#include "life/violence.h"

struct violence_t *
violence_temp_copy(struct violence_t *src)
{
    static struct violence_t temp_violence;
    static struct spray_t temp_sprays[256];
    int i;

    assert(src != &temp_violence);

    temp_violence = *src;
    if (src->sprays != NULL) {
        temp_violence.sprays = temp_sprays;

        i = 0;
        while (1) {
            assert(i < sizeof temp_sprays / sizeof temp_sprays[0]);
            temp_sprays[i] = src->sprays[i];
            if (src->sprays[i].blood_type == BLOOD_TYPE_NONE) {
                break;
            }
            i++;
        }
    }

    return &temp_violence;
}

void
violence_scale(struct violence_t *violence, double scale)
{
    struct spray_t *spray;

    violence->bleed_chance *= scale;

    VIOLENCE_SPRAYS_FOREACH(violence, spray) {
        spray->erupt.speed_base *= scale;
        spray->erupt.speed_rand *= scale;
        spray->count_base *= scale;
        spray->count_rand *= scale;
    }

    /* XXX: Stars. */
    /* XXX: Carnage? */
}

struct violence_t *
violence_select(struct violence_seq_t *seq, int damage, int hp)
{
    struct violence_t *entry;
    int i;

    for (i = 0; ; i++) {
        entry = seq->entries + i;

        if (damage < entry->min_damage) {
            continue;
        }

        if (hp > entry->max_hp) {
            continue;
        }

        if (entry->chance != 100 && rand_up_to(100) >= entry->chance) {
            continue;
        }

        return entry;
    }
}

