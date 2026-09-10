/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/file_cd_helpers.c.
 */
#include "../types.h"
#include "../game/file_cd_helpers.h"
#include "../game/movie_frame_pipeline.h"
#include "../psyq/libds.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

int File_Exists(int first, int second)
{
    register int result asm("$3") =
        (int)DsSearchFile((DslFILE *)second, (char *)first);
    register int output asm("$2") = -1;

    if (result == 0) {
        goto negative;
    }
    if (result != output) {
        output = 0;
        goto done;
    }
negative:
    output = -1;
done:
    return output;
}

