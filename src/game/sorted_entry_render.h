#ifndef MEMORIES_DECOMP_SORTED_ENTRY_RENDER_H
#define MEMORIES_DECOMP_SORTED_ENTRY_RENDER_H
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"

/* One vertex of a LINE_G4 (libgpu.h) as its two words: the r/g/b/pad (or
 * r/g/b/code) word and the x/y word. */
typedef struct FogLineVertex {
    u32 color;
    u32 xy;
} FogLineVertex;

/* The scratchpad LINE_G4 that Model_DrawFogTexturedTriangles builds its
 * triangle outlines in. The GTE stores fill the primitive's fields, and the
 * outline is closed by copying vertex 0 into vertex 3 a word at a time. */
typedef union FogLineG4 {
    LINE_G4 prim;
    struct {
        u32 tag;
        FogLineVertex v[4];
        u32 pad;
    } words;
} FogLineG4;

typedef char FogLineG4_size_must_match_LINE_G4[
    sizeof(FogLineG4) == sizeof(LINE_G4) ? 1 : -1
];

/* The fog drivers' packet output cursor. It starts at arg->out_packetp,
 * receives one primitive per drawn face, steps past that primitive, and ends
 * in gGraphics_pPacketWorkCursor. */
typedef union FogPacketCursor {
    PACKET *bytes;
    u32 *words;
    POLY_GT3 *gt3;
    POLY_GT4 *gt4;
    LINE_G4 *lg4;
    LINE_G2 *lg2;
} FogPacketCursor;

u32 *Model_DrawFogTexturedTriangles(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawFogTexturedQuads(GsARGUNIT_NORMAL *arg);

/* The list of pending eight-byte entries that SortedEntry_SortAndRelink sorts
 * and whose inverse index links it rebuilds.
 *
 * SortedEntry_BeginCollection opens the list: it points both the base
 * gSortedEntry_pBase and the append pointer gSortedEntry_pCursor at the same address and zeroes the count
 * gSortedEntry_nCount.  SortedEntry_Append appends one entry per call, advancing gSortedEntry_pCursor
 * by one entry and bumping gSortedEntry_nCount, so the live entries are always
 * [gSortedEntry_pBase, gSortedEntry_pCursor). SortedEntry_SortAndRelink sorts them, writes the
 * inverse links, and resets gSortedEntry_pCursor back to the base.
 *
 * The draw side reuses the cursor and the count. While bit 1 of
 * gModel_dwFogDrawFlags is set, Duel_DrawFieldCards (by 1) and the overworld
 * CampaignMap_UpdateView (by 10) raise gSortedEntry_nCount once per frame
 * until it reaches gSortedEntry_nSortedCount, and rewind gSortedEntry_pCursor
 * to the base. The fog-textured HMD drivers then step the cursor through the
 * entries and draw those whose sorted_position is below the count filled.
 *
 * SortedEntry_Compare compares the u32 at offset 0 of an entry. On
 * this target that word holds `distance` in the high halfword and `packed` in
 * the low one, so the order is ascending by distance, ties broken by `packed`.
 */
typedef struct SortedEntry {
    /* ratan2(dx, dz) / 16, clamped to 255 and shifted left by 8, or'd with
     * dy >> 4.  The two halves are never read apart, so they are not named
     * separately here. */
    s16 packed;
    /* SquareRoot0(dx * dx + dz * dz). */
    s16 distance;
    /* The value of gSortedEntry_nCount when the entry was appended, i.e. the entry's
     * position in append order. */
    s16 append_index;
    /* Written by SortedEntry_SortAndRelink, which stores each entry's sorted
     * position into the slot named by that entry's append_index. */
    s16 sorted_position;
} SortedEntry;

#define SORTED_ENTRY_OFFSET(member) ((u32)&(((SortedEntry *)0)->member))

typedef char SortedEntry_size_must_be_8[
    sizeof(SortedEntry) == 8 ? 1 : -1
];
typedef char SortedEntry_distance_must_be_at_2[
    SORTED_ENTRY_OFFSET(distance) == 2 ? 1 : -1
];
typedef char SortedEntry_append_index_must_be_at_4[
    SORTED_ENTRY_OFFSET(append_index) == 4 ? 1 : -1
];
typedef char SortedEntry_sorted_position_must_be_at_6[
    SORTED_ENTRY_OFFSET(sorted_position) == 6 ? 1 : -1
];

/* The grey fill word. Model_SetFogDrawFlags stores 0x808080 here alongside its write
 * to gModel_dwFogDrawFlags (src/game/sorted_entry_render.c), and four sites read it back:
 * the two palette builders take the whole word -- Model_DrawFogTexturedTriangles and
 * Model_DrawFogTexturedQuads (src/game/sorted_entry_render.c) each do
 * *grey = gModel_dwFogBaseColor; and *white = gModel_dwFogBaseColor; -- while
 * the card viewer's fade step reads the low byte through the address,
 * a = *(u8 *)&gModel_dwFogBaseColor, and writes the word back (src/game/duel_scene.c). */
extern u32 gModel_dwFogBaseColor;

extern SortedEntry *gSortedEntry_pBase;
/* The entry count, saved by SortedEntry_SortAndRelink across the sort and
 * compared against gSortedEntry_nCount by the walk in Duel_DrawFieldCards. */
extern u32 gSortedEntry_nSortedCount;
/* The fog-textured HMD drivers' mode word (Model_DrawFogTexturedTriangles and Model_DrawFogTexturedQuads in
 * hmd_fog_textured_drivers.c): bit 3 hands the block to another driver, bit 2
 * walks it through SortedEntry_Append, and bits 0-1 draw sorted entries.
 * SortedEntry_BeginCollection sets bit 2 when it opens the list and
 * SortedEntry_SortAndRelink clears it again. Duel_DrawFieldCards tests bit 1 and
 * clears bits 0 and 1. Only DebugEffect_UpdateViewControls's debug toggle of bit 0 sets either
 * in the decompiled tree, so what raises the flag Duel_DrawFieldCards tests is
 * not known here.  Model_SetFogDrawFlags writes the word wholesale, and all three of
 * its call sites pass 0. */
extern u32 gModel_dwFogDrawFlags;
extern SortedEntry *gSortedEntry_pCursor;
extern u32 gSortedEntry_nCount;

void SortedEntry_Append(s32 dx, s32 dy, s32 dz);
void SortedEntry_SortAndRelink(void);

/* The two text-render state setters. Model_SetFogDrawFlags publishes the render flags
 * word and resets gModel_dwFogBaseColor to 0x808080;
 * SortedEntry_BeginCollection installs the sorted entry list and raises bit 2
 * of the same flags word.
 *
 * SortedEntry_BeginCollection takes the list itself: both globals it writes are
 * SortedEntry *, so the parameter was a u32 only to be cast twice on the way
 * in. Nothing in C calls it, so the typed declaration costs no call site.
 *
 * Model_SetFogDrawFlags's parameter stays u32. It is a flags word, and the two files
 * that reached it spelled it s32; the value is a bit pattern either way, so
 * that difference is only a spelling. */
void Model_SetFogDrawFlags(u32 value);

void SortedEntry_BeginCollection(SortedEntry *list);

#endif
