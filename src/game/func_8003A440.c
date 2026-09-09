#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object_helpers.h"
#include "func_8003A440.h"

void func_8003A440(u8 **arg0, u32 arg1, s32 arg2)
{
    u8 *e;
    s8 c;
    u32 *w;
    s32 i;

    if (arg1 == 0) {
        for (i = 2; i >= 0; i--) {
            c = (s8)arg2;
            e = arg0[i];
            if (e != 0) {
                *(u32 *)(e + 4) = *(u32 *)(e + 4) & ~(GsALON | GsATWO | GsAONE);
                *(u32 *)(e + 4) = *(u32 *)(e + 4) | GsALON;
                func_800428EC(e, c);
                *(u32 *)(e + 0xC) = 0x808080;
                *(u16 *)(e + 0x42) -= 1;
            }
        }
    } else {
        for (i = 2; i >= 0; i--) {
            e = arg0[i];
            if (e != 0) {
                w = (u32 *)(e + 4);
                *w = (*(u32 *)(e + 4) & ~(GsALON | GsATWO | GsAONE)) | arg1;
                func_800428EC(e, (s8)arg2);
                if (arg1 == (GsALON | GsATWO)) {
                    *(u16 *)(e + 0x42) = 0xFD;
                } else {
                    *(u16 *)(e + 0x42) += 1;
                }
            }
        }
    }
}
