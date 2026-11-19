/*
 * Direct Ft3/Gt3/Ft4/Gt4 polygon drivers, all eight variants in image order
 * (0x800612C0-0x80067220): base, window-record, two-sided, two-sided
 * window-record, semi-transparent, semi window-record, two-sided semi and
 * two-sided semi window-record.
 *
 * The originals were written by hand: no stack frame, s0-s7 parked in the
 * GsARGUNIT block, the next primitive block returned in $v0. This is plain C
 * over the stock Psy-Q inline_c.h and libgpu.h macros, built with the
 * gcc_2_8_1_o2_massaged profile: GCC's -O2 output is rewritten into the
 * original instructions by tools/project/asm_massage.py with the edit files
 * in polygon_renderers.massage/ (one per function). See notes/asm-massage.md
 * and notes/direct-polygon-renderers.md.
 *
 * The Japanese and European executables link the base Gt3/Gt4 drivers well
 * before the rest of this file; they build this file once without those two
 * and once more, as the game/model_draw_gouraud_primitives object (with
 * MODEL_DRAW_DETACHED_GOURAUD_PRIMITIVES), with only those two.
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
#include "model_polygon_packets.h"
#include "model_slot_setup.h"
#include "primitive_handler_modes.h"
#include "direct_polygon_renderers.h"

#if !defined(MODEL_DRAW_DETACHED_GOURAUD_PRIMITIVES)

u32 *Model_DrawFt3Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt3Packet *packet = (ModelFt3Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt3Record *record =
        (ModelDrawFt3Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            normal = record->prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    setlen(packet, 7);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}
#endif

#if !(defined(VERSION_JAPAN) || defined(VERSION_EUROPE)) || defined(MODEL_DRAW_DETACHED_GOURAUD_PRIMITIVES)

u32 *Model_DrawGt3Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3Packet *packet = (ModelGt3Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3Record *record =
        (ModelDrawGt3Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            normal0 = record->prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            normal1 = record->prim.norm1 * sizeof(SVECTOR);
            normal2 = record->prim.norm2 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            if (mode != 0) {
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}
#endif

#if !defined(MODEL_DRAW_DETACHED_GOURAUD_PRIMITIVES)

u32 *Model_DrawFt4Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt4Packet *packet = (ModelFt4Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt4Record *record =
        (ModelDrawFt4Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;
            SVECTOR *vertex3;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            vertex3 = &vertices[record->prim.vert3];
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            gte_rtps();
            normal = record->prim.norm0 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 uv3 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy2(&packet->xy3);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    packet->uv3 = uv3;
                    setlen(packet, 9);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}
#endif

#if !(defined(VERSION_JAPAN) || defined(VERSION_EUROPE)) || defined(MODEL_DRAW_DETACHED_GOURAUD_PRIMITIVES)

u32 *Model_DrawGt4Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4Packet *packet = (ModelGt4Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4Record *record =
        (ModelDrawGt4Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;
            u32 normal3;
            SVECTOR *vertex3;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            vertex3 = &vertices[record->prim.vert3];
            gte_rtpt_b();
            normal3 = record->prim.norm3 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            normal0 = record->prim.norm0 * sizeof(SVECTOR);
            gte_rtps_b();
            normal1 = record->prim.norm1 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            gte_stsxy2(&packet->xy3);
            normal2 = record->prim.norm2 * sizeof(SVECTOR);
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal3);
                gte_nccs();
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal3 / 2));
                }
                gte_strgb(&packet->rgb3);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);
                u32 rgb3 = *(u32 *)(lit + normal3 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                packet->rgb3 = rgb3;
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 uv3 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet[1].rgb3 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}
#endif

#if !defined(MODEL_DRAW_DETACHED_GOURAUD_PRIMITIVES)

u32 *Model_DrawFt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt3WindowRecordPacket *packet = (ModelFt3WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt3WindowRecord *record =
        (ModelDrawFt3WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            normal = record->prim.prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            window = record->words[0];
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    packet->window = window;
                    packet->window_reset = 0xE2000000;
                    setlen(packet, 9);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawGt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3WindowRecordPacket *packet = (ModelGt3WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3WindowRecord *record =
        (ModelDrawGt3WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            normal0 = record->prim.prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            normal1 = record->prim.prim.norm1 * sizeof(SVECTOR);
            normal2 = record->prim.prim.norm2 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            window = record->words[0];
            if (mode != 0) {
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawFt4WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt4WindowRecordPacket *packet = (ModelFt4WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt4WindowRecord *record =
        (ModelDrawFt4WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;
            SVECTOR *vertex3;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            vertex3 = &vertices[record->prim.prim.vert3];
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            gte_rtps();
            normal = record->prim.prim.norm0 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            window = record->words[0];
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 uv3 = record->words[4];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy2(&packet->xy3);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    packet->uv3 = uv3;
                    packet->window = window;
                    packet->window_reset = 0xE2000000;
                    setlen(packet, 11);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawGt4WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4WindowRecordPacket *packet = (ModelGt4WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4WindowRecord *record =
        (ModelDrawGt4WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;
            u32 normal3;
            SVECTOR *vertex3;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            vertex3 = &vertices[record->prim.prim.vert3];
            gte_rtpt_b();
            normal3 = record->prim.prim.norm3 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            normal0 = record->prim.prim.norm0 * sizeof(SVECTOR);
            gte_rtps_b();
            normal1 = record->prim.prim.norm1 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            gte_stsxy2(&packet->xy3);
            normal2 = record->prim.prim.norm2 * sizeof(SVECTOR);
            window = record->words[0];
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal3);
                gte_nccs();
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal3 / 2));
                }
                gte_strgb(&packet->rgb3);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);
                u32 rgb3 = *(u32 *)(lit + normal3 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                packet->rgb3 = rgb3;
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 uv3 = record->words[4];
                u32 otz, semi;
                GsOT_TAG *slot;

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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet[1].rgb3 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedFt3Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt3Packet *packet = (ModelFt3Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt3Record *record =
        (ModelDrawFt3Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            normal = record->prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    setlen(packet, 7);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedGt3Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3Packet *packet = (ModelGt3Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3Record *record =
        (ModelDrawGt3Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            normal0 = record->prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            normal1 = record->prim.norm1 * sizeof(SVECTOR);
            normal2 = record->prim.norm2 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            if (mode != 0) {
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedFt4Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt4Packet *packet = (ModelFt4Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt4Record *record =
        (ModelDrawFt4Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;
            SVECTOR *vertex3;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            vertex3 = &vertices[record->prim.vert3];
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            gte_rtps();
            normal = record->prim.norm0 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 uv3 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy2(&packet->xy3);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    packet->uv3 = uv3;
                    setlen(packet, 9);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedGt4Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4Packet *packet = (ModelGt4Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4Record *record =
        (ModelDrawGt4Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;
            u32 normal3;
            SVECTOR *vertex3;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            vertex3 = &vertices[record->prim.vert3];
            gte_rtpt_b();
            normal3 = record->prim.norm3 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            normal0 = record->prim.norm0 * sizeof(SVECTOR);
            gte_rtps_b();
            normal1 = record->prim.norm1 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            gte_stsxy2(&packet->xy3);
            normal2 = record->prim.norm2 * sizeof(SVECTOR);
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal3);
                gte_nccs();
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal3 / 2));
                }
                gte_strgb(&packet->rgb3);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);
                u32 rgb3 = *(u32 *)(lit + normal3 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                packet->rgb3 = rgb3;
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 uv3 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet[1].rgb3 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedFt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt3WindowRecordPacket *packet = (ModelFt3WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt3WindowRecord *record =
        (ModelDrawFt3WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            normal = record->prim.prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            window = record->words[0];
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    packet->window = window;
                    packet->window_reset = 0xE2000000;
                    setlen(packet, 9);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedGt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3WindowRecordPacket *packet = (ModelGt3WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3WindowRecord *record =
        (ModelDrawGt3WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            normal0 = record->prim.prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            normal1 = record->prim.prim.norm1 * sizeof(SVECTOR);
            normal2 = record->prim.prim.norm2 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            window = record->words[0];
            if (mode != 0) {
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedFt4WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt4WindowRecordPacket *packet = (ModelFt4WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt4WindowRecord *record =
        (ModelDrawFt4WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;
            SVECTOR *vertex3;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            vertex3 = &vertices[record->prim.prim.vert3];
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            gte_rtps();
            normal = record->prim.prim.norm0 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            window = record->words[0];
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 uv3 = record->words[4];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy2(&packet->xy3);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    packet->uv3 = uv3;
                    packet->window = window;
                    packet->window_reset = 0xE2000000;
                    setlen(packet, 11);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedGt4WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4WindowRecordPacket *packet = (ModelGt4WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4WindowRecord *record =
        (ModelDrawGt4WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        colour = poly;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;
            u32 normal3;
            SVECTOR *vertex3;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            vertex3 = &vertices[record->prim.prim.vert3];
            gte_rtpt_b();
            normal3 = record->prim.prim.norm3 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            normal0 = record->prim.prim.norm0 * sizeof(SVECTOR);
            gte_rtps_b();
            normal1 = record->prim.prim.norm1 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            gte_stsxy2(&packet->xy3);
            normal2 = record->prim.prim.norm2 * sizeof(SVECTOR);
            window = record->words[0];
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal3);
                gte_nccs();
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal3 / 2));
                }
                gte_strgb(&packet->rgb3);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);
                u32 rgb3 = *(u32 *)(lit + normal3 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                packet->rgb3 = rgb3;
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 uv3 = record->words[4];
                u32 otz, semi;
                GsOT_TAG *slot;

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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet[1].rgb3 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawSemiFt3Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt3Packet *packet = (ModelFt3Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt3Record *record =
        (ModelDrawFt3Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[1] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.cba << 16;
                    colour = gModel_dwPolyFt3SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            normal = record->prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    setlen(packet, 7);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawSemiGt3Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3Packet *packet = (ModelGt3Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3Record *record =
        (ModelDrawGt3Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[1] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.cba << 16;
                    colour = gModel_dwPolyGt3SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            normal0 = record->prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            normal1 = record->prim.norm1 * sizeof(SVECTOR);
            normal2 = record->prim.norm2 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            if (mode != 0) {
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawSemiFt4Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt4Packet *packet = (ModelFt4Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt4Record *record =
        (ModelDrawFt4Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[1] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.cba << 16;
                    colour = gModel_dwPolyFt4SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;
            SVECTOR *vertex3;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            vertex3 = &vertices[record->prim.vert3];
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            gte_rtps();
            normal = record->prim.norm0 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 uv3 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy2(&packet->xy3);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    packet->uv3 = uv3;
                    setlen(packet, 9);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawSemiGt4Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4Packet *packet = (ModelGt4Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4Record *record =
        (ModelDrawGt4Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[1] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.cba << 16;
                    colour = gModel_dwPolyGt4SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;
            u32 normal3;
            SVECTOR *vertex3;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            vertex3 = &vertices[record->prim.vert3];
            gte_rtpt_b();
            normal3 = record->prim.norm3 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            normal0 = record->prim.norm0 * sizeof(SVECTOR);
            gte_rtps_b();
            normal1 = record->prim.norm1 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            gte_stsxy2(&packet->xy3);
            normal2 = record->prim.norm2 * sizeof(SVECTOR);
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal3);
                gte_nccs();
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal3 / 2));
                }
                gte_strgb(&packet->rgb3);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);
                u32 rgb3 = *(u32 *)(lit + normal3 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                packet->rgb3 = rgb3;
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 uv3 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet[1].rgb3 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawSemiFt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt3WindowRecordPacket *packet = (ModelFt3WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt3WindowRecord *record =
        (ModelDrawFt3WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[2] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.prim.cba << 16;
                    colour = gModel_dwPolyFt3SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            normal = record->prim.prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            window = record->words[0];
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    packet->window = window;
                    packet->window_reset = 0xE2000000;
                    setlen(packet, 9);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawSemiGt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3WindowRecordPacket *packet = (ModelGt3WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3WindowRecord *record =
        (ModelDrawGt3WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[2] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.prim.cba << 16;
                    colour = gModel_dwPolyGt3SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            normal0 = record->prim.prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            normal1 = record->prim.prim.norm1 * sizeof(SVECTOR);
            normal2 = record->prim.prim.norm2 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            window = record->words[0];
            if (mode != 0) {
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawSemiFt4WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt4WindowRecordPacket *packet = (ModelFt4WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt4WindowRecord *record =
        (ModelDrawFt4WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[2] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.prim.cba << 16;
                    colour = gModel_dwPolyFt4SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;
            SVECTOR *vertex3;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            vertex3 = &vertices[record->prim.prim.vert3];
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            gte_rtps();
            normal = record->prim.prim.norm0 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            window = record->words[0];
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 uv3 = record->words[4];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy2(&packet->xy3);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    packet->uv3 = uv3;
                    packet->window = window;
                    packet->window_reset = 0xE2000000;
                    setlen(packet, 11);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawSemiGt4WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4WindowRecordPacket *packet = (ModelGt4WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4WindowRecord *record =
        (ModelDrawGt4WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[2] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.prim.cba << 16;
                    colour = gModel_dwPolyGt4SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;
            u32 normal3;
            SVECTOR *vertex3;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            vertex3 = &vertices[record->prim.prim.vert3];
            gte_rtpt_b();
            normal3 = record->prim.prim.norm3 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;
                s32 opz;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
                gte_nclip_b();
                gte_stopz(&opz);
                if (opz <= 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            normal0 = record->prim.prim.norm0 * sizeof(SVECTOR);
            gte_rtps_b();
            normal1 = record->prim.prim.norm1 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            gte_stsxy2(&packet->xy3);
            normal2 = record->prim.prim.norm2 * sizeof(SVECTOR);
            window = record->words[0];
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal3);
                gte_nccs();
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal3 / 2));
                }
                gte_strgb(&packet->rgb3);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);
                u32 rgb3 = *(u32 *)(lit + normal3 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                packet->rgb3 = rgb3;
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 uv3 = record->words[4];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet[1].rgb3 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedSemiFt3Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt3Packet *packet = (ModelFt3Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt3Record *record =
        (ModelDrawFt3Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[1] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.cba << 16;
                    colour = gModel_dwPolyFt3SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            normal = record->prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    setlen(packet, 7);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedSemiGt3Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3Packet *packet = (ModelGt3Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3Record *record =
        (ModelDrawGt3Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[1] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.cba << 16;
                    colour = gModel_dwPolyGt3SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            normal0 = record->prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            normal1 = record->prim.norm1 * sizeof(SVECTOR);
            normal2 = record->prim.norm2 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            if (mode != 0) {
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedSemiFt4Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt4Packet *packet = (ModelFt4Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt4Record *record =
        (ModelDrawFt4Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[1] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.cba << 16;
                    colour = gModel_dwPolyFt4SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;
            SVECTOR *vertex3;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            vertex3 = &vertices[record->prim.vert3];
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            gte_rtps();
            normal = record->prim.norm0 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 uv3 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy2(&packet->xy3);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    packet->uv3 = uv3;
                    setlen(packet, 9);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedSemiGt4Primitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4Packet *packet = (ModelGt4Packet *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4Record *record =
        (ModelDrawGt4Record *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[1] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.cba << 16;
                    colour = gModel_dwPolyGt4SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;
            u32 normal3;
            SVECTOR *vertex3;

            gte_ldv0(&vertices[record->prim.vert0]);
            gte_ldv1(&vertices[record->prim.vert1]);
            gte_ldv2(&vertices[record->prim.vert2]);
            vertex3 = &vertices[record->prim.vert3];
            gte_rtpt_b();
            normal3 = record->prim.norm3 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            normal0 = record->prim.norm0 * sizeof(SVECTOR);
            gte_rtps_b();
            normal1 = record->prim.norm1 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            gte_stsxy2(&packet->xy3);
            normal2 = record->prim.norm2 * sizeof(SVECTOR);
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal3);
                gte_nccs();
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal3 / 2));
                }
                gte_strgb(&packet->rgb3);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);
                u32 rgb3 = *(u32 *)(lit + normal3 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                packet->rgb3 = rgb3;
                setcode(packet, code);
            }
            {
                u32 uv0 = record->words[0];
                u32 uv1 = record->words[1];
                u32 uv2 = record->words[2];
                u32 uv3 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet[1].rgb3 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedSemiFt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt3WindowRecordPacket *packet = (ModelFt3WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt3WindowRecord *record =
        (ModelDrawFt3WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[2] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.prim.cba << 16;
                    colour = gModel_dwPolyFt3SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            normal = record->prim.prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            window = record->words[0];
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    packet->window = window;
                    packet->window_reset = 0xE2000000;
                    setlen(packet, 9);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedSemiGt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt3WindowRecordPacket *packet = (ModelGt3WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt3WindowRecord *record =
        (ModelDrawGt3WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt3Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[2] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.prim.cba << 16;
                    colour = gModel_dwPolyGt3SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            normal0 = record->prim.prim.norm0 * sizeof(SVECTOR);
            gte_rtpt_b();
            normal1 = record->prim.prim.norm1 * sizeof(SVECTOR);
            normal2 = record->prim.prim.norm2 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz3_b();
            window = record->words[0];
            if (mode != 0) {
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedSemiFt4WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelFt4WindowRecordPacket *packet = (ModelFt4WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawFt4WindowRecord *record =
        (ModelDrawFt4WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyFt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[2] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.prim.cba << 16;
                    colour = gModel_dwPolyFt4SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offset of the record's normal; its lit colour sits at
             * half that offset in the colour cache. */
            u32 normal;
            SVECTOR *vertex3;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            vertex3 = &vertices[record->prim.prim.vert3];
            gte_rtpt_b();
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            gte_rtps();
            normal = record->prim.prim.norm0 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            window = record->words[0];
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal);
                gte_nccs();
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal / 2));
                }
                gte_strgb(&packet->rgb0);
                if (mode == 1) {
                    goto next;
                }
            } else {
                packet->rgb0 = *(u32 *)(lit + normal / 2);
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 uv3 = record->words[4];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
                uv1 |= blend;
                slot = &ot[otz >> shift];
                gte_stsxy2(&packet->xy3);
                for (;;) {
                    packet->uv0 = uv0;
                    packet->uv1 = uv1;
                    packet->uv2 = uv2;
                    packet->uv3 = uv3;
                    packet->window = window;
                    packet->window_reset = 0xE2000000;
                    setlen(packet, 11);
                    setaddr(packet, getaddr(slot));
                    setaddr(slot, packet);
                    if (semi == 0 || second_clut == 0) {
                        break;
                    }
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}

u32 *Model_DrawTwoSidedSemiGt4WindowRecordPrimitives(GsARGUNIT_NORMAL *arg)
{
    ModelPrimitiveBlockHeader *block = (ModelPrimitiveBlockHeader *)arg->primp;
    ModelGt4WindowRecordPacket *packet = (ModelGt4WindowRecordPacket *)arg->out_packetp;
    u32 count = block->record_count;
    ModelDrawGt4WindowRecord *record =
        (ModelDrawGt4WindowRecord *)(arg->primtop + block->word_offset);

    if (count != 0) {
        u32 poly = gModel_dwPolyGt4Template;
        u32 blend = gModel_dwTpageBlendRateBits;
        u32 second_clut = gModel_dwSecondPassClutBits;
        s8 mode = (s8)gModel_bCurrentDrawMode;
        u32 lit = gModel_dwLitColorCacheBase;
        u32 colour;
        u32 code;
        SVECTOR *vertices;
        SVECTOR *normals;
        GsOT_TAG *ot;
        u32 shift;
        u32 rgb;

        {
            s8 active = (s8)gModel_bBlendRateActive;

            poly |= 0x02000000;
            colour = poly;
            /* The first record's tpage blend bits and CLUT select a
             * second pass with the semi template. */
            if (active == 0) {
                blend = record->words[2] & 0x00600000;
                if (blend != 0) {
                    second_clut = 0;
                } else if (second_clut != 0) {
                    second_clut = record->prim.prim.cba << 16;
                    colour = gModel_dwPolyGt4SemiTemplate;
                    blend = 0x00200000;
                }
            } else {
                second_clut = 0;
            }
        }
        code = poly >> 24;
        vertices = arg->vertop;
        normals = arg->nortop;
        rgb = poly;
        /* The template's code byte rides through the lighting into the
         * packet's colour word. */
        gte_ldrgb(&rgb);
        ot = arg->tagp->org;
        shift = arg->shift;
        do {
            /* Byte offsets of the record's normals; their lit colours sit
             * at half those offsets in the colour cache. */
            u32 normal0, normal1, normal2;
            u32 normal3;
            SVECTOR *vertex3;
            u32 window;

            gte_ldv0(&vertices[record->prim.prim.vert0]);
            gte_ldv1(&vertices[record->prim.prim.vert1]);
            gte_ldv2(&vertices[record->prim.prim.vert2]);
            vertex3 = &vertices[record->prim.prim.vert3];
            gte_rtpt_b();
            normal3 = record->prim.prim.norm3 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_ldv0(vertex3);
            gte_stsxy3(&packet->xy0, &packet->xy1, &packet->xy2);
            normal0 = record->prim.prim.norm0 * sizeof(SVECTOR);
            gte_rtps_b();
            normal1 = record->prim.prim.norm1 * sizeof(SVECTOR);
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            gte_avsz4_b();
            gte_stsxy2(&packet->xy3);
            normal2 = record->prim.prim.norm2 * sizeof(SVECTOR);
            window = record->words[0];
            if (mode != 0) {
                gte_ldv0((u8 *)normals + normal3);
                gte_nccs();
                gte_ldv3((u8 *)normals + normal0, (u8 *)normals + normal1,
                         (u8 *)normals + normal2);
                if (lit != 0) {
                    gte_strgb((u32 *)(lit + normal3 / 2));
                }
                gte_strgb(&packet->rgb3);
                gte_ncct();
                if (lit != 0) {
                    gte_strgb3((u32 *)(lit + normal0 / 2), (u32 *)(lit + normal1 / 2),
                               (u32 *)(lit + normal2 / 2));
                }
                gte_strgb3(&packet->rgb0, &packet->rgb1, &packet->rgb2);
                if (mode == 1) {
                    goto next;
                }
            } else {
                u32 rgb0 = *(u32 *)(lit + normal0 / 2);
                u32 rgb1 = *(u32 *)(lit + normal1 / 2);
                u32 rgb2 = *(u32 *)(lit + normal2 / 2);
                u32 rgb3 = *(u32 *)(lit + normal3 / 2);

                packet->rgb0 = rgb0;
                packet->rgb1 = rgb1;
                packet->rgb2 = rgb2;
                packet->rgb3 = rgb3;
                setcode(&packet->window, code);
            }
            {
                u32 uv0 = record->words[1];
                u32 uv1 = record->words[2];
                u32 uv2 = record->words[3];
                u32 uv3 = record->words[4];
                u32 otz, semi;
                GsOT_TAG *slot;

                gte_stotz(&otz);
                semi = poly & 0x02000000;
                uv1 &= 0xFF9FFFFF;
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
                    /* Second pass: second CLUT, blend 1, flat colour. */
                    uv1 = (uv1 & 0xFF9FFFFF) | 0x00400000;
                    uv0 = (uv0 & 0xFFFF) | second_clut;
                    packet[1].xy0 = packet->xy0;
                    packet[1].xy1 = packet->xy1;
                    packet[1].xy2 = packet->xy2;
                    packet[1].xy3 = packet->xy3;
                    packet[1].rgb0 = colour;
                    packet[1].rgb1 = colour;
                    packet[1].rgb2 = colour;
                    packet[1].rgb3 = colour;
                    packet++;
                    semi = 0;
                }
                packet++;
            }
        next:
            record++;
        } while (--count != 0);
    }
    gGraphics_pPacketWorkCursor = (u32 *)packet;
    return (u32 *)(arg->primp + 2);
}
#endif
