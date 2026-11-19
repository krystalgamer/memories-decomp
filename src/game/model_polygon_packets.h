#ifndef MEMORIES_DECOMP_MODEL_POLYGON_PACKETS_H
#define MEMORIES_DECOMP_MODEL_POLYGON_PACKETS_H

#include "../types.h"
#include "../psyq/libgte.h"

/*
 * The textured polygon packets the model drivers build in the packet work
 * area (polygon_renderers.c and shared_polygon_renderers.c), as GPU command
 * words: the tag, then the words the GPU reads. The drivers copy whole
 * u/v/CLUT/TPAGE words from their records, so a vertex's UV is one word.
 */

/* POLY_FT3: the colour+code word, then XY and UV+CLUT/TPAGE per vertex. */
typedef struct {
    u32 tag;
    u32 rgb0;
    DVECTOR xy0;
    u32 uv0;
    DVECTOR xy1;
    u32 uv1;
    DVECTOR xy2;
    u32 uv2;
} ModelFt3Packet;

/* POLY_FT3: the colour+code word, then XY and UV+CLUT/TPAGE per vertex;
 * between the record's texture-window (E2) word and the E2 reset word. */
typedef struct {
    u32 tag;
    u32 window;
    u32 rgb0;
    DVECTOR xy0;
    u32 uv0;
    DVECTOR xy1;
    u32 uv1;
    DVECTOR xy2;
    u32 uv2;
    u32 window_reset;
} ModelFt3WindowRecordPacket;

/* POLY_GT3: per vertex a colour (the first carrying the code byte), XY and
 * UV+CLUT/TPAGE. */
typedef struct {
    u32 tag;
    u32 rgb0;
    DVECTOR xy0;
    u32 uv0;
    u32 rgb1;
    DVECTOR xy1;
    u32 uv1;
    u32 rgb2;
    DVECTOR xy2;
    u32 uv2;
} ModelGt3Packet;

/* POLY_GT3: per vertex a colour (the first carrying the code byte), XY and
 * UV+CLUT/TPAGE; between the record's texture-window (E2) word and the E2
 * reset word. */
typedef struct {
    u32 tag;
    u32 window;
    u32 rgb0;
    DVECTOR xy0;
    u32 uv0;
    u32 rgb1;
    DVECTOR xy1;
    u32 uv1;
    u32 rgb2;
    DVECTOR xy2;
    u32 uv2;
    u32 window_reset;
} ModelGt3WindowRecordPacket;

/* POLY_FT4: the colour+code word, then XY and UV+CLUT/TPAGE per vertex. */
typedef struct {
    u32 tag;
    u32 rgb0;
    DVECTOR xy0;
    u32 uv0;
    DVECTOR xy1;
    u32 uv1;
    DVECTOR xy2;
    u32 uv2;
    DVECTOR xy3;
    u32 uv3;
} ModelFt4Packet;

/* POLY_FT4: the colour+code word, then XY and UV+CLUT/TPAGE per vertex;
 * between the record's texture-window (E2) word and the E2 reset word. */
typedef struct {
    u32 tag;
    u32 window;
    u32 rgb0;
    DVECTOR xy0;
    u32 uv0;
    DVECTOR xy1;
    u32 uv1;
    DVECTOR xy2;
    u32 uv2;
    DVECTOR xy3;
    u32 uv3;
    u32 window_reset;
} ModelFt4WindowRecordPacket;

/* POLY_GT4: per vertex a colour (the first carrying the code byte), XY and
 * UV+CLUT/TPAGE. */
typedef struct {
    u32 tag;
    u32 rgb0;
    DVECTOR xy0;
    u32 uv0;
    u32 rgb1;
    DVECTOR xy1;
    u32 uv1;
    u32 rgb2;
    DVECTOR xy2;
    u32 uv2;
    u32 rgb3;
    DVECTOR xy3;
    u32 uv3;
} ModelGt4Packet;

/* POLY_GT4: per vertex a colour (the first carrying the code byte), XY and
 * UV+CLUT/TPAGE; between the record's texture-window (E2) word and the E2
 * reset word. */
typedef struct {
    u32 tag;
    u32 window;
    u32 rgb0;
    DVECTOR xy0;
    u32 uv0;
    u32 rgb1;
    DVECTOR xy1;
    u32 uv1;
    u32 rgb2;
    DVECTOR xy2;
    u32 uv2;
    u32 rgb3;
    DVECTOR xy3;
    u32 uv3;
    u32 window_reset;
} ModelGt4WindowRecordPacket;

#endif
