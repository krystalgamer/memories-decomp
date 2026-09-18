#include "../types.h"
#include "model_word_memory.h"
#include "model.h"
#include "model_slot_state_updates.h"
#include "model_control_slot_animation.h"

/* Starts, restores or stops one model slot's part animation.
 *
 * -1 clears every part's command index. 0 restores the words the active row
 * saved, three per part or five for parts flagged in field_BEC, and clears
 * the active row. Any other row saves the parts' current words, records each
 * part's command index and makes that row active, restoring the previous row
 * first when the caller allows it.
 *
 * Each loop keeps its own word count, row entry and rounded part index: the
 * part bit is found by shifting with the word count while it still holds 3,
 * which is the retail srav/sllv form, and a shared variable would give the
 * word count the slot's callee-saved register. */
void Model_ControlSlotAnimation(s32 index, s32 anim, s32 flag) {
    ModelSlot *m;
    ModelSlotPart **parts;
    u8 *dst;
    u8 *base;
    u16 *src;
    s32 i;
    s32 prev;
    s32 restore_words;
    s32 copy_words;
    s32 restore_row;
    s32 copy_row;
    s32 copy_bit;
    s32 restore_bit;

    m = &D_800F2C40[index];
    dst = m->field_DE0;
    base = m->field_DDC;
    parts = m->field_1E0;
    if (m->field_E1F == 0) {
        return;
    }
    switch (anim) {
    case -1:
        m->field_E10 = 0;
        for (i = 0; i < m->field_E1B; parts++, i++) {
            (*parts)->ii = 0xFFFF;
        }
        break;
    case 0:
        if (m->field_E0F == 0) {
            return;
        }
        for (i = 0; i < m->field_E1B; i++) {
            restore_row = m->field_2C8[m->field_E0F][i];
            src = (u16 *)(
                (u32)m->field_DD8 + (u32)&((s32 *)0)[restore_row]);
            if (restore_row != 0xFFFF) {
                restore_bit = i;
                if (i < 0) {
                    restore_bit = i + 7;
                }
                restore_words = 3;
                if ((m->field_BEC[restore_bit >> restore_words] >>
                     (i - ((restore_bit >> restore_words) << restore_words))) &
                    1) {
                    restore_words = 5;
                }
                func_8005B620((s32 *)(base + src[0] * 4), (const s32 *)dst,
                              restore_words);
                dst += restore_words * 4;
            }
        }
        m->field_E0F = 0;
        break;
    default:
        prev = m->field_E0F;
        if (anim != m->field_DFE + 3) {
            m->field_E0E = 2;
        }
        if (m->field_750[anim].max == 0) {
            return;
        }
        if (prev != 0) {
            if (flag == 0) {
                return;
            }
            m->field_BF6 = m->field_BF4;
            Model_ControlSlotAnimation(index, 0, 0);
        }
        if (anim == m->field_DFE + 3
            || m->field_DF8 == MODEL_SPECIAL_BATTLE_ID) {
            m->field_E0E = 7;
        }
        m->field_E0F = anim;
        if (prev == anim) {
            func_800597C8(index, 0, 0);
            return;
        }
        m->field_E10 = 1;
        for (i = 0; i < m->field_E1B; parts++, i++) {
            copy_row = m->field_2C8[m->field_E0F][i];
            src = (u16 *)(
                (u32)m->field_DD8 + (u32)&((s32 *)0)[copy_row]);
            if (copy_row != 0xFFFF) {
                copy_bit = i;
                if (i < 0) {
                    copy_bit = i + 7;
                }
                copy_words = 3;
                if ((m->field_BEC[copy_bit >> copy_words] >>
                     (i - ((copy_bit >> copy_words) << copy_words))) &
                    1) {
                    copy_words = 5;
                }
                (*parts)->ii = ((u8 *)src - (u8 *)m->field_DD8) >> 2;
                func_8005B620((s32 *)dst, (const s32 *)(base + src[0] * 4),
                              copy_words);
                dst += copy_words * 4;
            }
        }
        break;
    }
}
