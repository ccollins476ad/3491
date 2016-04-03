#include <assert.h>
#include <stddef.h>
#include "life/world.h"
#include "life/terr.h"
#include "life/terr_defs.h"
#include "life/data.h"
#include "life/vision.h"

int
vision_can_see(int seerx, int seery, int objx, int objy)
{
    terr_id_t terr_id;
    int intersections[1024][2];
    int num_intersections;
    int rc;
    int i;

    /* Trace a line from the object to the seer, failing if an opaque terr is
     * encountered.
     */
    num_intersections = sizeof intersections / sizeof intersections[0];
    rc = segment_tile_intersections(objx, objy, seerx, seery,
                                    WORLD_TILE_DIM, WORLD_TILE_DIM,
                                    intersections, &num_intersections);
    assert(rc == 0);

    for (i = 0; i < num_intersections; i++) {
        terr_id = terr_scape_get_raw(intersections[i][0], intersections[i][1]);
        if (terr_id & TERR_ID_F_OPAQUE) {
            return 0;
        }
    }

    return 1;
}

