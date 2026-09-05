#include "../../types.h"

extern s32 func_80042B98(void);
extern void func_8004036C(void *);
extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, s32);
extern void func_80040510(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80168808(void *);

void func_801688AC(u8 *object)
{
    u8 *piece;
    s32 dx;
    s32 dy;

    if (func_80042B98() == 0) {
        for (dy = 0; dy < 0x10; dy += 4) {
            for (dx = 0; dx < 0x10; dx += 4) {
                piece = func_800400AC(func_8004002C(), 1);
                if (piece != 0) {
                    func_80040510(piece,
                                  *(s16 *)(object + 0x30) + dx,
                                  *(s16 *)(object + 0x32) + dy,
                                  4, 4,
                                  object[0x5C] + dx,
                                  object[0x5D] + dy,
                                  object[0x66],
                                  *(u16 *)(object + 0x40),
                                  *(u16 *)(object + 0x42));
                    piece[0x6C] = 3;
                    *(void **)(piece + 0x24) = func_80168808;
                }
            }
        }
    } else {
        func_8004036C(object);
    }
}
