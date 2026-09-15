/* The unit compiles at -G8 like its neighbours, so the two small globals it
 * stores take their .data arms: retail writes both through $at rather than
 * gp-relative, the absolute form a .data declaration gives at -G8. */
#define D_8009B320_IN_DATA
#define GDUEL_WSELECTEDCARDID_IN_DATA
#include "../types.h"
#include "text_box_lifecycle.h"
#include "duel_effect.h"
#include "func_8002A2F4.h"
#include "library_grid_cursor.h"
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
    r = Library_GetGridCursorCardId(p);
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
