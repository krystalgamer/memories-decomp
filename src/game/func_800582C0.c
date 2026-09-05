#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

typedef float f32;
typedef double f64;
typedef char M2C_UNK;
typedef struct { s32 words[9]; } Mtx32;
typedef u8 State;
typedef u8 Record;
typedef u8 Pair;
typedef u8 Controller;
typedef u8 Entry;
typedef u8 Rec;
typedef u8 Block;
typedef struct { u32 words[2]; } Blk8;

extern s16 D_8009B470;
extern s16 D_8009B472;
extern s16 D_8009B474;
extern s16 D_8009B476;
extern s32 func_8005AE68();

void func_800582C0(s32 arg0, s32 arg1, s32 arg2) {
    u16 buf[0x400];
    u16 *q;
    s32 y;
    s32 i;

    if ((u32)arg0 < 2) {
        y = 0;

        if (arg1 >= 7 && (arg1 & 8) == 0 && arg2 >= 0x1000) {
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
            while (StoreImage2((RECT *)&D_8009B470, (u32 *)buf) != 0) {
            }
            while (IsIdleGPU(3) != 0) {
            }

            q = buf;
            for (i = 0; i < 0x400; i++, q++) {
                *q = func_8005AE68(*q, (u8)arg1, (u16)arg2);
            }

            D_8009B472 = y + 0xF0;
            while (IsIdleGPU(3) != 0) {
            }
            while (LoadImage2((RECT *)&D_8009B470, (u32 *)buf) != 0) {
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
    MoveImage((RECT *)&D_8009B470, arg0 << 8, 0xF0);
}
