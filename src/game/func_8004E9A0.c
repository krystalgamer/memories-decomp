#include "../types.h"
#include "../unmatched.h"
#include "../psyq/libetc.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "model_image_copy_frames.h"
#include "model_graphics_state.h"
#include "func_8004E9A0.h"

void func_8004E9A0(void) {
    s32 k;
    s32 u_0;
    s32 u_1;
    s32 r;
    s32 r4;
    s32 n;
    s32 q;

    if (D_800F569F[0] != 0) {
        k = D_8009AF88[0xA0];
        switch (k) {
        case 1:
            u_0 = VSync(-1) / 8;
            gModel_ImageCopyRectY = 0x1C0;
            gModel_ImageCopyRectW = 0x10;
            gModel_ImageCopyRectH = 0x40;
            r = u_0 % 3;
            gModel_ImageCopyRect.x = r * 16 + 0x2D0;
            MoveImage(&gModel_ImageCopyRect, 0x2C0, 0x1C0);
            break;
        case 4:
            n = MODEL_IMAGE_COPY_FRAME_COUNT;
            u_1 = VSync(-1) / 10;
            r4 = u_1 % n;
            gModel_ImageCopyRect.x = 0x260;
            q = gModel_abImageCopyFrameRows[r4];
            gModel_ImageCopyRectW = 0x20;
            gModel_ImageCopyRectH = 0x18;
            gModel_ImageCopyRectY = q * 3 * 8 + 0x180;
            MoveImage(&gModel_ImageCopyRect, 0x220, 0x168);
            break;
        }
    }
}
