/*
 * Base Ft3/Gt3/Ft4/Gt4 polygon drawers (0x800612C0-0x80061DDC).
 *
 * Hand-written in the original: no stack frame, borrowed s-registers parked
 * in scratch words of the GsARGUNIT block, the argument read from $a0 and the
 * next primitive block returned in $v0. Register variables and the
 * handwritten_asm.h macros reproduce that contract; see
 * notes/direct-polygon-renderers.md.
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
#include "direct_polygon_renderers.h"

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

u32 *Model_DrawFt3Primitives(GsARGUNIT_NORMAL *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    u32 zero;

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    HW_SAVE_REGS3(arg, 19, 0x2C, 22, 0x30, 23, 0x34);
    if (count != 0) {
        register u8 *verts asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");
        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyFt3Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_DrawMode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_ColorCache = gModel_dwLitColorCacheBase;
        gHw_Color = gHw_PolyTemplate;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        verts = (u8 *)arg->vertop;
        normals = *(u8 *volatile *)&arg->nortop;
        HW_GTE_WRITE(6, gHw_PolyTemplate);
        otp = arg->tagp;
        shift = arg->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u8 *v asm("$13");
            register s32 flag asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            gHw_Word0 = ((volatile u32 *)rec)[3];
            w1 = ((volatile u32 *)rec)[4];
            v = (u8 *)(gHw_Word0 >> 16);
            v = (u8 *)((u32)v << 3);
            v += (u32)verts;
            HW_GTE_LOAD_V0(v);
            v = (u8 *)(w1 << 16);
            v = (u8 *)((u32)v >> 13);
            v += (u32)verts;
            HW_GTE_LOAD_V1(v);
            v = (u8 *)(w1 >> 16);
            v = (u8 *)((u32)v << 3);
            v += (u32)verts;
            HW_GTE_LOAD_V2(v);
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            HW_GTE_RTPT();
            gHw_At = 1;
            if (gHw_DrawMode != gHw_At) {
                HW_GTE_READ_FLAG_WAIT(flag);
                if (flag < 0)
                    goto next;
                HW_GTE_NCLIP_MAC0(flag);
                if (flag <= 0)
                    goto next;
            }
            HW_GTE_AVSZ3();
            v = (u8 *)(gHw_Word0 + (u32)normals);
            if (gHw_DrawMode != 0) {
                HW_GTE_LOAD_V0(v);
                HW_GTE_NCCS();
                gHw_Word0 >>= 1;
                if (gHw_ColorCache != 0) {
                    gHw_Word0 += gHw_ColorCache;
                    HW_GTE_STORE(22, 0, gHw_Word0);
                }
                HW_GTE_STORE(22, 4, pkt);
                gHw_At = 1;
                if (gHw_DrawMode == gHw_At)
                    goto next;
            } else {
                gHw_Word0 >>= 1;
                gHw_Second = gHw_Word0 + gHw_ColorCache;
                gHw_Word0 = *(volatile u32 *)gHw_Second;
                pkt[1] = gHw_Word0;
                ((u8 *)pkt)[7] = gHw_PolyCode;
            }
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
            HW_GTE_STORE_SXY3(pkt, 8, 16, 24);
        emit:
            pkt[3] = gHw_Word0;
            pkt[5] = w1;
            pkt[7] = w2;
            tag = (tag << 8) | 7;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0 && gHw_SecondClut != 0) {
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[2];
                gHw_Word0 <<= 16;
                pkt[10] = gHw_Second;
                gHw_Second = pkt[4];
                gHw_Word0 >>= 16;
                pkt[12] = gHw_Second;
                gHw_Second = pkt[6];
                gHw_Word0 |= gHw_SecondClut;
                pkt[14] = gHw_Second;
                pkt[9] = gHw_Color;
                pkt += 8;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
            pkt += 8;
        next:
            count--;
            rec += 20;
        } while (count != 0);
    }
    HW_RESTORE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    HW_RESTORE_REGS3(arg, 19, 0x2C, 22, 0x30, 23, 0x34);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = (u32 *)arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}

u32 *Model_DrawGt3Primitives(GsARGUNIT_NORMAL *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    u32 zero;

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    HW_SAVE_REGS3(arg, 19, 0x2C, 22, 0x30, 23, 0x34);
    if (count != 0) {
        register u8 *verts asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");
        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt3Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_DrawMode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_ColorCache = gModel_dwLitColorCacheBase;
        gHw_Color = gHw_PolyTemplate;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        verts = (u8 *)arg->vertop;
        normals = *(u8 *volatile *)&arg->nortop;
        HW_GTE_WRITE(6, gHw_PolyTemplate);
        otp = arg->tagp;
        shift = arg->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u8 *v asm("$13");
            register s32 flag asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            gHw_Word0 = ((volatile u32 *)rec)[3];
            w1 = ((volatile u32 *)rec)[4];
            w2 = ((volatile u32 *)rec)[5];
            v = (u8 *)(gHw_Word0 >> 16);
            v = (u8 *)((u32)v << 3);
            v += (u32)verts;
            HW_GTE_LOAD_V0(v);
            v = (u8 *)(w1 >> 16);
            v = (u8 *)((u32)v << 3);
            v += (u32)verts;
            HW_GTE_LOAD_V1(v);
            v = (u8 *)(w2 >> 16);
            v = (u8 *)((u32)v << 3);
            v += (u32)verts;
            HW_GTE_LOAD_V2(v);
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            HW_GTE_RTPT();
            w1 <<= 16;
            w1 >>= 13;
            w2 <<= 16;
            gHw_At = 1;
            w2 >>= 13;
            if (gHw_DrawMode != gHw_At) {
                HW_GTE_READ_FLAG_WAIT(flag);
                if (flag < 0)
                    goto next;
                HW_GTE_NCLIP_MAC0(flag);
                if (flag <= 0)
                    goto next;
            }
            HW_GTE_AVSZ3();
            if (gHw_DrawMode != 0) {
                v = (u8 *)(gHw_Word0 + (u32)normals);
                HW_GTE_LOAD_V0(v);
                v = (u8 *)(w1 + (u32)normals);
                HW_GTE_LOAD_V1(v);
                v = (u8 *)(w2 + (u32)normals);
                HW_GTE_LOAD_V2(v);
                HW_GTE_NCCT();
                gHw_Word0 >>= 1;
                if (gHw_ColorCache != 0) {
                    gHw_Word0 += gHw_ColorCache;
                    w1 >>= 1;
                    w1 += gHw_ColorCache;
                    w2 >>= 1;
                    w2 += gHw_ColorCache;
                    HW_GTE_STORE(20, 0, gHw_Word0);
                    HW_GTE_STORE(21, 0, w1);
                    HW_GTE_STORE(22, 0, w2);
                }
                HW_GTE_STORE(20, 4, pkt);
                HW_GTE_STORE(21, 16, pkt);
                HW_GTE_STORE(22, 28, pkt);
                gHw_At = 1;
                if (gHw_DrawMode == gHw_At)
                    goto next;
            } else {
                gHw_Word0 >>= 1;
                gHw_Second = gHw_Word0 + gHw_ColorCache;
                gHw_Word0 = *(volatile u32 *)gHw_Second;
                w1 >>= 1;
                gHw_Second = w1 + gHw_ColorCache;
                w1 = *(volatile u32 *)gHw_Second;
                w2 >>= 1;
                gHw_Second = w2 + gHw_ColorCache;
                w2 = *(volatile u32 *)gHw_Second;
                pkt[1] = gHw_Word0;
                pkt[4] = w1;
                pkt[7] = w2;
                ((u8 *)pkt)[7] = gHw_PolyCode;
            }
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
            HW_GTE_STORE_SXY3(pkt, 8, 20, 32);
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
    HW_RESTORE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    HW_RESTORE_REGS3(arg, 19, 0x2C, 22, 0x30, 23, 0x34);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = (u32 *)arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}

u32 *Model_DrawFt4Primitives(GsARGUNIT_NORMAL *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    u32 zero;

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    HW_SAVE_REGS3(arg, 19, 0x2C, 22, 0x30, 23, 0x34);
    if (count != 0) {
        register u8 *verts asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");
        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyFt4Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_DrawMode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_ColorCache = gModel_dwLitColorCacheBase;
        gHw_Color = gHw_PolyTemplate;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        verts = (u8 *)arg->vertop;
        normals = *(u8 *volatile *)&arg->nortop;
        HW_GTE_WRITE(6, gHw_PolyTemplate);
        otp = arg->tagp;
        shift = arg->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u8 *v asm("$13");
            register s32 flag asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            gHw_Word0 = ((volatile u32 *)rec)[4];
            w1 = ((volatile u32 *)rec)[5];
            v = (u8 *)(gHw_Word0 << 16);
            v = (u8 *)((u32)v >> 13);
            v += (u32)verts;
            HW_GTE_LOAD_V0(v);
            v = (u8 *)(gHw_Word0 >> 16);
            v = (u8 *)((u32)v << 3);
            v += (u32)verts;
            HW_GTE_LOAD_V1(v);
            v = (u8 *)(w1 << 16);
            v = (u8 *)((u32)v >> 13);
            v += (u32)verts;
            HW_GTE_LOAD_V2(v);
            gHw_Word3 = w1 >> 16;
            gHw_Word3 <<= 3;
            HW_GTE_RTPT();
            v = (u8 *)(gHw_Word3 + (u32)verts);
            gHw_At = 1;
            if (gHw_DrawMode != gHw_At) {
                HW_GTE_READ_FLAG_WAIT(flag);
                if (flag < 0)
                    goto next;
                HW_GTE_NCLIP_MAC0(flag);
                if (flag <= 0)
                    goto next;
            }
            HW_GTE_LOAD_V0(v);
            HW_GTE_STORE_SXY3(pkt, 8, 16, 24);
            HW_GTE_RTPS();
            gHw_Word0 = *(u16 *)(rec + 14);
            gHw_Word0 <<= 3;
            gHw_At = 1;
            if (gHw_DrawMode != gHw_At) {
                HW_GTE_READ_FLAG_WAIT(flag);
                if (flag < 0)
                    goto next;
            }
            HW_GTE_AVSZ4();
            v = (u8 *)(gHw_Word0 + (u32)normals);
            if (gHw_DrawMode != 0) {
                HW_GTE_LOAD_V0(v);
                HW_GTE_NCCS();
                gHw_Word0 >>= 1;
                if (gHw_ColorCache != 0) {
                    gHw_Word0 += gHw_ColorCache;
                    HW_GTE_STORE(22, 0, gHw_Word0);
                }
                HW_GTE_STORE(22, 4, pkt);
                gHw_At = 1;
                if (gHw_DrawMode == gHw_At)
                    goto next;
            } else {
                gHw_Word0 >>= 1;
                gHw_Second = gHw_Word0 + gHw_ColorCache;
                gHw_Word0 = *(volatile u32 *)gHw_Second;
                pkt[1] = gHw_Word0;
                ((u8 *)pkt)[7] = gHw_PolyCode;
            }
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
            HW_GTE_STORE(14, 32, pkt);
        emit:
            pkt[3] = gHw_Word0;
            pkt[5] = w1;
            pkt[7] = w2;
            pkt[9] = gHw_Word3;
            tag = (tag << 8) | 9;
            gHw_At = tag >> 8;
            tag = (tag << 24) | gHw_At;
            pkt[0] = tag;
            tag = ((u32)pkt << 8) >> 8;
            *slot = tag;
            if (gHw_Second != 0 && gHw_SecondClut != 0) {
                gHw_At = 0xFF9FFFFF;
                w1 &= gHw_At;
                gHw_At = 0x00400000;
                w1 |= gHw_At;
                gHw_Second = pkt[2];
                gHw_Word0 <<= 16;
                pkt[12] = gHw_Second;
                gHw_Second = pkt[4];
                gHw_Word0 >>= 16;
                pkt[14] = gHw_Second;
                gHw_Second = pkt[6];
                gHw_Word0 |= gHw_SecondClut;
                pkt[16] = gHw_Second;
                gHw_Second = pkt[8];
                pkt[11] = gHw_Color;
                pkt[18] = gHw_Second;
                pkt += 10;
                gHw_Second = gHw_Zero - zero;
                goto emit;
            }
            pkt += 10;
        next:
            count--;
            rec += 24;
        } while (count != 0);
    }
    HW_RESTORE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    HW_RESTORE_REGS3(arg, 19, 0x2C, 22, 0x30, 23, 0x34);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = (u32 *)arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}

u32 *Model_DrawGt4Primitives(GsARGUNIT_NORMAL *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    u32 zero;

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    zero = 0;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    HW_SAVE_REGS3(arg, 19, 0x2C, 22, 0x30, 23, 0x34);
    if (count != 0) {
        register u8 *verts asm("$5");
        register u8 *normals asm("$6");
        register GsOT *otp asm("$13");
        register u32 shift asm("$14");
        register u32 *ot asm("$15");
        rec = base + off;
        gHw_PolyTemplate = gModel_dwPolyGt4Template;
        gHw_BlendBits = gModel_dwTpageBlendRateBits;
        gHw_SecondClut = gModel_dwSecondPassClutBits;
        gHw_DrawMode = *(s8 *)&gModel_bCurrentDrawMode;
        gHw_ColorCache = gModel_dwLitColorCacheBase;
        gHw_Color = gHw_PolyTemplate;
        gHw_PolyCode = gHw_PolyTemplate >> 24;
        verts = (u8 *)arg->vertop;
        normals = *(u8 *volatile *)&arg->nortop;
        HW_GTE_WRITE(6, gHw_PolyTemplate);
        otp = arg->tagp;
        shift = arg->shift;
        ot = (u32 *)otp->org;
        do {
            register u32 w1 asm("$9");
            register u32 w2 asm("$10");
            register u8 *v asm("$13");
            register s32 flag asm("$2");
            register u32 *slot asm("$2");
            register u32 tag asm("$13");
            gHw_Word0 = ((volatile u32 *)rec)[3];
            HW_LWR(gHw_Word0, 10, rec);
            w1 = ((volatile u32 *)rec)[4];
            w2 = ((volatile u32 *)rec)[5];
            gHw_Word3 = ((volatile u32 *)rec)[6];
            v = (u8 *)(gHw_Word0 >> 16);
            v = (u8 *)((u32)v << 3);
            v += (u32)verts;
            HW_GTE_LOAD_V0(v);
            v = (u8 *)(w1 >> 16);
            v = (u8 *)((u32)v << 3);
            v += (u32)verts;
            HW_GTE_LOAD_V1(v);
            v = (u8 *)(w2 >> 16);
            v = (u8 *)((u32)v << 3);
            v += (u32)verts;
            HW_GTE_LOAD_V2(v);
            v = (u8 *)(gHw_Word3 >> 16);
            v = (u8 *)((u32)v << 3);
            HW_GTE_RTPT();
            gHw_Word3 <<= 16;
            gHw_Word3 >>= 13;
            gHw_At = 1;
            v += (u32)verts;
            if (gHw_DrawMode != gHw_At) {
                HW_GTE_READ_FLAG_WAIT(flag);
                if (flag < 0)
                    goto next;
                HW_GTE_NCLIP_MAC0(flag);
                if (flag <= 0)
                    goto next;
            }
            HW_GTE_LOAD_V0(v);
            HW_GTE_STORE_SXY3(pkt, 8, 20, 32);
            gHw_Word0 <<= 16;
            gHw_Word0 >>= 13;
            HW_GTE_RTPS_RAW();
            w1 <<= 16;
            gHw_At = 1;
            w1 >>= 13;
            if (gHw_DrawMode != gHw_At) {
                HW_GTE_READ_FLAG_WAIT(flag);
                if (flag < 0)
                    goto next;
            }
            HW_GTE_AVSZ4();
            HW_GTE_STORE(14, 44, pkt);
            w2 <<= 16;
            w2 >>= 13;
            v = (u8 *)(gHw_Word3 + (u32)normals);
            if (gHw_DrawMode != 0) {
                HW_GTE_LOAD_V0(v);
                HW_GTE_NCCS();
                v = (u8 *)(gHw_Word0 + (u32)normals);
                HW_GTE_LOAD_V0(v);
                v = (u8 *)(w1 + (u32)normals);
                HW_GTE_LOAD_V1(v);
                v = (u8 *)(w2 + (u32)normals);
                HW_GTE_LOAD_V2(v);
                gHw_Word3 >>= 1;
                if (gHw_ColorCache != 0) {
                    gHw_Word3 += gHw_ColorCache;
                    HW_GTE_STORE(22, 0, gHw_Word3);
                }
                HW_GTE_STORE(22, 40, pkt);
                HW_GTE_NCCT();
                gHw_Word0 >>= 1;
                if (gHw_ColorCache != 0) {
                    gHw_Word0 += gHw_ColorCache;
                    w1 >>= 1;
                    w1 += gHw_ColorCache;
                    w2 >>= 1;
                    w2 += gHw_ColorCache;
                    HW_GTE_STORE(20, 0, gHw_Word0);
                    HW_GTE_STORE(21, 0, w1);
                    HW_GTE_STORE(22, 0, w2);
                }
                HW_GTE_STORE(20, 4, pkt);
                HW_GTE_STORE(21, 16, pkt);
                HW_GTE_STORE(22, 28, pkt);
                gHw_At = 1;
                if (gHw_DrawMode == gHw_At)
                    goto next;
            } else {
                gHw_Word0 >>= 1;
                gHw_Second = gHw_Word0 + gHw_ColorCache;
                gHw_Word0 = *(volatile u32 *)gHw_Second;
                w1 >>= 1;
                gHw_Second = w1 + gHw_ColorCache;
                w1 = *(volatile u32 *)gHw_Second;
                w2 >>= 1;
                gHw_Second = w2 + gHw_ColorCache;
                w2 = *(volatile u32 *)gHw_Second;
                gHw_Word3 >>= 1;
                gHw_Second = gHw_Word3 + gHw_ColorCache;
                gHw_Word3 = *(volatile u32 *)gHw_Second;
                pkt[1] = gHw_Word0;
                pkt[4] = w1;
                pkt[7] = w2;
                pkt[10] = gHw_Word3;
                ((u8 *)pkt)[7] = gHw_PolyCode;
            }
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
    HW_RESTORE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    HW_RESTORE_REGS3(arg, 19, 0x2C, 22, 0x30, 23, 0x34);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = (u32 *)arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}
