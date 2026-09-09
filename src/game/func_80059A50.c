#include "../types.h"
#include "model_texture_upload.h"

/* Copies the image header onto the stack before forwarding it, so the
   callee can't alias the caller's copy. */
u32 func_80059A50(s32 a0, s32 a1, GsIMAGE *src) {
    GsIMAGE buf = *src;
    return func_80058A7C(a0, a1, &buf);
}
