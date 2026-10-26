/*
 * Shared Gt3/Gt4 polygon drawers (0x80067354-0x80069E44).
 *
 * Hand-written in the original: no stack frame, borrowed s-registers parked
 * in scratch words of the GsARGUNIT block, the argument read from $a0 and the
 * next primitive block returned in $v0. Register variables and the
 * handwritten_asm.h macros reproduce that contract; see
 * notes/shared-primitive-renderers.md.
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
#include "gpu_packets.h"
#include "shared_primitive_renderers.h"

register u32 gHw_PolyTemplate asm("$16");
register u32 gHw_BlendBits asm("$17");
register u32 gHw_SecondClut asm("$18");
register u32 gHw_PolyCode asm("$19");
register u32 gHw_WindowCode asm("$20");
register u32 gHw_WindowWord asm("$21");
register s32 gHw_DrawMode asm("$22");
register u32 gHw_ColorCache asm("$23");
register u32 gHw_At asm("$1");
register u32 gHw_Word0 asm("$8");
register u32 gHw_Second asm("$3");
/*
 * $30 is unused by the originals. Reserving it leaves each function's "zero"
 * pseudo without a register, so reload substitutes the constant and
 * "gHw_Zero - zero" assembles to the original "li reg, 0" while
 * "gHw_Zero <<= zero" assembles to the original delay-slot nop.
 */
register u32 gHw_Zero asm("$0");
register u32 gHw_Color asm("$12");
register u32 gHw_Word3 asm("$11");
register u32 gHw_Unused30 asm("$30");

u32 *Model_DrawSharedGt3Primitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        HW_KEEP_LABEL(mode_test);
        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt3Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
    mode_test:
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[3];
            w1 = ((volatile u32 *)rec)[4];
            w2 = ((volatile u32 *)rec)[5];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[2] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[5] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[8] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            HW_GTE_NCLIP();
            w1 <<= 16;
            w1 >>= 13;
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word0 += (u32)normals;
            HW_GTE_READ(24, z);
            w1 += (u32)normals;
            w2 += (u32)normals;
            if (z <= 0)
                goto next;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            gHw_Word0 |= gHw_PolyCode;
            pkt[1] = gHw_Word0;
            pkt[4] = w1;
            pkt[7] = w2;
            HW_GTE_AVSZ3();
            gHw_Word0 = ((volatile u32 *)rec)[0];
            w1 = ((volatile u32 *)rec)[1];
            w2 = ((volatile u32 *)rec)[2];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[3] = gHw_Word0;
            pkt[6] = w1;
            pkt[9] = w2;
            tag = (tag << 8) | 9;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[2];
                pkt[11] = gHw_Color;
                pkt[12] = gHw_Second;
                gHw_Second = pkt[5];
                pkt[14] = gHw_Color;
                pkt[15] = gHw_Second;
                gHw_Second = pkt[8];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                pkt += 10;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 10;
        next:
            count--;
            rec += 24;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}

u32 *Model_DrawSharedGt4Primitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        HW_KEEP_LABEL(mode_test);
        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt4Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
    mode_test:
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            HW_KEEP_LABEL(cull_test);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[3];
            HW_LWR(gHw_Word0, 10, rec);
            w1 = ((volatile u32 *)rec)[4];
            w2 = ((volatile u32 *)rec)[5];
            gHw_Word3 = ((volatile u32 *)rec)[6];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(16, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[2] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[5] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[8] = e;
            e = gHw_Word3 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            pkt[11] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            HW_GTE_NCLIP();
            w1 <<= 16;
            HW_GTE_READ(24, z);
            w1 >>= 13;
        cull_test:
            if (z > 0)
                goto visible;
            HW_GTE_WRITE(12, e);
            HW_GTE_NCLIP_MAC0_WAIT(z);
            if (z > 0)
                goto next;
        visible:
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word3 <<= 16;
            gHw_Word3 >>= 13;
            gHw_Word0 += (u32)normals;
            w1 += (u32)normals;
            w2 += (u32)normals;
            gHw_Word3 += (u32)normals;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            gHw_Word3 = *(volatile u32 *)gHw_Word3;
            gHw_Word0 |= gHw_PolyCode;
            pkt[1] = gHw_Word0;
            pkt[4] = w1;
            pkt[7] = w2;
            pkt[10] = gHw_Word3;
            HW_GTE_AVSZ4();
            gHw_Word0 = ((volatile u32 *)rec)[0];
            w1 = ((volatile u32 *)rec)[1];
            w2 = ((volatile u32 *)rec)[2];
            gHw_Word3 = ((volatile u32 *)rec)[3];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[3] = gHw_Word0;
            pkt[6] = w1;
            pkt[9] = w2;
            pkt[12] = gHw_Word3;
            tag = (tag << 8) | 12;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[2];
                pkt[14] = gHw_Color;
                pkt[15] = gHw_Second;
                gHw_Second = pkt[5];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                gHw_Second = pkt[8];
                pkt[20] = gHw_Color;
                pkt[21] = gHw_Second;
                gHw_Second = pkt[11];
                pkt[23] = gHw_Color;
                pkt[24] = gHw_Second;
                pkt += 13;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 13;
        next:
            count--;
            rec += 28;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}

u32 *Model_DrawSharedGt3WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_SAVE_REGS2(args, 20, 0x38, 21, 0x3C);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt3Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
        gHw_WindowCode = 0xE2000000;
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[4];
            w1 = ((volatile u32 *)rec)[5];
            w2 = ((volatile u32 *)rec)[6];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[3] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[6] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[9] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            HW_GTE_NCLIP();
            w1 <<= 16;
            w1 >>= 13;
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word0 += (u32)normals;
            HW_GTE_READ(24, z);
            w1 += (u32)normals;
            w2 += (u32)normals;
            if (z <= 0)
                goto next;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            gHw_Word0 |= gHw_PolyCode;
            pkt[2] = gHw_Word0;
            pkt[5] = w1;
            pkt[8] = w2;
            HW_GTE_AVSZ3();
            gHw_WindowWord = ((volatile u32 *)rec)[0];
            gHw_Word0 = ((volatile u32 *)rec)[1];
            w1 = ((volatile u32 *)rec)[2];
            w2 = ((volatile u32 *)rec)[3];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[4] = gHw_Word0;
            pkt[7] = w1;
            pkt[10] = w2;
            pkt[1] = gHw_WindowWord;
            pkt[11] = gHw_WindowCode;
            tag = (tag << 8) | 11;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[3];
                pkt[14] = gHw_Color;
                pkt[15] = gHw_Second;
                gHw_Second = pkt[6];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                gHw_Second = pkt[9];
                pkt[20] = gHw_Color;
                pkt[21] = gHw_Second;
                pkt += 12;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 12;
        next:
            count--;
            rec += 28;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_RESTORE_REGS2(args, 20, 0x38, 21, 0x3C);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}

u32 *Model_DrawSharedGt4WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_SAVE_REGS2(args, 20, 0x38, 21, 0x3C);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt4Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
        gHw_WindowCode = 0xE2000000;
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            HW_KEEP_LABEL(cull_test);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[4];
            HW_LWR(gHw_Word0, 14, rec);
            w1 = ((volatile u32 *)rec)[5];
            w2 = ((volatile u32 *)rec)[6];
            gHw_Word3 = ((volatile u32 *)rec)[7];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(16, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[3] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[6] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[9] = e;
            e = gHw_Word3 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            pkt[12] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            HW_GTE_NCLIP();
            w1 <<= 16;
            HW_GTE_READ(24, z);
            w1 >>= 13;
        cull_test:
            if (z > 0)
                goto visible;
            HW_GTE_WRITE(12, e);
            HW_GTE_NCLIP_MAC0_WAIT(z);
            if (z > 0)
                goto next;
        visible:
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word3 <<= 16;
            gHw_Word3 >>= 13;
            gHw_Word0 += (u32)normals;
            w1 += (u32)normals;
            w2 += (u32)normals;
            gHw_Word3 += (u32)normals;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            gHw_Word3 = *(volatile u32 *)gHw_Word3;
            gHw_Word0 |= gHw_PolyCode;
            pkt[2] = gHw_Word0;
            pkt[5] = w1;
            pkt[8] = w2;
            pkt[11] = gHw_Word3;
            HW_GTE_AVSZ4();
            gHw_WindowWord = ((volatile u32 *)rec)[0];
            gHw_Word0 = ((volatile u32 *)rec)[1];
            w1 = ((volatile u32 *)rec)[2];
            w2 = ((volatile u32 *)rec)[3];
            gHw_Word3 = ((volatile u32 *)rec)[4];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[4] = gHw_Word0;
            pkt[7] = w1;
            pkt[10] = w2;
            pkt[13] = gHw_Word3;
            pkt[1] = gHw_WindowWord;
            pkt[14] = gHw_WindowCode;
            tag = (tag << 8) | 14;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[3];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                gHw_Second = pkt[6];
                pkt[20] = gHw_Color;
                pkt[21] = gHw_Second;
                gHw_Second = pkt[9];
                pkt[23] = gHw_Color;
                pkt[24] = gHw_Second;
                gHw_Second = pkt[12];
                pkt[26] = gHw_Color;
                pkt[27] = gHw_Second;
                pkt += 15;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 15;
        next:
            count--;
            rec += 32;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_RESTORE_REGS2(args, 20, 0x38, 21, 0x3C);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}

u32 *Model_DrawSharedTwoSidedGt3Primitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        HW_KEEP_LABEL(mode_test);
        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt3Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
    mode_test:
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[3];
            w1 = ((volatile u32 *)rec)[4];
            w2 = ((volatile u32 *)rec)[5];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[2] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[5] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[8] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            w1 <<= 16;
            w1 >>= 13;
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word0 += (u32)normals;
            w1 += (u32)normals;
            w2 += (u32)normals;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            HW_GTE_AVSZ3();
            gHw_Word0 |= gHw_PolyCode;
            pkt[1] = gHw_Word0;
            pkt[4] = w1;
            pkt[7] = w2;
            gHw_Word0 = ((volatile u32 *)rec)[0];
            w1 = ((volatile u32 *)rec)[1];
            w2 = ((volatile u32 *)rec)[2];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[3] = gHw_Word0;
            pkt[6] = w1;
            pkt[9] = w2;
            tag = (tag << 8) | 9;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[2];
                pkt[11] = gHw_Color;
                pkt[12] = gHw_Second;
                gHw_Second = pkt[5];
                pkt[14] = gHw_Color;
                pkt[15] = gHw_Second;
                gHw_Second = pkt[8];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                pkt += 10;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 10;
        next:
            count--;
            rec += 24;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}

u32 *Model_DrawSharedTwoSidedGt4Primitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        HW_KEEP_LABEL(mode_test);
        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt4Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
    mode_test:
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[3];
            HW_LWR(gHw_Word0, 10, rec);
            w1 = ((volatile u32 *)rec)[4];
            w2 = ((volatile u32 *)rec)[5];
            gHw_Word3 = ((volatile u32 *)rec)[6];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(16, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[2] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[5] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[8] = e;
            e = gHw_Word3 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            pkt[11] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            w1 <<= 16;
            w1 >>= 13;
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word3 <<= 16;
            gHw_Word3 >>= 13;
            gHw_Word0 += (u32)normals;
            w1 += (u32)normals;
            w2 += (u32)normals;
            gHw_Word3 += (u32)normals;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            gHw_Word3 = *(volatile u32 *)gHw_Word3;
            HW_GTE_AVSZ4();
            gHw_Word0 |= gHw_PolyCode;
            pkt[1] = gHw_Word0;
            pkt[4] = w1;
            pkt[7] = w2;
            pkt[10] = gHw_Word3;
            gHw_Word0 = ((volatile u32 *)rec)[0];
            w1 = ((volatile u32 *)rec)[1];
            w2 = ((volatile u32 *)rec)[2];
            gHw_Word3 = ((volatile u32 *)rec)[3];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[3] = gHw_Word0;
            pkt[6] = w1;
            pkt[9] = w2;
            pkt[12] = gHw_Word3;
            tag = (tag << 8) | 12;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[2];
                pkt[14] = gHw_Color;
                pkt[15] = gHw_Second;
                gHw_Second = pkt[5];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                gHw_Second = pkt[8];
                pkt[20] = gHw_Color;
                pkt[21] = gHw_Second;
                gHw_Second = pkt[11];
                pkt[23] = gHw_Color;
                pkt[24] = gHw_Second;
                pkt += 13;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 13;
        next:
            count--;
            rec += 28;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}

u32 *Model_DrawSharedTwoSidedGt3WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_SAVE_REGS2(args, 20, 0x38, 21, 0x3C);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt3Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
        gHw_WindowCode = 0xE2000000;
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[4];
            w1 = ((volatile u32 *)rec)[5];
            w2 = ((volatile u32 *)rec)[6];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[3] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[6] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[9] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            w1 <<= 16;
            w1 >>= 13;
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word0 += (u32)normals;
            w1 += (u32)normals;
            w2 += (u32)normals;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            HW_GTE_AVSZ3();
            gHw_Word0 |= gHw_PolyCode;
            pkt[2] = gHw_Word0;
            pkt[5] = w1;
            pkt[8] = w2;
            gHw_WindowWord = ((volatile u32 *)rec)[0];
            gHw_Word0 = ((volatile u32 *)rec)[1];
            w1 = ((volatile u32 *)rec)[2];
            w2 = ((volatile u32 *)rec)[3];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[4] = gHw_Word0;
            pkt[7] = w1;
            pkt[10] = w2;
            pkt[1] = gHw_WindowWord;
            pkt[11] = gHw_WindowCode;
            tag = (tag << 8) | 11;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[3];
                pkt[14] = gHw_Color;
                pkt[15] = gHw_Second;
                gHw_Second = pkt[6];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                gHw_Second = pkt[9];
                pkt[20] = gHw_Color;
                pkt[21] = gHw_Second;
                pkt += 12;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 12;
        next:
            count--;
            rec += 28;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_RESTORE_REGS2(args, 20, 0x38, 21, 0x3C);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}

u32 *Model_DrawSharedTwoSidedGt4WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_SAVE_REGS2(args, 20, 0x38, 21, 0x3C);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt4Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
        gHw_WindowCode = 0xE2000000;
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[4];
            HW_LWR(gHw_Word0, 14, rec);
            w1 = ((volatile u32 *)rec)[5];
            w2 = ((volatile u32 *)rec)[6];
            gHw_Word3 = ((volatile u32 *)rec)[7];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(16, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[3] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[6] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[9] = e;
            e = gHw_Word3 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            pkt[12] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            w1 <<= 16;
            w1 >>= 13;
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word3 <<= 16;
            gHw_Word3 >>= 13;
            gHw_Word0 += (u32)normals;
            w1 += (u32)normals;
            w2 += (u32)normals;
            gHw_Word3 += (u32)normals;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            gHw_Word3 = *(volatile u32 *)gHw_Word3;
            gHw_Word0 |= gHw_PolyCode;
            pkt[2] = gHw_Word0;
            pkt[5] = w1;
            pkt[8] = w2;
            pkt[11] = gHw_Word3;
            HW_GTE_AVSZ4();
            gHw_WindowWord = ((volatile u32 *)rec)[0];
            gHw_Word0 = ((volatile u32 *)rec)[1];
            w1 = ((volatile u32 *)rec)[2];
            w2 = ((volatile u32 *)rec)[3];
            gHw_Word3 = ((volatile u32 *)rec)[4];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[4] = gHw_Word0;
            pkt[7] = w1;
            pkt[10] = w2;
            pkt[13] = gHw_Word3;
            pkt[1] = gHw_WindowWord;
            pkt[14] = gHw_WindowCode;
            tag = (tag << 8) | 14;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[3];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                gHw_Second = pkt[6];
                pkt[20] = gHw_Color;
                pkt[21] = gHw_Second;
                gHw_Second = pkt[9];
                pkt[23] = gHw_Color;
                pkt[24] = gHw_Second;
                gHw_Second = pkt[12];
                pkt[26] = gHw_Color;
                pkt[27] = gHw_Second;
                pkt += 15;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 15;
        next:
            count--;
            rec += 32;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_RESTORE_REGS2(args, 20, 0x38, 21, 0x3C);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}

u32 *Model_DrawSharedSemiGt3Primitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        HW_KEEP_LABEL(mode_test);
        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt3Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        {
            register s32 active asm("$2");
            HW_KEEP_LABEL(slot_nop);
            active = *(s8 *)&gModel_bBlendRateActive;
            gHw_At = 0x02000000;
            gHw_PolyTemplate |= gHw_At;
            gHw_Color = gHw_PolyTemplate;
            if (active != 0)
                goto no_second;
            gHw_BlendBits = ((volatile u32 *)rec)[1];
            gHw_At = 0x00600000;
            gHw_BlendBits &= gHw_At;
            if (gHw_BlendBits != 0)
                goto no_second;
        slot_nop:
            gHw_Zero <<= zero;
            if (gHw_SecondClut == 0)
                goto setup;
            gHw_SecondClut = *(u16 *)(rec + 2);
            gHw_Color = gModel_dwPolyGt3SemiTemplate;
            gHw_SecondClut <<= 16;
            gHw_BlendBits = 0x00200000;
            goto setup;
        no_second:
            gHw_SecondClut = gHw_Zero - zero;
        }
    setup:
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
    mode_test:
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[3];
            w1 = ((volatile u32 *)rec)[4];
            w2 = ((volatile u32 *)rec)[5];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[2] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[5] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[8] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            HW_GTE_NCLIP();
            w1 <<= 16;
            w1 >>= 13;
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word0 += (u32)normals;
            HW_GTE_READ(24, z);
            w1 += (u32)normals;
            w2 += (u32)normals;
            if (z <= 0)
                goto next;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            gHw_Word0 |= gHw_PolyCode;
            pkt[1] = gHw_Word0;
            pkt[4] = w1;
            pkt[7] = w2;
            HW_GTE_AVSZ3();
            gHw_Word0 = ((volatile u32 *)rec)[0];
            w1 = ((volatile u32 *)rec)[1];
            w2 = ((volatile u32 *)rec)[2];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            gHw_At = 0xFF9FFFFF;
            w1 &= gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[3] = gHw_Word0;
            pkt[6] = w1;
            pkt[9] = w2;
            tag = (tag << 8) | 9;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[2];
                pkt[11] = gHw_Color;
                pkt[12] = gHw_Second;
                gHw_Second = pkt[5];
                pkt[14] = gHw_Color;
                pkt[15] = gHw_Second;
                gHw_Second = pkt[8];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                pkt += 10;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 10;
        next:
            count--;
            rec += 24;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}

u32 *Model_DrawSharedSemiGt4Primitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        HW_KEEP_LABEL(mode_test);
        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt4Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        {
            register s32 active asm("$2");
            HW_KEEP_LABEL(slot_nop);
            active = *(s8 *)&gModel_bBlendRateActive;
            gHw_At = 0x02000000;
            gHw_PolyTemplate |= gHw_At;
            gHw_Color = gHw_PolyTemplate;
            if (active != 0)
                goto no_second;
            gHw_BlendBits = ((volatile u32 *)rec)[1];
            gHw_At = 0x00600000;
            gHw_BlendBits &= gHw_At;
            if (gHw_BlendBits != 0)
                goto no_second;
        slot_nop:
            gHw_Zero <<= zero;
            if (gHw_SecondClut == 0)
                goto setup;
            gHw_SecondClut = *(u16 *)(rec + 2);
            gHw_Color = gModel_dwPolyGt4SemiTemplate;
            gHw_SecondClut <<= 16;
            gHw_BlendBits = 0x00200000;
            goto setup;
        no_second:
            gHw_SecondClut = gHw_Zero - zero;
        }
    setup:
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
    mode_test:
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            HW_KEEP_LABEL(cull_test);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[3];
            HW_LWR(gHw_Word0, 10, rec);
            w1 = ((volatile u32 *)rec)[4];
            w2 = ((volatile u32 *)rec)[5];
            gHw_Word3 = ((volatile u32 *)rec)[6];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(16, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[2] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[5] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[8] = e;
            e = gHw_Word3 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            pkt[11] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            HW_GTE_NCLIP();
            w1 <<= 16;
            HW_GTE_READ(24, z);
            w1 >>= 13;
        cull_test:
            if (z > 0)
                goto visible;
            HW_GTE_WRITE(12, e);
            HW_GTE_NCLIP_MAC0_WAIT(z);
            if (z > 0)
                goto next;
        visible:
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word3 <<= 16;
            gHw_Word3 >>= 13;
            gHw_Word0 += (u32)normals;
            w1 += (u32)normals;
            w2 += (u32)normals;
            gHw_Word3 += (u32)normals;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            gHw_Word3 = *(volatile u32 *)gHw_Word3;
            gHw_Word0 |= gHw_PolyCode;
            pkt[1] = gHw_Word0;
            pkt[4] = w1;
            pkt[7] = w2;
            pkt[10] = gHw_Word3;
            HW_GTE_AVSZ4();
            gHw_Word0 = ((volatile u32 *)rec)[0];
            w1 = ((volatile u32 *)rec)[1];
            w2 = ((volatile u32 *)rec)[2];
            gHw_Word3 = ((volatile u32 *)rec)[3];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            gHw_At = 0xFF9FFFFF;
            w1 &= gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[3] = gHw_Word0;
            pkt[6] = w1;
            pkt[9] = w2;
            pkt[12] = gHw_Word3;
            tag = (tag << 8) | 12;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[2];
                pkt[14] = gHw_Color;
                pkt[15] = gHw_Second;
                gHw_Second = pkt[5];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                gHw_Second = pkt[8];
                pkt[20] = gHw_Color;
                pkt[21] = gHw_Second;
                gHw_Second = pkt[11];
                pkt[23] = gHw_Color;
                pkt[24] = gHw_Second;
                pkt += 13;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 13;
        next:
            count--;
            rec += 28;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}

u32 *Model_DrawSharedSemiGt3WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_SAVE_REGS2(args, 20, 0x38, 21, 0x3C);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt3Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        {
            register s32 active asm("$2");
            HW_KEEP_LABEL(slot_nop);
            active = *(s8 *)&gModel_bBlendRateActive;
            gHw_At = 0x02000000;
            gHw_PolyTemplate |= gHw_At;
            gHw_Color = gHw_PolyTemplate;
            if (active != 0)
                goto no_second;
            gHw_BlendBits = ((volatile u32 *)rec)[2];
            gHw_At = 0x00600000;
            gHw_BlendBits &= gHw_At;
            if (gHw_BlendBits != 0)
                goto no_second;
        slot_nop:
            gHw_Zero <<= zero;
            if (gHw_SecondClut == 0)
                goto setup;
            gHw_SecondClut = *(u16 *)(rec + 6);
            gHw_Color = gModel_dwPolyGt3SemiTemplate;
            gHw_SecondClut <<= 16;
            gHw_BlendBits = 0x00200000;
            goto setup;
        no_second:
            gHw_SecondClut = gHw_Zero - zero;
        }
    setup:
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
        gHw_WindowCode = 0xE2000000;
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[4];
            w1 = ((volatile u32 *)rec)[5];
            w2 = ((volatile u32 *)rec)[6];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[3] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[6] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[9] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            HW_GTE_NCLIP();
            w1 <<= 16;
            w1 >>= 13;
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word0 += (u32)normals;
            HW_GTE_READ(24, z);
            w1 += (u32)normals;
            w2 += (u32)normals;
            if (z <= 0)
                goto next;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            gHw_Word0 |= gHw_PolyCode;
            pkt[2] = gHw_Word0;
            pkt[5] = w1;
            pkt[8] = w2;
            HW_GTE_AVSZ3();
            gHw_WindowWord = ((volatile u32 *)rec)[0];
            gHw_Word0 = ((volatile u32 *)rec)[1];
            w1 = ((volatile u32 *)rec)[2];
            w2 = ((volatile u32 *)rec)[3];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            gHw_At = 0xFF9FFFFF;
            w1 &= gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[4] = gHw_Word0;
            pkt[7] = w1;
            pkt[10] = w2;
            pkt[1] = gHw_WindowWord;
            pkt[11] = gHw_WindowCode;
            tag = (tag << 8) | 11;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[3];
                pkt[14] = gHw_Color;
                pkt[15] = gHw_Second;
                gHw_Second = pkt[6];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                gHw_Second = pkt[9];
                pkt[20] = gHw_Color;
                pkt[21] = gHw_Second;
                pkt += 12;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 12;
        next:
            count--;
            rec += 28;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_RESTORE_REGS2(args, 20, 0x38, 21, 0x3C);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}

u32 *Model_DrawSharedSemiGt4WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_SAVE_REGS2(args, 20, 0x38, 21, 0x3C);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt4Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        {
            register s32 active asm("$2");
            HW_KEEP_LABEL(slot_nop);
            active = *(s8 *)&gModel_bBlendRateActive;
            gHw_At = 0x02000000;
            gHw_PolyTemplate |= gHw_At;
            gHw_Color = gHw_PolyTemplate;
            if (active != 0)
                goto no_second;
            gHw_BlendBits = ((volatile u32 *)rec)[2];
            gHw_At = 0x00600000;
            gHw_BlendBits &= gHw_At;
            if (gHw_BlendBits != 0)
                goto no_second;
        slot_nop:
            gHw_Zero <<= zero;
            if (gHw_SecondClut == 0)
                goto setup;
            gHw_SecondClut = *(u16 *)(rec + 6);
            gHw_Color = gModel_dwPolyGt4SemiTemplate;
            gHw_SecondClut <<= 16;
            gHw_BlendBits = 0x00200000;
            goto setup;
        no_second:
            gHw_SecondClut = gHw_Zero - zero;
        }
    setup:
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
        gHw_WindowCode = 0xE2000000;
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            HW_KEEP_LABEL(cull_test);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[4];
            HW_LWR(gHw_Word0, 14, rec);
            w1 = ((volatile u32 *)rec)[5];
            w2 = ((volatile u32 *)rec)[6];
            gHw_Word3 = ((volatile u32 *)rec)[7];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(16, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[3] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[6] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[9] = e;
            e = gHw_Word3 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            pkt[12] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            HW_GTE_NCLIP();
            w1 <<= 16;
            HW_GTE_READ(24, z);
            w1 >>= 13;
        cull_test:
            if (z > 0)
                goto visible;
            HW_GTE_WRITE(12, e);
            HW_GTE_NCLIP_MAC0_WAIT(z);
            if (z > 0)
                goto next;
        visible:
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word3 <<= 16;
            gHw_Word3 >>= 13;
            gHw_Word0 += (u32)normals;
            w1 += (u32)normals;
            w2 += (u32)normals;
            gHw_Word3 += (u32)normals;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            gHw_Word3 = *(volatile u32 *)gHw_Word3;
            gHw_Word0 |= gHw_PolyCode;
            pkt[2] = gHw_Word0;
            pkt[5] = w1;
            pkt[8] = w2;
            pkt[11] = gHw_Word3;
            HW_GTE_AVSZ4();
            gHw_WindowWord = ((volatile u32 *)rec)[0];
            gHw_Word0 = ((volatile u32 *)rec)[1];
            w1 = ((volatile u32 *)rec)[2];
            w2 = ((volatile u32 *)rec)[3];
            gHw_Word3 = ((volatile u32 *)rec)[4];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            gHw_At = 0xFF9FFFFF;
            w1 &= gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[4] = gHw_Word0;
            pkt[7] = w1;
            pkt[10] = w2;
            pkt[13] = gHw_Word3;
            pkt[1] = gHw_WindowWord;
            pkt[14] = gHw_WindowCode;
            tag = (tag << 8) | 14;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[3];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                gHw_Second = pkt[6];
                pkt[20] = gHw_Color;
                pkt[21] = gHw_Second;
                gHw_Second = pkt[9];
                pkt[23] = gHw_Color;
                pkt[24] = gHw_Second;
                gHw_Second = pkt[12];
                pkt[26] = gHw_Color;
                pkt[27] = gHw_Second;
                pkt += 15;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 15;
        next:
            count--;
            rec += 32;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_RESTORE_REGS2(args, 20, 0x38, 21, 0x3C);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}

u32 *Model_DrawSharedTwoSidedSemiGt3Primitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        HW_KEEP_LABEL(mode_test);
        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt3Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        {
            register s32 active asm("$2");
            HW_KEEP_LABEL(slot_nop);
            active = *(s8 *)&gModel_bBlendRateActive;
            gHw_At = 0x02000000;
            gHw_PolyTemplate |= gHw_At;
            gHw_Color = gHw_PolyTemplate;
            if (active != 0)
                goto no_second;
            gHw_BlendBits = ((volatile u32 *)rec)[1];
            gHw_At = 0x00600000;
            gHw_BlendBits &= gHw_At;
            if (gHw_BlendBits != 0)
                goto no_second;
        slot_nop:
            gHw_Zero <<= zero;
            if (gHw_SecondClut == 0)
                goto setup;
            gHw_SecondClut = *(u16 *)(rec + 2);
            gHw_Color = gModel_dwPolyGt3SemiTemplate;
            gHw_SecondClut <<= 16;
            gHw_BlendBits = 0x00200000;
            goto setup;
        no_second:
            gHw_SecondClut = gHw_Zero - zero;
        }
    setup:
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
    mode_test:
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[3];
            w1 = ((volatile u32 *)rec)[4];
            w2 = ((volatile u32 *)rec)[5];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[2] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[5] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[8] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            w1 <<= 16;
            w1 >>= 13;
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word0 += (u32)normals;
            w1 += (u32)normals;
            w2 += (u32)normals;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            HW_GTE_AVSZ3();
            gHw_Word0 |= gHw_PolyCode;
            pkt[1] = gHw_Word0;
            pkt[4] = w1;
            pkt[7] = w2;
            gHw_Word0 = ((volatile u32 *)rec)[0];
            w1 = ((volatile u32 *)rec)[1];
            w2 = ((volatile u32 *)rec)[2];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            gHw_At = 0xFF9FFFFF;
            w1 &= gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[3] = gHw_Word0;
            pkt[6] = w1;
            pkt[9] = w2;
            tag = (tag << 8) | 9;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[2];
                pkt[11] = gHw_Color;
                pkt[12] = gHw_Second;
                gHw_Second = pkt[5];
                pkt[14] = gHw_Color;
                pkt[15] = gHw_Second;
                gHw_Second = pkt[8];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                pkt += 10;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 10;
        next:
            count--;
            rec += 24;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}

u32 *Model_DrawSharedTwoSidedSemiGt4Primitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        HW_KEEP_LABEL(mode_test);
        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt4Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        {
            register s32 active asm("$2");
            HW_KEEP_LABEL(slot_nop);
            active = *(s8 *)&gModel_bBlendRateActive;
            gHw_At = 0x02000000;
            gHw_PolyTemplate |= gHw_At;
            gHw_Color = gHw_PolyTemplate;
            if (active != 0)
                goto no_second;
            gHw_BlendBits = ((volatile u32 *)rec)[1];
            gHw_At = 0x00600000;
            gHw_BlendBits &= gHw_At;
            if (gHw_BlendBits != 0)
                goto no_second;
        slot_nop:
            gHw_Zero <<= zero;
            if (gHw_SecondClut == 0)
                goto setup;
            gHw_SecondClut = *(u16 *)(rec + 2);
            gHw_Color = gModel_dwPolyGt4SemiTemplate;
            gHw_SecondClut <<= 16;
            gHw_BlendBits = 0x00200000;
            goto setup;
        no_second:
            gHw_SecondClut = gHw_Zero - zero;
        }
    setup:
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
    mode_test:
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[3];
            HW_LWR(gHw_Word0, 10, rec);
            w1 = ((volatile u32 *)rec)[4];
            w2 = ((volatile u32 *)rec)[5];
            gHw_Word3 = ((volatile u32 *)rec)[6];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(16, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[2] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[5] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[8] = e;
            e = gHw_Word3 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            pkt[11] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            w1 <<= 16;
            w1 >>= 13;
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word3 <<= 16;
            gHw_Word3 >>= 13;
            gHw_Word0 += (u32)normals;
            w1 += (u32)normals;
            w2 += (u32)normals;
            gHw_Word3 += (u32)normals;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            gHw_Word3 = *(volatile u32 *)gHw_Word3;
            HW_GTE_AVSZ4();
            gHw_Word0 |= gHw_PolyCode;
            pkt[1] = gHw_Word0;
            pkt[4] = w1;
            pkt[7] = w2;
            pkt[10] = gHw_Word3;
            gHw_Word0 = ((volatile u32 *)rec)[0];
            w1 = ((volatile u32 *)rec)[1];
            w2 = ((volatile u32 *)rec)[2];
            gHw_Word3 = ((volatile u32 *)rec)[3];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            gHw_At = 0xFF9FFFFF;
            w1 &= gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[3] = gHw_Word0;
            pkt[6] = w1;
            pkt[9] = w2;
            pkt[12] = gHw_Word3;
            tag = (tag << 8) | 12;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[2];
                pkt[14] = gHw_Color;
                pkt[15] = gHw_Second;
                gHw_Second = pkt[5];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                gHw_Second = pkt[8];
                pkt[20] = gHw_Color;
                pkt[21] = gHw_Second;
                gHw_Second = pkt[11];
                pkt[23] = gHw_Color;
                pkt[24] = gHw_Second;
                pkt += 13;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 13;
        next:
            count--;
            rec += 28;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}

u32 *Model_DrawSharedTwoSidedSemiGt3WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_SAVE_REGS2(args, 20, 0x38, 21, 0x3C);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt3Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        {
            register s32 active asm("$2");
            HW_KEEP_LABEL(slot_nop);
            active = *(s8 *)&gModel_bBlendRateActive;
            gHw_At = 0x02000000;
            gHw_PolyTemplate |= gHw_At;
            gHw_Color = gHw_PolyTemplate;
            if (active != 0)
                goto no_second;
            gHw_BlendBits = ((volatile u32 *)rec)[2];
            gHw_At = 0x00600000;
            gHw_BlendBits &= gHw_At;
            if (gHw_BlendBits != 0)
                goto no_second;
        slot_nop:
            gHw_Zero <<= zero;
            if (gHw_SecondClut == 0)
                goto setup;
            gHw_SecondClut = *(u16 *)(rec + 6);
            gHw_Color = gModel_dwPolyGt3SemiTemplate;
            gHw_SecondClut <<= 16;
            gHw_BlendBits = 0x00200000;
            goto setup;
        no_second:
            gHw_SecondClut = gHw_Zero - zero;
        }
    setup:
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
        gHw_WindowCode = 0xE2000000;
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[4];
            w1 = ((volatile u32 *)rec)[5];
            w2 = ((volatile u32 *)rec)[6];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[3] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[6] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[9] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            w1 <<= 16;
            w1 >>= 13;
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word0 += (u32)normals;
            w1 += (u32)normals;
            w2 += (u32)normals;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            HW_GTE_AVSZ3();
            gHw_Word0 |= gHw_PolyCode;
            pkt[2] = gHw_Word0;
            pkt[5] = w1;
            pkt[8] = w2;
            gHw_WindowWord = ((volatile u32 *)rec)[0];
            gHw_Word0 = ((volatile u32 *)rec)[1];
            w1 = ((volatile u32 *)rec)[2];
            w2 = ((volatile u32 *)rec)[3];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            gHw_At = 0xFF9FFFFF;
            w1 &= gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[4] = gHw_Word0;
            pkt[7] = w1;
            pkt[10] = w2;
            pkt[1] = gHw_WindowWord;
            pkt[11] = gHw_WindowCode;
            tag = (tag << 8) | 11;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[3];
                pkt[14] = gHw_Color;
                pkt[15] = gHw_Second;
                gHw_Second = pkt[6];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                gHw_Second = pkt[9];
                pkt[20] = gHw_Color;
                pkt[21] = gHw_Second;
                pkt += 12;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 12;
        next:
            count--;
            rec += 28;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_RESTORE_REGS2(args, 20, 0x38, 21, 0x3C);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}

u32 *Model_DrawSharedTwoSidedSemiGt4WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    register GsARGUNIT_SHARED *args asm("$4");
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register s32 mode asm("$13");
    u32 zero;

    args = arg;
    rec = (u8 *)args->primp;
    pkt = (u32 *)args->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&args->primtop;
    off <<= 2;
    HW_SAVE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_SAVE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_SAVE_REGS2(args, 20, 0x38, 21, 0x3C);
    if (count != 0) {
        register u8 *cache asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");

        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt4Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_Color = gHw_PolyTemplate;
        {
            register s32 active asm("$2");
            HW_KEEP_LABEL(slot_nop);
            active = *(s8 *)&gModel_bBlendRateActive;
            gHw_At = 0x02000000;
            gHw_PolyTemplate |= gHw_At;
            gHw_Color = gHw_PolyTemplate;
            if (active != 0)
                goto no_second;
            gHw_BlendBits = ((volatile u32 *)rec)[2];
            gHw_At = 0x00600000;
            gHw_BlendBits &= gHw_At;
            if (gHw_BlendBits != 0)
                goto no_second;
        slot_nop:
            gHw_Zero <<= zero;
            if (gHw_SecondClut == 0)
                goto setup;
            gHw_SecondClut = *(u16 *)(rec + 6);
            gHw_Color = gModel_dwPolyGt4SemiTemplate;
            gHw_SecondClut <<= 16;
            gHw_BlendBits = 0x00200000;
            goto setup;
        no_second:
            gHw_SecondClut = gHw_Zero - zero;
        }
    setup:
        mode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        gHw_PolyCode <<= 24;
        gHw_At = 1;
        gHw_WindowCode = 0xE2000000;
        if (mode != gHw_At) {        mode = gModel_dwLitColorCacheBase;
        cache = (u8 *)args->vertop2;
        normals = *(u8 *volatile *)&args->nortop2;
        if (mode != 0)
            normals += 4;
        otp = args->tagp;
        shift = args->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u32 e asm("$13");
            register s32 z asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            u32 *entry;
            HW_KEEP_LABEL(next);
            gHw_Second = (u32)-1;
            gHw_Word0 = ((volatile u32 *)rec)[4];
            HW_LWR(gHw_Word0, 14, rec);
            w1 = ((volatile u32 *)rec)[5];
            w2 = ((volatile u32 *)rec)[6];
            gHw_Word3 = ((volatile u32 *)rec)[7];
            e = gHw_Word0 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(16, z, gHw_Second, next);
            HW_GTE_WRITE(12, e);
            pkt[3] = e;
            e = w1 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(17, z, gHw_Second, next);
            HW_GTE_WRITE(13, e);
            pkt[6] = e;
            e = w2 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(18, z, gHw_Second, next);
            HW_GTE_WRITE(14, e);
            pkt[9] = e;
            e = gHw_Word3 >> 16;
            e <<= 3;
            entry = (u32 *)(e + (u32)cache);
            z = ((volatile u32 *)entry)[1];
            e = *(volatile u32 *)entry;
            HW_GTE_WRITE_BEQ(19, z, gHw_Second, next);
            pkt[12] = e;
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            w1 <<= 16;
            w1 >>= 13;
            w2 <<= 16;
            w2 >>= 13;
            gHw_Word3 <<= 16;
            gHw_Word3 >>= 13;
            gHw_Word0 += (u32)normals;
            w1 += (u32)normals;
            w2 += (u32)normals;
            gHw_Word3 += (u32)normals;
            gHw_Word0 = *(volatile u32 *)gHw_Word0;
            w1 = *(volatile u32 *)w1;
            w2 = *(volatile u32 *)w2;
            gHw_Word3 = *(volatile u32 *)gHw_Word3;
            gHw_Word0 |= gHw_PolyCode;
            pkt[2] = gHw_Word0;
            pkt[5] = w1;
            pkt[8] = w2;
            pkt[11] = gHw_Word3;
            HW_GTE_AVSZ4();
            gHw_WindowWord = ((volatile u32 *)rec)[0];
            gHw_Word0 = ((volatile u32 *)rec)[1];
            w1 = ((volatile u32 *)rec)[2];
            w2 = ((volatile u32 *)rec)[3];
            gHw_Word3 = ((volatile u32 *)rec)[4];
            HW_GTE_OTZ(slot);
            gHw_At = 0x02000000;
            gHw_Second = gHw_PolyTemplate & gHw_At;
            gHw_At = 0xFF9FFFFF;
            w1 &= gHw_At;
            w1 |= gHw_BlendBits;
            slot = (u32 *)((u32)slot >> shift);
            slot = (u32 *)((u32)slot << 2);
            slot = (u32 *)((u32)slot + (u32)ot);
            tag = *slot;
        emit:
            pkt[4] = gHw_Word0;
            pkt[7] = w1;
            pkt[10] = w2;
            pkt[13] = gHw_Word3;
            pkt[1] = gHw_WindowWord;
            pkt[14] = gHw_WindowCode;
            tag = (tag << 8) | 14;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0) {
                gHw_Word0 <<= 16;
                if (gHw_SecondClut == 0)
                    goto skip;
                gHw_Word0 >>= 16;
                gHw_Word0 |= gHw_SecondClut;
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[3];
                pkt[17] = gHw_Color;
                pkt[18] = gHw_Second;
                gHw_Second = pkt[6];
                pkt[20] = gHw_Color;
                pkt[21] = gHw_Second;
                gHw_Second = pkt[9];
                pkt[23] = gHw_Color;
                pkt[24] = gHw_Second;
                gHw_Second = pkt[12];
                pkt[26] = gHw_Color;
                pkt[27] = gHw_Second;
                pkt += 15;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
        skip:
            pkt += 15;
        next:
            count--;
            rec += 32;
        } while (count != 0);
        }
    }
    HW_RESTORE_REGS2(args, 16, 0x28, 17, 0x2C);
    HW_RESTORE_REGS2(args, 18, 0x30, 19, 0x34);
    HW_RESTORE_REGS2(args, 20, 0x38, 21, 0x3C);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done2);

        next = (u32 *)args->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done2:
        return next;
    }
}
