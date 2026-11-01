#define GRAPHICS_VIEWPORT_IN_DATA
#define D_8009B09C_IN_DATA
#define D_8009B320_IN_DATA
#define GDUEL_WSELECTEDCARDID_IN_DATA
#include "../types.h"

/* Zero-initialized storage this unit owns at 0x8009B258-0x8009B260, defined in
 * address order ahead of the headers that declare it (#2602). */
s8 gCardGrid_bCursorColumn;
s8 gCardGrid_bCursorRow;
static u16 D_8009B25A;
static u32 D_8009B25C;

#include "text_box_blocking_build.h"
#include "graphics_frame.h"
#include "library_runtime.h"
#include "card_constants.h"
#include "../psyq/libgs.h"
#include "ordering_tables.h"
#include "card_grid.h"
#include "display_object.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "sound.h"
#include "library_refresh_selected_card_text.h"
#include "library_grid_cursor.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "text_staging.h"
#include "text_message_staging.h"
#include "library_card_grid.h"

/* The Library card grid: the card-flag lookup Library_GetCardFlags and
 * Library_DrawCardGrid, the per-frame draw callback that calls it, followed by
 * the selected-card text refresh and the cursor tween that drives it. All five
 * functions are contiguous and share gcc_2_8_1_g8_split. */

#define GS_SPRITE_COLOR_WORD(sprite) (*(u32 *)&(sprite)->r)

#ifndef LIBRARY_GRID_TEXTURE_WORD
#define LIBRARY_GRID_TEXTURE_WORD 0xF70130
#endif
#ifndef LIBRARY_GRID_SCREEN_HEIGHT
#define LIBRARY_GRID_SCREEN_HEIGHT 0xF0
#endif

u32 Library_GetCardFlags(const LibraryState *base, s32 index)
{
    return base->cards[index].flags;
}

/* Draws the scrolling card-list grid straight into the scratchpad primitive at
   0x1F800320. The first visible row comes from the viewport scroll divided by
   the 178-pixel row pitch; from there four rows of ten lines are walked, each
   line holding ten cells on the left at column 8 and, while the paired index
   is still below CARD_ID_END, ten more on the right at column 0xA8. A cell is drawn
   only when Library_GetCardFlags reports bit 0x80, tinted 0x808080 or 0x404040 on
   bit 0, and the run stops as soon as a line falls off the bottom of the
   screen. The tail then builds the cursor box at 0x1F800000, colouring it from
   the low seven bits of gMain_dwFrameCounter through a four-way ramp, and draws its four
   edges with GsSortGLine.

   Pure C under the uniform gcc_2_8_1_g8_split profile, the profile of the
   Library functions that follow it in this unit. What the -G0 candidate
   could not reach:

   - Under -G8 the viewport scalars and gMain_dwFrameCounter are small data, so their
     loads stay one unsplit instruction through sched2 and the assembler
     expands each into an adjacent lui/load pair in one register. The two
     IN_DATA macros above keep them absolute rather than gp-relative. The
     ordering table is read as gGraphics_apOrderingTables[3]: the 16-byte array is not small
     data, so that address stays split and reload stores the pointer to its
     stack slot through the dying %hi register ($v0) instead of $t1.
   - `first` is a single-set local, so sched1 keeps its shift next to the
     multiply that consumes it, after the p->x store. That store sits between
     the spill of `b` and its reuse, which is what makes reload load `b` back
     into $t1 rather than reuse $v1.
   - The attribute word is set ahead of the block that stores it. The loop
     note keeps the constant live across the stores before it, which is what
     gives it $v1 and lets sched2 float its lui above the p->y store.
   - The cursor's y is a chained assignment, y1 then y0, from one value. */
void Library_DrawCardGrid(void)
{
    GsSPRITE *p;
    GsGLINE *q;
    u8 *pj;
    u8 *pk;
    GsOT *ot;
    s32 n;
    s32 idx;
    s32 i;
    s32 c;
    s32 j;
    s32 k;
    s32 r;
    s32 white;
    s32 grey;
    s32 a;
    s32 b;
    s32 y;
    s32 v;
    LibraryMotionState *list;
    const LibraryState *tb;
    s32 e;
    s32 f;
    s32 first;
    s32 attribute;

    p = (GsSPRITE *)0x1F800320;
    n = (gGraphics_sViewportY - 8) / 178;
    ot = gGraphics_apOrderingTables[3];
    if (n < 0) {
        return;
    }
    e = 0xE000C;
    f = LIBRARY_GRID_TEXTURE_WORD;
    white = 0x808080;
    grey = 0x404040;
    a = n * 178 + 8;
    b = n * 25;
    p->x = 8;
    first = b * 8;
    p->y = first * 178 + 8;
    *(u32 *)&p->w = e;
    *(u32 *)&p->cx = f;
    attribute = 0x8000000;
    do {
#ifndef LIBRARY_GRID_SKIP_CY_STORE
        p->cy = 0xF7;
#endif
        *(u16 *)&p->u = 0xF060;
        p->attribute = attribute;
        p->tpage = 0x1B;
    } while (0);
    do {
        idx = b * 8;
        p->y = a;
        p->y = a - gGraphics_sViewportY;
        for (i = 0; i < 10; idx += 10, i++) {
            y = p->y;
            j = idx + 1;
            tb = &gLibrary_State;
            if (y + p->w > 0) {
                if (y >= LIBRARY_GRID_SCREEN_HEIGHT) {
                    goto done;
                }
                k = idx + 0x65;
                c = 0;
                /* Keep the cursors at the unshifted state base: rebasing them
                   to cards[] changes the retail address calculations. */
                pk = (u8 *)&gLibrary_State + k * sizeof(LibraryCardRecord);
                pj = (u8 *)&gLibrary_State + j * sizeof(LibraryCardRecord);
                do {
                    r = Library_GetCardFlags(tb, j);
                    if (r & LIBRARY_CARD_FLAG_KNOWN) {
                        GS_SPRITE_COLOR_WORD(p) = white;
                        if (r & LIBRARY_CARD_FLAG_UNOWNED) {
                            GS_SPRITE_COLOR_WORD(p) = grey;
                        }
                        p->x = c + 8;
                        p->cx = (u16)((LibraryState *)pj)->cards[0].clut_x;
                        GsSortFastSprite(p, ot, 2);
                    }
                    if (k < CARD_ID_END) {
                        r = Library_GetCardFlags(tb, k);
                        if (r & LIBRARY_CARD_FLAG_KNOWN) {
                            GS_SPRITE_COLOR_WORD(p) = white;
                            if (r & LIBRARY_CARD_FLAG_UNOWNED) {
                                GS_SPRITE_COLOR_WORD(p) = grey;
                            }
                            p->x = c + 0xA8;
                            p->cx = (u16)((LibraryState *)pk)->cards[0].clut_x;
                            GsSortFastSprite(p, ot, 2);
                        }
                    }
                    pj += sizeof(LibraryCardRecord);
                    j++;
                    pk += sizeof(LibraryCardRecord);
                    c += 0xE;
                    k++;
                } while (c < 0x8A);
            }
            p->y = p->y + 0x10;
        }
        a += 178;
        n++;
        b += 25;
    } while (n < 4);

done:
    q = (GsGLINE *)0x1F800000;
    v = gMain_dwFrameCounter & 0x7F;
    q->attribute = 0x50000000;
    q->b0 = 0;
    q->g0 = 0;
    q->r0 = 0;
    q->b1 = 0;
    q->g1 = 0;
    q->r1 = 0;
    list = &gLibrary_State.motion;
    switch (v / 32) {
    case 0:
        q->g0 = v * 8;
        break;
    case 1:
        q->g0 = 0xFF;
        q->g1 = (v - 0x20) * 8;
        break;
    case 2:
        q->g0 = (0x5F - v) * 8;
        q->g1 = 0xFF;
        break;
    case 3:
        q->g1 = (0x7F - v) * 8;
        break;
    }
    q->x1 = 0;
    q->x0 = (u16)list->x - gGraphics_sViewportX;
    q->y0 = q->y1 = (u16)list->y - gGraphics_sViewportY;
    GsSortGLine(q, ot, 1);
    q->x1 = 0x140;
    GsSortGLine(q, ot, 1);
    q->y1 = 0;
    q->x1 = q->x0;
    GsSortGLine(q, ot, 1);
    q->y1 = LIBRARY_GRID_SCREEN_HEIGHT;
    GsSortGLine(q, ot, 1);
}

/* Regional value: the text box's y (0xCA -> 0xD5). The European build
 * (src/game/european/) defines its own. */
#ifndef LIBRARY_CARD_TEXT_BOX_Y
#define LIBRARY_CARD_TEXT_BOX_Y 0xCA
#endif

void Library_RefreshSelectedCardText(const LibraryState *p)
{
    TextStagingValues *q = gText_StagedValues;
    s32 r;
    s32 t;
    s32 n;
    s32 mode;
    DuelEffectChannel *o;

    mode = 0;
    r = Library_GetGridCursorCardId(p);
    gDuel_wSelectedCardID = r;
    t = (s16)r;
    q->card.card_id = t;
    n = t;

    if (n != 0) {
        r = Library_GetCardFlags(p, n);
        mode = 5;
        if ((r & LIBRARY_CARD_FLAG_KNOWN) == 0) {
            gDuel_wSelectedCardID = 0;
        }
    }

    o = TextBox_Create(1, mode, 0x10, LIBRARY_CARD_TEXT_BOX_Y, 0x120, 0x30);
    gText_SelectedCardColorIndex = o->color_index_54;
    if (p->cards[n].flags & LIBRARY_CARD_FLAG_UNOWNED) {
        gText_SelectedCardColorIndex = 4;
    }
    TextBox_BuildBlockingWithGlyphScratch((struct DuelEffectChannel *)&gText_Channel1);
}

s32 Library_UpdateCursorMotion(void)
{
    LibraryMotionState *object = &gLibrary_State.motion;

    if (!object->active)
        return object->active;
    {
        s32 position;
        position = ((s32)object->x << 16) | object->x_fraction;
        position += object->velocity_x;
        object->x_fraction = position;
        object->x = position >> 16;
        position = ((s32)object->y << 16) | object->y_fraction;
        position += object->velocity_y;
        object->y = position >> 16;
        object->y_fraction = position;
    }
    if (--object->frames == 0) {
        object->active = 0;
        object->x = object->rest_x;
        object->y = object->rest_y;
        SD_SEPlayFull(53);
        Library_RefreshSelectedCardText((const LibraryState *)object);
    }
    object->display_objects.by_role.render->field_30.h.field_30 = object->x;
    object->display_objects.by_role.render->field_30.h.field_32 = object->y;
    return object->active;
}

/*
 * Keep one row snapshot: `Library_StartCursorMotion`
 *
 * The 440-byte library-cursor motion setup matches under the existing uniform
 * `gcc_2_8_1_g8_split` profile with no register pins or inline assembly.
 * The shared `LibraryMotionState` already supplies the complete offsets and
 * signed integer/unsigned fractional halves missing from the terminal
 * incomplete-type attempts. The reference's two copies of its `col` argument leave the
 * unpinned entry move after the unchanged-cell guard, changing ten instruction
 * positions. Keeping one row snapshot through that guard and the Y-coordinate
 * calculation restores retail's initial move into `$t1` without pinning it.
 * The reference's argument names are reversed relative to the confirmed grid
 * symbols: the first argument drives X/`gCardGrid_bCursorColumn`, and the second
 * drives Y/`gCardGrid_bCursorRow`. The matching source uses those existing
 * `card_grid.h` declarations, without adding duplicate address-named globals.
 *
 * The same signed `delta` is assigned and shifted in place for both 16.16
 * velocity divisions. The unsigned fractional fields preserve the positive
 * `0x8000` constant and its `ori` instruction with the ordinary supported
 * profile; no additional assembler flags are needed. Same-cell requests return
 * without altering an active animation. Otherwise, the original frame count,
 * target coordinates, half-pixel initial fractions and active flag are retained.
 * The historical canonical rows and six-row terminal refinement history remain
 * intact, followed by one post-terminal resolution with the new source evidence.
 */
void Library_StartCursorMotion(s32 column, s32 row, s32 frames)
{
    s32 target_row = row;
    LibraryMotionState *state = &gLibrary_State.motion;
    s32 x;
    s32 delta;

    if (column == gCardGrid_bCursorColumn && target_row == gCardGrid_bCursorRow) {
        return;
    }
    gCardGrid_bCursorColumn = column;
    gCardGrid_bCursorRow = target_row;
    state->frames = frames;
    if (column >= 10) {
        x = (column % 10) * 14 + 0xAE;
    } else {
        x = (column % 10) * 14 + 0xE;
    }
    {
        s32 velocity_x;
        s32 velocity_y;
        s32 bank = target_row / 10;
        s32 y;

        delta = (s16)x - state->x;
        delta <<= 16;
        velocity_x = delta / frames;
        y = bank * 178 + (target_row % 10) * 16 + 0xE;
        delta = (s16)y - state->y;
        delta <<= 16;
        velocity_y = delta / frames;
        state->rest_x = (s16)x;
        state->rest_y = (s16)y;
        state->y_fraction = 0x8000;
        state->x_fraction = 0x8000;
        delta = 1;
        state->active = delta;
        state->velocity_x = velocity_x;
        state->velocity_y = velocity_y;
    }
}
