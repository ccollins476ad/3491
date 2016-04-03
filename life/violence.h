#ifndef H_VIOLENCE_
#define H_VIOLENCE_

struct stain_star_info_t;
struct carnage_info_t;
struct spray_t;

#define VIOLENCE_SEQ_CAPACITY   16

struct violence_t {
    int min_damage;
    int max_hp;
    int chance;

    int signal;
    struct spray_t *sprays;
    int bleed_chance;
    int bleed_dec;
    struct carnage_info_t *carnage_info;
    struct stain_star_info_t *star_info;
};

struct violence_seq_t {
    struct violence_t *entries;
};

struct violence_t *violence_temp_copy(struct violence_t *src);
void violence_scale(struct violence_t *violence, double scale);
struct violence_t *violence_select(struct violence_seq_t *seq, int damage,
                                   int hp);

#define VIOLENCE_SPRAYS_FOREACH(violence, spray)        \
    if ((violence)->sprays != NULL)                     \
        for ((spray) = (violence)->sprays;              \
             (spray)->blood_type != -1;                 \
             (spray)++)

#define VIOLENCE_TYPE_PISTOL            0
#define VIOLENCE_TYPE_RIFLE             1
#define VIOLENCE_TYPE_SHOTGUN           2
#define VIOLENCE_TYPE_HUNTING_RIFLE     3
#define VIOLENCE_TYPE_ROCKET_PROX       4
#define VIOLENCE_TYPE_BLADE             5
#define VIOLENCE_TYPE_MAX               6

#endif

