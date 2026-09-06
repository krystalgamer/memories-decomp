#include "../types.h"

extern void (*D_800E9DB0[4])(void); /* per-frame callback slots */
extern s32 runtime_gp;
extern s32 D_8009B0A4;
extern s32 D_8009B0B0;
extern void (*D_8009B0B8)(void);
extern s32 D_8009B0BC;
extern s32 D_8009B0D4;

extern void func_800136D4(void);
extern void func_80014A5C(s32 a0);
extern void Fade_DrawOverlay(void);
extern void func_80041340(void);

/* Per-frame dispatcher: runs the two fixed housekeeping calls, then each of
   the 4 slots in D_800E9DB0 and the single D_8009B0B8 callback if set. If
   neither of the two progress pairs (f1A8/f19C, f1B4/f1CC) has advanced and
   the watchdog counter D_8009AF08 underflows, resets the counter to 0x3C
   and re-syncs both progress pairs. Finishes with func_80014A5C/func_800136D4. */
void func_8001306C(void) {
    void (*fn)(void);
    s32 i;
    s32 cnt;

    Fade_DrawOverlay();
    func_80041340();

    for (i = 0; i < 4; i++) {
        fn = D_800E9DB0[i];
        if (fn != 0) {
            fn();
        }
    }

    fn = D_8009B0B8;
    if (fn != 0) {
        fn();
    }

    if (D_8009B0B0 < D_8009B0A4 || D_8009B0BC < D_8009B0D4) {
        goto reset;
    }
    cnt = runtime_gp - 1;
    runtime_gp = cnt;
    if (cnt < 0) {
    reset:
        runtime_gp = 0x3C;
        D_8009B0B0 = D_8009B0A4;
        D_8009B0BC = D_8009B0D4;
    }

    func_80014A5C(0);
    func_800136D4();
}
