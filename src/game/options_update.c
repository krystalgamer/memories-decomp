#include "../types.h"

extern u8 gOptions_bState;
extern void Fade_WaitOut(void);
extern void Options_HandleInput(void);
s32 Options_Update(void) {
    switch (gOptions_bState & 0xF) {
    case 0: Fade_WaitOut(); break;
    case 1: Options_HandleInput(); break;
    case 2: break;
    case 3: gOptions_bState = 1; break;
    }
    return gOptions_bState;
}
