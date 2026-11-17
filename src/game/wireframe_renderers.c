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
#include "model.h"
#include "gpu_packets.h"
#include "model_slot_setup.h"
#include "wireframe_renderers.h"

register u32 gHw_LineTemplate asm("$16");
register u32 gHw_DrawModeTemplate asm("$17");
register u32 gHw_LineTerminator asm("$18");
register u32 gHw_At asm("$1");

/*
 * Each drawer converts its SDK pointers once on entry: the primitive block
 * to its ModelPrimitiveBlockHeader, primtop plus the word offset to its
 * ModelDraw*Record type (model_slot_setup.h), and the vertex (or vertop2
 * cache) and OT bases to u32 addresses. The record cursor points to volatile
 * data because the original reads its words in a fixed order. Typed pointer
 * arithmetic swaps the addu operands, so cache entry and OT slot addresses
 * are integers converted at their dereference. The draw mode is read as the
 * whole word holding gModel_bCurrentDrawMode, as the original's lw does.
 */

u32 *Model_DrawFt3Wireframes(GsARGUNIT_NORMAL *arg)
{
    register volatile ModelPrimitiveBlockHeader *blk asm("$7");
    register volatile ModelDrawFt3Record *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    blk = (ModelPrimitiveBlockHeader *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = blk->record_count;
    off = blk->word_offset;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = (ModelDrawFt3Record *)(base + off);
        if (mode != gHw_At) {
            register u32 verts asm("$5");
            register u32 normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF4Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u32)arg->vertop;
            normals = (u32)*(SVECTOR *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32)otp->org;
            do {
                register u32 w0 asm("$8") = rec->words[3];
                register u32 w1 asm("$9") = rec->words[4];
                register u32 v asm("$13");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 otz asm("$2");
                register u32 t asm("$13");
                v = w0 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V0(v);
                v = w1 << 16;
                v >>= 13;
                v += verts;
                HW_GTE_LOAD_V1(v);
                v = w1 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V2(v);
                pkt[1] = gHw_DrawModeTemplate;
                pkt[7] = gHw_LineTerminator;
                HW_GTE_RTPT_FLAG(flag);
                if (flag >= 0) {
                    HW_GTE_AVSZ3_OTZ(otz);
                    pkt[2] = gHw_LineTemplate;
                    otz >>= shift;
                    otz <<= 2;
                    otz += ot;
                    slot = (u32 *)otz;
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
                rec++;
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
    register volatile ModelPrimitiveBlockHeader *blk asm("$7");
    register volatile ModelDrawGt3Record *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    blk = (ModelPrimitiveBlockHeader *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = blk->record_count;
    off = blk->word_offset;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = (ModelDrawGt3Record *)(base + off);
        if (mode != gHw_At) {
            register u32 verts asm("$5");
            register u32 normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF4Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u32)arg->vertop;
            normals = (u32)*(SVECTOR *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32)otp->org;
            do {
                register u32 w0 asm("$8") = rec->words[3];
                register u32 w1 asm("$9") = rec->words[4];
                register u32 w2 asm("$10") = rec->words[5];
                register u32 v asm("$13");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 otz asm("$2");
                register u32 t asm("$13");
                v = w0 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V0(v);
                v = w1 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V1(v);
                v = w2 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V2(v);
                pkt[1] = gHw_DrawModeTemplate;
                pkt[7] = gHw_LineTerminator;
                HW_GTE_RTPT_FLAG(flag);
                if (flag >= 0) {
                    HW_GTE_AVSZ3_OTZ(otz);
                    pkt[2] = gHw_LineTemplate;
                    otz >>= shift;
                    otz <<= 2;
                    otz += ot;
                    slot = (u32 *)otz;
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
                rec++;
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
    register volatile ModelPrimitiveBlockHeader *blk asm("$7");
    register volatile ModelDrawFt4Record *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    blk = (ModelPrimitiveBlockHeader *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = blk->record_count;
    off = blk->word_offset;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = (ModelDrawFt4Record *)(base + off);
        if (mode != gHw_At) {
            register u32 verts asm("$5");
            register u32 normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF3Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u32)arg->vertop;
            normals = (u32)*(SVECTOR *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32)otp->org;
            do {
                register u32 w0 asm("$8") = rec->words[4];
                register u32 w1 asm("$9") = rec->words[5];
                register u32 v asm("$13");
                register u32 v3 asm("$11");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 otz asm("$2");
                register u32 t asm("$13");
                v = w0 << 16;
                v >>= 13;
                v += verts;
                HW_GTE_LOAD_V0(v);
                v = w0 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V1(v);
                v = w1 << 16;
                v >>= 13;
                v += verts;
                HW_GTE_LOAD_V2(v);
                v3 = w1 >> 16;
                v3 <<= 3;
                HW_GTE_RTPT();
                HW_GTE_READ_FLAG(flag);
                v = v3 + verts;
                pkt[1] = gHw_DrawModeTemplate;
                if (flag >= 0) {
                    HW_GTE_LOAD_V0(v);
                    HW_GTE_STORE_SXY_LOOP(pkt, 0xC, 0x10, 0x24, 0x28);
                    pkt[6] = gHw_LineTerminator;
                    pkt[11] = gHw_LineTerminator;
                    HW_GTE_RTPS_FLAG(flag);
                    if (flag >= 0) {
                        HW_GTE_AVSZ4_OTZ(otz);
                        pkt[2] = gHw_LineTemplate;
                        pkt[7] = gHw_LineTemplate;
                        otz >>= shift;
                        otz <<= 2;
                        otz += ot;
                        slot = (u32 *)otz;
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
                rec++;
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
    register volatile ModelPrimitiveBlockHeader *blk asm("$7");
    register volatile ModelDrawGt4Record *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    blk = (ModelPrimitiveBlockHeader *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = blk->record_count;
    off = blk->word_offset;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = (ModelDrawGt4Record *)(base + off);
        if (mode != gHw_At) {
            register u32 verts asm("$5");
            register u32 normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF3Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u32)arg->vertop;
            normals = (u32)*(SVECTOR *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32)otp->org;
            do {
                register u32 w0 asm("$8") = rec->words[3];
                register u32 w1 asm("$9") = rec->words[4];
                register u32 w2 asm("$10") = rec->words[5];
                register u32 w3 asm("$11") = rec->words[6];
                register u32 v asm("$13");
                register u32 v3 asm("$13");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 otz asm("$2");
                register u32 t asm("$13");
                v = w0 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V0(v);
                v = w1 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V1(v);
                v = w2 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V2(v);
                v3 = w3 >> 16;
                v3 <<= 3;
                HW_GTE_RTPT();
                HW_GTE_READ_FLAG(flag);
                v = v3 + verts;
                pkt[1] = gHw_DrawModeTemplate;
                if (flag >= 0) {
                    HW_GTE_LOAD_V0(v);
                    HW_GTE_STORE_SXY_LOOP(pkt, 0xC, 0x10, 0x24, 0x28);
                    pkt[6] = gHw_LineTerminator;
                    pkt[11] = gHw_LineTerminator;
                    HW_GTE_RTPS_FLAG(flag);
                    if (flag >= 0) {
                        HW_GTE_AVSZ4_OTZ(otz);
                        pkt[2] = gHw_LineTemplate;
                        pkt[7] = gHw_LineTemplate;
                        otz >>= shift;
                        otz <<= 2;
                        otz += ot;
                        slot = (u32 *)otz;
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
                rec++;
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
    register volatile ModelPrimitiveBlockHeader *blk asm("$7");
    register volatile ModelDrawFt3WindowRecord *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    blk = (ModelPrimitiveBlockHeader *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = blk->record_count;
    off = blk->word_offset;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = (ModelDrawFt3WindowRecord *)(base + off);
        if (mode != gHw_At) {
            register u32 verts asm("$5");
            register u32 normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF4Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u32)arg->vertop;
            normals = (u32)*(SVECTOR *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32)otp->org;
            do {
                register u32 w0 asm("$8") = rec->words[4];
                register u32 w1 asm("$9") = rec->words[5];
                register u32 v asm("$13");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 otz asm("$2");
                register u32 t asm("$13");
                v = w0 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V0(v);
                v = w1 << 16;
                v >>= 13;
                v += verts;
                HW_GTE_LOAD_V1(v);
                v = w1 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V2(v);
                pkt[1] = gHw_DrawModeTemplate;
                pkt[7] = gHw_LineTerminator;
                HW_GTE_RTPT_FLAG(flag);
                if (flag >= 0) {
                    HW_GTE_AVSZ3_OTZ(otz);
                    pkt[2] = gHw_LineTemplate;
                    otz >>= shift;
                    otz <<= 2;
                    otz += ot;
                    slot = (u32 *)otz;
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
                rec++;
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
    register volatile ModelPrimitiveBlockHeader *blk asm("$7");
    register volatile ModelDrawGt3WindowRecord *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    blk = (ModelPrimitiveBlockHeader *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = blk->record_count;
    off = blk->word_offset;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = (ModelDrawGt3WindowRecord *)(base + off);
        if (mode != gHw_At) {
            register u32 verts asm("$5");
            register u32 normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF4Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u32)arg->vertop;
            normals = (u32)*(SVECTOR *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32)otp->org;
            do {
                register u32 w0 asm("$8") = rec->words[4];
                register u32 w1 asm("$9") = rec->words[5];
                register u32 w2 asm("$10") = rec->words[6];
                register u32 v asm("$13");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 otz asm("$2");
                register u32 t asm("$13");
                v = w0 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V0(v);
                v = w1 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V1(v);
                v = w2 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V2(v);
                pkt[1] = gHw_DrawModeTemplate;
                pkt[7] = gHw_LineTerminator;
                HW_GTE_RTPT_FLAG(flag);
                if (flag >= 0) {
                    HW_GTE_AVSZ3_OTZ(otz);
                    pkt[2] = gHw_LineTemplate;
                    otz >>= shift;
                    otz <<= 2;
                    otz += ot;
                    slot = (u32 *)otz;
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
                rec++;
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
    register volatile ModelPrimitiveBlockHeader *blk asm("$7");
    register volatile ModelDrawFt4WindowRecord *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    blk = (ModelPrimitiveBlockHeader *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = blk->record_count;
    off = blk->word_offset;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = (ModelDrawFt4WindowRecord *)(base + off);
        if (mode != gHw_At) {
            register u32 verts asm("$5");
            register u32 normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF3Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u32)arg->vertop;
            normals = (u32)*(SVECTOR *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32)otp->org;
            do {
                register u32 w0 asm("$8") = rec->words[5];
                register u32 w1 asm("$9") = rec->words[6];
                register u32 v asm("$13");
                register u32 v3 asm("$11");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 otz asm("$2");
                register u32 t asm("$13");
                v = w0 << 16;
                v >>= 13;
                v += verts;
                HW_GTE_LOAD_V0(v);
                v = w0 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V1(v);
                v = w1 << 16;
                v >>= 13;
                v += verts;
                HW_GTE_LOAD_V2(v);
                v3 = w1 >> 16;
                v3 <<= 3;
                HW_GTE_RTPT();
                HW_GTE_READ_FLAG(flag);
                v = v3 + verts;
                pkt[1] = gHw_DrawModeTemplate;
                if (flag >= 0) {
                    HW_GTE_LOAD_V0(v);
                    HW_GTE_STORE_SXY_LOOP(pkt, 0xC, 0x10, 0x24, 0x28);
                    pkt[6] = gHw_LineTerminator;
                    pkt[11] = gHw_LineTerminator;
                    HW_GTE_RTPS_FLAG(flag);
                    if (flag >= 0) {
                        HW_GTE_AVSZ4_OTZ(otz);
                        pkt[2] = gHw_LineTemplate;
                        pkt[7] = gHw_LineTemplate;
                        otz >>= shift;
                        otz <<= 2;
                        otz += ot;
                        slot = (u32 *)otz;
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
                rec++;
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
    register volatile ModelPrimitiveBlockHeader *blk asm("$7");
    register volatile ModelDrawGt4WindowRecord *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    blk = (ModelPrimitiveBlockHeader *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = blk->record_count;
    off = blk->word_offset;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x20, 17, 0x24, 18, 0x28);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = (ModelDrawGt4WindowRecord *)(base + off);
        if (mode != gHw_At) {
            register u32 verts asm("$5");
            register u32 normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF3Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            verts = (u32)arg->vertop;
            normals = (u32)*(SVECTOR *volatile *)&arg->nortop;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32)otp->org;
            do {
                register u32 w0 asm("$8") = rec->words[4];
                register u32 w1 asm("$9") = rec->words[5];
                register u32 w2 asm("$10") = rec->words[6];
                register u32 w3 asm("$11") = rec->words[7];
                register u32 v asm("$13");
                register u32 v3 asm("$13");
                register s32 flag asm("$2");
                register u32 *slot asm("$2");
                register u32 otz asm("$2");
                register u32 t asm("$13");
                v = w0 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V0(v);
                v = w1 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V1(v);
                v = w2 >> 16;
                v <<= 3;
                v += verts;
                HW_GTE_LOAD_V2(v);
                v3 = w3 >> 16;
                v3 <<= 3;
                HW_GTE_RTPT();
                HW_GTE_READ_FLAG(flag);
                v = v3 + verts;
                pkt[1] = gHw_DrawModeTemplate;
                if (flag >= 0) {
                    HW_GTE_LOAD_V0(v);
                    HW_GTE_STORE_SXY_LOOP(pkt, 0xC, 0x10, 0x24, 0x28);
                    pkt[6] = gHw_LineTerminator;
                    pkt[11] = gHw_LineTerminator;
                    HW_GTE_RTPS_FLAG(flag);
                    if (flag >= 0) {
                        HW_GTE_AVSZ4_OTZ(otz);
                        pkt[2] = gHw_LineTemplate;
                        pkt[7] = gHw_LineTemplate;
                        otz >>= shift;
                        otz <<= 2;
                        otz += ot;
                        slot = (u32 *)otz;
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
                rec++;
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
    register volatile ModelPrimitiveBlockHeader *blk asm("$7");
    register volatile ModelDrawGt3Record *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    blk = (ModelPrimitiveBlockHeader *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = blk->record_count;
    off = blk->word_offset;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x28, 17, 0x2C, 18, 0x30);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = (ModelDrawGt3Record *)(base + off);
        if (mode != gHw_At) {
            register u32 cache asm("$5");
            register u32 normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF4Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            cache = (u32)arg->vertop2;
            normals = (u32)*(SVECTOR *volatile *)&arg->nortop2;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32)otp->org;
            do {
                register s32 empty asm("$3");
                empty = -1;
                {
                register u32 w0 asm("$8") = rec->words[3];
                register u32 w1 asm("$9") = rec->words[4];
                register u32 w2 asm("$10") = rec->words[5];
                register u32 e asm("$13");
                register s32 z asm("$2");
                volatile u32 *entry;
                register u32 *slot asm("$2");
                register u32 otz asm("$2");
                register u32 t asm("$13");
                e = w0 >> 16;
                e <<= 3;
                entry = (volatile u32 *)(e + cache);
                z = entry[1];
                e = entry[0];
                pkt[3] = e;
                if (z != empty) {
                    HW_GTE_WRITE(17, z);
                    pkt[6] = e;
                    e = w1 >> 16;
                    e <<= 3;
                    entry = (volatile u32 *)(e + cache);
                    z = entry[1];
                    e = entry[0];
                    pkt[4] = e;
                    if (z != empty) {
                        HW_GTE_WRITE(18, z);
                        e = w2 >> 16;
                        e <<= 3;
                        entry = (volatile u32 *)(e + cache);
                        z = entry[1];
                        e = entry[0];
                        pkt[5] = e;
                        if (z != empty) {
                            HW_GTE_WRITE(19, z);
                            pkt[1] = gHw_DrawModeTemplate;
                            pkt[7] = gHw_LineTerminator;
                            HW_GTE_AVSZ3_OTZ(otz);
                            pkt[2] = gHw_LineTemplate;
                            otz >>= shift;
                            otz <<= 2;
                            otz += ot;
                            slot = (u32 *)otz;
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
                rec++;
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
    register volatile ModelPrimitiveBlockHeader *blk asm("$7");
    register volatile ModelDrawGt4Record *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    blk = (ModelPrimitiveBlockHeader *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = blk->record_count;
    off = blk->word_offset;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x28, 17, 0x2C, 18, 0x30);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = (ModelDrawGt4Record *)(base + off);
        if (mode != gHw_At) {
            register u32 cache asm("$5");
            register u32 normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF3Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            cache = (u32)arg->vertop2;
            normals = (u32)*(SVECTOR *volatile *)&arg->nortop2;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32)otp->org;
            do {
                register s32 empty asm("$3");
                empty = -1;
                {
                register u32 w0 asm("$8") = rec->words[3];
                register u32 w1 asm("$9") = rec->words[4];
                register u32 w2 asm("$10") = rec->words[5];
                register u32 w3 asm("$11") = rec->words[6];
                register u32 e asm("$13");
                register s32 z asm("$2");
                volatile u32 *entry;
                register u32 *slot asm("$2");
                register u32 otz asm("$2");
                register u32 t asm("$13");
                e = w0 >> 16;
                e <<= 3;
                entry = (volatile u32 *)(e + cache);
                z = entry[1];
                e = entry[0];
                pkt[3] = e;
                if (z != empty) {
                    HW_GTE_WRITE(16, z);
                    pkt[10] = e;
                    e = w1 >> 16;
                    e <<= 3;
                    entry = (volatile u32 *)(e + cache);
                    z = entry[1];
                    e = entry[0];
                    pkt[4] = e;
                    if (z != empty) {
                        HW_GTE_WRITE(17, z);
                        e = w2 >> 16;
                        e <<= 3;
                        entry = (volatile u32 *)(e + cache);
                        z = entry[1];
                        e = entry[0];
                        pkt[9] = e;
                        if (z != empty) {
                            HW_GTE_WRITE(18, z);
                            e = w3 >> 16;
                            e <<= 3;
                            entry = (volatile u32 *)(e + cache);
                            z = entry[1];
                            e = entry[0];
                            pkt[5] = e;
                            if (z != empty) {
                                HW_GTE_WRITE(19, z);
                                pkt[8] = e;
                                pkt[6] = gHw_LineTerminator;
                                pkt[11] = gHw_LineTerminator;
                                HW_GTE_AVSZ4_OTZ(otz);
                                pkt[2] = gHw_LineTemplate;
                                pkt[7] = gHw_LineTemplate;
                                otz >>= shift;
                                otz <<= 2;
                                otz += ot;
                                slot = (u32 *)otz;
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
                rec++;
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
    register volatile ModelPrimitiveBlockHeader *blk asm("$7");
    register volatile ModelDrawGt3WindowRecord *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    blk = (ModelPrimitiveBlockHeader *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = blk->record_count;
    off = blk->word_offset;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x28, 17, 0x2C, 18, 0x30);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = (ModelDrawGt3WindowRecord *)(base + off);
        if (mode != gHw_At) {
            register u32 cache asm("$5");
            register u32 normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF4Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            cache = (u32)arg->vertop2;
            normals = (u32)*(SVECTOR *volatile *)&arg->nortop2;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32)otp->org;
            do {
                register s32 empty asm("$3");
                empty = -1;
                {
                register u32 w0 asm("$8") = rec->words[4];
                register u32 w1 asm("$9") = rec->words[5];
                register u32 w2 asm("$10") = rec->words[6];
                register u32 e asm("$13");
                register s32 z asm("$2");
                volatile u32 *entry;
                register u32 *slot asm("$2");
                register u32 otz asm("$2");
                register u32 t asm("$13");
                e = w0 >> 16;
                e <<= 3;
                entry = (volatile u32 *)(e + cache);
                z = entry[1];
                e = entry[0];
                pkt[3] = e;
                if (z != empty) {
                    HW_GTE_WRITE(17, z);
                    pkt[6] = e;
                    e = w1 >> 16;
                    e <<= 3;
                    entry = (volatile u32 *)(e + cache);
                    z = entry[1];
                    e = entry[0];
                    pkt[4] = e;
                    if (z != empty) {
                        HW_GTE_WRITE(18, z);
                        e = w2 >> 16;
                        e <<= 3;
                        entry = (volatile u32 *)(e + cache);
                        z = entry[1];
                        e = entry[0];
                        pkt[5] = e;
                        if (z != empty) {
                            HW_GTE_WRITE(19, z);
                            pkt[1] = gHw_DrawModeTemplate;
                            pkt[7] = gHw_LineTerminator;
                            HW_GTE_AVSZ3_OTZ(otz);
                            pkt[2] = gHw_LineTemplate;
                            otz >>= shift;
                            otz <<= 2;
                            otz += ot;
                            slot = (u32 *)otz;
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
                rec++;
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
    register volatile ModelPrimitiveBlockHeader *blk asm("$7");
    register volatile ModelDrawGt4WindowRecord *rec asm("$7");
    register u32 *pkt asm("$25");
    register u32 count asm("$24");
    register u32 off asm("$6");
    register u8 *base asm("$5");
    register u32 mode asm("$13");

    blk = (ModelPrimitiveBlockHeader *)arg->primp;
    pkt = (u32 *)arg->out_packetp;
    count = blk->record_count;
    off = blk->word_offset;
    base = *(u8 *volatile *)&arg->primtop;
    off <<= 2;
    HW_SAVE_REGS3(arg, 16, 0x28, 17, 0x2C, 18, 0x30);
    mode = *(u32 *)&gModel_bCurrentDrawMode;
    if (count != 0) {
        gHw_At = 1;
        rec = (ModelDrawGt4WindowRecord *)(base + off);
        if (mode != gHw_At) {
            register u32 cache asm("$5");
            register u32 normals asm("$6");
            register GsOT *otp asm("$13");
            register u32 shift asm("$14");
            register u32 ot asm("$15");
            gHw_LineTemplate = gModel_dwLineF3Template;
            gHw_DrawModeTemplate = gModel_dwDrawModeTemplate;
            gHw_LineTerminator = 0x55555555;
            cache = (u32)arg->vertop2;
            normals = (u32)*(SVECTOR *volatile *)&arg->nortop2;
            otp = arg->tagp;
            shift = arg->shift;
            ot = (u32)otp->org;
            do {
                register s32 empty asm("$3");
                empty = -1;
                {
                register u32 w0 asm("$8") = rec->words[4];
                register u32 w1 asm("$9") = rec->words[5];
                register u32 w2 asm("$10") = rec->words[6];
                register u32 w3 asm("$11") = rec->words[7];
                register u32 e asm("$13");
                register s32 z asm("$2");
                volatile u32 *entry;
                register u32 *slot asm("$2");
                register u32 otz asm("$2");
                register u32 t asm("$13");
                e = w0 >> 16;
                e <<= 3;
                entry = (volatile u32 *)(e + cache);
                z = entry[1];
                e = entry[0];
                pkt[3] = e;
                if (z != empty) {
                    HW_GTE_WRITE(16, z);
                    pkt[10] = e;
                    e = w1 >> 16;
                    e <<= 3;
                    entry = (volatile u32 *)(e + cache);
                    z = entry[1];
                    e = entry[0];
                    pkt[4] = e;
                    if (z != empty) {
                        HW_GTE_WRITE(17, z);
                        e = w2 >> 16;
                        e <<= 3;
                        entry = (volatile u32 *)(e + cache);
                        z = entry[1];
                        e = entry[0];
                        pkt[9] = e;
                        if (z != empty) {
                            HW_GTE_WRITE(18, z);
                            e = w3 >> 16;
                            e <<= 3;
                            entry = (volatile u32 *)(e + cache);
                            z = entry[1];
                            e = entry[0];
                            pkt[5] = e;
                            if (z != empty) {
                                HW_GTE_WRITE(19, z);
                                pkt[8] = e;
                                pkt[6] = gHw_LineTerminator;
                                pkt[11] = gHw_LineTerminator;
                                HW_GTE_AVSZ4_OTZ(otz);
                                pkt[2] = gHw_LineTemplate;
                                pkt[7] = gHw_LineTemplate;
                                otz >>= shift;
                                otz <<= 2;
                                otz += ot;
                                slot = (u32 *)otz;
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
                rec++;
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
