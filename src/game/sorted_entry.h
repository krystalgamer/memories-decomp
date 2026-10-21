#ifndef MEMORIES_DECOMP_SORTED_ENTRY_H
#define MEMORIES_DECOMP_SORTED_ENTRY_H

#include "../types.h"

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
typedef struct {
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
 * to gModel_dwFogDrawFlags (src/game/text_render_state.c:8), and four sites read it back:
 * the two palette builders take the whole word -- Model_DrawFogTexturedTriangles and
 * Model_DrawFogTexturedQuads (src/game/hmd_fog_textured_drivers.c) each do
 * CVECTOR_WORD(grey) = gModel_dwFogBaseColor; and CVECTOR_WORD(white) = gModel_dwFogBaseColor; -- while
 * the card viewer's fade step reads the low byte through the address,
 * a = *(u8 *)&gModel_dwFogBaseColor, and writes the word back (src/game/duel_scene_field_actions.c).
 *
 * That byte view is why DuelScene_UpdateFieldActions wants this symbol outside
 * small data: it defines D_8009B300_IN_DATA to take the .data arm, the same
 * arrangement the state pointers below use. */
#ifdef D_8009B300_IN_DATA
extern u32 gModel_dwFogBaseColor __attribute__((section(".data")));
#else
extern u32 gModel_dwFogBaseColor;
#endif

#ifdef SORTED_ENTRY_STATE_IN_DATA
extern SortedEntry *G32 gSortedEntry_pBase __attribute__((section(".data")));
#else
extern SortedEntry *G32 gSortedEntry_pBase;
#endif
/* The entry count, saved by SortedEntry_SortAndRelink across the sort and
 * compared against gSortedEntry_nCount by the walk in Duel_DrawFieldCards. */
#ifdef SORTED_ENTRY_STATE_IN_DATA
extern u32 gSortedEntry_nSortedCount __attribute__((section(".data")));
#else
extern u32 gSortedEntry_nSortedCount;
#endif
/* The fog-textured HMD drivers' mode word (Model_DrawFogTexturedTriangles and Model_DrawFogTexturedQuads in
 * hmd_fog_textured_drivers.c): bit 3 hands the block to another driver, bit 2
 * walks it through SortedEntry_Append, and bits 0-1 draw sorted entries.
 * SortedEntry_BeginCollection sets bit 2 when it opens the list and
 * SortedEntry_SortAndRelink clears it again. Duel_DrawFieldCards tests bit 1 and
 * clears bits 0 and 1. Only func_800220B8's debug toggle of bit 0 sets either
 * in the decompiled tree, so what raises the flag Duel_DrawFieldCards tests is
 * not known here.  Model_SetFogDrawFlags writes the word wholesale, and all three of
 * its call sites pass 0. */
#ifdef D_8009B30C_AS_SIGNED_DATA
extern s32 gModel_dwFogDrawFlags __attribute__((section(".data")));
#elif defined(SORTED_ENTRY_STATE_IN_DATA)
extern u32 gModel_dwFogDrawFlags __attribute__((section(".data")));
extern SortedEntry *G32 gSortedEntry_pCursor __attribute__((section(".data")));
extern u32 gSortedEntry_nCount __attribute__((section(".data")));
#else
extern u32 gModel_dwFogDrawFlags;
extern SortedEntry *G32 gSortedEntry_pCursor;
extern u32 gSortedEntry_nCount;
#endif

void SortedEntry_Append(s32 dx, s32 dy, s32 dz);
void SortedEntry_SortAndRelink(void);

#endif
