#include "../types.h"
#include "model_texture_upload.h"

/* The scratch buffer is eight words, four more than GsIMAGE needs. Kept at
 * that size rather than declared as a GsIMAGE, because shrinking it changes
 * the frame this function reserves. */
typedef struct { int words[8]; } Local;

void func_800599FC(int a, int b, int c, int d, int e, int f, int g)
{
    Local local;
    func_80058B4C((GsIMAGE *)&local, a, b, c, d, e, f, g);
}
