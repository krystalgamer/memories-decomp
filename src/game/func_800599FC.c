#include "../types.h"
#include "model_texture_upload.h"

void func_800599FC(char *path, int b, int c, int d, int e, int f, int g)
{
    /* Four words larger than GsIMAGE so the retail stack frame is preserved. */
    s32 local[8];

    ModelTexture_LoadTim((GsIMAGE *)local, path, b, c, d, e, f, g);
}
