#define GDUEL_WSELECTEDCARDID_IN_DATA
#define D_8009B320_IN_DATA
#include "../types.h"
#include "text_box_lifecycle.h"
#include "duel_effect.h"
#include "../unmatched.h"
#include "func_8002A2F4.h"
#include "func_8002A788.h"
#include "func_80029EB0.h"
#include "text_box_runtime.h"
#include "text_staging.h"

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
    func_80039A60((u8 *)&D_800EB15C);
}
