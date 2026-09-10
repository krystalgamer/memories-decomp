#include "../types.h"
#include "file_cd_helpers.h"
#include "movie_frame_pipeline.h"
#include "../psyq/libds.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

int func_8005C530(void)
{
    int result = func_8005BE3C();

    if (result != 0) {
        func_8005BB7C(0);
    }
    return result;
}

int func_8005C568(int first, int second)
{
    DISPENV local;

    GetDispEnv(&local);
    while (IsIdleGPU(3) != 0) {}
    while (MoveImage2(&local.disp, first, second) != 0) {}
    return 0;
}
