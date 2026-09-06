#include "../../types.h"

extern u8 D_800F2848[];
extern u32 D_8009B304;
extern u32 D_8009B308;
extern u32 D_8009B30C;
extern u32 D_8009B310;
extern u32 D_8009B314;
extern void func_800878D0(int);
extern void func_800878B0(int, int);
extern void SetFarColor(long, long, long);
extern void SetFogNearFar(long, long, long);
extern void func_800540B4(int);

void func_80168258(void)
{
    u8 *camera = D_800F2848;
    u32 flags;

    func_800878D0(*(s16 *)(camera + 0xE));
    func_800878B0(0xA0, 0x78);
    SetFarColor(0, 0, 0);
    SetFogNearFar(0x7D0, 0x960, *(s16 *)(camera + 0xE));

    flags = D_8009B30C;
    if (flags & 2) {
        D_8009B314 += 0xA;
        if (D_8009B314 >= D_8009B308) {
            D_8009B30C = flags & ~3;
        }
        D_8009B310 = D_8009B304;
    }
    func_800540B4(2);
}
