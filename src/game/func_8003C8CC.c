#include "../types.h"

extern u8 D_8009B37C;
extern void Fade_WaitOut(void);
extern void func_8003C7A0(void);
s32 func_8003C8CC(void) {
    switch (D_8009B37C & 0xF) {
    case 0: Fade_WaitOut(); break;
    case 1: func_8003C7A0(); break;
    case 2: break;
    case 3: D_8009B37C = 1; break;
    }
    return D_8009B37C;
}
