#include "../types.h"
#include "display_object_core.h"
#include "display_object_layout.h"
#include "script_image_objects.h"
#include "script_image_rebuild.h"
#include "display_object.h"
#include "scene_image_overlay_tables.h"
#include "display_object_config.h"

/* MATCH. Real C -- this retires the ASSEMBLY-DEBT transcription (Unchiga's
 * port, a 24-instruction `__asm__` block with s0/s1 pins) that stood here.
 * gp=0, at=0, no addressing work; default flags.
 *
 * Rebuilt from the m2c draft, 36 differences to 0, five levers, every one a
 * rule already in WORKFLOW:
 *   - +0x3C and p+4 (slot 0's value) are HALFWORDS. Stored through `s32`
 *     the CENSUS read `lw +1, sw +2, sh -2, lh -1`; the difference count
 *     pointed at neither.
 *     36 -> 33, empty census.
 *   - `n` is REUSED for the decoded BCD index -- retail's `addu $s1,$v1,$v0`
 *     writes the index back into the argument's own register. 33 -> 22.
 *   - the slot's field_10 store (then `p[0x10] = 1;`) goes BELOW the two
 *     read-modify-writes. 22 -> 14. The
 *     August park said "above (22 -> 19)", measured before the `n` reuse.
 *   - the D_80090BA8 base local is assigned AFTER the else arm's index, not
 *     before (14 -> 9), and that index is `s32 k`, not `s16 k` (9 -> 5).
 *     The August entry recorded `s16` as 8 and `s32` as 19, both measured
 *     with the three faults above open -- a spelling rejected on a broken
 *     base has not been measured.
 *   - and the last five: the flag byte REUSES `n` as well. Retail reads it
 *     `lbu $s1,0($s0)` -- into the register `n` has just vacated, which is
 *     one pseudo, not two. A separate `m` gets $s0/$s1 exchanged at all
 *     five sites. Same rule as the decode above, one value further on.
 *
 * Measured and DEAD on the finished base: `do { } while (0);` round the
 * element read (5, no change) or round the base assignment (15); the base
 * written inline with no local (+4/150); `b` assigned before the index (10).
 * And earlier, on the 9-difference base: dropping the base local (+4/145),
 * `u8 m` / `s16 m` / the flag read as `*(u8 *)(b + k*2)` (all 9), four
 * declaration orders, a named entry pointer (all 9), borrowing the other
 * arm's `t` (+2/145) or `o` (35).
 *
 * The record's three 0x14-byte image slots are ScriptImageEntry records,
 * reached as ((ScriptImageEntry *)p)[0..2], and the object each one holds
 * is a DisplayObject. ScriptImageObjectSet names the image-id halfword at
 * +0x3C immediately after those three slots.
 */

void ScriptImage_RebuildObjects(ScriptImageObjectSet *p, s32 arg1) {
    s32 n;
    u8 *o;
    u8 *t;
    u8 *b;
    s32 k;

    n = arg1;
    if (n < 0) {
        n = p->image_id;
    }
    p->image_id = n;
    if (n >= 0x200) {
        o = func_800400AC(func_8004002C(), 3);
        func_80040510(o, 0, 0, 0x200, 0x100, 0, 0, 0x10, 0, 0xF0);
        n = ((n >> 4) & 0xF) * 10 + (n & 0xF);
        ((DisplayObject *)o)->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
        ((DisplayObject *)o)->flags &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        p->entries[0].field_10 = 1;
        p->entries[0].pointer = o;
        t = &D_80090C00[n * 6];
        p->entries[0].value = 0;
        if (t[0] & 1) {
            ScriptImage_CreateObject((u8 *)&p->entries[1],
                                     0x14, 1);
            ((DisplayObject *)p->entries[1].pointer)
                ->field_30.h.field_30 = t[1];
            ((DisplayObject *)p->entries[1].pointer)
                ->field_30.h.field_32 = t[2];
        }
        if (t[0] & 2) {
            ScriptImage_CreateObject((u8 *)&p->entries[2],
                                     0x16, 2);
            ((DisplayObject *)p->entries[2].pointer)
                ->field_30.h.field_30 = t[3];
            ((DisplayObject *)p->entries[2].pointer)
                ->field_30.h.field_32 = t[4];
            p->entries[2].value = t[5];
            if (t[0] & 0x80) {
                ((DisplayObject *)p->entries[2].pointer)->flags |=
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
            }
        }
    } else {
        ScriptImage_CreateObject((u8 *)p, 0x10, 0);
        if (n >= 0x100) {
            k = ((n >> 4) & 0xF) * 10 + (n & 0xF);
            b = D_80090BA8;
            n = b[k * 2];
            if (n & 1) {
                ScriptImage_CreateObject((u8 *)&p->entries[1],
                                         0x12, 1);
            }
            if (n & 2) {
                ScriptImage_CreateObject((u8 *)&p->entries[2],
                                         0x14, 2);
                p->entries[2].value = b[k * 2 + 1];
            }
        } else {
            p->entries[1].pointer = 0;
            p->entries[2].pointer = 0;
        }
    }
}
