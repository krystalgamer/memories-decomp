/*
 * Wireframe drivers for the GsARGUNIT polygon formats (0x80069E44-0x8006AF74).
 *
 * The originals were written by hand: no stack frame, s0-s2 parked in the
 * GsARGUNIT block, the next primitive block returned in $v0. This is plain C
 * over the stock Psy-Q inline_c.h and libgpu.h macros, built with the
 * gcc_2_8_1_o2_massaged profile: GCC's -O2 output is rewritten into the
 * original instructions by tools/project/asm_massage.py with the edit files
 * in wireframe_renderers.massage/ (one per function). See
 * notes/asm-massage.md and notes/wireframe-renderers.md.
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/inline_c.h"
#include "gpu_packets.h"
#include "model.h"
#include "model_handler_state.h"
#include "model_slot_setup.h"
#include "wireframe_renderers.h"

/* Wireframe FT3 driver. Each visible triangle becomes a closed four-point
 * LINE_F4 (draw-mode word, colour/code word, four XYs, terminator) linked
 * into the ordering table at its average Z. */
u32 *Model_DrawFt3Wireframes(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    u32 *packet = (u32 *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt3Record *record =
        (ModelDrawFt3Record *)(arg->primtop + block->word_offset);

    /* The original tests the whole word at gModel_bCurrentDrawMode (draw
     * mode, blend flag, tint red/green) against 1. Model_DrawSlot returns
     * early for draw mode 1 (model.c, before the part loop), so this is
     * effectively always true. */
    if (count != 0 &&
        ((u32)gModel_bCurrentDrawMode | (u32)gModel_bBlendRateActive << 8 |
         (u32)gModel_wDrawTintRedGreen << 16) != 1) {
        u32 line = gModel_dwLineF4Template;
        u32 draw_mode = gModel_dwDrawModeTemplate;
        u32 terminator = 0x55555555;
        SVECTOR *vertices = arg->vertop;
        GsOT_TAG *ot = arg->tagp->org;
        u32 shift = arg->shift;

        do {
            s32 flag;
            u32 otz;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            packet[1] = draw_mode;
            packet[7] = terminator;
            gte_rtpt_b();
            gte_stflg(&flag);
            if (flag >= 0) {
                GsOT_TAG *slot;
                u32 next;

                gte_avsz3_b();
                gte_stotz(&otz);
                packet[2] = line;
                slot = &ot[otz >> shift];
                next = getaddr(slot);
                gte_stsxy0(&packet[3]);
                gte_stsxy1(&packet[4]);
                gte_stsxy2(&packet[5]);
                gte_stsxy0(&packet[6]);
                setlen(packet, 7);
                setaddr(packet, next);
                setaddr(slot, packet);
                packet += 8;
            }
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = packet;
    return (u32 *)(arg->primp + 2);
}

/* Wireframe GT3 driver. Each visible triangle becomes a closed four-point
 * LINE_F4 (draw-mode word, colour/code word, four XYs, terminator) linked
 * into the ordering table at its average Z. */
u32 *Model_DrawGt3Wireframes(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    u32 *packet = (u32 *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3Record *record =
        (ModelDrawGt3Record *)(arg->primtop + block->word_offset);

    /* The original tests the whole word at gModel_bCurrentDrawMode (draw
     * mode, blend flag, tint red/green) against 1. Model_DrawSlot returns
     * early for draw mode 1 (model.c, before the part loop), so this is
     * effectively always true. */
    if (count != 0 &&
        ((u32)gModel_bCurrentDrawMode | (u32)gModel_bBlendRateActive << 8 |
         (u32)gModel_wDrawTintRedGreen << 16) != 1) {
        u32 line = gModel_dwLineF4Template;
        u32 draw_mode = gModel_dwDrawModeTemplate;
        u32 terminator = 0x55555555;
        SVECTOR *vertices = arg->vertop;
        GsOT_TAG *ot = arg->tagp->org;
        u32 shift = arg->shift;

        do {
            s32 flag;
            u32 otz;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            packet[1] = draw_mode;
            packet[7] = terminator;
            gte_rtpt_b();
            gte_stflg(&flag);
            if (flag >= 0) {
                GsOT_TAG *slot;
                u32 next;

                gte_avsz3_b();
                gte_stotz(&otz);
                packet[2] = line;
                slot = &ot[otz >> shift];
                next = getaddr(slot);
                gte_stsxy0(&packet[3]);
                gte_stsxy1(&packet[4]);
                gte_stsxy2(&packet[5]);
                gte_stsxy0(&packet[6]);
                setlen(packet, 7);
                setaddr(packet, next);
                setaddr(slot, packet);
                packet += 8;
            }
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = packet;
    return (u32 *)(arg->primp + 2);
}

/* Wireframe FT4 driver. Each visible quad becomes two three-point LINE_F3
 * polylines (0-1-3 and 3-2-0) behind one draw-mode word, linked into the
 * ordering table at its average Z. RTPT projects vertices 0-2; RTPS then
 * projects vertex 3. */
u32 *Model_DrawFt4Wireframes(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    u32 *packet = (u32 *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt4Record *record =
        (ModelDrawFt4Record *)(arg->primtop + block->word_offset);

    /* Whole-word draw-mode test, as in Model_DrawFt3Wireframes. */
    if (count != 0 &&
        ((u32)gModel_bCurrentDrawMode | (u32)gModel_bBlendRateActive << 8 |
         (u32)gModel_wDrawTintRedGreen << 16) != 1) {
        u32 line = gModel_dwLineF3Template;
        u32 draw_mode = gModel_dwDrawModeTemplate;
        u32 terminator = 0x55555555;
        SVECTOR *vertices = arg->vertop;
        GsOT_TAG *ot = arg->tagp->org;
        u32 shift = arg->shift;

        do {
            s32 flag;
            u32 otz;
            SVECTOR *fourth;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            fourth = &vertices[record->prim.vert3];
            gte_rtpt_b();
            gte_stflg(&flag);
            packet[1] = draw_mode;
            if (flag >= 0) {
                gte_ldv0(fourth);
                gte_stsxy0(&packet[3]);
                gte_stsxy1(&packet[4]);
                gte_stsxy2(&packet[9]);
                gte_stsxy0(&packet[10]);
                packet[6] = terminator;
                packet[11] = terminator;
                gte_rtps_b();
                gte_stflg(&flag);
                if (flag >= 0) {
                    GsOT_TAG *slot;
                    u32 next;

                    gte_avsz4_b();
                    gte_stotz(&otz);
                    packet[2] = line;
                    packet[7] = line;
                    slot = &ot[otz >> shift];
                    next = getaddr(slot);
                    gte_stsxy2(&packet[5]);
                    gte_stsxy2(&packet[8]);
                    setlen(packet, 11);
                    setaddr(packet, next);
                    setaddr(slot, packet);
                    packet += 12;
                }
            }
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = packet;
    return (u32 *)(arg->primp + 2);
}

/* Wireframe GT4 driver. Each visible quad becomes two three-point LINE_F3
 * polylines (0-1-3 and 3-2-0) behind one draw-mode word, linked into the
 * ordering table at its average Z. RTPT projects vertices 0-2; RTPS then
 * projects vertex 3. */
u32 *Model_DrawGt4Wireframes(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    u32 *packet = (u32 *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4Record *record =
        (ModelDrawGt4Record *)(arg->primtop + block->word_offset);

    /* Whole-word draw-mode test, as in Model_DrawFt3Wireframes. */
    if (count != 0 &&
        ((u32)gModel_bCurrentDrawMode | (u32)gModel_bBlendRateActive << 8 |
         (u32)gModel_wDrawTintRedGreen << 16) != 1) {
        u32 line = gModel_dwLineF3Template;
        u32 draw_mode = gModel_dwDrawModeTemplate;
        u32 terminator = 0x55555555;
        SVECTOR *vertices = arg->vertop;
        GsOT_TAG *ot = arg->tagp->org;
        u32 shift = arg->shift;

        do {
            s32 flag;
            u32 otz;
            SVECTOR *fourth;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            fourth = &vertices[record->prim.vert3];
            gte_rtpt_b();
            gte_stflg(&flag);
            packet[1] = draw_mode;
            if (flag >= 0) {
                gte_ldv0(fourth);
                gte_stsxy0(&packet[3]);
                gte_stsxy1(&packet[4]);
                gte_stsxy2(&packet[9]);
                gte_stsxy0(&packet[10]);
                packet[6] = terminator;
                packet[11] = terminator;
                gte_rtps_b();
                gte_stflg(&flag);
                if (flag >= 0) {
                    GsOT_TAG *slot;
                    u32 next;

                    gte_avsz4_b();
                    gte_stotz(&otz);
                    packet[2] = line;
                    packet[7] = line;
                    slot = &ot[otz >> shift];
                    next = getaddr(slot);
                    gte_stsxy2(&packet[5]);
                    gte_stsxy2(&packet[8]);
                    setlen(packet, 11);
                    setaddr(packet, next);
                    setaddr(slot, packet);
                    packet += 12;
                }
            }
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = packet;
    return (u32 *)(arg->primp + 2);
}

/* Wireframe FT3 window-record driver (the record starts with the
 * texture-window word, which wireframes skip). Each visible triangle
 * becomes a closed four-point LINE_F4 (draw-mode word, colour/code word,
 * four XYs, terminator) linked into the ordering table at its average Z. */
u32 *Model_DrawFt3WindowRecordWireframes(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    u32 *packet = (u32 *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt3WindowRecord *record =
        (ModelDrawFt3WindowRecord *)(arg->primtop + block->word_offset);

    /* The original tests the whole word at gModel_bCurrentDrawMode (draw
     * mode, blend flag, tint red/green) against 1. Model_DrawSlot returns
     * early for draw mode 1 (model.c, before the part loop), so this is
     * effectively always true. */
    if (count != 0 &&
        ((u32)gModel_bCurrentDrawMode | (u32)gModel_bBlendRateActive << 8 |
         (u32)gModel_wDrawTintRedGreen << 16) != 1) {
        u32 line = gModel_dwLineF4Template;
        u32 draw_mode = gModel_dwDrawModeTemplate;
        u32 terminator = 0x55555555;
        SVECTOR *vertices = arg->vertop;
        GsOT_TAG *ot = arg->tagp->org;
        u32 shift = arg->shift;

        do {
            s32 flag;
            u32 otz;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            packet[1] = draw_mode;
            packet[7] = terminator;
            gte_rtpt_b();
            gte_stflg(&flag);
            if (flag >= 0) {
                GsOT_TAG *slot;
                u32 next;

                gte_avsz3_b();
                gte_stotz(&otz);
                packet[2] = line;
                slot = &ot[otz >> shift];
                next = getaddr(slot);
                gte_stsxy0(&packet[3]);
                gte_stsxy1(&packet[4]);
                gte_stsxy2(&packet[5]);
                gte_stsxy0(&packet[6]);
                setlen(packet, 7);
                setaddr(packet, next);
                setaddr(slot, packet);
                packet += 8;
            }
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = packet;
    return (u32 *)(arg->primp + 2);
}

/* Wireframe GT3 window-record driver (the record starts with the
 * texture-window word, which wireframes skip). Each visible triangle
 * becomes a closed four-point LINE_F4 (draw-mode word, colour/code word,
 * four XYs, terminator) linked into the ordering table at its average Z. */
u32 *Model_DrawGt3WindowRecordWireframes(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    u32 *packet = (u32 *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3WindowRecord *record =
        (ModelDrawGt3WindowRecord *)(arg->primtop + block->word_offset);

    /* The original tests the whole word at gModel_bCurrentDrawMode (draw
     * mode, blend flag, tint red/green) against 1. Model_DrawSlot returns
     * early for draw mode 1 (model.c, before the part loop), so this is
     * effectively always true. */
    if (count != 0 &&
        ((u32)gModel_bCurrentDrawMode | (u32)gModel_bBlendRateActive << 8 |
         (u32)gModel_wDrawTintRedGreen << 16) != 1) {
        u32 line = gModel_dwLineF4Template;
        u32 draw_mode = gModel_dwDrawModeTemplate;
        u32 terminator = 0x55555555;
        SVECTOR *vertices = arg->vertop;
        GsOT_TAG *ot = arg->tagp->org;
        u32 shift = arg->shift;

        do {
            s32 flag;
            u32 otz;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            packet[1] = draw_mode;
            packet[7] = terminator;
            gte_rtpt_b();
            gte_stflg(&flag);
            if (flag >= 0) {
                GsOT_TAG *slot;
                u32 next;

                gte_avsz3_b();
                gte_stotz(&otz);
                packet[2] = line;
                slot = &ot[otz >> shift];
                next = getaddr(slot);
                gte_stsxy0(&packet[3]);
                gte_stsxy1(&packet[4]);
                gte_stsxy2(&packet[5]);
                gte_stsxy0(&packet[6]);
                setlen(packet, 7);
                setaddr(packet, next);
                setaddr(slot, packet);
                packet += 8;
            }
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = packet;
    return (u32 *)(arg->primp + 2);
}

/* Wireframe FT4 window-record driver (the record starts with the
 * texture-window word, which wireframes skip). Each visible quad becomes
 * two three-point LINE_F3 polylines (0-1-3 and 3-2-0) behind one draw-mode
 * word, linked into the ordering table at its average Z. RTPT projects
 * vertices 0-2; RTPS then projects vertex 3. */
u32 *Model_DrawFt4WindowRecordWireframes(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    u32 *packet = (u32 *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt4WindowRecord *record =
        (ModelDrawFt4WindowRecord *)(arg->primtop + block->word_offset);

    /* Whole-word draw-mode test, as in Model_DrawFt3Wireframes. */
    if (count != 0 &&
        ((u32)gModel_bCurrentDrawMode | (u32)gModel_bBlendRateActive << 8 |
         (u32)gModel_wDrawTintRedGreen << 16) != 1) {
        u32 line = gModel_dwLineF3Template;
        u32 draw_mode = gModel_dwDrawModeTemplate;
        u32 terminator = 0x55555555;
        SVECTOR *vertices = arg->vertop;
        GsOT_TAG *ot = arg->tagp->org;
        u32 shift = arg->shift;

        do {
            s32 flag;
            u32 otz;
            SVECTOR *fourth;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            fourth = &vertices[record->prim.prim.vert3];
            gte_rtpt_b();
            gte_stflg(&flag);
            packet[1] = draw_mode;
            if (flag >= 0) {
                gte_ldv0(fourth);
                gte_stsxy0(&packet[3]);
                gte_stsxy1(&packet[4]);
                gte_stsxy2(&packet[9]);
                gte_stsxy0(&packet[10]);
                packet[6] = terminator;
                packet[11] = terminator;
                gte_rtps_b();
                gte_stflg(&flag);
                if (flag >= 0) {
                    GsOT_TAG *slot;
                    u32 next;

                    gte_avsz4_b();
                    gte_stotz(&otz);
                    packet[2] = line;
                    packet[7] = line;
                    slot = &ot[otz >> shift];
                    next = getaddr(slot);
                    gte_stsxy2(&packet[5]);
                    gte_stsxy2(&packet[8]);
                    setlen(packet, 11);
                    setaddr(packet, next);
                    setaddr(slot, packet);
                    packet += 12;
                }
            }
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = packet;
    return (u32 *)(arg->primp + 2);
}

/* Wireframe GT4 window-record driver (the record starts with the
 * texture-window word, which wireframes skip). Each visible quad becomes
 * two three-point LINE_F3 polylines (0-1-3 and 3-2-0) behind one draw-mode
 * word, linked into the ordering table at its average Z. RTPT projects
 * vertices 0-2; RTPS then projects vertex 3. */
u32 *Model_DrawGt4WindowRecordWireframes(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    u32 *packet = (u32 *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4WindowRecord *record =
        (ModelDrawGt4WindowRecord *)(arg->primtop + block->word_offset);

    /* Whole-word draw-mode test, as in Model_DrawFt3Wireframes. */
    if (count != 0 &&
        ((u32)gModel_bCurrentDrawMode | (u32)gModel_bBlendRateActive << 8 |
         (u32)gModel_wDrawTintRedGreen << 16) != 1) {
        u32 line = gModel_dwLineF3Template;
        u32 draw_mode = gModel_dwDrawModeTemplate;
        u32 terminator = 0x55555555;
        SVECTOR *vertices = arg->vertop;
        GsOT_TAG *ot = arg->tagp->org;
        u32 shift = arg->shift;

        do {
            s32 flag;
            u32 otz;
            SVECTOR *fourth;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            fourth = &vertices[record->prim.prim.vert3];
            gte_rtpt_b();
            gte_stflg(&flag);
            packet[1] = draw_mode;
            if (flag >= 0) {
                gte_ldv0(fourth);
                gte_stsxy0(&packet[3]);
                gte_stsxy1(&packet[4]);
                gte_stsxy2(&packet[9]);
                gte_stsxy0(&packet[10]);
                packet[6] = terminator;
                packet[11] = terminator;
                gte_rtps_b();
                gte_stflg(&flag);
                if (flag >= 0) {
                    GsOT_TAG *slot;
                    u32 next;

                    gte_avsz4_b();
                    gte_stotz(&otz);
                    packet[2] = line;
                    packet[7] = line;
                    slot = &ot[otz >> shift];
                    next = getaddr(slot);
                    gte_stsxy2(&packet[5]);
                    gte_stsxy2(&packet[8]);
                    setlen(packet, 11);
                    setaddr(packet, next);
                    setaddr(slot, packet);
                    packet += 12;
                }
            }
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = packet;
    return (u32 *)(arg->primp + 2);
}

/* Wireframe driver for shared GT3 records. The vertices were projected
 * earlier into the GsWORKUNIT cache (vertop2); an entry whose depth word
 * (otz and p) is all ones is invalid and drops the triangle. */
u32 *Model_DrawSharedGt3Wireframes(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    WireTrianglePacket *packet = (WireTrianglePacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3Record *record =
        (ModelDrawGt3Record *)(arg->primtop + block->word_offset);

    /* Whole-word draw-mode test, as in Model_DrawFt3Wireframes. */
    if (count != 0 &&
        ((u32)gModel_bCurrentDrawMode | (u32)gModel_bBlendRateActive << 8 |
         (u32)gModel_wDrawTintRedGreen << 16) != 1) {
        u32 line = gModel_dwLineF4Template;
        u32 draw_mode = gModel_dwDrawModeTemplate;
        u32 terminator = 0x55555555;
        GsWORKUNIT *cache = arg->vertop2;
        GsOT_TAG *ot = arg->tagp->org;
        u32 shift = arg->shift;

        do {
            /* Depth words go to the 16-bit SZ registers, which keep otz. */
            GsWORKUNIT *v0 = &cache[record->prim.vert0];
            u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;

            packet->xy[0] = v0->vec;
            if (z0 != 0xFFFFFFFF) {
                GsWORKUNIT *v1;
                u32 z1;

                packet->xy[3] = v0->vec;
                v1 = &cache[record->prim.vert1];
                z1 = (u32)(u16)v1->otz | (u32)(u16)v1->p << 16;
                packet->xy[1] = v1->vec;
                if (z1 != 0xFFFFFFFF) {
                    GsWORKUNIT *v2 = &cache[record->prim.vert2];
                    u32 z2 = (u32)(u16)v2->otz | (u32)(u16)v2->p << 16;

                    packet->xy[2] = v2->vec;
                    if (z2 != 0xFFFFFFFF) {
                        u32 otz;
                        GsOT_TAG *slot;
                        u32 next;

                        gte_ldsz3(z0, z1, z2);
                        packet->draw_mode = draw_mode;
                        packet->terminator = terminator;
                        gte_avsz3_b();
                        gte_stotz(&otz);
                        packet->line = line;
                        slot = &ot[otz >> shift];
                        next = getaddr(slot);
                        setlen(packet, 7);
                        setaddr(packet, next);
                        setaddr(slot, packet);
                        packet++;
                    }
                }
            }
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Wireframe driver for shared GT4 records, reading the projected GsWORKUNIT
 * cache (vertop2) like Model_DrawSharedGt3Wireframes. */
u32 *Model_DrawSharedGt4Wireframes(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    WireQuadPacket *packet = (WireQuadPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4Record *record =
        (ModelDrawGt4Record *)(arg->primtop + block->word_offset);

    /* Whole-word draw-mode test, as in Model_DrawFt3Wireframes. */
    if (count != 0 &&
        ((u32)gModel_bCurrentDrawMode | (u32)gModel_bBlendRateActive << 8 |
         (u32)gModel_wDrawTintRedGreen << 16) != 1) {
        u32 line = gModel_dwLineF3Template;
        u32 draw_mode = gModel_dwDrawModeTemplate;
        u32 terminator = 0x55555555;
        GsWORKUNIT *cache = arg->vertop2;
        GsOT_TAG *ot = arg->tagp->org;
        u32 shift = arg->shift;

        do {
            /* Depth words go to the 16-bit SZ registers, which keep otz. */
            GsWORKUNIT *v0 = &cache[record->prim.vert0];
            u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;

            packet->edge[0].xy[0] = v0->vec;
            if (z0 != 0xFFFFFFFF) {
                GsWORKUNIT *v1;
                u32 z1;

                packet->edge[1].xy[2] = v0->vec;
                v1 = &cache[record->prim.vert1];
                z1 = (u32)(u16)v1->otz | (u32)(u16)v1->p << 16;
                packet->edge[0].xy[1] = v1->vec;
                if (z1 != 0xFFFFFFFF) {
                    GsWORKUNIT *v2 = &cache[record->prim.vert2];
                    u32 z2 = (u32)(u16)v2->otz | (u32)(u16)v2->p << 16;

                    packet->edge[1].xy[1] = v2->vec;
                    if (z2 != 0xFFFFFFFF) {
                        GsWORKUNIT *v3 = &cache[record->prim.vert3];
                        u32 z3 = (u32)(u16)v3->otz | (u32)(u16)v3->p << 16;

                        packet->edge[0].xy[2] = v3->vec;
                        if (z3 != 0xFFFFFFFF) {
                            u32 otz;
                            GsOT_TAG *slot;
                            u32 next;

                            packet->edge[1].xy[0] = v3->vec;
                            gte_ldsz4(z0, z1, z2, z3);
                            packet->edge[0].terminator = terminator;
                            packet->edge[1].terminator = terminator;
                            gte_avsz4_b();
                            gte_stotz(&otz);
                            packet->edge[0].line = line;
                            packet->edge[1].line = line;
                            slot = &ot[otz >> shift];
                            next = getaddr(slot);
                            packet->draw_mode = draw_mode;
                            setlen(packet, 11);
                            setaddr(packet, next);
                            setaddr(slot, packet);
                            packet++;
                        }
                    }
                }
            }
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Wireframe driver for shared GT3 window records (the record starts with
 * the texture-window word, which wireframes skip). The vertices were
 * projected earlier into the GsWORKUNIT cache (vertop2); an entry whose
 * depth word (otz and p) is all ones is invalid and drops the triangle. */
u32 *Model_DrawSharedGt3WindowRecordWireframes(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    WireTrianglePacket *packet = (WireTrianglePacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3WindowRecord *record =
        (ModelDrawGt3WindowRecord *)(arg->primtop + block->word_offset);

    /* Whole-word draw-mode test, as in Model_DrawFt3Wireframes. */
    if (count != 0 &&
        ((u32)gModel_bCurrentDrawMode | (u32)gModel_bBlendRateActive << 8 |
         (u32)gModel_wDrawTintRedGreen << 16) != 1) {
        u32 line = gModel_dwLineF4Template;
        u32 draw_mode = gModel_dwDrawModeTemplate;
        u32 terminator = 0x55555555;
        GsWORKUNIT *cache = arg->vertop2;
        GsOT_TAG *ot = arg->tagp->org;
        u32 shift = arg->shift;

        do {
            /* Depth words go to the 16-bit SZ registers, which keep otz. */
            GsWORKUNIT *v0 = &cache[record->prim.prim.vert0];
            u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;

            packet->xy[0] = v0->vec;
            if (z0 != 0xFFFFFFFF) {
                GsWORKUNIT *v1;
                u32 z1;

                packet->xy[3] = v0->vec;
                v1 = &cache[record->prim.prim.vert1];
                z1 = (u32)(u16)v1->otz | (u32)(u16)v1->p << 16;
                packet->xy[1] = v1->vec;
                if (z1 != 0xFFFFFFFF) {
                    GsWORKUNIT *v2 = &cache[record->prim.prim.vert2];
                    u32 z2 = (u32)(u16)v2->otz | (u32)(u16)v2->p << 16;

                    packet->xy[2] = v2->vec;
                    if (z2 != 0xFFFFFFFF) {
                        u32 otz;
                        GsOT_TAG *slot;
                        u32 next;

                        gte_ldsz3(z0, z1, z2);
                        packet->draw_mode = draw_mode;
                        packet->terminator = terminator;
                        gte_avsz3_b();
                        gte_stotz(&otz);
                        packet->line = line;
                        slot = &ot[otz >> shift];
                        next = getaddr(slot);
                        setlen(packet, 7);
                        setaddr(packet, next);
                        setaddr(slot, packet);
                        packet++;
                    }
                }
            }
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Wireframe driver for shared GT4 window records (the record starts with
 * the texture-window word, which wireframes skip), reading the projected
 * GsWORKUNIT cache (vertop2) like Model_DrawSharedGt3Wireframes. */
u32 *Model_DrawSharedGt4WindowRecordWireframes(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    WireQuadPacket *packet = (WireQuadPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4WindowRecord *record =
        (ModelDrawGt4WindowRecord *)(arg->primtop + block->word_offset);

    /* Whole-word draw-mode test, as in Model_DrawFt3Wireframes. */
    if (count != 0 &&
        ((u32)gModel_bCurrentDrawMode | (u32)gModel_bBlendRateActive << 8 |
         (u32)gModel_wDrawTintRedGreen << 16) != 1) {
        u32 line = gModel_dwLineF3Template;
        u32 draw_mode = gModel_dwDrawModeTemplate;
        u32 terminator = 0x55555555;
        GsWORKUNIT *cache = arg->vertop2;
        GsOT_TAG *ot = arg->tagp->org;
        u32 shift = arg->shift;

        do {
            /* Depth words go to the 16-bit SZ registers, which keep otz. */
            GsWORKUNIT *v0 = &cache[record->prim.prim.vert0];
            u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;

            packet->edge[0].xy[0] = v0->vec;
            if (z0 != 0xFFFFFFFF) {
                GsWORKUNIT *v1;
                u32 z1;

                packet->edge[1].xy[2] = v0->vec;
                v1 = &cache[record->prim.prim.vert1];
                z1 = (u32)(u16)v1->otz | (u32)(u16)v1->p << 16;
                packet->edge[0].xy[1] = v1->vec;
                if (z1 != 0xFFFFFFFF) {
                    GsWORKUNIT *v2 = &cache[record->prim.prim.vert2];
                    u32 z2 = (u32)(u16)v2->otz | (u32)(u16)v2->p << 16;

                    packet->edge[1].xy[1] = v2->vec;
                    if (z2 != 0xFFFFFFFF) {
                        GsWORKUNIT *v3 = &cache[record->prim.prim.vert3];
                        u32 z3 = (u32)(u16)v3->otz | (u32)(u16)v3->p << 16;

                        packet->edge[0].xy[2] = v3->vec;
                        if (z3 != 0xFFFFFFFF) {
                            u32 otz;
                            GsOT_TAG *slot;
                            u32 next;

                            packet->edge[1].xy[0] = v3->vec;
                            gte_ldsz4(z0, z1, z2, z3);
                            packet->edge[0].terminator = terminator;
                            packet->edge[1].terminator = terminator;
                            gte_avsz4_b();
                            gte_stotz(&otz);
                            packet->edge[0].line = line;
                            packet->edge[1].line = line;
                            slot = &ot[otz >> shift];
                            next = getaddr(slot);
                            packet->draw_mode = draw_mode;
                            setlen(packet, 11);
                            setaddr(packet, next);
                            setaddr(slot, packet);
                            packet++;
                        }
                    }
                }
            }
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}
