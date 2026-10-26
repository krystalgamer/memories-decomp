/*
 * Wireframe drawers for the GsARGUNIT polygon formats (0x80069E44-0x8006AF74).
 *
 * These were written by hand in the original: no stack frame, s0-s2 parked in
 * scratch words of the GsARGUNIT block, the argument read straight from $a0 and
 * the next primitive pointer returned in $v0. The register variables and
 * handwritten_asm.h macros reproduce that contract; see
 * notes/wireframe-renderers.md.
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "handwritten_asm.h"
#include "model_handler_state.h"
#include "model_primitive_templates.h"
#include "gpu_packets.h"
#include "wireframe_renderers.h"

register u32 gHw_LineTemplate asm("$16");
register u32 gHw_DrawModeTemplate asm("$17");
register u32 gHw_LineTerminator asm("$18");
register u32 gHw_At asm("$1");

u32 *Model_DrawFt3Wireframes(GsARGUNIT_NORMAL *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = base + off;
        if (mode != gHw_At) {
            register u8 *verts asm("$5");
            register u8 *normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 *ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF4Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u8 *)arg->vertop;
            normals = *(u8 *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32 *)otp->org;
            do {
                register u32 w0 asm("$8") = ((volatile u32 *)rec)[3];
                register u32 w1 asm("$9") = ((volatile u32 *)rec)[4];
                register u8 *v asm("$13");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 t asm("$13");
                v = (u8 *)(w0 >> 16);
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
                pkt[1] = gHw_DrawModeTemplate;
                pkt[7] = gHw_LineTerminator;
                HW_GTE_RTPT_FLAG(flag);
                if (flag >= 0) {
                    HW_GTE_AVSZ3_OTZ(slot);
                    pkt[2] = gHw_LineTemplate;
                    slot = (u32 *)((u32)slot >> shift);
                    slot = (u32 *)((u32)slot << 2);
                    slot = (u32 *)((u32)slot + (u32)ot);
                    t = *slot;
                    HW_GTE_STORE_SXY_LOOP(pkt, 0xC, 0x10, 0x14, 0x18);
                    t = (t << 8) | 7;
                    gHw_At = t >> 8;
                    t = (t << 24) | gHw_At;
                    pkt[0] = t;
                    t = ((u32)pkt << 8) >> 8;
                    *slot = t;
                    pkt += 8;
                }
                count--;
                rec += 0x14;
            } while (count != 0);
        }
    }
    HW_RESTORE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}

u32 *Model_DrawGt3Wireframes(GsARGUNIT_NORMAL *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = base + off;
        if (mode != gHw_At) {
            register u8 *verts asm("$5");
            register u8 *normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 *ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF4Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u8 *)arg->vertop;
            normals = *(u8 *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32 *)otp->org;
            do {
                register u32 w0 asm("$8") = ((volatile u32 *)rec)[3];
                register u32 w1 asm("$9") = ((volatile u32 *)rec)[4];
                register u32 w2 asm("$10") = ((volatile u32 *)rec)[5];
                register u8 *v asm("$13");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 t asm("$13");
                v = (u8 *)(w0 >> 16);
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
                pkt[1] = gHw_DrawModeTemplate;
                pkt[7] = gHw_LineTerminator;
                HW_GTE_RTPT_FLAG(flag);
                if (flag >= 0) {
                    HW_GTE_AVSZ3_OTZ(slot);
                    pkt[2] = gHw_LineTemplate;
                    slot = (u32 *)((u32)slot >> shift);
                    slot = (u32 *)((u32)slot << 2);
                    slot = (u32 *)((u32)slot + (u32)ot);
                    t = *slot;
                    HW_GTE_STORE_SXY_LOOP(pkt, 0xC, 0x10, 0x14, 0x18);
                    t = (t << 8) | 7;
                    gHw_At = t >> 8;
                    t = (t << 24) | gHw_At;
                    pkt[0] = t;
                    t = ((u32)pkt << 8) >> 8;
                    *slot = t;
                    pkt += 8;
                }
                count--;
                rec += 0x18;
            } while (count != 0);
        }
    }
    HW_RESTORE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}

u32 *Model_DrawFt4Wireframes(GsARGUNIT_NORMAL *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = base + off;
        if (mode != gHw_At) {
            register u8 *verts asm("$5");
            register u8 *normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 *ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF3Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u8 *)arg->vertop;
            normals = *(u8 *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32 *)otp->org;
            do {
                register u32 w0 asm("$8") = ((volatile u32 *)rec)[4];
                register u32 w1 asm("$9") = ((volatile u32 *)rec)[5];
                register u8 *v asm("$13");
                register u32 v3 asm("$11");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 t asm("$13");
                v = (u8 *)(w0 << 16);
                v = (u8 *)((u32)v >> 13);
                v += (u32)verts;
                HW_GTE_LOAD_V0(v);
                v = (u8 *)(w0 >> 16);
                v = (u8 *)((u32)v << 3);
                v += (u32)verts;
                HW_GTE_LOAD_V1(v);
                v = (u8 *)(w1 << 16);
                v = (u8 *)((u32)v >> 13);
                v += (u32)verts;
                HW_GTE_LOAD_V2(v);
                v3 = w1 >> 16;
                v3 <<= 3;
                HW_GTE_RTPT();
                HW_GTE_READ_FLAG(flag);
                v = (u8 *)v3 + (u32)verts;
                pkt[1] = gHw_DrawModeTemplate;
                if (flag >= 0) {
                    HW_GTE_LOAD_V0(v);
                    HW_GTE_STORE_SXY_LOOP(pkt, 0xC, 0x10, 0x24, 0x28);
                    pkt[6] = gHw_LineTerminator;
                    pkt[11] = gHw_LineTerminator;
                    HW_GTE_RTPS_FLAG(flag);
                    if (flag >= 0) {
                        HW_GTE_AVSZ4_OTZ(slot);
                        pkt[2] = gHw_LineTemplate;
                        pkt[7] = gHw_LineTemplate;
                        slot = (u32 *)((u32)slot >> shift);
                        slot = (u32 *)((u32)slot << 2);
                        slot = (u32 *)((u32)slot + (u32)ot);
                        t = *slot;
                        HW_GTE_STORE(14, 0x14, pkt);
                        HW_GTE_STORE(14, 0x20, pkt);
                        t = (t << 8) | 0xB;
                        gHw_At = t >> 8;
                        t = (t << 24) | gHw_At;
                        pkt[0] = t;
                        t = ((u32)pkt << 8) >> 8;
                        *slot = t;
                        pkt += 12;
                    }
                }
                count--;
                rec += 0x18;
            } while (count != 0);
        }
    }
    HW_RESTORE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}

u32 *Model_DrawGt4Wireframes(GsARGUNIT_NORMAL *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = base + off;
        if (mode != gHw_At) {
            register u8 *verts asm("$5");
            register u8 *normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 *ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF3Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u8 *)arg->vertop;
            normals = *(u8 *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32 *)otp->org;
            do {
                register u32 w0 asm("$8") = ((volatile u32 *)rec)[3];
                register u32 w1 asm("$9") = ((volatile u32 *)rec)[4];
                register u32 w2 asm("$10") = ((volatile u32 *)rec)[5];
                register u32 w3 asm("$11") = ((volatile u32 *)rec)[6];
                register u8 *v asm("$13");
                register u32 v3 asm("$13");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 t asm("$13");
                v = (u8 *)(w0 >> 16);
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
                v3 = w3 >> 16;
                v3 <<= 3;
                HW_GTE_RTPT();
                HW_GTE_READ_FLAG(flag);
                v = (u8 *)v3 + (u32)verts;
                pkt[1] = gHw_DrawModeTemplate;
                if (flag >= 0) {
                    HW_GTE_LOAD_V0(v);
                    HW_GTE_STORE_SXY_LOOP(pkt, 0xC, 0x10, 0x24, 0x28);
                    pkt[6] = gHw_LineTerminator;
                    pkt[11] = gHw_LineTerminator;
                    HW_GTE_RTPS_FLAG(flag);
                    if (flag >= 0) {
                        HW_GTE_AVSZ4_OTZ(slot);
                        pkt[2] = gHw_LineTemplate;
                        pkt[7] = gHw_LineTemplate;
                        slot = (u32 *)((u32)slot >> shift);
                        slot = (u32 *)((u32)slot << 2);
                        slot = (u32 *)((u32)slot + (u32)ot);
                        t = *slot;
                        HW_GTE_STORE(14, 0x14, pkt);
                        HW_GTE_STORE(14, 0x20, pkt);
                        t = (t << 8) | 0xB;
                        gHw_At = t >> 8;
                        t = (t << 24) | gHw_At;
                        pkt[0] = t;
                        t = ((u32)pkt << 8) >> 8;
                        *slot = t;
                        pkt += 12;
                    }
                }
                count--;
                rec += 0x1c;
            } while (count != 0);
        }
    }
    HW_RESTORE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}

u32 *Model_DrawFt3WindowRecordWireframes(GsARGUNIT_NORMAL *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = base + off;
        if (mode != gHw_At) {
            register u8 *verts asm("$5");
            register u8 *normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 *ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF4Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u8 *)arg->vertop;
            normals = *(u8 *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32 *)otp->org;
            do {
                register u32 w0 asm("$8") = ((volatile u32 *)rec)[4];
                register u32 w1 asm("$9") = ((volatile u32 *)rec)[5];
                register u8 *v asm("$13");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 t asm("$13");
                v = (u8 *)(w0 >> 16);
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
                pkt[1] = gHw_DrawModeTemplate;
                pkt[7] = gHw_LineTerminator;
                HW_GTE_RTPT_FLAG(flag);
                if (flag >= 0) {
                    HW_GTE_AVSZ3_OTZ(slot);
                    pkt[2] = gHw_LineTemplate;
                    slot = (u32 *)((u32)slot >> shift);
                    slot = (u32 *)((u32)slot << 2);
                    slot = (u32 *)((u32)slot + (u32)ot);
                    t = *slot;
                    HW_GTE_STORE_SXY_LOOP(pkt, 0xC, 0x10, 0x14, 0x18);
                    t = (t << 8) | 7;
                    gHw_At = t >> 8;
                    t = (t << 24) | gHw_At;
                    pkt[0] = t;
                    t = ((u32)pkt << 8) >> 8;
                    *slot = t;
                    pkt += 8;
                }
                count--;
                rec += 0x18;
            } while (count != 0);
        }
    }
    HW_RESTORE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}

u32 *Model_DrawGt3WindowRecordWireframes(GsARGUNIT_NORMAL *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = base + off;
        if (mode != gHw_At) {
            register u8 *verts asm("$5");
            register u8 *normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 *ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF4Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u8 *)arg->vertop;
            normals = *(u8 *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32 *)otp->org;
            do {
                register u32 w0 asm("$8") = ((volatile u32 *)rec)[4];
                register u32 w1 asm("$9") = ((volatile u32 *)rec)[5];
                register u32 w2 asm("$10") = ((volatile u32 *)rec)[6];
                register u8 *v asm("$13");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 t asm("$13");
                v = (u8 *)(w0 >> 16);
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
                pkt[1] = gHw_DrawModeTemplate;
                pkt[7] = gHw_LineTerminator;
                HW_GTE_RTPT_FLAG(flag);
                if (flag >= 0) {
                    HW_GTE_AVSZ3_OTZ(slot);
                    pkt[2] = gHw_LineTemplate;
                    slot = (u32 *)((u32)slot >> shift);
                    slot = (u32 *)((u32)slot << 2);
                    slot = (u32 *)((u32)slot + (u32)ot);
                    t = *slot;
                    HW_GTE_STORE_SXY_LOOP(pkt, 0xC, 0x10, 0x14, 0x18);
                    t = (t << 8) | 7;
                    gHw_At = t >> 8;
                    t = (t << 24) | gHw_At;
                    pkt[0] = t;
                    t = ((u32)pkt << 8) >> 8;
                    *slot = t;
                    pkt += 8;
                }
                count--;
                rec += 0x1c;
            } while (count != 0);
        }
    }
    HW_RESTORE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}

u32 *Model_DrawFt4WindowRecordWireframes(GsARGUNIT_NORMAL *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = base + off;
        if (mode != gHw_At) {
            register u8 *verts asm("$5");
            register u8 *normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 *ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF3Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u8 *)arg->vertop;
            normals = *(u8 *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32 *)otp->org;
            do {
                register u32 w0 asm("$8") = ((volatile u32 *)rec)[5];
                register u32 w1 asm("$9") = ((volatile u32 *)rec)[6];
                register u8 *v asm("$13");
                register u32 v3 asm("$11");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 t asm("$13");
                v = (u8 *)(w0 << 16);
                v = (u8 *)((u32)v >> 13);
                v += (u32)verts;
                HW_GTE_LOAD_V0(v);
                v = (u8 *)(w0 >> 16);
                v = (u8 *)((u32)v << 3);
                v += (u32)verts;
                HW_GTE_LOAD_V1(v);
                v = (u8 *)(w1 << 16);
                v = (u8 *)((u32)v >> 13);
                v += (u32)verts;
                HW_GTE_LOAD_V2(v);
                v3 = w1 >> 16;
                v3 <<= 3;
                HW_GTE_RTPT();
                HW_GTE_READ_FLAG(flag);
                v = (u8 *)v3 + (u32)verts;
                pkt[1] = gHw_DrawModeTemplate;
                if (flag >= 0) {
                    HW_GTE_LOAD_V0(v);
                    HW_GTE_STORE_SXY_LOOP(pkt, 0xC, 0x10, 0x24, 0x28);
                    pkt[6] = gHw_LineTerminator;
                    pkt[11] = gHw_LineTerminator;
                    HW_GTE_RTPS_FLAG(flag);
                    if (flag >= 0) {
                        HW_GTE_AVSZ4_OTZ(slot);
                        pkt[2] = gHw_LineTemplate;
                        pkt[7] = gHw_LineTemplate;
                        slot = (u32 *)((u32)slot >> shift);
                        slot = (u32 *)((u32)slot << 2);
                        slot = (u32 *)((u32)slot + (u32)ot);
                        t = *slot;
                        HW_GTE_STORE(14, 0x14, pkt);
                        HW_GTE_STORE(14, 0x20, pkt);
                        t = (t << 8) | 0xB;
                        gHw_At = t >> 8;
                        t = (t << 24) | gHw_At;
                        pkt[0] = t;
                        t = ((u32)pkt << 8) >> 8;
                        *slot = t;
                        pkt += 12;
                    }
                }
                count--;
                rec += 0x1c;
            } while (count != 0);
        }
    }
    HW_RESTORE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}

u32 *Model_DrawGt4WindowRecordWireframes(GsARGUNIT_NORMAL *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = base + off;
        if (mode != gHw_At) {
            register u8 *verts asm("$5");
            register u8 *normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 *ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF3Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u8 *)arg->vertop;
            normals = *(u8 *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32 *)otp->org;
            do {
                register u32 w0 asm("$8") = ((volatile u32 *)rec)[4];
                register u32 w1 asm("$9") = ((volatile u32 *)rec)[5];
                register u32 w2 asm("$10") = ((volatile u32 *)rec)[6];
                register u32 w3 asm("$11") = ((volatile u32 *)rec)[7];
                register u8 *v asm("$13");
                register u32 v3 asm("$13");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 t asm("$13");
                v = (u8 *)(w0 >> 16);
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
                v3 = w3 >> 16;
                v3 <<= 3;
                HW_GTE_RTPT();
                HW_GTE_READ_FLAG(flag);
                v = (u8 *)v3 + (u32)verts;
                pkt[1] = gHw_DrawModeTemplate;
                if (flag >= 0) {
                    HW_GTE_LOAD_V0(v);
                    HW_GTE_STORE_SXY_LOOP(pkt, 0xC, 0x10, 0x24, 0x28);
                    pkt[6] = gHw_LineTerminator;
                    pkt[11] = gHw_LineTerminator;
                    HW_GTE_RTPS_FLAG(flag);
                    if (flag >= 0) {
                        HW_GTE_AVSZ4_OTZ(slot);
                        pkt[2] = gHw_LineTemplate;
                        pkt[7] = gHw_LineTemplate;
                        slot = (u32 *)((u32)slot >> shift);
                        slot = (u32 *)((u32)slot << 2);
                        slot = (u32 *)((u32)slot + (u32)ot);
                        t = *slot;
                        HW_GTE_STORE(14, 0x14, pkt);
                        HW_GTE_STORE(14, 0x20, pkt);
                        t = (t << 8) | 0xB;
                        gHw_At = t >> 8;
                        t = (t << 24) | gHw_At;
                        pkt[0] = t;
                        t = ((u32)pkt << 8) >> 8;
                        *slot = t;
                        pkt += 12;
                    }
                }
                count--;
                rec += 0x20;
            } while (count != 0);
        }
    }
    HW_RESTORE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}

u32 *Model_DrawSharedGt3Wireframes(GsARGUNIT_SHARED *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x28, 17, 0x2C, 18, 0x30);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = base + off;
        if (mode != gHw_At) {
            register u8 *cache asm("$5");
            register u8 *normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 *ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF4Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            cache = (u8 *)arg->vertop2;
            normals = *(u8 *volatile *)&arg->nortop2;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32 *)otp->org;
            do {
                register s32 empty asm("$3");
                empty = -1;
                {
                register u32 w0 asm("$8") = ((volatile u32 *)rec)[3];
                register u32 w1 asm("$9") = ((volatile u32 *)rec)[4];
                register u32 w2 asm("$10") = ((volatile u32 *)rec)[5];
                register u32 e asm("$13");
                register s32 z asm("$2");
                u32 *entry;
                register u32 *slot asm("$2");
                register u32 t asm("$13");
                e = w0 >> 16;
                e <<= 3;
                entry = (u32 *)(e + (u32)cache);
                z = ((volatile u32 *)entry)[1];
                e = *(volatile u32 *)entry;
                pkt[3] = e;
                if (z != empty) {
                    HW_GTE_WRITE(17, z);
                    pkt[6] = e;
                    e = w1 >> 16;
                    e <<= 3;
                    entry = (u32 *)(e + (u32)cache);
                    z = ((volatile u32 *)entry)[1];
                    e = *(volatile u32 *)entry;
                    pkt[4] = e;
                    if (z != empty) {
                        HW_GTE_WRITE(18, z);
                        e = w2 >> 16;
                        e <<= 3;
                        entry = (u32 *)(e + (u32)cache);
                        z = ((volatile u32 *)entry)[1];
                        e = *(volatile u32 *)entry;
                        pkt[5] = e;
                        if (z != empty) {
                            HW_GTE_WRITE(19, z);
                            pkt[1] = gHw_DrawModeTemplate;
                            pkt[7] = gHw_LineTerminator;
                            HW_GTE_AVSZ3_OTZ(slot);
                            pkt[2] = gHw_LineTemplate;
                            slot = (u32 *)((u32)slot >> shift);
                            slot = (u32 *)((u32)slot << 2);
                            slot = (u32 *)((u32)slot + (u32)ot);
                            t = *slot;
                            t = (t << 8) | 7;
                            gHw_At = t >> 8;
                            t = (t << 24) | gHw_At;
                            pkt[0] = t;
                            t = ((u32)pkt << 8) >> 8;
                            *slot = t;
                            pkt += 8;
                        }
                    }
                }
                }
                count--;
                rec += 0x18;
            } while (count != 0);
        }
    }
    HW_RESTORE_REGS3(arg, 16, 0x28, 17, 0x2C, 18, 0x30);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}

u32 *Model_DrawSharedGt4Wireframes(GsARGUNIT_SHARED *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x28, 17, 0x2C, 18, 0x30);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = base + off;
        if (mode != gHw_At) {
            register u8 *cache asm("$5");
            register u8 *normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 *ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF3Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            cache = (u8 *)arg->vertop2;
            normals = *(u8 *volatile *)&arg->nortop2;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32 *)otp->org;
            do {
                register s32 empty asm("$3");
                empty = -1;
                {
                register u32 w0 asm("$8") = ((volatile u32 *)rec)[3];
                register u32 w1 asm("$9") = ((volatile u32 *)rec)[4];
                register u32 w2 asm("$10") = ((volatile u32 *)rec)[5];
                register u32 w3 asm("$11") = ((volatile u32 *)rec)[6];
                register u32 e asm("$13");
                register s32 z asm("$2");
                u32 *entry;
                register u32 *slot asm("$2");
                register u32 t asm("$13");
                e = w0 >> 16;
                e <<= 3;
                entry = (u32 *)(e + (u32)cache);
                z = ((volatile u32 *)entry)[1];
                e = *(volatile u32 *)entry;
                pkt[3] = e;
                if (z != empty) {
                    HW_GTE_WRITE(16, z);
                    pkt[10] = e;
                    e = w1 >> 16;
                    e <<= 3;
                    entry = (u32 *)(e + (u32)cache);
                    z = ((volatile u32 *)entry)[1];
                    e = *(volatile u32 *)entry;
                    pkt[4] = e;
                    if (z != empty) {
                        HW_GTE_WRITE(17, z);
                        e = w2 >> 16;
                        e <<= 3;
                        entry = (u32 *)(e + (u32)cache);
                        z = ((volatile u32 *)entry)[1];
                        e = *(volatile u32 *)entry;
                        pkt[9] = e;
                        if (z != empty) {
                            HW_GTE_WRITE(18, z);
                            e = w3 >> 16;
                            e <<= 3;
                            entry = (u32 *)(e + (u32)cache);
                            z = ((volatile u32 *)entry)[1];
                            e = *(volatile u32 *)entry;
                            pkt[5] = e;
                            if (z != empty) {
                                HW_GTE_WRITE(19, z);
                                pkt[8] = e;
                                pkt[6] = gHw_LineTerminator;
                                pkt[11] = gHw_LineTerminator;
                                HW_GTE_AVSZ4_OTZ(slot);
                                pkt[2] = gHw_LineTemplate;
                                pkt[7] = gHw_LineTemplate;
                                slot = (u32 *)((u32)slot >> shift);
                                slot = (u32 *)((u32)slot << 2);
                                slot = (u32 *)((u32)slot + (u32)ot);
                                t = *slot;
                                pkt[1] = gHw_DrawModeTemplate;
                                t = (t << 8) | 0xB;
                                gHw_At = t >> 8;
                                t = (t << 24) | gHw_At;
                                pkt[0] = t;
                                t = ((u32)pkt << 8) >> 8;
                                *slot = t;
                                pkt += 12;
                            }
                        }
                    }
                }
                }
                count--;
                rec += 0x1c;
            } while (count != 0);
        }
    }
    HW_RESTORE_REGS3(arg, 16, 0x28, 17, 0x2C, 18, 0x30);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}

u32 *Model_DrawSharedGt3WindowRecordWireframes(GsARGUNIT_SHARED *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x28, 17, 0x2C, 18, 0x30);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = base + off;
        if (mode != gHw_At) {
            register u8 *cache asm("$5");
            register u8 *normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 *ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF4Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            cache = (u8 *)arg->vertop2;
            normals = *(u8 *volatile *)&arg->nortop2;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32 *)otp->org;
            do {
                register s32 empty asm("$3");
                empty = -1;
                {
                register u32 w0 asm("$8") = ((volatile u32 *)rec)[4];
                register u32 w1 asm("$9") = ((volatile u32 *)rec)[5];
                register u32 w2 asm("$10") = ((volatile u32 *)rec)[6];
                register u32 e asm("$13");
                register s32 z asm("$2");
                u32 *entry;
                register u32 *slot asm("$2");
                register u32 t asm("$13");
                e = w0 >> 16;
                e <<= 3;
                entry = (u32 *)(e + (u32)cache);
                z = ((volatile u32 *)entry)[1];
                e = *(volatile u32 *)entry;
                pkt[3] = e;
                if (z != empty) {
                    HW_GTE_WRITE(17, z);
                    pkt[6] = e;
                    e = w1 >> 16;
                    e <<= 3;
                    entry = (u32 *)(e + (u32)cache);
                    z = ((volatile u32 *)entry)[1];
                    e = *(volatile u32 *)entry;
                    pkt[4] = e;
                    if (z != empty) {
                        HW_GTE_WRITE(18, z);
                        e = w2 >> 16;
                        e <<= 3;
                        entry = (u32 *)(e + (u32)cache);
                        z = ((volatile u32 *)entry)[1];
                        e = *(volatile u32 *)entry;
                        pkt[5] = e;
                        if (z != empty) {
                            HW_GTE_WRITE(19, z);
                            pkt[1] = gHw_DrawModeTemplate;
                            pkt[7] = gHw_LineTerminator;
                            HW_GTE_AVSZ3_OTZ(slot);
                            pkt[2] = gHw_LineTemplate;
                            slot = (u32 *)((u32)slot >> shift);
                            slot = (u32 *)((u32)slot << 2);
                            slot = (u32 *)((u32)slot + (u32)ot);
                            t = *slot;
                            t = (t << 8) | 7;
                            gHw_At = t >> 8;
                            t = (t << 24) | gHw_At;
                            pkt[0] = t;
                            t = ((u32)pkt << 8) >> 8;
                            *slot = t;
                            pkt += 8;
                        }
                    }
                }
                }
                count--;
                rec += 0x1c;
            } while (count != 0);
        }
    }
    HW_RESTORE_REGS3(arg, 16, 0x28, 17, 0x2C, 18, 0x30);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}

u32 *Model_DrawSharedGt4WindowRecordWireframes(GsARGUNIT_SHARED *arg)
{
    register u8 *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    rec = (u8 *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = *(volatile u16 *)(rec + 2);
    off = ((volatile u32 *)rec)[1];
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x28, 17, 0x2C, 18, 0x30);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = base + off;
        if (mode != gHw_At) {
            register u8 *cache asm("$5");
            register u8 *normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 *ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF3Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            cache = (u8 *)arg->vertop2;
            normals = *(u8 *volatile *)&arg->nortop2;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32 *)otp->org;
            do {
                register s32 empty asm("$3");
                empty = -1;
                {
                register u32 w0 asm("$8") = ((volatile u32 *)rec)[4];
                register u32 w1 asm("$9") = ((volatile u32 *)rec)[5];
                register u32 w2 asm("$10") = ((volatile u32 *)rec)[6];
                register u32 w3 asm("$11") = ((volatile u32 *)rec)[7];
                register u32 e asm("$13");
                register s32 z asm("$2");
                u32 *entry;
                register u32 *slot asm("$2");
                register u32 t asm("$13");
                e = w0 >> 16;
                e <<= 3;
                entry = (u32 *)(e + (u32)cache);
                z = ((volatile u32 *)entry)[1];
                e = *(volatile u32 *)entry;
                pkt[3] = e;
                if (z != empty) {
                    HW_GTE_WRITE(16, z);
                    pkt[10] = e;
                    e = w1 >> 16;
                    e <<= 3;
                    entry = (u32 *)(e + (u32)cache);
                    z = ((volatile u32 *)entry)[1];
                    e = *(volatile u32 *)entry;
                    pkt[4] = e;
                    if (z != empty) {
                        HW_GTE_WRITE(17, z);
                        e = w2 >> 16;
                        e <<= 3;
                        entry = (u32 *)(e + (u32)cache);
                        z = ((volatile u32 *)entry)[1];
                        e = *(volatile u32 *)entry;
                        pkt[9] = e;
                        if (z != empty) {
                            HW_GTE_WRITE(18, z);
                            e = w3 >> 16;
                            e <<= 3;
                            entry = (u32 *)(e + (u32)cache);
                            z = ((volatile u32 *)entry)[1];
                            e = *(volatile u32 *)entry;
                            pkt[5] = e;
                            if (z != empty) {
                                HW_GTE_WRITE(19, z);
                                pkt[8] = e;
                                pkt[6] = gHw_LineTerminator;
                                pkt[11] = gHw_LineTerminator;
                                HW_GTE_AVSZ4_OTZ(slot);
                                pkt[2] = gHw_LineTemplate;
                                pkt[7] = gHw_LineTemplate;
                                slot = (u32 *)((u32)slot >> shift);
                                slot = (u32 *)((u32)slot << 2);
                                slot = (u32 *)((u32)slot + (u32)ot);
                                t = *slot;
                                pkt[1] = gHw_DrawModeTemplate;
                                t = (t << 8) | 0xB;
                                gHw_At = t >> 8;
                                t = (t << 24) | gHw_At;
                                pkt[0] = t;
                                t = ((u32)pkt << 8) >> 8;
                                *slot = t;
                                pkt += 12;
                            }
                        }
                    }
                }
                }
                count--;
                rec += 0x20;
            } while (count != 0);
        }
    }
    HW_RESTORE_REGS3(arg, 16, 0x28, 17, 0x2C, 18, 0x30);
    {
        register u32 *next asm("$2");
        HW_KEEP_LABEL(done);

        next = arg->primp;
        gGraphics_pPacketWorkCursor = pkt;
        next += 2;
    done:
        return next;
    }
}
