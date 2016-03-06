#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "gen/container.h"

/*****************************************************************************
 * @bit_map                                                                  *
 *****************************************************************************/

void
bit_map_bit(int idx, int *byte, unsigned char *mask)
{
    *byte = idx / CHAR_BIT;
    *mask = 1 << idx % CHAR_BIT;
}

int
bit_map_idx(int byte, int bit)
{
    return byte * CHAR_BIT + bit;
}

int
bit_map_get(struct bit_map_t *bit_map, int idx)
{
    unsigned char mask;
    int byte;

    bit_map_bit(idx, &byte, &mask);
    return !!(bit_map->bits[byte] & mask);
}

void
bit_map_set(struct bit_map_t *bit_map, int idx, int val)
{
    unsigned char mask;
    int byte;

    bit_map_bit(idx, &byte, &mask);
    if (val) {
        bit_map->bits[byte] |= mask;
    } else {
        bit_map->bits[byte] &= ~mask;
    }
}

void
bit_map_clear(struct bit_map_t *bit_map)
{
    memset(bit_map->bits, 0, bit_map->num_bytes);
}

/*****************************************************************************
 * @vect                                                                     *
 *****************************************************************************/

static int
vect_new_capacity(int old_capacity, int new_size)
{
    int capacity;

    if (old_capacity >= new_size) {
        return old_capacity;
    }
    if (old_capacity == 0) {
        capacity = 1;
    } else {
        capacity = old_capacity;
    }

    while (capacity < new_size) {
        capacity *= 2;
    }

    return capacity;
}

static void
vect_adjust(struct vect_t* vect, int at, int delta)
{
    unsigned char *src;
    unsigned char *dst;
    int num_bytes;

    assert(at >= 0 && at < vect->capacity);
    assert(at + delta >= 0);
    assert(vect->size + delta <= vect->capacity);

    src = vect->data + at * vect->elem_size;
    dst = src + delta * vect->elem_size;
    num_bytes = (vect->size - at) * vect->elem_size;

    memmove(dst, src, num_bytes);
}

void
vect_create(struct vect_t* vect, int elem_size)
{
    vect->data = NULL;
    vect->elem_size = elem_size;
    vect->size = 0;
    vect->capacity = 0;
}

void
vect_destroy(struct vect_t* vect)
{
    free(vect->data);
    memset(vect, 0, sizeof *vect);
}

int
vect_reserve(struct vect_t* vect, int capacity)
{
    unsigned char *new_data;

    assert(capacity > 0);

    new_data = realloc(vect->data, capacity * vect->elem_size);
    if (new_data == NULL) {
        return ENOMEM;
    }

    vect->data = new_data;
    vect->capacity = capacity;

    return 0;
}

void *
vect_at(struct vect_t* vect, int at)
{
    assert(at >= 0 && at <= vect->size);

    return vect->data + at * vect->elem_size;
}

void *
vect_insert_block(struct vect_t* vect, int at, int count)
{
    int new_capacity;
    int rc;

    assert(at >= 0 && at <= vect->size);

    if (vect->size + count > vect->capacity) {
        new_capacity = vect_new_capacity(vect->capacity, vect->size + count);
        rc = vect_reserve(vect, new_capacity);
        if (rc != 0) {
            return NULL;
        }
    }

    if (at < vect->size) {
        vect_adjust(vect, at, count);
    }
    vect->size += count;

    return vect_at(vect, at);
}

void *
vect_insert(struct vect_t* vect, int at)
{
    return vect_insert_block(vect, at, 1);
}

void
vect_erase(struct vect_t* vect, int at)
{
    assert(at >= 0 && at < vect->size);

    if (at != vect->size - 1) {
        vect_adjust(vect, at, -1);
    }
    --vect->size;
}

void
vect_clear(struct vect_t* vect)
{
    vect->size = 0;
}

/*****************************************************************************
 * @vect_helpers                                                             *
 *****************************************************************************/

void *
vect_h_insert_copy(struct vect_t* vect, int at, void *val)
{
    void *new_val;

    new_val = vect_insert(vect, at);
    if (new_val != NULL) {
        memcpy(new_val, val, vect->elem_size);
    }

    return new_val;
}

void *
vect_h_push_back(struct vect_t* vect)
{
    return vect_insert(vect, vect->size);
}

int
vect_h_push_back_int(struct vect_t *vect, int val)
{
    int *mem;

    mem = vect_h_push_back(vect);
    if (mem == NULL) {
        return ENOMEM;
    }
    *mem = val;
    return 0;
}

int
vect_h_int_at(struct vect_t *vect, int at)
{
    void *val;
    int *ival;

    assert(at >= 0 && at < vect->size);
    val = vect_at(vect, at);
    ival = val;
    return *ival;
}

/*****************************************************************************
 * @pslab                                                                    *
 *****************************************************************************/

int
pslab_create(struct pslab_t *pslab, int elem_size, int capacity)
{
    pslab->data = malloc(elem_size * capacity);
    if (pslab->data == NULL) {
        return ENOMEM;
    }

    pslab->elem_size = elem_size;
    pslab->size = 0;
    pslab->capacity = capacity;

    return 0;
}

void
pslab_destroy(struct pslab_t *pslab)
{
    free(pslab->data);
    memset(pslab, 0, sizeof *pslab);
}

void *
pslab_at(struct pslab_t *pslab, int at)
{
    if (at >= pslab->size) {
        return NULL;
    } else {
        return pslab->data + at * pslab->elem_size;
    }
}

void *
pslab_insert(struct pslab_t *pslab)
{
    if (pslab->size >= pslab->capacity) {
        return NULL;
    }
    ++pslab->size;
    return pslab_at(pslab, pslab->size - 1);
}

void
pslab_erase_at(struct pslab_t *pslab, int at)
{
    int new_size;

    assert(at >= 0 && at < pslab->size);

    new_size = pslab->size - 1;
    if (at != new_size) {
        memcpy(pslab_at(pslab, at), pslab_at(pslab, new_size),
                        pslab->elem_size);
    }
    pslab->size = new_size;
}

void
pslab_erase_elem(struct pslab_t *pslab, void *elem)
{
    int idx;

    idx = ((unsigned char *)elem - pslab->data) / pslab->elem_size;
    pslab_erase_at(pslab, idx);
}

void
pslab_clear(struct pslab_t *pslab)
{
    pslab->size = 0;
}

/*****************************************************************************
 * @slab                                                                    *
 *****************************************************************************/

int
slab_create(struct slab_t *slab, int elem_size, int capacity)
{
    memset(slab, 0, sizeof *slab);

    slab->capacity = capacity;
    slab->used.num_bytes = BIT_MAP_BUFSZ(capacity);
    slab->used.num_bits = capacity;

    slab->data = malloc(elem_size * capacity + slab->used.num_bytes);
    if (slab->data == NULL) {
        goto err;
    }
    slab->elem_size = elem_size;

    slab->used.bits = slab->data + elem_size * capacity;
    memset(slab->used.bits, 0, slab->used.num_bytes);

    return 0;

err:
    slab_destroy(slab);
    return ENOMEM;
}

void
slab_destroy(struct slab_t *slab)
{
    free(slab->data);
    memset(slab, 0, sizeof *slab);
}

static void *
slab_at_raw(struct slab_t *slab, int at)
{
    return slab->data + slab->elem_size * at;
}

void *
slab_at(struct slab_t *slab, int at)
{
    if (!bit_map_get(&slab->used, at)) {
        return NULL;
    }

    return slab_at_raw(slab, at);
}

static int
slab_first_unused(struct slab_t *slab, int *byte, int *bit)
{
    unsigned char mask;
    int i;
    int j;

    for (i = 0; i < slab->used.num_bytes; ++i) {
        if (slab->used.bits[i] != UCHAR_MAX) {
            for (j = 0; j < CHAR_BIT; ++j) {
                mask = 1 << j;
                if (!(slab->used.bits[i] & mask)) {
                    if (bit_map_idx(i, j) >= slab->used.num_bits) {
                        return -1;
                    } else {
                        *byte = i;
                        *bit = j;
                        return 0;
                    }
                }
            }
        }
    }

    return -1;
}

void *
slab_insert(struct slab_t *slab)
{
    int byte;
    int bit;
    int idx;
    int rc;

    rc = slab_first_unused(slab, &byte, &bit);
    if (rc != 0) {
        return NULL;
    }

    assert(slab->size < slab->capacity);

    slab->used.bits[byte] |= 1 << bit;
    slab->size++;

    idx = bit_map_idx(byte, bit);
    return slab_at_raw(slab, idx);
}

void
slab_erase_at(struct slab_t *slab, int at)
{
    unsigned char mask;
    int byte;

    bit_map_bit(at, &byte, &mask);
    slab->used.bits[byte] &= ~mask;
    slab->size--;
}

void
slab_erase_elem(struct slab_t *slab, void *elem)
{
    int idx;

    if (elem != NULL) {
        idx = ((unsigned char *)elem - slab->data) / slab->elem_size;
        assert(idx >= 0 && idx < slab->capacity);
        slab_erase_at(slab, idx);
    }
}

int
slab_foreach(struct slab_t *slab, slab_foreach_fn *cb, void *arg)
{
    unsigned char mask;
    void *elem;
    int idx;
    int rc;
    int i;
    int j;

    for (i = 0; i < slab->used.num_bytes; ++i) {
        if (slab->used.bits[i] != 0) {
            for (j = 0; j < 8; ++j) {
                mask = 1 << j;
                if (slab->used.bits[i] & mask) {
                    idx = bit_map_idx(i, j);
                    elem = slab_at_raw(slab, idx);
                    rc = cb(elem, arg);
                    if (rc != 0) {
                        return rc;
                    }
                }
            }
        }
    }

    return 0;
}

int
slab_max(struct slab_t *slab)
{
    int max;
    int i;

    max = 0;

    /* Find last used slot. */
    for (i = 0; i < slab->used.num_bytes; ++i) {
        if (slab->used.bits[i] != 0) {
            max = (i + 1) * CHAR_BIT;
        }
    }

    return max;
}

void
slab_clear(struct slab_t *slab)
{
    bit_map_clear(&slab->used);
    slab->size = 0;
}

/*****************************************************************************
 * @kv                                                                       *
 *****************************************************************************/

struct kv_elem_t *
kv_get(struct kv_elem_t *kv, int key)
{
    struct kv_elem_t *elem;
    int i;

    assert(key >= 0);

    i = 0;
    while (1) {
        elem = kv + i;
        if (elem == NULL) {
            return NULL;
        }
        if (elem->key == key) {
            return elem;
        }

        i++;
    }
}

struct kv_elem_t *
kv_get_assert(struct kv_elem_t *kv, int key)
{
    struct kv_elem_t *elem;

    elem = kv_get(kv, key);
    assert(elem != NULL);
    return elem;
}

int
circbuf_create(struct circbuf_t *circbuf, int elem_size, int capacity)
{
    int rc;

    rc = pslab_create(&circbuf->pslab, elem_size, capacity);
    if (rc != 0) {
        return rc;
    }

    circbuf->next = 0;

    return 0;
}

void
circbuf_destroy(struct circbuf_t *circbuf)
{
    pslab_destroy(&circbuf->pslab);
}

void *
circbuf_insert(struct circbuf_t *circbuf, void *item)
{
    void **next_addr;
    void *cur_item;

    cur_item = circbuf_at(circbuf, circbuf->next);
    if (cur_item == NULL) {
        assert(circbuf->pslab.size < circbuf->pslab.capacity);
        assert(circbuf->pslab.size == circbuf->next);

        next_addr = pslab_insert(&circbuf->pslab);
    } else {
        next_addr = pslab_at(&circbuf->pslab, circbuf->next);
    }

    *next_addr = item;

    circbuf->next++;
    if (circbuf->next >= circbuf->pslab.capacity) {
        circbuf->next = 0;
    }

    return cur_item;
}

void *
circbuf_at(struct circbuf_t *circbuf, int at)
{
    void **addr;
    void *item;

    addr = pslab_at(&circbuf->pslab, at);
    if (addr == NULL) {
        item = NULL;
    } else {
        item = *addr;
    }

    return item;
}

void *
circbuf_from_end(struct circbuf_t *circbuf, int offset_from_end)
{
    int idx;

    assert(offset_from_end >= 0);
    if (offset_from_end >= circbuf->pslab.size) {
        return NULL;
    }

    idx = circbuf->next - 1 - offset_from_end;
    if (idx < 0) {
        idx += circbuf->pslab.size;
    }

    return circbuf_at(circbuf, idx);
}

void
circbuf_clear(struct circbuf_t *circbuf)
{
    pslab_clear(&circbuf->pslab);
    circbuf->next = 0;
}
