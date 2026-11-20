/*
 * Shared-vertex primitive cache preparation and shared Gt3/Gt4 polygon
 * drivers (0x80067220-0x80069E44).
 *
 * Model_PrepareSharedPrimitiveCaches projects a block's shared vertices
 * into the vertop2 cache (SXY2 plus an IR0 << 16 | SZ3 word, or all ones
 * when RTPS overflowed) and, in lit modes, lights its shared normals into
 * the nortop2 colour cache; the sixteen cached-input drivers then read
 * those caches.
 *
 * The originals were written by hand: no stack frame, s0-s7 parked in the
 * GsARGUNIT block, the next primitive block returned in $v0, and the
 * preparation loops load the next vector while the GTE works on the
 * current one. This is plain C over the stock Psy-Q inline_c.h and libgpu.h
 * macros, built with the gcc_2_8_1_o2_massaged profile: GCC's -O2 output is
 * rewritten into the original instructions by tools/project/asm_massage.py
 * with the edit files in shared_polygon_renderers.massage/ (one per
 * function). See notes/asm-massage.md and
 * notes/shared-primitive-renderers.md.
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
#include "primitive_handler_modes.h"
#include "shared_primitive_renderers.h"

u32 *Model_PrepareSharedPrimitiveCaches(GsARGUNIT_SHARED *arg)
{
    ModelSharedCacheBlock *block = (ModelSharedCacheBlock *)arg->primp;
    u32 rgb = gModel_dwPolyGt3Template & 0xFFFFFF;
    s8 mode = (s8)gModel_bCurrentDrawMode;
    /* The colour cache holds two words per normal; a nonzero cache base
     * selects the second. */
    u32 offset = gModel_dwLitColorCacheBase;
    u32 count;

    gte_ldrgb(&rgb);
    if (offset != 0) {
        offset = sizeof(u32);
    }
    count = block->vertex_count;
    if (count != 0) {
        SVECTOR *vertex = &arg->vertop[block->vertex_index];
        ModelSharedCacheEntry *entry =
            (ModelSharedCacheEntry *)&arg->vertop2[block->vertex_cache_index];

        do {
            gte_ldv0(vertex);
            count--;
            vertex++;
            gte_rtps_b();
            /* Rejected until the projection is accepted. */
            entry->words[1] = 0xFFFFFFFF;
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    continue;
                }
            }
            {
                u32 p, sz;

                gte_stsxy2(&entry->words[0]);
                gte_stdp(&p);
                gte_stsz(&sz);
                entry->words[1] = p << 16 | sz;
            }
        } while (entry++, count != 0);
    }
    if (mode != 0) {
        count = block->normal_count;
        if (count != 0) {
            SVECTOR *normal = &arg->nortop[block->normal_index];
            u32 *colour = (u32 *)((u8 *)&arg->nortop2[block->colour_cache_index] + offset);

            do {
                gte_ldv0(normal);
                count--;
                normal++;
                gte_nccs_b();
                gte_strgb(colour);
                colour += 2;
            } while (count != 0);
        }
    }
    return (u32 *)(block + 1);
}

/* Shared GT3 driver. The vertices were projected into the GsWORKUNIT cache
 * (vertop2: SXY, then otz/p, all ones when rejected) and lit into the colour
 * cache (nortop2 holds two colour words per normal; a nonzero
 * gModel_dwLitColorCacheBase selects the second). Visible triangles become
 * POLY_GT3 packets; a semi-transparent template adds a second pass with the
 * template colour, the second-pass CLUT and blend mode 1. */
u32 *Model_DrawSharedGt3Primitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3Packet *packet =
        (ModelGt3Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3Record *record =
        (ModelDrawGt3Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            u32 *c0, *c1, *c2;
                            s32 opz;

                            gte_ldsz3(z0, z1, z2);
                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            c0 = &colors[record->prim.norm0 * 2];
                            gte_nclip_b();
                            c1 = &colors[record->prim.norm1 * 2];
                            c2 = &colors[record->prim.norm2 * 2];
                            gte_stopz(&opz);
                            if (opz > 0) {
                                {
                                    u32 uv0, uv1, uv2, semi, otz;
                                    GsOT_TAG *slot;
                                    u32 rgb0 = *c0;
                                    u32 rgb1 = *c1;
                                    u32 rgb2 = *c2;

                                    packet->rgb0 = rgb0 | code;
                                    packet->rgb1 = rgb1;
                                    packet->rgb2 = rgb2;
                                    gte_avsz3_b();
                                    uv0 = record->words[0];
                                    uv1 = record->words[1];
                                    uv2 = record->words[2];
                                    gte_stotz(&otz);
                                    semi = poly & 0x02000000;
                                    uv1 |= blend;
                                    slot = &ot[otz >> shift];
                                    for (;;) {
                                        packet->uv0 = uv0;
                                        packet->uv1 = uv1;
                                        packet->uv2 = uv2;
                                        setlen(packet, 9);
                                        setaddr(packet, getaddr(slot));
                                        setaddr(slot, packet);
                                        if (semi == 0 || second_clut == 0) {
                                            break;
                                        }
                                        /* Second pass: CLUT 2, blend 1, flat colour. */
                                        uv0 = (uv0 & 0xFFFF) | second_clut;
                                        uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                        packet[1].rgb0 = colour;
                                        packet[1].xy0 = packet->xy0;
                                        packet[1].rgb1 = colour;
                                        packet[1].xy1 = packet->xy1;
                                        packet[1].rgb2 = colour;
                                        packet[1].xy2 = packet->xy2;
                                        packet++;
                                        semi = 0;
                                    }
                                    packet++;
                                }
                            }
                        }
                    }
                }
                record++;
            } while (--count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Shared GT4 driver. See shared/gt3.c for the cache formats and the second
 * pass. */
u32 *Model_DrawSharedGt4Primitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4Packet *packet =
        (ModelGt4Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4Record *record =
        (ModelDrawGt4Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            GsWORKUNIT *v3;
                            u32 z3, xy3;

                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            v3 = &cache[record->prim.vert3];
                            z3 = (u32)(u16)v3->otz |
                                (u32)(u16)v3->p << 16;
                            xy3 = (u32)(u16)v3->vec.vx |
                                (u32)(u16)v3->vec.vy << 16;
                            if (z3 != 0xFFFFFFFF) {
                                u32 *c0, *c1, *c2, *c3;
                                s32 opz;

                                gte_ldsz4(z0, z1, z2, z3);
                                packet->xy3 = v3->vec;
                                c0 = &colors[record->prim.norm0 * 2];
                                gte_nclip_b();
                                c1 = &colors[record->prim.norm1 * 2];
                                gte_stopz(&opz);
                                if (opz <= 0) {
                                    /* The other diagonal: vertices 3, 1, 2. */
                                    gte_ldsxy0(xy3);
                                    gte_nclip();
                                    gte_stopz(&opz);
                                    if (opz > 0) {
                                        continue;
                                    }
                                }
                                c2 = &colors[record->prim.norm2 * 2];
                                c3 = &colors[record->prim.norm3 * 2];
                                {
                                    u32 uv0, uv1, uv2, uv3, semi, otz;
                                    GsOT_TAG *slot;
                                    u32 rgb0 = *c0;
                                    u32 rgb1 = *c1;
                                    u32 rgb2 = *c2;
                                    u32 rgb3 = *c3;

                                    packet->rgb0 = rgb0 | code;
                                    packet->rgb1 = rgb1;
                                    packet->rgb2 = rgb2;
                                    packet->rgb3 = rgb3;
                                    gte_avsz4_b();
                                    uv0 = record->words[0];
                                    uv1 = record->words[1];
                                    uv2 = record->words[2];
                                    uv3 = record->words[3];
                                    gte_stotz(&otz);
                                    semi = poly & 0x02000000;
                                    uv1 |= blend;
                                    slot = &ot[otz >> shift];
                                    for (;;) {
                                        packet->uv0 = uv0;
                                        packet->uv1 = uv1;
                                        packet->uv2 = uv2;
                                        packet->uv3 = uv3;
                                        setlen(packet, 12);
                                        setaddr(packet, getaddr(slot));
                                        setaddr(slot, packet);
                                        if (semi == 0 || second_clut == 0) {
                                            break;
                                        }
                                        /* Second pass: CLUT 2, blend 1, flat colour. */
                                        uv0 = (uv0 & 0xFFFF) | second_clut;
                                        uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                        packet[1].rgb0 = colour;
                                        packet[1].xy0 = packet->xy0;
                                        packet[1].rgb1 = colour;
                                        packet[1].xy1 = packet->xy1;
                                        packet[1].rgb2 = colour;
                                        packet[1].xy2 = packet->xy2;
                                        packet[1].rgb3 = colour;
                                        packet[1].xy3 = packet->xy3;
                                        packet++;
                                        semi = 0;
                                    }
                                    packet++;
                                }
                            }
                        }
                    }
                }
            } while (record++, --count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Shared GT3 driver, for window records (texture-window word first). See
 * shared/gt3.c for the cache formats and the second pass. */
u32 *Model_DrawSharedGt3WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3WindowRecordPacket *packet =
        (ModelGt3WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3WindowRecord *record =
        (ModelDrawGt3WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            u32 *c0, *c1, *c2;
                            s32 opz;

                            gte_ldsz3(z0, z1, z2);
                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            c0 = &colors[record->prim.prim.norm0 * 2];
                            gte_nclip_b();
                            c1 = &colors[record->prim.prim.norm1 * 2];
                            c2 = &colors[record->prim.prim.norm2 * 2];
                            gte_stopz(&opz);
                            if (opz > 0) {
                                {
                                    u32 window, uv0, uv1, uv2, semi, otz;
                                    GsOT_TAG *slot;
                                    u32 rgb0 = *c0;
                                    u32 rgb1 = *c1;
                                    u32 rgb2 = *c2;

                                    packet->rgb0 = rgb0 | code;
                                    packet->rgb1 = rgb1;
                                    packet->rgb2 = rgb2;
                                    gte_avsz3_b();
                                    window = record->words[0];
                                    uv0 = record->words[1];
                                    uv1 = record->words[2];
                                    uv2 = record->words[3];
                                    gte_stotz(&otz);
                                    semi = poly & 0x02000000;
                                    uv1 |= blend;
                                    slot = &ot[otz >> shift];
                                    for (;;) {
                                        packet->uv0 = uv0;
                                        packet->uv1 = uv1;
                                        packet->uv2 = uv2;
                                        packet->window = window;
                                        packet->window_reset = 0xE2000000;
                                        setlen(packet, 11);
                                        setaddr(packet, getaddr(slot));
                                        setaddr(slot, packet);
                                        if (semi == 0 || second_clut == 0) {
                                            break;
                                        }
                                        /* Second pass: CLUT 2, blend 1, flat colour. */
                                        uv0 = (uv0 & 0xFFFF) | second_clut;
                                        uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                        packet[1].rgb0 = colour;
                                        packet[1].xy0 = packet->xy0;
                                        packet[1].rgb1 = colour;
                                        packet[1].xy1 = packet->xy1;
                                        packet[1].rgb2 = colour;
                                        packet[1].xy2 = packet->xy2;
                                        packet++;
                                        semi = 0;
                                    }
                                    packet++;
                                }
                            }
                        }
                    }
                }
                record++;
            } while (--count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Shared GT4 driver, for window records (texture-window word first). See
 * shared/gt3.c for the cache formats and the second pass. */
u32 *Model_DrawSharedGt4WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4WindowRecordPacket *packet =
        (ModelGt4WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4WindowRecord *record =
        (ModelDrawGt4WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            GsWORKUNIT *v3;
                            u32 z3, xy3;

                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            v3 = &cache[record->prim.prim.vert3];
                            z3 = (u32)(u16)v3->otz |
                                (u32)(u16)v3->p << 16;
                            xy3 = (u32)(u16)v3->vec.vx |
                                (u32)(u16)v3->vec.vy << 16;
                            if (z3 != 0xFFFFFFFF) {
                                u32 *c0, *c1, *c2, *c3;
                                s32 opz;

                                gte_ldsz4(z0, z1, z2, z3);
                                packet->xy3 = v3->vec;
                                c0 = &colors[record->prim.prim.norm0 * 2];
                                gte_nclip_b();
                                c1 = &colors[record->prim.prim.norm1 * 2];
                                gte_stopz(&opz);
                                if (opz <= 0) {
                                    /* The other diagonal: vertices 3, 1, 2. */
                                    gte_ldsxy0(xy3);
                                    gte_nclip();
                                    gte_stopz(&opz);
                                    if (opz > 0) {
                                        continue;
                                    }
                                }
                                c2 = &colors[record->prim.prim.norm2 * 2];
                                c3 = &colors[record->prim.prim.norm3 * 2];
                                {
                                    u32 window, uv0, uv1, uv2, uv3, semi, otz;
                                    GsOT_TAG *slot;
                                    u32 rgb0 = *c0;
                                    u32 rgb1 = *c1;
                                    u32 rgb2 = *c2;
                                    u32 rgb3 = *c3;

                                    packet->rgb0 = rgb0 | code;
                                    packet->rgb1 = rgb1;
                                    packet->rgb2 = rgb2;
                                    packet->rgb3 = rgb3;
                                    gte_avsz4_b();
                                    window = record->words[0];
                                    uv0 = record->words[1];
                                    uv1 = record->words[2];
                                    uv2 = record->words[3];
                                    uv3 = record->words[4];
                                    gte_stotz(&otz);
                                    semi = poly & 0x02000000;
                                    uv1 |= blend;
                                    slot = &ot[otz >> shift];
                                    for (;;) {
                                        packet->uv0 = uv0;
                                        packet->uv1 = uv1;
                                        packet->uv2 = uv2;
                                        packet->uv3 = uv3;
                                        packet->window = window;
                                        packet->window_reset = 0xE2000000;
                                        setlen(packet, 14);
                                        setaddr(packet, getaddr(slot));
                                        setaddr(slot, packet);
                                        if (semi == 0 || second_clut == 0) {
                                            break;
                                        }
                                        /* Second pass: CLUT 2, blend 1, flat colour. */
                                        uv0 = (uv0 & 0xFFFF) | second_clut;
                                        uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                        packet[1].rgb0 = colour;
                                        packet[1].xy0 = packet->xy0;
                                        packet[1].rgb1 = colour;
                                        packet[1].xy1 = packet->xy1;
                                        packet[1].rgb2 = colour;
                                        packet[1].xy2 = packet->xy2;
                                        packet[1].rgb3 = colour;
                                        packet[1].xy3 = packet->xy3;
                                        packet++;
                                        semi = 0;
                                    }
                                    packet++;
                                }
                            }
                        }
                    }
                }
            } while (record++, --count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Shared GT3 driver, without the NCLIP back-face test. See shared/gt3.c for
 * the cache formats and the second pass. */
u32 *Model_DrawSharedTwoSidedGt3Primitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3Packet *packet =
        (ModelGt3Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3Record *record =
        (ModelDrawGt3Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            u32 *c0, *c1, *c2;

                            gte_ldsz3(z0, z1, z2);
                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            c0 = &colors[record->prim.norm0 * 2];
                            c1 = &colors[record->prim.norm1 * 2];
                            c2 = &colors[record->prim.norm2 * 2];
                            gte_avsz3_b();
                            {
                                u32 uv0, uv1, uv2, semi, otz;
                                GsOT_TAG *slot;
                                u32 rgb0 = *c0;
                                u32 rgb1 = *c1;
                                u32 rgb2 = *c2;

                                packet->rgb0 = rgb0 | code;
                                packet->rgb1 = rgb1;
                                packet->rgb2 = rgb2;
                                uv0 = record->words[0];
                                uv1 = record->words[1];
                                uv2 = record->words[2];
                                gte_stotz(&otz);
                                semi = poly & 0x02000000;
                                uv1 |= blend;
                                slot = &ot[otz >> shift];
                                for (;;) {
                                    packet->uv0 = uv0;
                                    packet->uv1 = uv1;
                                    packet->uv2 = uv2;
                                    setlen(packet, 9);
                                    setaddr(packet, getaddr(slot));
                                    setaddr(slot, packet);
                                    if (semi == 0 || second_clut == 0) {
                                        break;
                                    }
                                    /* Second pass: CLUT 2, blend 1, flat colour. */
                                    uv0 = (uv0 & 0xFFFF) | second_clut;
                                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                    packet[1].rgb0 = colour;
                                    packet[1].xy0 = packet->xy0;
                                    packet[1].rgb1 = colour;
                                    packet[1].xy1 = packet->xy1;
                                    packet[1].rgb2 = colour;
                                    packet[1].xy2 = packet->xy2;
                                    packet++;
                                    semi = 0;
                                }
                                packet++;
                            }
                        }
                    }
                }
                record++;
            } while (--count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Shared GT4 driver, without the NCLIP back-face test. See shared/gt3.c for
 * the cache formats and the second pass. */
u32 *Model_DrawSharedTwoSidedGt4Primitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4Packet *packet =
        (ModelGt4Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4Record *record =
        (ModelDrawGt4Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            GsWORKUNIT *v3;
                            u32 z3, xy3;

                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            v3 = &cache[record->prim.vert3];
                            z3 = (u32)(u16)v3->otz |
                                (u32)(u16)v3->p << 16;
                            xy3 = (u32)(u16)v3->vec.vx |
                                (u32)(u16)v3->vec.vy << 16;
                            if (z3 != 0xFFFFFFFF) {
                                u32 *c0, *c1, *c2, *c3;

                                gte_ldsz4(z0, z1, z2, z3);
                                packet->xy3 = v3->vec;
                                c0 = &colors[record->prim.norm0 * 2];
                                c1 = &colors[record->prim.norm1 * 2];
                                c2 = &colors[record->prim.norm2 * 2];
                                c3 = &colors[record->prim.norm3 * 2];
                                gte_avsz4_b();
                                {
                                    u32 uv0, uv1, uv2, uv3, semi, otz;
                                    GsOT_TAG *slot;
                                    u32 rgb0 = *c0;
                                    u32 rgb1 = *c1;
                                    u32 rgb2 = *c2;
                                    u32 rgb3 = *c3;

                                    packet->rgb0 = rgb0 | code;
                                    packet->rgb1 = rgb1;
                                    packet->rgb2 = rgb2;
                                    packet->rgb3 = rgb3;
                                    uv0 = record->words[0];
                                    uv1 = record->words[1];
                                    uv2 = record->words[2];
                                    uv3 = record->words[3];
                                    gte_stotz(&otz);
                                    semi = poly & 0x02000000;
                                    uv1 |= blend;
                                    slot = &ot[otz >> shift];
                                    for (;;) {
                                        packet->uv0 = uv0;
                                        packet->uv1 = uv1;
                                        packet->uv2 = uv2;
                                        packet->uv3 = uv3;
                                        setlen(packet, 12);
                                        setaddr(packet, getaddr(slot));
                                        setaddr(slot, packet);
                                        if (semi == 0 || second_clut == 0) {
                                            break;
                                        }
                                        /* Second pass: CLUT 2, blend 1, flat colour. */
                                        uv0 = (uv0 & 0xFFFF) | second_clut;
                                        uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                        packet[1].rgb0 = colour;
                                        packet[1].xy0 = packet->xy0;
                                        packet[1].rgb1 = colour;
                                        packet[1].xy1 = packet->xy1;
                                        packet[1].rgb2 = colour;
                                        packet[1].xy2 = packet->xy2;
                                        packet[1].rgb3 = colour;
                                        packet[1].xy3 = packet->xy3;
                                        packet++;
                                        semi = 0;
                                    }
                                    packet++;
                                }
                            }
                        }
                    }
                }
                record++;
            } while (--count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Shared GT3 driver, for window records (texture-window word first),
 * without the NCLIP back-face test. See shared/gt3.c for the cache formats
 * and the second pass. */
u32 *Model_DrawSharedTwoSidedGt3WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3WindowRecordPacket *packet =
        (ModelGt3WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3WindowRecord *record =
        (ModelDrawGt3WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            u32 *c0, *c1, *c2;

                            gte_ldsz3(z0, z1, z2);
                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            c0 = &colors[record->prim.prim.norm0 * 2];
                            c1 = &colors[record->prim.prim.norm1 * 2];
                            c2 = &colors[record->prim.prim.norm2 * 2];
                            gte_avsz3_b();
                            {
                                u32 window, uv0, uv1, uv2, semi, otz;
                                GsOT_TAG *slot;
                                u32 rgb0 = *c0;
                                u32 rgb1 = *c1;
                                u32 rgb2 = *c2;

                                packet->rgb0 = rgb0 | code;
                                packet->rgb1 = rgb1;
                                packet->rgb2 = rgb2;
                                window = record->words[0];
                                uv0 = record->words[1];
                                uv1 = record->words[2];
                                uv2 = record->words[3];
                                gte_stotz(&otz);
                                semi = poly & 0x02000000;
                                uv1 |= blend;
                                slot = &ot[otz >> shift];
                                for (;;) {
                                    packet->uv0 = uv0;
                                    packet->uv1 = uv1;
                                    packet->uv2 = uv2;
                                    packet->window = window;
                                    packet->window_reset = 0xE2000000;
                                    setlen(packet, 11);
                                    setaddr(packet, getaddr(slot));
                                    setaddr(slot, packet);
                                    if (semi == 0 || second_clut == 0) {
                                        break;
                                    }
                                    /* Second pass: CLUT 2, blend 1, flat colour. */
                                    uv0 = (uv0 & 0xFFFF) | second_clut;
                                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                    packet[1].rgb0 = colour;
                                    packet[1].xy0 = packet->xy0;
                                    packet[1].rgb1 = colour;
                                    packet[1].xy1 = packet->xy1;
                                    packet[1].rgb2 = colour;
                                    packet[1].xy2 = packet->xy2;
                                    packet++;
                                    semi = 0;
                                }
                                packet++;
                            }
                        }
                    }
                }
                record++;
            } while (--count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Shared GT4 driver, for window records (texture-window word first),
 * without the NCLIP back-face test. See shared/gt3.c for the cache formats
 * and the second pass. */
u32 *Model_DrawSharedTwoSidedGt4WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4WindowRecordPacket *packet =
        (ModelGt4WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4WindowRecord *record =
        (ModelDrawGt4WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            GsWORKUNIT *v3;
                            u32 z3, xy3;

                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            v3 = &cache[record->prim.prim.vert3];
                            z3 = (u32)(u16)v3->otz |
                                (u32)(u16)v3->p << 16;
                            xy3 = (u32)(u16)v3->vec.vx |
                                (u32)(u16)v3->vec.vy << 16;
                            if (z3 != 0xFFFFFFFF) {
                                u32 *c0, *c1, *c2, *c3;

                                gte_ldsz4(z0, z1, z2, z3);
                                packet->xy3 = v3->vec;
                                c0 = &colors[record->prim.prim.norm0 * 2];
                                c1 = &colors[record->prim.prim.norm1 * 2];
                                c2 = &colors[record->prim.prim.norm2 * 2];
                                c3 = &colors[record->prim.prim.norm3 * 2];
                                gte_avsz4_b();
                                {
                                    u32 window, uv0, uv1, uv2, uv3, semi, otz;
                                    GsOT_TAG *slot;
                                    u32 rgb0 = *c0;
                                    u32 rgb1 = *c1;
                                    u32 rgb2 = *c2;
                                    u32 rgb3 = *c3;

                                    packet->rgb0 = rgb0 | code;
                                    packet->rgb1 = rgb1;
                                    packet->rgb2 = rgb2;
                                    packet->rgb3 = rgb3;
                                    window = record->words[0];
                                    uv0 = record->words[1];
                                    uv1 = record->words[2];
                                    uv2 = record->words[3];
                                    uv3 = record->words[4];
                                    gte_stotz(&otz);
                                    semi = poly & 0x02000000;
                                    uv1 |= blend;
                                    slot = &ot[otz >> shift];
                                    for (;;) {
                                        packet->uv0 = uv0;
                                        packet->uv1 = uv1;
                                        packet->uv2 = uv2;
                                        packet->uv3 = uv3;
                                        packet->window = window;
                                        packet->window_reset = 0xE2000000;
                                        setlen(packet, 14);
                                        setaddr(packet, getaddr(slot));
                                        setaddr(slot, packet);
                                        if (semi == 0 || second_clut == 0) {
                                            break;
                                        }
                                        /* Second pass: CLUT 2, blend 1, flat colour. */
                                        uv0 = (uv0 & 0xFFFF) | second_clut;
                                        uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                        packet[1].rgb0 = colour;
                                        packet[1].xy0 = packet->xy0;
                                        packet[1].rgb1 = colour;
                                        packet[1].xy1 = packet->xy1;
                                        packet[1].rgb2 = colour;
                                        packet[1].xy2 = packet->xy2;
                                        packet[1].rgb3 = colour;
                                        packet[1].xy3 = packet->xy3;
                                        packet++;
                                        semi = 0;
                                    }
                                    packet++;
                                }
                            }
                        }
                    }
                }
                record++;
            } while (--count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Shared GT3 driver, forcing the semi-transparent code bit. See
 * shared/gt3.c for the cache formats and the second pass. */
u32 *Model_DrawSharedSemiGt3Primitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3Packet *packet =
        (ModelGt3Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3Record *record =
        (ModelDrawGt3Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template | 0x02000000;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        /* A second pass needs blending off, a record without a blend
         * mode of its own, and a second-pass CLUT: then it uses the record's
         * CLUT, the semi template colour and blend mode 1 (0x00200000). */
        if ((s8)gModel_bBlendRateActive == 0 &&
            (blend = record->words[1] & 0x00600000) == 0) {
            if (second_clut != 0) {
                second_clut = (u32)record->prim.cba << 16;
                colour = gModel_dwPolyGt3SemiTemplate;
                blend = 0x00200000;
            }
        } else {
            second_clut = 0;
        }
        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            u32 *c0, *c1, *c2;
                            s32 opz;

                            gte_ldsz3(z0, z1, z2);
                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            c0 = &colors[record->prim.norm0 * 2];
                            gte_nclip_b();
                            c1 = &colors[record->prim.norm1 * 2];
                            c2 = &colors[record->prim.norm2 * 2];
                            gte_stopz(&opz);
                            if (opz > 0) {
                                {
                                    u32 uv0, uv1, uv2, semi, otz;
                                    GsOT_TAG *slot;
                                    u32 rgb0 = *c0;
                                    u32 rgb1 = *c1;
                                    u32 rgb2 = *c2;

                                    packet->rgb0 = rgb0 | code;
                                    packet->rgb1 = rgb1;
                                    packet->rgb2 = rgb2;
                                    gte_avsz3_b();
                                    uv0 = record->words[0];
                                    uv1 = record->words[1];
                                    uv2 = record->words[2];
                                    gte_stotz(&otz);
                                    semi = poly & 0x02000000;
                                    uv1 = (uv1 & 0xFF9FFFFF) | blend;
                                    slot = &ot[otz >> shift];
                                    for (;;) {
                                        packet->uv0 = uv0;
                                        packet->uv1 = uv1;
                                        packet->uv2 = uv2;
                                        setlen(packet, 9);
                                        setaddr(packet, getaddr(slot));
                                        setaddr(slot, packet);
                                        if (semi == 0 || second_clut == 0) {
                                            break;
                                        }
                                        /* Second pass: CLUT 2, blend 1, flat colour. */
                                        uv0 = (uv0 & 0xFFFF) | second_clut;
                                        uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                        packet[1].rgb0 = colour;
                                        packet[1].xy0 = packet->xy0;
                                        packet[1].rgb1 = colour;
                                        packet[1].xy1 = packet->xy1;
                                        packet[1].rgb2 = colour;
                                        packet[1].xy2 = packet->xy2;
                                        packet++;
                                        semi = 0;
                                    }
                                    packet++;
                                }
                            }
                        }
                    }
                }
                record++;
            } while (--count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Shared GT4 driver, forcing the semi-transparent code bit. See
 * shared/gt3.c for the cache formats and the second pass. */
u32 *Model_DrawSharedSemiGt4Primitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4Packet *packet =
        (ModelGt4Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4Record *record =
        (ModelDrawGt4Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template | 0x02000000;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        /* A second pass needs blending off, a record without a blend
         * mode of its own, and a second-pass CLUT: then it uses the record's
         * CLUT, the semi template colour and blend mode 1 (0x00200000). */
        if ((s8)gModel_bBlendRateActive == 0 &&
            (blend = record->words[1] & 0x00600000) == 0) {
            if (second_clut != 0) {
                second_clut = (u32)record->prim.cba << 16;
                colour = gModel_dwPolyGt4SemiTemplate;
                blend = 0x00200000;
            }
        } else {
            second_clut = 0;
        }
        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            GsWORKUNIT *v3;
                            u32 z3, xy3;

                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            v3 = &cache[record->prim.vert3];
                            z3 = (u32)(u16)v3->otz |
                                (u32)(u16)v3->p << 16;
                            xy3 = (u32)(u16)v3->vec.vx |
                                (u32)(u16)v3->vec.vy << 16;
                            if (z3 != 0xFFFFFFFF) {
                                u32 *c0, *c1, *c2, *c3;
                                s32 opz;

                                gte_ldsz4(z0, z1, z2, z3);
                                packet->xy3 = v3->vec;
                                c0 = &colors[record->prim.norm0 * 2];
                                gte_nclip_b();
                                c1 = &colors[record->prim.norm1 * 2];
                                gte_stopz(&opz);
                                if (opz <= 0) {
                                    /* The other diagonal: vertices 3, 1, 2. */
                                    gte_ldsxy0(xy3);
                                    gte_nclip();
                                    gte_stopz(&opz);
                                    if (opz > 0) {
                                        continue;
                                    }
                                }
                                c2 = &colors[record->prim.norm2 * 2];
                                c3 = &colors[record->prim.norm3 * 2];
                                {
                                    u32 uv0, uv1, uv2, uv3, semi, otz;
                                    GsOT_TAG *slot;
                                    u32 rgb0 = *c0;
                                    u32 rgb1 = *c1;
                                    u32 rgb2 = *c2;
                                    u32 rgb3 = *c3;

                                    packet->rgb0 = rgb0 | code;
                                    packet->rgb1 = rgb1;
                                    packet->rgb2 = rgb2;
                                    packet->rgb3 = rgb3;
                                    gte_avsz4_b();
                                    uv0 = record->words[0];
                                    uv1 = record->words[1];
                                    uv2 = record->words[2];
                                    uv3 = record->words[3];
                                    gte_stotz(&otz);
                                    semi = poly & 0x02000000;
                                    uv1 = (uv1 & 0xFF9FFFFF) | blend;
                                    slot = &ot[otz >> shift];
                                    for (;;) {
                                        packet->uv0 = uv0;
                                        packet->uv1 = uv1;
                                        packet->uv2 = uv2;
                                        packet->uv3 = uv3;
                                        setlen(packet, 12);
                                        setaddr(packet, getaddr(slot));
                                        setaddr(slot, packet);
                                        if (semi == 0 || second_clut == 0) {
                                            break;
                                        }
                                        /* Second pass: CLUT 2, blend 1, flat colour. */
                                        uv0 = (uv0 & 0xFFFF) | second_clut;
                                        uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                        packet[1].rgb0 = colour;
                                        packet[1].xy0 = packet->xy0;
                                        packet[1].rgb1 = colour;
                                        packet[1].xy1 = packet->xy1;
                                        packet[1].rgb2 = colour;
                                        packet[1].xy2 = packet->xy2;
                                        packet[1].rgb3 = colour;
                                        packet[1].xy3 = packet->xy3;
                                        packet++;
                                        semi = 0;
                                    }
                                    packet++;
                                }
                            }
                        }
                    }
                }
            } while (record++, --count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Shared GT3 driver, for window records (texture-window word first),
 * forcing the semi-transparent code bit. See shared/gt3.c for the cache
 * formats and the second pass. */
u32 *Model_DrawSharedSemiGt3WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3WindowRecordPacket *packet =
        (ModelGt3WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3WindowRecord *record =
        (ModelDrawGt3WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template | 0x02000000;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        /* A second pass needs blending off, a record without a blend
         * mode of its own, and a second-pass CLUT: then it uses the record's
         * CLUT, the semi template colour and blend mode 1 (0x00200000). */
        if ((s8)gModel_bBlendRateActive == 0 &&
            (blend = record->words[2] & 0x00600000) == 0) {
            if (second_clut != 0) {
                second_clut = (u32)record->prim.prim.cba << 16;
                colour = gModel_dwPolyGt3SemiTemplate;
                blend = 0x00200000;
            }
        } else {
            second_clut = 0;
        }
        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            u32 *c0, *c1, *c2;
                            s32 opz;

                            gte_ldsz3(z0, z1, z2);
                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            c0 = &colors[record->prim.prim.norm0 * 2];
                            gte_nclip_b();
                            c1 = &colors[record->prim.prim.norm1 * 2];
                            c2 = &colors[record->prim.prim.norm2 * 2];
                            gte_stopz(&opz);
                            if (opz > 0) {
                                {
                                    u32 window, uv0, uv1, uv2, semi, otz;
                                    GsOT_TAG *slot;
                                    u32 rgb0 = *c0;
                                    u32 rgb1 = *c1;
                                    u32 rgb2 = *c2;

                                    packet->rgb0 = rgb0 | code;
                                    packet->rgb1 = rgb1;
                                    packet->rgb2 = rgb2;
                                    gte_avsz3_b();
                                    window = record->words[0];
                                    uv0 = record->words[1];
                                    uv1 = record->words[2];
                                    uv2 = record->words[3];
                                    gte_stotz(&otz);
                                    semi = poly & 0x02000000;
                                    uv1 = (uv1 & 0xFF9FFFFF) | blend;
                                    slot = &ot[otz >> shift];
                                    for (;;) {
                                        packet->uv0 = uv0;
                                        packet->uv1 = uv1;
                                        packet->uv2 = uv2;
                                        packet->window = window;
                                        packet->window_reset = 0xE2000000;
                                        setlen(packet, 11);
                                        setaddr(packet, getaddr(slot));
                                        setaddr(slot, packet);
                                        if (semi == 0 || second_clut == 0) {
                                            break;
                                        }
                                        /* Second pass: CLUT 2, blend 1, flat colour. */
                                        uv0 = (uv0 & 0xFFFF) | second_clut;
                                        uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                        packet[1].rgb0 = colour;
                                        packet[1].xy0 = packet->xy0;
                                        packet[1].rgb1 = colour;
                                        packet[1].xy1 = packet->xy1;
                                        packet[1].rgb2 = colour;
                                        packet[1].xy2 = packet->xy2;
                                        packet++;
                                        semi = 0;
                                    }
                                    packet++;
                                }
                            }
                        }
                    }
                }
                record++;
            } while (--count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Shared GT4 driver, for window records (texture-window word first),
 * forcing the semi-transparent code bit. See shared/gt3.c for the cache
 * formats and the second pass. */
u32 *Model_DrawSharedSemiGt4WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4WindowRecordPacket *packet =
        (ModelGt4WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4WindowRecord *record =
        (ModelDrawGt4WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template | 0x02000000;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        /* A second pass needs blending off, a record without a blend
         * mode of its own, and a second-pass CLUT: then it uses the record's
         * CLUT, the semi template colour and blend mode 1 (0x00200000). */
        if ((s8)gModel_bBlendRateActive == 0 &&
            (blend = record->words[2] & 0x00600000) == 0) {
            if (second_clut != 0) {
                second_clut = (u32)record->prim.prim.cba << 16;
                colour = gModel_dwPolyGt4SemiTemplate;
                blend = 0x00200000;
            }
        } else {
            second_clut = 0;
        }
        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            GsWORKUNIT *v3;
                            u32 z3, xy3;

                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            v3 = &cache[record->prim.prim.vert3];
                            z3 = (u32)(u16)v3->otz |
                                (u32)(u16)v3->p << 16;
                            xy3 = (u32)(u16)v3->vec.vx |
                                (u32)(u16)v3->vec.vy << 16;
                            if (z3 != 0xFFFFFFFF) {
                                u32 *c0, *c1, *c2, *c3;
                                s32 opz;

                                gte_ldsz4(z0, z1, z2, z3);
                                packet->xy3 = v3->vec;
                                c0 = &colors[record->prim.prim.norm0 * 2];
                                gte_nclip_b();
                                c1 = &colors[record->prim.prim.norm1 * 2];
                                gte_stopz(&opz);
                                if (opz <= 0) {
                                    /* The other diagonal: vertices 3, 1, 2. */
                                    gte_ldsxy0(xy3);
                                    gte_nclip();
                                    gte_stopz(&opz);
                                    if (opz > 0) {
                                        continue;
                                    }
                                }
                                c2 = &colors[record->prim.prim.norm2 * 2];
                                c3 = &colors[record->prim.prim.norm3 * 2];
                                {
                                    u32 window, uv0, uv1, uv2, uv3, semi, otz;
                                    GsOT_TAG *slot;
                                    u32 rgb0 = *c0;
                                    u32 rgb1 = *c1;
                                    u32 rgb2 = *c2;
                                    u32 rgb3 = *c3;

                                    packet->rgb0 = rgb0 | code;
                                    packet->rgb1 = rgb1;
                                    packet->rgb2 = rgb2;
                                    packet->rgb3 = rgb3;
                                    gte_avsz4_b();
                                    window = record->words[0];
                                    uv0 = record->words[1];
                                    uv1 = record->words[2];
                                    uv2 = record->words[3];
                                    uv3 = record->words[4];
                                    gte_stotz(&otz);
                                    semi = poly & 0x02000000;
                                    uv1 = (uv1 & 0xFF9FFFFF) | blend;
                                    slot = &ot[otz >> shift];
                                    for (;;) {
                                        packet->uv0 = uv0;
                                        packet->uv1 = uv1;
                                        packet->uv2 = uv2;
                                        packet->uv3 = uv3;
                                        packet->window = window;
                                        packet->window_reset = 0xE2000000;
                                        setlen(packet, 14);
                                        setaddr(packet, getaddr(slot));
                                        setaddr(slot, packet);
                                        if (semi == 0 || second_clut == 0) {
                                            break;
                                        }
                                        /* Second pass: CLUT 2, blend 1, flat colour. */
                                        uv0 = (uv0 & 0xFFFF) | second_clut;
                                        uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                        packet[1].rgb0 = colour;
                                        packet[1].xy0 = packet->xy0;
                                        packet[1].rgb1 = colour;
                                        packet[1].xy1 = packet->xy1;
                                        packet[1].rgb2 = colour;
                                        packet[1].xy2 = packet->xy2;
                                        packet[1].rgb3 = colour;
                                        packet[1].xy3 = packet->xy3;
                                        packet++;
                                        semi = 0;
                                    }
                                    packet++;
                                }
                            }
                        }
                    }
                }
            } while (record++, --count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Shared GT3 driver, without the NCLIP back-face test, forcing the semi-
 * transparent code bit. See shared/gt3.c for the cache formats and the
 * second pass. */
u32 *Model_DrawSharedTwoSidedSemiGt3Primitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3Packet *packet =
        (ModelGt3Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3Record *record =
        (ModelDrawGt3Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template | 0x02000000;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        /* A second pass needs blending off, a record without a blend
         * mode of its own, and a second-pass CLUT: then it uses the record's
         * CLUT, the semi template colour and blend mode 1 (0x00200000). */
        if ((s8)gModel_bBlendRateActive == 0 &&
            (blend = record->words[1] & 0x00600000) == 0) {
            if (second_clut != 0) {
                second_clut = (u32)record->prim.cba << 16;
                colour = gModel_dwPolyGt3SemiTemplate;
                blend = 0x00200000;
            }
        } else {
            second_clut = 0;
        }
        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            u32 *c0, *c1, *c2;

                            gte_ldsz3(z0, z1, z2);
                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            c0 = &colors[record->prim.norm0 * 2];
                            c1 = &colors[record->prim.norm1 * 2];
                            c2 = &colors[record->prim.norm2 * 2];
                            gte_avsz3_b();
                            {
                                u32 uv0, uv1, uv2, semi, otz;
                                GsOT_TAG *slot;
                                u32 rgb0 = *c0;
                                u32 rgb1 = *c1;
                                u32 rgb2 = *c2;

                                packet->rgb0 = rgb0 | code;
                                packet->rgb1 = rgb1;
                                packet->rgb2 = rgb2;
                                uv0 = record->words[0];
                                uv1 = record->words[1];
                                uv2 = record->words[2];
                                gte_stotz(&otz);
                                semi = poly & 0x02000000;
                                uv1 = (uv1 & 0xFF9FFFFF) | blend;
                                slot = &ot[otz >> shift];
                                for (;;) {
                                    packet->uv0 = uv0;
                                    packet->uv1 = uv1;
                                    packet->uv2 = uv2;
                                    setlen(packet, 9);
                                    setaddr(packet, getaddr(slot));
                                    setaddr(slot, packet);
                                    if (semi == 0 || second_clut == 0) {
                                        break;
                                    }
                                    /* Second pass: CLUT 2, blend 1, flat colour. */
                                    uv0 = (uv0 & 0xFFFF) | second_clut;
                                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                    packet[1].rgb0 = colour;
                                    packet[1].xy0 = packet->xy0;
                                    packet[1].rgb1 = colour;
                                    packet[1].xy1 = packet->xy1;
                                    packet[1].rgb2 = colour;
                                    packet[1].xy2 = packet->xy2;
                                    packet++;
                                    semi = 0;
                                }
                                packet++;
                            }
                        }
                    }
                }
                record++;
            } while (--count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Shared GT4 driver, without the NCLIP back-face test, forcing the semi-
 * transparent code bit. See shared/gt3.c for the cache formats and the
 * second pass. */
u32 *Model_DrawSharedTwoSidedSemiGt4Primitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4Packet *packet =
        (ModelGt4Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4Record *record =
        (ModelDrawGt4Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template | 0x02000000;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        /* A second pass needs blending off, a record without a blend
         * mode of its own, and a second-pass CLUT: then it uses the record's
         * CLUT, the semi template colour and blend mode 1 (0x00200000). */
        if ((s8)gModel_bBlendRateActive == 0 &&
            (blend = record->words[1] & 0x00600000) == 0) {
            if (second_clut != 0) {
                second_clut = (u32)record->prim.cba << 16;
                colour = gModel_dwPolyGt4SemiTemplate;
                blend = 0x00200000;
            }
        } else {
            second_clut = 0;
        }
        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            GsWORKUNIT *v3;
                            u32 z3, xy3;

                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            v3 = &cache[record->prim.vert3];
                            z3 = (u32)(u16)v3->otz |
                                (u32)(u16)v3->p << 16;
                            xy3 = (u32)(u16)v3->vec.vx |
                                (u32)(u16)v3->vec.vy << 16;
                            if (z3 != 0xFFFFFFFF) {
                                u32 *c0, *c1, *c2, *c3;

                                gte_ldsz4(z0, z1, z2, z3);
                                packet->xy3 = v3->vec;
                                c0 = &colors[record->prim.norm0 * 2];
                                c1 = &colors[record->prim.norm1 * 2];
                                c2 = &colors[record->prim.norm2 * 2];
                                c3 = &colors[record->prim.norm3 * 2];
                                gte_avsz4_b();
                                {
                                    u32 uv0, uv1, uv2, uv3, semi, otz;
                                    GsOT_TAG *slot;
                                    u32 rgb0 = *c0;
                                    u32 rgb1 = *c1;
                                    u32 rgb2 = *c2;
                                    u32 rgb3 = *c3;

                                    packet->rgb0 = rgb0 | code;
                                    packet->rgb1 = rgb1;
                                    packet->rgb2 = rgb2;
                                    packet->rgb3 = rgb3;
                                    uv0 = record->words[0];
                                    uv1 = record->words[1];
                                    uv2 = record->words[2];
                                    uv3 = record->words[3];
                                    gte_stotz(&otz);
                                    semi = poly & 0x02000000;
                                    uv1 = (uv1 & 0xFF9FFFFF) | blend;
                                    slot = &ot[otz >> shift];
                                    for (;;) {
                                        packet->uv0 = uv0;
                                        packet->uv1 = uv1;
                                        packet->uv2 = uv2;
                                        packet->uv3 = uv3;
                                        setlen(packet, 12);
                                        setaddr(packet, getaddr(slot));
                                        setaddr(slot, packet);
                                        if (semi == 0 || second_clut == 0) {
                                            break;
                                        }
                                        /* Second pass: CLUT 2, blend 1, flat colour. */
                                        uv0 = (uv0 & 0xFFFF) | second_clut;
                                        uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                        packet[1].rgb0 = colour;
                                        packet[1].xy0 = packet->xy0;
                                        packet[1].rgb1 = colour;
                                        packet[1].xy1 = packet->xy1;
                                        packet[1].rgb2 = colour;
                                        packet[1].xy2 = packet->xy2;
                                        packet[1].rgb3 = colour;
                                        packet[1].xy3 = packet->xy3;
                                        packet++;
                                        semi = 0;
                                    }
                                    packet++;
                                }
                            }
                        }
                    }
                }
                record++;
            } while (--count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Shared GT3 driver, for window records (texture-window word first),
 * without the NCLIP back-face test, forcing the semi-transparent code bit.
 * See shared/gt3.c for the cache formats and the second pass. */
u32 *Model_DrawSharedTwoSidedSemiGt3WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3WindowRecordPacket *packet =
        (ModelGt3WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3WindowRecord *record =
        (ModelDrawGt3WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template | 0x02000000;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        /* A second pass needs blending off, a record without a blend
         * mode of its own, and a second-pass CLUT: then it uses the record's
         * CLUT, the semi template colour and blend mode 1 (0x00200000). */
        if ((s8)gModel_bBlendRateActive == 0 &&
            (blend = record->words[2] & 0x00600000) == 0) {
            if (second_clut != 0) {
                second_clut = (u32)record->prim.prim.cba << 16;
                colour = gModel_dwPolyGt3SemiTemplate;
                blend = 0x00200000;
            }
        } else {
            second_clut = 0;
        }
        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            u32 *c0, *c1, *c2;

                            gte_ldsz3(z0, z1, z2);
                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            c0 = &colors[record->prim.prim.norm0 * 2];
                            c1 = &colors[record->prim.prim.norm1 * 2];
                            c2 = &colors[record->prim.prim.norm2 * 2];
                            gte_avsz3_b();
                            {
                                u32 window, uv0, uv1, uv2, semi, otz;
                                GsOT_TAG *slot;
                                u32 rgb0 = *c0;
                                u32 rgb1 = *c1;
                                u32 rgb2 = *c2;

                                packet->rgb0 = rgb0 | code;
                                packet->rgb1 = rgb1;
                                packet->rgb2 = rgb2;
                                window = record->words[0];
                                uv0 = record->words[1];
                                uv1 = record->words[2];
                                uv2 = record->words[3];
                                gte_stotz(&otz);
                                semi = poly & 0x02000000;
                                uv1 = (uv1 & 0xFF9FFFFF) | blend;
                                slot = &ot[otz >> shift];
                                for (;;) {
                                    packet->uv0 = uv0;
                                    packet->uv1 = uv1;
                                    packet->uv2 = uv2;
                                    packet->window = window;
                                    packet->window_reset = 0xE2000000;
                                    setlen(packet, 11);
                                    setaddr(packet, getaddr(slot));
                                    setaddr(slot, packet);
                                    if (semi == 0 || second_clut == 0) {
                                        break;
                                    }
                                    /* Second pass: CLUT 2, blend 1, flat colour. */
                                    uv0 = (uv0 & 0xFFFF) | second_clut;
                                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                    packet[1].rgb0 = colour;
                                    packet[1].xy0 = packet->xy0;
                                    packet[1].rgb1 = colour;
                                    packet[1].xy1 = packet->xy1;
                                    packet[1].rgb2 = colour;
                                    packet[1].xy2 = packet->xy2;
                                    packet++;
                                    semi = 0;
                                }
                                packet++;
                            }
                        }
                    }
                }
                record++;
            } while (--count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

/* Shared GT4 driver, for window records (texture-window word first),
 * without the NCLIP back-face test, forcing the semi-transparent code bit.
 * See shared/gt3.c for the cache formats and the second pass. */
u32 *Model_DrawSharedTwoSidedSemiGt4WindowRecordPrimitives(GsARGUNIT_SHARED *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4WindowRecordPacket *packet =
        (ModelGt4WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4WindowRecord *record =
        (ModelDrawGt4WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template | 0x02000000;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        u32 colour = poly;
        s8 mode;
        u32 code;

        /* A second pass needs blending off, a record without a blend
         * mode of its own, and a second-pass CLUT: then it uses the record's
         * CLUT, the semi template colour and blend mode 1 (0x00200000). */
        if ((s8)gModel_bBlendRateActive == 0 &&
            (blend = record->words[2] & 0x00600000) == 0) {
            if (second_clut != 0) {
                second_clut = (u32)record->prim.prim.cba << 16;
                colour = gModel_dwPolyGt4SemiTemplate;
                blend = 0x00200000;
            }
        } else {
            second_clut = 0;
        }
        mode = (s8)gModel_bCurrentDrawMode;
        code = poly >> 24 << 24;
        if (mode != 1) {
            u32 lit = gModel_dwLitColorCacheBase;
            GsWORKUNIT *cache = arg->vertop2;
            /* The colour cache shares the SVECTOR-typed nortop2 field. */
            u32 *colors = (u32 *)arg->nortop2;
            GsOT_TAG *ot;
            u32 shift;

            if (lit != 0) {
                colors++;
            }
            ot = arg->tagp->org;
            shift = arg->shift;
            do {
                /* SXY words are x | y << 16, the GTE register format; the
                 * depth word (otz | p << 16) feeds the 16-bit SZ register. */
                GsWORKUNIT *v0 = &cache[record->prim.prim.vert0];
                u32 z0 = (u32)(u16)v0->otz | (u32)(u16)v0->p << 16;
                u32 xy0 =
                    (u32)(u16)v0->vec.vx | (u32)(u16)v0->vec.vy << 16;

                if (z0 != 0xFFFFFFFF) {
                    GsWORKUNIT *v1;
                    u32 z1, xy1;

                    gte_ldsxy0(xy0);
                    packet->xy0 = v0->vec;
                    v1 = &cache[record->prim.prim.vert1];
                    z1 = (u32)(u16)v1->otz |
                        (u32)(u16)v1->p << 16;
                    xy1 = (u32)(u16)v1->vec.vx |
                        (u32)(u16)v1->vec.vy << 16;
                    if (z1 != 0xFFFFFFFF) {
                        GsWORKUNIT *v2;
                        u32 z2, xy2;

                        gte_ldsxy1(xy1);
                        packet->xy1 = v1->vec;
                        v2 = &cache[record->prim.prim.vert2];
                        z2 = (u32)(u16)v2->otz |
                            (u32)(u16)v2->p << 16;
                        xy2 = (u32)(u16)v2->vec.vx |
                            (u32)(u16)v2->vec.vy << 16;
                        if (z2 != 0xFFFFFFFF) {
                            GsWORKUNIT *v3;
                            u32 z3, xy3;

                            gte_ldsxy2(xy2);
                            packet->xy2 = v2->vec;
                            v3 = &cache[record->prim.prim.vert3];
                            z3 = (u32)(u16)v3->otz |
                                (u32)(u16)v3->p << 16;
                            xy3 = (u32)(u16)v3->vec.vx |
                                (u32)(u16)v3->vec.vy << 16;
                            if (z3 != 0xFFFFFFFF) {
                                u32 *c0, *c1, *c2, *c3;

                                gte_ldsz4(z0, z1, z2, z3);
                                packet->xy3 = v3->vec;
                                c0 = &colors[record->prim.prim.norm0 * 2];
                                c1 = &colors[record->prim.prim.norm1 * 2];
                                c2 = &colors[record->prim.prim.norm2 * 2];
                                c3 = &colors[record->prim.prim.norm3 * 2];
                                gte_avsz4_b();
                                {
                                    u32 window, uv0, uv1, uv2, uv3, semi, otz;
                                    GsOT_TAG *slot;
                                    u32 rgb0 = *c0;
                                    u32 rgb1 = *c1;
                                    u32 rgb2 = *c2;
                                    u32 rgb3 = *c3;

                                    packet->rgb0 = rgb0 | code;
                                    packet->rgb1 = rgb1;
                                    packet->rgb2 = rgb2;
                                    packet->rgb3 = rgb3;
                                    window = record->words[0];
                                    uv0 = record->words[1];
                                    uv1 = record->words[2];
                                    uv2 = record->words[3];
                                    uv3 = record->words[4];
                                    gte_stotz(&otz);
                                    semi = poly & 0x02000000;
                                    uv1 = (uv1 & 0xFF9FFFFF) | blend;
                                    slot = &ot[otz >> shift];
                                    for (;;) {
                                        packet->uv0 = uv0;
                                        packet->uv1 = uv1;
                                        packet->uv2 = uv2;
                                        packet->uv3 = uv3;
                                        packet->window = window;
                                        packet->window_reset = 0xE2000000;
                                        setlen(packet, 14);
                                        setaddr(packet, getaddr(slot));
                                        setaddr(slot, packet);
                                        if (semi == 0 || second_clut == 0) {
                                            break;
                                        }
                                        /* Second pass: CLUT 2, blend 1, flat colour. */
                                        uv0 = (uv0 & 0xFFFF) | second_clut;
                                        uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                                        packet[1].rgb0 = colour;
                                        packet[1].xy0 = packet->xy0;
                                        packet[1].rgb1 = colour;
                                        packet[1].xy1 = packet->xy1;
                                        packet[1].rgb2 = colour;
                                        packet[1].xy2 = packet->xy2;
                                        packet[1].rgb3 = colour;
                                        packet[1].xy3 = packet->xy3;
                                        packet++;
                                        semi = 0;
                                    }
                                    packet++;
                                }
                            }
                        }
                    }
                }
                record++;
            } while (--count != 0);
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}
