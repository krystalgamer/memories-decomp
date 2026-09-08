#include "../types.h"
#include "main_frame.h"
#include "fade.h"
#include "main_modes.h"

extern u8 D_8009B26C;
extern void func_8002CDE8(void);
extern void Main_ResetFrontendRuntime(void);
void Main_Loop(void) {
    func_8002CDE8();
    for (;;) {
        u8 v;
        func_80012D4C();
        v = D_8009B26C;
        if ((v & 0x80) == 0) {
            D_8009B26C = v | 0x80;
            Main_ResetFrontendRuntime();
        } else {
            gMain_apfnModeRunner[v & 0x1F]();
            if ((D_8009B26C & 0x40) == 0) Fade_WaitOut();
        }
    }
}
