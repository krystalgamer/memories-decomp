#include "../types.h"
#include "../unmatched.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "color_constants.h"
#include "color.h"
#include "model_apply_texture_tint.h"

#define TEXTURE_TINT_RECT ((RECT *)&D_8009B470)
#define TEXTURE_TINT_WORDS(buffer) ((u32 *)(buffer))

void Model_ApplyTextureTint(s32 arg0, s32 arg1, s32 arg2)
{
    u16 buf[0x400];
    u16 *q;
    s32 y;
    s32 i;

    if ((u32)arg0 < 2) {
        y = 0;

        if (arg1 >= COLOR_TINT_KEEP_HUE &&
            (arg1 & COLOR_TINT_INVERT) == 0 && arg2 >= COLOR_FIXED_ONE) {
            goto big;
        }

        D_8009B470 = arg0 << 8;
        D_8009B472 = 0xF8;
        D_8009B474 = 0x100;
        D_8009B476 = 4;

        do {
            D_8009B472 = y + 0xF8;
            while (IsIdleGPU(3) != 0) {
            }
            while (StoreImage2(TEXTURE_TINT_RECT, TEXTURE_TINT_WORDS(buf)) != 0) {
            }
            while (IsIdleGPU(3) != 0) {
            }

            q = buf;
            for (i = 0; i < 0x400; i++, q++) {
                *q = Color_TintBgr555Pixel(*q, (u8)arg1, (u16)arg2);
            }

            D_8009B472 = y + 0xF0;
            while (IsIdleGPU(3) != 0) {
            }
            while (LoadImage2(TEXTURE_TINT_RECT, TEXTURE_TINT_WORDS(buf)) != 0) {
            }
            while (IsIdleGPU(3) != 0) {
            }
            y += 4;
        } while (y < 8);
    }

    return;

big:
    D_8009B472 = 0xF8;
    D_8009B474 = 0x100;
    D_8009B470 = arg0 << 8;
    D_8009B476 = 8;
    MoveImage(TEXTURE_TINT_RECT, arg0 << 8, 0xF0);
}
