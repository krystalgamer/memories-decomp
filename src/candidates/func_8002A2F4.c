/* Reclassified from matching_c (#3859). This was src/game/func_8002A2F4.c,
 * byte-exact only under gcc_2_8_1_cc_g8_as_g0_split, whose compiler and
 * assembler disagree about small data (GCC -G8, MASPSX -G0). Under
 * gcc_2_8_1_g0_split, a single threshold, it is 54 of 54 instructions with
 * 33 differing, opcode distance 4. The source below is the match, unchanged
 * apart from its include paths. */
#include "../types.h"
#include "../game/text_box_lifecycle.h"
#include "../game/duel_effect.h"
#include "../unmatched.h"
#include "../game/func_8002A788.h"
#include "../game/func_80029EB0.h"
#include "../game/text_box_runtime.h"
#include "../game/text_staging.h"

void func_8002A2F4(u8 *p)
{
    TextStagingValues *q = D_801D5608;
    s32 r;
    s32 t;
    s32 n;
    s32 mode;
    u8 *o;

    mode = 0;
    r = func_8002A6B8();
    gDuel_wSelectedCardID = r;
    t = (s16)r;
    q->card.card_id = t;
    n = t;

    if (n != 0) {
        r = func_80029EB0(p, n);
        mode = 5;
        if ((r & 0x80) == 0) {
            gDuel_wSelectedCardID = 0;
        }
    }

    o = TextBox_Create(1, mode, 0x10, 0xCA, 0x120, 0x30);
    D_8009B320 = o[0x54];
    if (*(p + (n << 2) + 0x56) & 1) {
        D_8009B320 = 4;
    }
    func_80039A60((struct DuelEffectChannel *)&D_800EB15C);
}
