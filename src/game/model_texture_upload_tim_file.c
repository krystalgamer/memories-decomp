#include "../types.h"
#include "model_texture_upload.h"

void ModelTexture_UploadTimFile(char *path, int b, int c, int d, int e, int f, int g)
{
    /* The 32-byte retail scratch is one word larger than GsIMAGE. */
    s32 local[8];

    ModelTexture_LoadTim((GsIMAGE *)local, path, b, c, d, e, f, g);
}
