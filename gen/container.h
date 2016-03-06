#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <stddef.h>
#include <limits.h>

#define BIT_MAP_BUFSZ(num_bits) (((num_bits) + CHAR_BIT - 1) / CHAR_BIT)

/* @bit_map */
struct bit_map_t {
    unsigned char *bits;
    int num_bytes;
    int num_bits;
};

int bit_map_get(struct bit_map_t *bit_map, int idx);
void bit_map_set(struct bit_map_t *bit_map, int idx, int val);
void bit_map_bit(int idx, int *byte, unsigned char *mask);
int bit_map_idx(int byte, int bit);
int bit_map_bytes_required(int num_bits);
void bit_map_clear(struct bit_map_t *bit_map);

/* @vect */
struct vect_t {
    unsigned char *data;
    int elem_size;
    int size;
    int capacity;
};

void vect_create(struct vect_t* vect, int elem_size);
void vect_destroy(struct vect_t* vect);
int vect_reserve(struct vect_t* vect, int capacity);
void *vect_at(struct vect_t* vect, int at);
void *vect_insert_block(struct vect_t* vect, int at, int count);
void *vect_insert(struct vect_t* vect, int at);
void vect_erase(struct vect_t* vect, int at);
void vect_clear(struct vect_t* vect);

/* @vect_helpers */
void *vect_h_insert_copy(struct vect_t *vect, int at, void *val);
void *vect_h_push_back(struct vect_t *vect);
int vect_h_push_back_int(struct vect_t *vect, int val);
int vect_h_int_at(struct vect_t *vect, int at);

/* @pslab */
struct pslab_t {
    unsigned char *data;
    int elem_size;
    int size;
    int capacity;
};

int pslab_create(struct pslab_t *pslab, int elem_size, int capacity);
void pslab_destroy(struct pslab_t *pslab);
void *pslab_at(struct pslab_t *pslab, int at);
void *pslab_insert(struct pslab_t *pslab);
void pslab_erase_at(struct pslab_t *pslab, int at);
void pslab_erase_elem(struct pslab_t *pslab, void *elem);
void pslab_clear(struct pslab_t *pslab);

/* @slab */
struct slab_t {
    struct bit_map_t used;
    int capacity;

    unsigned char *data;
    int elem_size;
    int size;
};

int slab_create(struct slab_t *slab, int elem_size, int capacity);
void slab_destroy(struct slab_t *slab);
void *slab_insert(struct slab_t *slab);
void slab_erase_at(struct slab_t *slab, int at);
void slab_erase_elem(struct slab_t *slab, void *elem);
void *slab_at(struct slab_t *slab, int at);
int slab_max(struct slab_t *slab);
void slab_clear(struct slab_t *slab);
void slab_copy(struct slab_t *to, struct slab_t *from);

typedef int slab_foreach_fn(void *elem, void *arg);
int slab_foreach(struct slab_t *slab, slab_foreach_fn *cb, void *arg);

#define SLAB_FOREACH(slab_p, elem)                                          \
    for (int SFi_=0, SFseen_=0, SFsize_=(slab_p)->size; SFseen_ < SFsize_;) \
        if (1) {                                                            \
            (elem) = slab_at((slab_p), SFi_++);                             \
            if ((elem) == NULL) {                                           \
                continue;                                                   \
            }                                                               \
            SFseen_++;                                                      \
            goto SFbody_;                                                   \
        } else                                                              \
            SFbody_:                                                        \
            /* <loop body goes here> */


/* @kv */

#define KV_KEY_NULL  (-1)

struct kv_elem_t {
    int key;
    union {
        int i;
        void *v;
    } val;
};

struct kv_elem_t *kv_get(struct kv_elem_t *kv, int key);
struct kv_elem_t *kv_get_assert(struct kv_elem_t *kv, int key);

#define KV_GET_VAL_ASSERT(kv, key, field) \
    (kv_get_assert((kv), (key))->val.field)

#define KV_GET_VAL_DFLT(kv, key, field, dflt) \
    (kv_get((kv), (key)) != NULL ? kv_get((kv), (key))->val.field : NULL)

/* @circbuf */

struct circbuf_t {
    struct pslab_t pslab;
    int next;
};

int circbuf_create(struct circbuf_t *circbuf, int elem_size, int capacity);
void circbuf_destroy(struct circbuf_t *circbuf);
void *circbuf_insert(struct circbuf_t *circbuf, void *item);
void *circbuf_at(struct circbuf_t *circbuf, int at);
void *circbuf_from_end(struct circbuf_t *circbuf, int offset_from_end);
void circbuf_clear(struct circbuf_t *circbuf);

#endif

