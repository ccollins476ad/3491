#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "gen/container.h"
#include "gen/gen_text.h"

static int
gen_text_find_delims(char *str, char delim, struct vect_t *locs)
{
    char *cur;
    char *c;
    int loc;
    int rc;

    cur = str;
    while (1) {
        c = strchr(cur, delim);
        if (c == NULL) {
            break;
        }

        loc = c - str;
        rc = vect_h_push_back_int(locs, loc);
        if (rc != 0) {
            return rc;
        }

        cur = c;
    }

    return 0;
}

static int
gen_text_push_substring(char *str, int prev_loc, int loc,
                        struct vect_t *substrings)
{
    char **sub_ptr;
    char *sub;
    int sub_len;

    sub_len = loc - prev_loc;
    sub = malloc(sub_len + 1);
    if (sub == NULL) {
        return ENOMEM;
    }

    memcpy(sub, str + prev_loc, sub_len);
    sub[sub_len] = '\0';

    sub_ptr = vect_h_push_back(substrings);
    if (sub_ptr == NULL) {
        return ENOMEM;
    }

    *sub_ptr = sub;

    return 0;
}

int
gen_text_split(char *str, char delim, struct vect_t *substrings)
{
    struct vect_t locs;
    int prev_loc;
    int loc;
    int rc;
    int i;

    rc = gen_text_find_delims(str, delim, &locs);
    if (rc != 0) {
        return rc;
    }

    prev_loc = 0;
    for (i = 0; i < locs.size; i++) {
        loc = vect_h_int_at(&locs, i);
        rc = gen_text_push_substring(str, prev_loc, loc, substrings);
        if (rc != 0) {
            return rc;
        }

        prev_loc = loc + 1;
    }

    loc = prev_loc + strlen(str + prev_loc);
    rc = gen_text_push_substring(str, prev_loc, loc, substrings);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
