#include <string.h>
#include "gen/res.h"
#include "gen/strlcat.h"

char *
res_bmp_path(char *filename)
{
    static char path[1024];

    strcpy(path, "../../res/");
    strlcat(path, filename, sizeof path);
    return path;
}
