/*
 * Recomputes a model's audible radius. Current best: 252 instructions
 * against 251, opcode distance 5.
 *
 * The magnitude of the s16 at +0x20 sets the base radius at +0x22, capped at
 * 0x4000; when that field is negative the two 8-byte descriptors after the
 * header are each scored and the largest kept. Type 0x80/0x81 asks
 * func_80058DD8 whether the slot named by the low seven bits is live, pulls
 * its position with Model_CopySlotU16Values and measures the distance to the
 * matching entry in D_800F5768. Type 1 has func_8005EBF4 fill a ten-point
 * path and sums its nine segments. Type 4 scales a fixed 6284 per unit. All
 * three divide by 1000 against the same 0x10624DD3 magic.
 *
 * Structure is exact and the case bodies must stay in retail's layout order
 * (0x80/0x81, then 1, then 4) rather than numeric order. Residual: case 4
 * addresses D_800F5768 + 0x10 twice and retail materialises the base once,
 * where this build emits the assembler macro form twice -- a u8 * local does
 * not survive, GCC constant-propagates it back into both references. Plus a
 * mult duplicated into a branch delay slot, and p and q swapped between $s4
 * and $s5.
 */
#include "../types.h"
#include "../game/camera_view.h"
#include "../game/model_copy_slot_u16_values.h"
#include "../game/func_80058DD8.h"
#include "../game/func_8005EBF4.h"
#include "../game/model_transfer_flags.h"
#include "../game/model_transfer_state.h"

/* Recomputes a model's audible radius. The magnitude of the s16 at +0x20 sets
   the base radius at +0x22, capped at 0x4000, and when that field is negative
   the two 8-byte descriptors that follow the header are each scored and the
   largest result wins: type 0x80/0x81 measures the straight distance from the
   descriptor's offset position to the matching entry in D_800F5768, type 1
   sums the segment lengths of a ten-point path filled in by func_8005EBF4,
   and type 4 scales a fixed 6284 per unit by the descriptor's second word. */
void func_8005E808(u8 *p)
{
    s16 pos[3];
    s16 buf[10][3];
    u8 *e;
    u8 *q;
    u8 *g;
    s32 i;
    s32 j;
    s32 off;
    s32 a;
    s32 k;
    s32 sum;
    s32 t;
    u32 d;
    s32 dx;
    s32 dy;
    s32 dz;
    s32 slot;
    u8 *prev;
    s32 n;
    u16 v;

    n = ((Key *)p)->magnitude;
    a = n;
    if (n < 0) {
        a = -a;
    }
    t = a;
    if (a <= 0) {
        t = 1;
    }
    k = t * 2;
    func_8005FB30((Key *)p);
    ((Key *)p)->progress = 0;
    if (D_8009B074->ready != 0) {
        return;
    }
    ((Key *)p)->ready = 1;
    if (a >= 0x4000) {
        ((Key *)p)->radius = 0x4000;
    } else {
        ((Key *)p)->radius = k;
    }
    if (((Key *)p)->magnitude >= 0) {
        return;
    }

    e = p;
    g = (u8 *)D_800F5768;
    for (i = 0; i < 2; i++, e += 8, g += 8) {
        q = e + 2;
        sum = 0;
        switch (*(s16 *)(q + 4)) {
        case 0x80:
        case 0x81:
            slot = (s16)(*(u16 *)(q + 4) & 0xFF7F);
            if (func_80058DD8(slot) != 1) {
                continue;
            }
            Model_CopySlotU16Values(slot, pos);
            pos[0] = pos[0] + *(u16 *)e;
            pos[1] = pos[1] + *(u16 *)q;
            pos[2] = pos[2] + *(u16 *)(q + 2);
            dx = pos[0] - *(s16 *)g;
            dy = pos[1] - *(s16 *)(g + 2);
            dz = pos[2] - *(s16 *)(g + 4);
            d = k * SquareRoot0(dx * dx + dy * dy + dz * dz) / 1000;
            if (((Key *)p)->radius < d) {
                ((Key *)p)->radius = d;
            }
            continue;
        case 1:
            for (j = 0; j < 10; j++) {
                func_8005EBF4((Key *)p, i, j, 0x1E, buf[j]);
            }
            off = 0;
            for (j = 1; j < 10; j++) {
                prev = (u8 *)buf + off;
                dx = *(s16 *)prev - buf[j][0];
                dy = *(s16 *)(prev + 2) - buf[j][1];
                dz = *(s16 *)(prev + 4) - buf[j][2];
                sum += SquareRoot0(dx * dx + dy * dy + dz * dz);
                off += 6;
            }
            d = k * sum / 1000;
            break;
        case 4:
            v = *(u16 *)((u8 *)D_800F5768 + 0x10);
            if (*(s16 *)((u8 *)D_800F5768 + 0x10) < *(s16 *)e) {
                v = *(u16 *)e;
            }
            t = (s16)v * 6284 / 1000;
            t = k * t;
            slot = *(s16 *)q;
            if (slot < 0) {
                slot = -slot;
            }
            d = (u32)(t * slot) / 4096000;
            break;
        default:
            continue;
        }
        if (((Key *)p)->radius < d) {
            ((Key *)p)->radius = d;
        }
    }
}
