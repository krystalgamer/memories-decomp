#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libpress.h"

extern u8 D_8009B060;
extern u8 D_8009B062;
extern u8 D_8009B063;
extern u8 D_8009B064;
extern u8 D_8009B065;
extern u8 D_8009B066;
extern u8 D_8009B067;
extern u8 *D_8009B498;

extern void func_80044F58(s32 arg0);
extern s32 func_8005BFC8(s32 arg0);
extern s32 func_8005C5D4(void);

/* Decodes and presents one movie frame. D_8009B063 marks the stream as
 * finished and D_8009B064 as fading out, in which case the fade level in
 * D_8009B065 drops by 0x40 a frame and the stream ends once it reaches zero.
 * The work area at D_8009B498 + 0x40000 carries two RECTs, the frame rect at
 * +0x2420 and the display rect at +0x2428. Both are centred against the
 * current drawing clip: horizontally in the 320- or 640-wide screen chosen by
 * D_8009B060, vertically in 240. The ring slots are 0xE000 apart from
 * +0x1B000 for the coded data and 0x2D00 apart from +0x37000 for the decoded
 * output. Returns non-zero once the stream is done. */
s32 func_8005BE3C(void) {
    DRAWENV env;
    s32 fade;
    s32 result;
    s32 x;
    s32 y;
    u8 *frame;
    u8 *display;
    u8 *rects;
    u8 *out;
    s32 slot;
    s32 side;

    if (D_8009B063 != 0) {
        return 1;
    }
    if (D_8009B064 != 0) {
        if (D_8009B065 == 0) {
            return 1;
        }
        fade = D_8009B065 - 0x40;
        if (fade < 0) {
            fade = 0;
        }
        D_8009B065 = fade;
        func_80044F58(D_8009B065);
    }
    /* volatile: retail sets up the following call's argument above this store
       and leaves its delay slot empty, which only a store that cannot be
       moved into a slot gives. */
    *(volatile u8 *)&D_8009B062 = 0;
    GetDrawEnv(&env);

    frame = D_8009B498 + 0x40000;
    display = D_8009B498 + 0x40000;
    x = env.clip.x +
        ((D_8009B060 != 0 ? 0x1E0 : 0x140) - *(s16 *)(frame + 0x2424)) / 2;
    side = D_8009B060;
    slot = D_8009B066;
    *(s16 *)(display + 0x2428) = x;
    *(s16 *)(frame + 0x2420) = x;

    rects = D_8009B498 + 0x40000;
    y = *(u16 *)&env.clip.y + (0xF0 - *(s16 *)(rects + 0x2426)) / 2;
    *(s16 *)(rects + 0x242A) = y;
    *(s16 *)(rects + 0x2422) = y;
    DecDCTin((u32 *)(D_8009B498 + 0x1B000 + slot * 0xE000), side);

    out = D_8009B498 + 0x40000;
    DecDCTout((u32 *)(D_8009B498 + 0x37000 + D_8009B067 * 0x2D00),
              *(s16 *)(out + 0x242C) * *(s16 *)(out + 0x242E) / 2);

    result = func_8005BFC8(1);
    if (result != 0) {
        return result;
    }
    func_8005C5D4();
    return 0;
}
