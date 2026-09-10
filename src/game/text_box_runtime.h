#ifndef MEMORIES_DECOMP_TEXT_BOX_RUNTIME_H
#define MEMORIES_DECOMP_TEXT_BOX_RUNTIME_H

#include "../types.h"

/* Declared through the tag rather than the typedef, and duel_effect.h is
   deliberately not included: src/overlays/free_duel/cursor_layout.c reaches
   this header and carries its own `extern u8 D_800EB0F8[]`, which the shared
   declaration would collide with. That local one is a matching lever there. */
struct DuelEffectChannel;
void TextBox_BuildStep(struct DuelEffectChannel *object);

/* The choice index TextBox_BuildStep publishes for the pump to act on. It
 * stores the script word it has just read -- `D_8009B35A = D_8009B33A` -- and
 * func_80039794 is the consumer:
 *
 *     idx = D_8009B35A;
 *     if (idx >= 0) { kind = D_801D9000[idx].hi & 7; ... }
 *
 * -1 means "nothing selected": func_80039794 arms it with that before pumping
 * and again at each reset, and only a non-negative value indexes the table.
 *
 * That `>= 0` is why it is signed, even though the value written comes
 * straight out of the u16 at D_8009B33A. The reader needs the sentinel, so
 * the reader fixes the type. */
extern s16 D_8009B35A;
void func_80039140(u8 *record);
void TextBox_SetPos(u8 *record, s32 x, s32 y);

/* Builds the display objects a record draws through, and is reached only from
 * TextBox_BuildStep above. It creates the object at +0x28 on first use, then
 * on every call copies the record's four geometry halfwords at 0x3C, 0x40,
 * 0x3E and 0x42 into it and derives two half-extents from the latter pair.
 *
 * When bit 0x20 of the halfword at 0x34 is set it also builds the second
 * object at +0x2C, releasing any previous one, and finishes by calling
 * func_80039140 on the same record -- which is why it is declared beside it
 * rather than with the display-object headers whose fields it fills. The
 * DisplayObjects are ones it allocates; the record is what it operates on. */
void func_800391E4(u8 *record);

/* Requests a build and pumps the text box until it reports done. The two
   entry points differ only in the flag word they set first. */
void func_80039A14(u8 *object);
void func_80039A60(u8 *object);

#endif
