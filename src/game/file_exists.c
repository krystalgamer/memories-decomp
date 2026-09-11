#include "../types.h"
#include "file_cd_helpers.h"
#include "movie_frame_pipeline.h"
#include "../psyq/libds.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

int File_Exists(int first, int second)
{
    DslFILE *result = DsSearchFile((DslFILE *)second, (char *)first);

    if (result == 0 || result == (DslFILE *)-1) {
        return -1;
    }
    return 0;
}
