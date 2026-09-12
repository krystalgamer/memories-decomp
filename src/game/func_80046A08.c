#include "../types.h"
#include "../psyq/stdio.h"
#include "../unmatched.h"
#include "func_80047788.h"
#include "sound.h"
#include "sound_output.h"

/* MATCH 2026-09-06, first-day function from the m2c draft (248 instructions,
 * the sound bank loader state machine: SE header, SE groups, then the CD and
 * SMF mixer-out banks with their "VolInf" signature checks). Jump table on
 * the state word, so only the full build proves it. Levers, in order: the
 * 8-byte header copied as ONE packed-struct assignment (retail batches the
 * two lwl/lwr pairs before the swl/swr pairs); the state increment written
 * INLINE in every arm rather than as a shared tail (retail cross-jumps the
 * identical ones itself); the shared func_800470B0 call block placed between
 * cases 8 and 9 -- where retail has it -- so the return block is preceded by
 * the default arm and no store gets cross-jumped into it; case 8's 0x3E7
 * arm out of line after that call block through a goto; the scaled argument
 * in case 2 as two statements against one name; the buffer base assigned
 * before the signature test; the two signature bytes D_801E27F8/D_801E8FF8
 * in .data (bare form beside the gp-relative g_SDValue); and the first
 * signature byte held in a u8 local, whose zero-extension is the andi 0xFF
 * retail keeps in a second register for the last comparison.
 */

void func_80046A08(void) {
    u8 *b;
    u8 *a0;
    s32 a1;
    s32 a2;
    s32 a3;
    u8 c;

    switch (g_SDValue->field_003C) {
    case 0:
        if (g_SDValue->flags_004A & 1) {
            a0 = (u8 *)0x801E6800;
            a1 = 0;
            a2 = 0x800;
            a3 = 0x10;
            goto call;
        }
        g_SDValue->field_003C += 6;
        return;
    case 1:
        if ((func_8004703C() & 7) == 0) {
            *(SDBankHeaderWords *)g_SDValue =
                *(SDBankHeaderWords *)0x801E6800;
            g_SDValue->field_003C += 1;
            return;
        }
        return;
    case 2:
        printf(D_800107A8, g_SDValue->field_0002);
        a0 = (u8 *)0x801E2800;
        a1 = 1;
        a2 = g_SDValue->field_0002;
        a3 = 0x10;
        a2 = a2 * 8;
        goto call;
    case 3:
        if ((func_8004703C() & 7) == 0) {
            func_80047480();
            g_SDValue->field_003C += 1;
            return;
        }
        break;
    case 4:
        func_80047788(0);
        g_SDValue->field_003C += 1;
        return;
    case 5:
        if (func_8004703C() & 7) {
            return;
        }
        g_SDValue->field_003C += 1;
        return;
    case 6:
        if (g_SDValue->flags_004A & 2) {
            a0 = (u8 *)0x801E2000;
            a1 = 0;
            a2 = 0x800;
            a3 = 0x20;
            goto call;
        }
        g_SDValue->field_003C += 2;
        return;
    case 7:
        if ((func_8004703C() & 7) == 0) {
            b = (u8 *)0x801E2000;
            if (D_801E27F8 == 0x56) {
                c = b[0x7F9];
                if (c == 0x6F && 0x6C == b[0x7FA] && 0x49 == b[0x7FB] && 0x6E == b[0x7FC] && 0x66 == b[0x7FD] && b[0x7FE] == c) {
                    g_SDValue->field_164A = b[0x7FF];
                    g_SDValue->field_0044 = g_SDValue->field_164A;
                    printf(D_800107DC, g_SDValue->field_164A);
                }
            }
            g_SDValue->field_003C += 1;
            return;
        }
        break;
    case 8:
        if ((g_SDValue->flags_004A & 0x80) == 0) {
            goto no8;
        }
        a0 = (u8 *)0x801E8800;
        a1 = 0;
        a2 = 0x2000;
        a3 = 0x40;
    call:
        func_800470B0((s32)a0, a1, a2, a3);
        g_SDValue->field_003C += 1;
        return;
    no8:
        g_SDValue->field_003C = 0x3E7;
        return;
    case 9:
        if ((func_8004703C() & 7) == 0) {
            b = (u8 *)0x801E8800;
            if (D_801E8FF8 == 0x56) {
                c = b[0x7F9];
                if (c == 0x6F && 0x6C == b[0x7FA] && 0x49 == b[0x7FB] && 0x6E == b[0x7FC] && 0x66 == b[0x7FD] && b[0x7FE] == c) {
                    g_SDValue->field_1649 = b[0x7FF];
                    g_SDValue->mix_scale = g_SDValue->field_1649;
                    printf(D_800107F4, g_SDValue->field_1649);
                }
            }
            g_SDValue->bank_0518[0] = (u8 *)0x801E8800;
            g_SDValue->bank_0518[1] = (u8 *)0x801E9000;
            g_SDValue->bank_0518[2] = (u8 *)0x801EA000;
            g_SDValue->field_003C += 1;
            return;
        }
        break;
    default:
        if ((func_8004703C() & 7) == 0) {
            g_SDValue->flags_0040 &= 0xFFF7;
        }
        break;
    }
    return;
}
