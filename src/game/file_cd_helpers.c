#include "../types.h"
#include "file_cd_helpers.h"
#include "movie_frame_pipeline.h"
#include "../psyq/libds.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

s32 File_Exists(const char *path, CdlFILE *file)
{
    DslFILE *result = DsSearchFile((DslFILE *)file, (char *)path);

    if (result == 0 || result == (DslFILE *)-1) {
        return -1;
    }
    return 0;
}

int Movie_StepPlayback(void)
{
    int result = Movie_DecodeAndPresentFrame();

    if (result != 0) {
        Movie_StopStream(0);
    }
    return result;
}

int Movie_MoveDisplayImage(int first, int second)
{
    DISPENV local;

    GetDispEnv(&local);
    while (IsIdleGPU(3) != 0) {}
    while (MoveImage2(&local.disp, first, second) != 0) {}
    return 0;
}
