#define GINPUT_PAD1_PRESSED_IN_DATA
#include "../types.h"
#include "input.h"
#include "duel_effect.h"
#include "duel_effect_process_entries.h"
#include "sound.h"
#include "display_object_api.h"
#include "func_80039794.h"
#include "text_box_runtime.h"

/* The retail body walks two pointers over the same four records: the record
 * base it hands to the per-record calls, and a second cursor parked on the
 * record's 0x30 pair, so the choice object and its flags are reached at
 * displacements 0 and 4. Folding the cursor into the base costs a register and
 * four instructions, so the view stays. */
extern TblEnt D_801D9000[];
extern s16 D_8009B35A;
extern u8 D_8009B356;
extern u8 D_8009B0C1 __attribute__((section(".data")));

extern void *Dialog_OpenChoice(void *);
extern void func_8003B50C(s32);

void func_80039794(void)
{
    DuelEffectChannel *p;
    ChoiceView *q;
    s32 reset_value;
    TblEnt *table;
    s32 n;
    s32 cnt;
    s32 idx;
    s32 kind;
    s32 arg;
    u16 f;

    p = D_800EB0F8;
    n = 4;
    reset_value = -1;
    table = D_801D9000;
    q = (ChoiceView *)&p->field_30;
    do {
        if (q->flags & 0x8000) {
            D_8009B35A = 0;
            if ((q->flags & 0x2000) == 0) {
                D_8009B35A = reset_value;
                cnt = -1;
                for (;;) {
                    TextBox_BuildStep(p);
                    cnt++;
                    f = q->flags;
                    if (f & 0x2000) {
                        if (f & 8) {
                            q->obj = Dialog_OpenChoice(p);
                        }
                        break;
                    }
                    if (f & 0x1C00) {
                        goto reset;
                    }
                    if (cnt >= D_8009B0C1) {
                        break;
                    }
                    continue;
reset:
                    D_8009B35A = reset_value;
                }
            } else {
                if (q->flags & 8) {
                    if (gInput_wPad1Pressed & 0xC0) {
                        q->flags &= 0xFFF7;
                        func_8004036C(q->obj);
                        q->obj = 0;
                        SD_SEPlayFull(0xB);
                    }
                }
            }
            arg = -1;
            idx = D_8009B35A;
            if (idx >= 0) {
                kind = table[idx].hi & 7;
                if (kind == 4) {
                    arg = D_8009B356;
                } else {
                    D_8009B356 = kind;
                    arg = kind;
                }
            }
            func_8003B50C(arg);
            DuelEffect_ProcessEntries(p);
        }
        q = (ChoiceView *)((u8 *)q + sizeof(DuelEffectChannel));
        n--;
        p++;
    } while (n != 0);
}
