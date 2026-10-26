/*
 * Model_PrepareSharedPrimitiveCaches (0x80067220-0x80067354).
 *
 * Hand-written in the original: no stack frame, the argument read from $a0
 * and the next primitive block returned in $v0. It projects a block's shared
 * vertices into vertop2 (SXY2 plus an IR0 << 16 | SZ3 word, or -1 when RTPS
 * overflowed) and, in lit modes, lights its shared normals into nortop2.
 * Register variables and the handwritten_asm.h macros reproduce that
 * contract; see notes/shared-primitive-renderers.md.
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "handwritten_asm.h"
#include "model_handler_state.h"
#include "model_primitive_templates.h"
#include "primitive_handler_modes.h"
#include "shared_primitive_renderers.h"

register u32 gHw_At asm("$1");
register s32 gHw_Empty asm("$25");
register u32 gHw_ByteMask asm("$12");

u32 *Model_PrepareSharedPrimitiveCaches(GsARGUNIT_SHARED *arg)
{
    register u32 *rec asm("$7");
    register u32 rgb asm("$13");
    register s32 mode asm("$14");
    register u32 off asm("$15");
    register u32 count asm("$24");
    register u32 *src asm("$5");
    register u32 *dst asm("$6");
    register u32 lo asm("$2");
    register u32 hi asm("$3");
    HW_KEEP_LABEL(verts_done);
    HW_KEEP_LABEL(done);

    rec = (u32 *)arg->primp;
    rgb = gModel_dwPolyGt3Template;
    mode = (s8)gModel_bCurrentDrawMode;
    off = gModel_dwLitColorCacheBase;
    gHw_At = 0xFFFFFF;
    rgb &= gHw_At;
    HW_GTE_WRITE(6, rgb);
    if (off != 0) {
        off = 4;
    }
    count = rec[1];
    gHw_Empty = -1;
    gHw_ByteMask = 0xFF;
    HW_BEQZ_LOAD(count, verts_done, lo, 8, rec);
    {
        src = (u32 *)arg->vertop;
        lo <<= 3;
        src = (u32 *)((u8 *)src + lo);
        hi = rec[3];
        dst = (u32 *)arg->vertop2;
        hi <<= 3;
        dst = (u32 *)((u8 *)dst + hi);
        lo = src[0];
        hi = src[1];
        do {
            HW_GTE_WRITE(0, lo);
            HW_GTE_WRITE(1, hi);
            count--;
            src += 2;
            HW_GTE_RTPS_RAW();
            lo = src[0];
            hi = src[1];
            gHw_At = 1;
            dst[1] = gHw_Empty;
            if (mode != gHw_At) {
                register s32 flag asm("$11");
                HW_GTE_READ_FLAG_WAIT(flag);
                if (flag < 0) {
                    goto next;
                }
            }
            {
                register u32 depth asm("$8");
                register u32 z asm("$9");
                HW_GTE_STORE(14, 0, dst);
                HW_GTE_READ(8, depth);
                HW_GTE_READ(19, z);
                depth <<= 16;
                depth |= z;
                dst[1] = depth;
            }
        next:
            dst += 2;
        } while (count != 0);
    }
verts_done:
    HW_BEQZ_LOAD(mode, done, count, 16, rec);
    {
        lo = rec[5];
        HW_BEQZ_LOAD(count, done, src, 32, arg);
        {
            lo <<= 3;
            src = (u32 *)((u8 *)src + lo);
            hi = rec[6];
            dst = (u32 *)arg->nortop2;
            hi <<= 3;
            dst = (u32 *)((u8 *)dst + hi);
            dst = (u32 *)((u8 *)dst + off);
            lo = src[0];
            hi = src[1];
            do {
                HW_GTE_WRITE(0, lo);
                HW_GTE_WRITE(1, hi);
                count--;
                src += 2;
                HW_GTE_NCCS_RAW();
                lo = src[0];
                hi = src[1];
                HW_GTE_STORE(22, 0, dst);
                dst += 2;
            } while (count != 0);
        }
    }
done:
    {
        register u32 *ret asm("$2");
        HW_KEEP_LABEL(ret_slot);

        ret = rec + 7;
    ret_slot:
        return ret;
    }
}
