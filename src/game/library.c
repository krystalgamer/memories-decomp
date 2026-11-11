#include "../types.h"

/* Zero-initialized storage this unit owns at 0x8009B258-0x8009B260, defined in
 * address order ahead of the headers that declare it (#2602). */
s8 gCardGrid_bCursorColumn;
s8 gCardGrid_bCursorRow;
static PADDING_VARIABLE(2);
static PADDING_VARIABLE(4);

#include "../ygo_types.h"
#include "text_box_blocking_build.h"
#include "main.h"
#include "library.h"
#include "card_constants.h"
#include "../psyq/libgs.h"
#include "ordering_tables.h"
#include "card_grid.h"
#include "display_object.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "sound.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "text_staging.h"
#include "text_message_staging.h"
#include "input.h"
#include "main_mode_state.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "model_slot_getters.h"
#include "camera_view.h"
#include "file_transfer.h"
#include "image_transfer_rects.h"
#include "library_reference_y.h"
#include "model_slot_support.h"
#include "model_update_view_metrics.h"
#include "sound_voice_data.h"
#include "view_state_orbit.h"
#include "../psyq/rand.h"
#include "model_slot_control.h"
#include "mem_card.h"
#include "campaign_flags.h"
#include "display_object_core.h"
#include "display_object_layout.h"
#include "text_helpers.h"
#include "main_services.h"
#include "duel_card.h"
#include "duel_effect_resource_record.h"
#include "display_object_helpers.h"
#include "duel_deck_lookup.h"
#include "library_create_grid_section_sprites.h"
#include "sorted_entry_render.h"
#include "display_object_config.h"
#include "display_object_interpolation.h"
#include "duel_effect_resource_setup.h"
#include "fade.h"
#include "card_preview_objects.h"
#include "library_draw_wireframe_globe.h"
#include "text_box_update_channels.h"
#include "model_scene_runtime.h"
#include "model_camera_effects.h"
#include "model.h"
#include "model_load_step.h"
#include "model_scene_controllers.h"
#include "graphics_packet_submission.h"
#include "display_object_projection.h"
#include "gpu_packets.h"

/* The Library screen, one translation unit from 0x80029EB0 to the end of the
 * runtime (#2602). It holds the card grid (the card-flag lookup
 * Library_GetCardFlags and Library_DrawCardGrid, the per-frame draw callback
 * that calls it, the selected-card text refresh and the cursor tween that
 * drives it), the grid scroll, the grid cursor and the Library runtime. Retail
 * reads the grid cursor bytes above gp-relative only from these functions;
 * all of them share gcc_2_8_1_g8_split. */

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
     expands each into an adjacent lui/load pair in one register. The
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
    TextBox_BuildBlockingWithGlyphScratch(TEXT_CHANNEL(1));
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
    object->display_objects.by_role.render->xy.h.x = object->x;
    object->display_objects.by_role.render->xy.h.y = object->y;
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

/* Regional value: the lower bound past which the viewport follows the motion
 * (0xB0 -> 0xC0). The European build (src/game/european/) defines its own. */
#ifndef LIBRARY_VIEWPORT_FOLLOW_BOTTOM
#define LIBRARY_VIEWPORT_FOLLOW_BOTTOM 0xB0
#endif

void Library_UpdateGridScroll(const LibraryMotionState *record)
{
    const LibraryMotionState *motion;
    s32 d =
        record->y - gGraphics_sViewportY;
    s32 h;

    motion = record;
    gGraphics_sViewportX = 0;
    h = (u16)motion->y;
    if (d < 0x40) {
        gGraphics_sViewportY = h - 0x40;
    }
    if (d >= LIBRARY_VIEWPORT_FOLLOW_BOTTOM) {
        gGraphics_sViewportY = (u16)motion->y - LIBRARY_VIEWPORT_FOLLOW_BOTTOM;
    }
}

s32 Library_GetGridCursorCardId(const LibraryState *state)
{
    s32 row = gCardGrid_bCursorRow;
    s32 row_tens = row / CARD_GRID_SECTION_SIDE_LENGTH;
    s8 row_tens_byte = (s8)row_tens;
    s32 index = row_tens_byte * CARD_GRID_SECTION_ROW_CARD_COUNT;
    s8 row_ones =
        (s8)(row - row_tens * CARD_GRID_SECTION_SIDE_LENGTH);
    s32 base;
    s32 column;
    s32 column_tens;
    s8 column_ones;
    s32 result;

    index += row_ones * CARD_GRID_SECTION_SIDE_LENGTH;

    column = gCardGrid_bCursorColumn;
    if (column >= CARD_GRID_SECTION_SIDE_LENGTH) {
        index += CARD_GRID_SECTION_CARD_COUNT;
    }
    base = index + 1;
    column_tens = column / CARD_GRID_SECTION_SIDE_LENGTH;
    column_ones =
        (s8)(column - column_tens * CARD_GRID_SECTION_SIDE_LENGTH);
    index = base + column_ones;
    if (index >= CARD_ID_END) {
        return 0;
    }
    return index;
}

#ifndef LIBRARY_GRID_CONFIRM_BUTTON
#define LIBRARY_GRID_CONFIRM_BUTTON PAD_BUTTON_CROSS
#endif

#ifndef LIBRARY_GRID_CANCEL_BUTTON
#define LIBRARY_GRID_CANCEL_BUTTON PAD_BUTTON_CANCEL
#endif

/*
 * State 1 of the Library screen. The held-input view remains volatile because
 * every direction path must retain the retail input reloads.
 */
void Library_UpdateGridCursor(LibraryState *state)
{
    s32 card_id;
    s32 column;
    s32 row;
    s32 frames;
    s32 distance;

    if (Library_UpdateCursorMotion() == 0) {
        if ((gInput_awPadPressed[0] & LIBRARY_GRID_CONFIRM_BUTTON) != 0) {
            card_id = Library_GetGridCursorCardId(state);
            if ((Library_GetCardFlags(state, card_id) &
                 LIBRARY_CARD_FLAG_KNOWN) != 0) {
                state->motion.selected_card_id = card_id;
                state->motion.state = 2;
                return;
            }
        }
        if ((gInput_awPadPressed[0] & LIBRARY_GRID_CANCEL_BUTTON) != 0) {
            gMain_bActiveModeState = gMain_bReturnMode;
            return;
        }
        if ((gInput_awPadHeld[0] & (PAD_DIRECTION_MASK | PAD_BUTTON_L1_R1_MASK)) != 0) {
            column = gCardGrid_bCursorColumn;
            row = gCardGrid_bCursorRow;
            frames = 6;
            if ((gInput_awPadHeld[0] & PAD_BUTTON_SQUARE) != 0) {
                frames = 2;
            }
            if ((gInput_awPadHeld[0] & PAD_BUTTON_L1_R1_MASK) != 0) {
                if ((gInput_awPadHeld[0] & PAD_BUTTON_R1) != 0) {
                    row += 0xA;
                    if (row >= 0x28) {
                        row = 0x27;
                    }
                } else {
                    row -= 0xA;
                    if (row < 0) {
                        row = 0;
                    }
                }
                distance = row - gCardGrid_bCursorRow;
                if (distance >= 0) {
                    frames = distance * 2;
                } else {
                    frames = (gCardGrid_bCursorRow - row) * 2;
                }
            } else {
                if ((gInput_awPadHeld[0] & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
                    if ((gInput_awPadHeld[0] & PAD_DIRECTION_RIGHT) != 0) {
                        column += 1;
                        if (column >= 0x14) {
                            column = 0x13;
                            if (row < 0x27) {
                                column = 0;
                                row += 1;
                                goto wrap_delay;
                            }
                        }
                    } else {
                        column -= 1;
                        if (column < 0) {
                            column = 0;
                            if (row != 0) {
                                column = 0x13;
                                row -= 1;
wrap_delay:
                                frames *= 4;
                            }
                        }
                    }
                }
                if ((gInput_awPadHeld[0] & PAD_DIRECTION_VERTICAL_MASK) != 0) {
                    if ((gInput_awPadHeld[0] & PAD_DIRECTION_DOWN) != 0) {
                        row += 1;
                        if (row >= 0x28) {
                            row = 0x27;
                        }
                    } else {
                        row -= 1;
                        if (row < 0) {
                            row = 0;
                        }
                    }
                }
            }
            Library_StartCursorMotion(column, row, frames);
        }
    }
    Library_UpdateGridScroll(&state->motion);
}

/* The draw callback Library_CreateFadingCardOutline installs at 0x4C on the object it
   builds: it fades the object out over its own field_60 countdown, projects
   it, and emits a four-vertex flat polyline (one colour word, length 6)
   and then, with the length cut to 3 and x3/y3 copied into x1/y1, a single
   flat line from the first vertex to the last, both through Graphics_SubmitDrawModePacket.
   The record is the canonical DisplayObject.

   The scratchpad side is typed as well: a LINE_F4 at 0x1F8002A0, the four
   SVECTOR corners RotAverage4 projects into its x0..x3, and the
   ProjectionOut DisplayObject_ProjectRotationFacing leaves at 0x1F800398. That
   is load-bearing. Written through byte pointers, those stores are not struct
   references, and
   GCC floats the object's struct-member reads of 0x32 and 0x3E across them
   where the target does not. With both sides typed the plain member reads
   keep their place.

   b0[1].vx and b0[2].vy are b1->vx and b2->vy: the target addresses those
   two stores from the first corner's base, and spelling them through b1 and
   b2 changes the code. */
void Library_DrawFadingCardOutline(DisplayObject *o, GsOT *ot)
{
    s32 sp28;
    s32 sp2C;
    LINE_F4 *q;
    struct ProjectionOut *r;
    SVECTOR *b0;
    SVECTOR *b1;
    SVECTOR *b2;
    SVECTOR *b3;
    s32 n;
    s32 e;
    s32 m;
    s32 z;
    s32 k;
    s32 v;
    s32 w;

    n = *(u16 *)&o->field_60 - 0x10;
    o->field_60 = n;
    if ((s16)n <= 0) {
        DisplayObject_ReleaseIfPresent(o);
        return;
    }

    r = (struct ProjectionOut *)0x1F800398;
    ((u8 *)&o->rgb)[2] = *(u8 *)&o->field_60;
    DisplayObject_ProjectRotationFacing(o,
                  *(s16 *)&o->xy.h.x + *(s16 *)&o->field_18.center.x,
                  *(s16 *)&o->xy.h.y + *(s16 *)&o->field_18.center.y,
                  r);

    q = (LINE_F4 *)0x1F8002A0;
    b0 = (SVECTOR *)0x1F800300;
    m = 0x55555555;
    b2 = (SVECTOR *)0x1F800310;
    b3 = (SVECTOR *)0x1F800318;
    b1 = (SVECTOR *)0x1F800308;

    z = *(s32 *)&o->rgb;
    setlen(q, 6);
    q->pad = m;
    *(s32 *)&q->r0 = z;
    q->code = 0x4C;

    v = o->xy.h.x - (u16)r->x;
    b2->vx = v;
    b0->vx = v;
    v += o->field_3C.size.w;
    b3->vx = v;
    b0[1].vx = v;

    w = o->xy.h.y - (u16)r->y;
    b1->vy = w;
    b0->vy = w;
    e = o->field_3C.size.h;
    b3->vz = 0;
    b2->vz = 0;
    b1->vz = 0;
    b0->vz = 0;

    w += e;
    b3->vy = w;
    b0[2].vy = w;

    RotAverage4(
        b0, b1, b2, b3,
        (long *)&q->x0, (long *)&q->x1,
        (long *)&q->x3, (long *)&q->x2,
        (long *)&sp28, (long *)&sp2C
    );

    Graphics_SubmitDrawModePacket((u32 *)q, ot, *(u16 *)&o->ot_depth, 1);
    setlen(q, 3);
    q->code = 0x40;
    k = *(u16 *)&o->ot_depth;
    *(s32 *)&q->x1 = *(s32 *)&q->x3;
    Graphics_SubmitDrawModePacket((u32 *)q, ot, (u16)k, 1);
}

DisplayObject*Library_CreateFadingCardOutline(DisplayObject*src,int add){DisplayObject*o=DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(),6);o->field_48.word=src->field_48.word;*(s32*)&o->field_18.center.x=*(s32*)&src->field_18.center.x;o->field_60=0x100;*(s16*)&o->field_3C.size.w=*(s16*)&o->field_18.center.x*2;*(s16*)&o->field_3C.size.h=*(s16*)&o->field_18.center.y*2;o->xy.word=src->xy.word;o->field_20.word=src->field_20.word;o->field_44.word=src->field_44.word;o->rgb=0;DisplayObject_SelectOrderingTable1(o);DisplayObject_SetDepthOffset(o,(signed char)(*(u8*)&src->depth_offset+add));o->back_face_handler=1;o->field_4C=(s32)Library_DrawFadingCardOutline;o->attribute=(o->attribute|(GsALON | GsAONE))&~GsROTOFF;return o;}

/* The seven contiguous functions of the Library screen: the card-view state
   handler Library_UpdateCardView, the trivial state handlers for states 0 and 3, the
   per-frame dispatcher Library_UpdateScreen, the package-transfer callback, the
   owned-card pass and the screen entry Library_InitializeScreen.

   The state handlers were recorded at gcc_2_8_1_g8. They compile to
   identical objects at gcc_2_8_1_g8_split, which the dispatcher and entry
   need, so they build there with the rest of the screen. */

/* Library screen state 2, the card view. It opens the view, slides the model
   and text box in, rotates the model light with the view angle and walks the
   close sequence back to the grid. Owned state and stored pointers use their
   canonical members; remaining scalar/object views retain measured accesses.

   What the retail object depends on:
   - the two slide states are separate arms; cross-jumping merges their
     settle and Widget_SlideSine tails;
   - state 0 updates the object's flag words through `bits` and `attr`, each
     assigned twice, so sched1 does not glue their loads to their uses, and
     reads the object's y origin into the reused `id` ahead of the position
     stores;
   - the selected card ID is stored and read straight back as the stats index;
   - gGraphics_bSyncVBlankTarget is volatile in this unit, which keeps the delay slot of the
     close path's DisplayObject_SetResourceVariant call empty, and the pad
     word is volatile because state 5 loads it twice;
   - the typed card-table flag lvalues must retain the retail base-first
     address sum. */
#define B(p, o) (*((u8 *)(p) + (o)))
#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))

#ifndef LIBRARY_CARD_VIEW_BOX_FIELD_54
#define LIBRARY_CARD_VIEW_BOX_FIELD_54 0
#endif
#ifndef LIBRARY_CARD_VIEW_X
#define LIBRARY_CARD_VIEW_X 0x94
#endif
#ifndef LIBRARY_CARD_VIEW_BOX_WIDTH
#define LIBRARY_CARD_VIEW_BOX_WIDTH 0xA8
#endif
/* Regional values (US defaults): the card view's y (0xE; European 6), the
   loading icon's y (0xCD; 0xDD), the card text box's height (0xC0; 0xD0),
   the zoomed card's y (4; 0) and its smallest scale byte (0xD8; 0xD7). */
#ifndef LIBRARY_CARD_VIEW_Y
#define LIBRARY_CARD_VIEW_Y 0xE
#endif
#ifndef LIBRARY_LOADING_ICON_Y
#define LIBRARY_LOADING_ICON_Y 0xCD
#endif
#ifndef LIBRARY_CARD_VIEW_BOX_HEIGHT
#define LIBRARY_CARD_VIEW_BOX_HEIGHT 0xC0
#endif
#ifndef LIBRARY_CARD_ZOOM_Y
#define LIBRARY_CARD_ZOOM_Y 4
#endif
#ifndef LIBRARY_CARD_ZOOM_MIN_SCALE
#define LIBRARY_CARD_ZOOM_MIN_SCALE 0xD8
#endif
#ifndef LIBRARY_CARD_VIEW_CANCEL_BUTTON
#define LIBRARY_CARD_VIEW_CANCEL_BUTTON PAD_BUTTON_CIRCLE
#endif
#ifndef LIBRARY_CARD_VIEW_ACTION_MASK
#define LIBRARY_CARD_VIEW_ACTION_MASK 0x20C0
#endif

void Library_UpdateCardView(LibraryState *state)
{
    DuelEffectResourceRecord *rec;
    DisplayObject *o;
    ModelSlotLightEntry *light;
    DuelEffectChannel *box;
    s32 flags;
    s32 sel;
    s32 mode;
    s32 id;
    s32 i;
    s32 b;
    s32 a;
    s32 c;
    s32 layout;
    GsRVIEW2 *rv;
    SVECTOR *p;
    ViewState *vs;
    s32 bits;
    s32 attr;

    rec = gCardPreview_Resources;
    flags = state->motion.state;
    if (!(flags & 0x80)) {
        state->motion.state = flags | 0x80;
        state->motion.card_view_phase = 0;
        state->motion.card_view_slide_phase = 0;
        state->motion.model_load_phase = 1;
        Model_ResetSceneView();
        Model_ResetSceneState();
        gViewState_Current.distance = 0x334;
        gViewState_Current.angle = 0x400;
        gViewState_Current.orbit_angle = 0xC4;
        gViewState_Current.field_0C = 0;
        gViewState_Current.view.rz = 0;
        gViewState_Current.view.super = 0;
        gViewState_Current.projection = 0x12C;
        GsSetProjection(0x12C);
        rv = &gViewState_Current.view;
        rv->vrx = 0;
        gViewState_Current.field_06 = 0;
        rv->vry = 0;
        gViewState_Current.field_08 = 0;
        rv->vrz = 0;
        gViewState_Current.field_0A = 0;
        ViewState_ApplyOrbit();
        CardPreview_RequestResourceTransfer(0, (u16)state->motion.selected_card_id);
        Fade_StartOutKeepOverlay();
        gFade_State.step = 6;
        state->loading_icon = 0;
        return;
    }
    TextBox_UpdateChannels();
    mode = state->motion.model_load_phase;
    if ((mode & 0xF) && ((gFile_dwTransferState & 0x02000030) | gFile_dwCancelRequestState) == 0) {
        if (mode == 1) {
            state->motion.model_load_phase = 0;
            id = (u16)state->motion.selected_card_id - 1;
            if (((gDuel_adwCardStats[id] >> 0x1A) & 0x1F) < 0x14) {
                Model_LoadMonsterMerge(0, id, 0, 0, 0, 0, 4);
                state->motion.animation_countdown = 0x12C;
                state->motion.model_load_phase = 2;
            }
        } else {
            Model_AdvanceSlotLoad(0);
            if (Model_GetSlotReadiness(0) == 1) {
                state->motion.model_load_phase = 0;
                o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
                DisplayObject_ConfigureSpriteAtPosition(o, 0x130, LIBRARY_LOADING_ICON_Y, 3, 0, 2, 0xB, 0x20C);
                o->flags |= 0x28;
                DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(o));
                DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(o), 0xA);
                state->loading_icon = DISPLAY_OBJECT_VIEW(o);
            }
        }
    }

    flags = state->motion.card_view_slide_phase;
    switch (flags & 0xF) {
    case 1:
        o = state->card_view;
        if (!(flags & 0x80)) {
            state->motion.card_view_slide_phase = flags | 0x80;
            S(o, 0x30) = 0x148;
            S(o, 0x32) = LIBRARY_CARD_VIEW_Y;
            DisplayObject_SavePosition((void *)o);
            o->field_60 = -0x400;
        }
        H(o, 0x60) += 0x33;
        if (o->field_60 >= 0) {
            S(o, 0x30) = LIBRARY_CARD_VIEW_X;
            S(o, 0x32) = LIBRARY_CARD_VIEW_Y;
            state->motion.card_view_slide_phase = 0;
        } else {
            Widget_SlideSine(
                (void *)o, LIBRARY_CARD_VIEW_X, LIBRARY_CARD_VIEW_Y, o->field_60);
        }
        TextBox_SetPos(&gText_aChannels[0], S(o, 0x30), S(o, 0x32));
        break;
    case 2:
        o = state->card_view;
        if (!(flags & 0x80)) {
            state->motion.card_view_slide_phase = flags | 0x80;
            DisplayObject_SavePosition((void *)o);
            o->field_60 = 0x400;
        }
        H(o, 0x60) -= 0x33;
        if (o->field_60 <= 0) {
            S(o, 0x30) = 0x148;
            S(o, 0x32) = LIBRARY_CARD_VIEW_Y;
            state->motion.card_view_slide_phase = 0;
        } else {
            Widget_SlideSine((void *)o, 0x148, LIBRARY_CARD_VIEW_Y, o->field_60);
        }
        TextBox_SetPos(&gText_aChannels[0], S(o, 0x30), S(o, 0x32));
        break;
    }

    switch (state->motion.card_view_phase & 0x1F) {
    case 0:
        state->cards[(u16)state->motion.selected_card_id].flags &= 0x7F;
        o = (DisplayObject *)CardPreview_CreateResourceObjects(0, -1, -1);
        bits = o->flags;
        bits |= 4;
        o->flags = bits;
        attr = o->attribute;
        attr &= 0xF7FFFFFF;
        o->attribute = attr;
        id = H(o, 0x4A);
        S(o, 0x30) = state->motion.rest_x - gGraphics_sViewportX - H(o, 0x48);
        S(o, 0x32) = state->motion.rest_y - gGraphics_sViewportY - id;
        o->field_44.scale.x = o->field_44.scale.y = 0;
        DisplayObject_SavePosition((void *)o);
        o->field_60 = 0;
        SD_SEPlayFull(0x32);
        W(rec->secondary_object, 4) |= 0x80000000;
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(o, 0x148, LIBRARY_CARD_VIEW_Y, 0, 2, 0, 0x1B, 0x107);
        B(o, 0x5F) = 0x80;
        o->field_60 = -0x400;
        o->flags |= 8;
        DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(o));
        DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(o), 4);
        state->card_view = DISPLAY_OBJECT_VIEW(o);
        state->motion.card_view_slide_phase = 1;
        gDuel_wSelectedCardID = (u16)state->motion.selected_card_id;
        layout = 3;
        if (((gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >> 0x1A) & 0x1F) >= 0x14) {
            layout = 4;
        }
        box = TextBox_Create(
            0, layout, LIBRARY_CARD_VIEW_X, LIBRARY_CARD_VIEW_Y,
            LIBRARY_CARD_VIEW_BOX_WIDTH, LIBRARY_CARD_VIEW_BOX_HEIGHT);
#ifndef VERSION_EUROPE
        /* The European build does not write color_index_54 here. */
        box->color_index_54 = LIBRARY_CARD_VIEW_BOX_FIELD_54;
#endif
        box->glyph_delay = 1;
        box->depth_offset = 4;
        state->motion.card_view_phase = 1;
        return;
    case 1:
        o = rec->primary_object;
        Library_CreateFadingCardOutline(DISPLAY_OBJECT_VIEW(o), -1);
        if (!(o->attribute & 0x08000000)) {
            o->field_20.b.rotation_z += 0xC;
            o->field_20.b.rotation_y += 6;
            H(o, 0x46) = H(o, 0x44) += 0xCC;
            H(o, 0x60) += 0x66;
            DisplayObject_InterpolatePositionCosine((void *)o, 2, LIBRARY_CARD_ZOOM_Y, o->field_60);
            if (o->field_60 >= 0x800) {
                o->field_20.word = 0x8000;
                o->xy.word = (LIBRARY_CARD_ZOOM_Y << 16) | 2;
                o->field_44.word = 0x10001000;
                W(rec->secondary_object, 4) &= 0x7FFFFFFF;
                o->attribute |= 0x08000000;
            }
            if (!(o->attribute & 0x08000000)) {
                break;
            }
        }
        if (state->motion.card_view_slide_phase == 0) {
            state->motion.card_view_phase = 2;
        }
        return;
    case 2:
        if (state->motion.model_load_phase != 1 && !(gFade_State.flags & 0x80)) {
            gMain_apFrameServiceCallbacks[3] = 0;
            i = 0;
            do {
                state->motion.display_objects.all[i]->flags &= 0xFFBF;
                i++;
            } while (i < 9);
            H(TEXT_CHANNEL(3)->render_object, 8) &= 0xFFBF;
            state->motion.card_view_phase = 3;
            SD_SEPlayFull(0x31);
    case 3:
            o = rec->primary_object;
            o->field_20.b.rotation_y += 8;
            if (!(o->field_20.b.rotation_y & 0xFF)) {
                o->flags &= 0xFFFB;
                state->motion.card_view_phase = 5;
                return;
            }
        }
        break;
    case 4:
        light = (ModelSlotLightEntry *)Model_GetFlatLight(0, 2);
        vs = &gViewState_Current;
        a = -vs->distance;
        b = a * rcos(vs->orbit_angle) / 4096;
        c = a * rsin(vs->orbit_angle) / 4096;
        a = b * rsin(vs->angle + 0x800) / 4096;
        b = b * rcos(vs->angle + 0x800) / 4096;
        light->direction_x = b;
        light->direction_y = c;
        light->direction_z = a;
        flags = state->motion.card_view_phase;
        if (!(flags & 0x80)) {
            state->motion.card_view_phase = flags | 0x80;
            state->motion.card_view_subphase = 0;
            gGraphics_bSyncVBlankTarget = 1;
            gMain_apFrameServiceCallbacks[2] = (void (*)(void))Library_DrawWireframeGlobe;
            flags = state->motion.state;
            state->motion.globe_radius = 0;
            if (!(flags & 0x40)) {
                state->motion.state = flags | 0x40;
                light = (ModelSlotLightEntry *)Model_GetFlatLight(0, 0);
                light[0].direction_y = -0x1000;
                light[0].direction_x = 0;
                light[0].direction_z = 0;
                W(light, 0xC) = 0xFFFFFF;
                light[1].direction_x = 0;
                light[1].direction_y = 0x1000;
                light[1].direction_z = 0;
                W(light, 0x1C) = 0x404040;
                W(light, 0x2C) = 0xC0C0C0;
                Model_SetSlotAmbient(0, 0x400, 0x400, 0x400);
                Model_CopySlotPosition(0, (u16 *)0x80181000);
                p = (SVECTOR *)0x80181000;
                gViewState_Current.view.vrx = p->vx;
                gViewState_Current.view.vry = p->vy;
                gViewState_Current.view.vrz = p->vz;
                gViewState_Current.distance = Model_AdjustCameraDistanceForSlot(0, 0x334);
                Model_SetSlotShadowEnabled(0, 1);
                Model_SetScreenYOverride(0);
            }
            Model_ControlSlotAnimation(0, 0, 1);
            Model_SeekSlotAnimation(0, 1, 0);
            state->motion.animation_countdown = 0x12C;
            state->motion.card_view_slide_phase = 2;
            o = rec->primary_object;
            o->flags |= 4;
            DisplayObject_SavePosition((void *)o);
            o->field_60 = 0;
            SD_SEPlayFull(0x34);
            Fade_StartInKeepOverlay();
            return;
        }
        sel = state->motion.card_view_subphase;
        switch (sel) {
        case 0:
            o = rec->primary_object;
            if (o->field_60 < 0x800) {
                o->field_20.b.rotation_y -= 4;
                if (o->field_20.b.rotation_y < LIBRARY_CARD_ZOOM_MIN_SCALE) {
                    o->field_20.b.rotation_y = LIBRARY_CARD_ZOOM_MIN_SCALE;
                }
                H(o, 0x60) += 0xCC;
                DisplayObject_InterpolatePositionCosine((void *)o, -0x16, LIBRARY_CARD_ZOOM_Y, o->field_60);
                if (o->field_60 >= 0x800) {
                    o->field_20.b.rotation_y = LIBRARY_CARD_ZOOM_MIN_SCALE;
                    S(o, 0x30) = -0x16;
                    S(o, 0x32) = LIBRARY_CARD_ZOOM_Y;
                }
            }
            state->motion.globe_radius = (u16)state->motion.globe_radius + 0x2A;
            if (state->motion.globe_radius >= 0x400) {
                state->motion.globe_radius = 0x400;
                if (!(gFade_State.flags & 0x80) && state->motion.card_view_slide_phase == 0) {
                    state->motion.card_view_subphase = 1;
                    DisplayObject_SetResourceVariant((void *)state->loading_icon, 1);
                    return;
                }
            }
            break;
        case 1:
            if (gInput_awPadPressed[0] & 0x80E0) {
                Fade_StartOutKeepOverlay();
                state->motion.card_view_slide_phase = 1;
                state->motion.card_view_subphase = 2;
                SD_SEPlayFull(0x34);
                o = rec->primary_object;
                DisplayObject_SavePosition((void *)o);
                o->field_60 = 0;
                return;
            }
            break;
        case 2:
            o = rec->primary_object;
            if (o->flags & 4) {
                o->field_20.b.rotation_y += 4;
                H(o, 0x60) += 0xCC;
                DisplayObject_InterpolatePositionCosine((void *)o, 2, LIBRARY_CARD_ZOOM_Y, o->field_60);
                if (o->field_60 >= 0x800) {
                    o->field_20.b.rotation_y = 0;
                    S(o, 0x30) = 2;
                    S(o, 0x32) = LIBRARY_CARD_ZOOM_Y;
                    o->flags &= 0xFFFB;
                }
                if (o->flags & 4) {
                    break;
                }
            }
            if (!(gFade_State.flags & 0x80) && state->motion.card_view_slide_phase == 0) {
                SD_KeyOffVoiceSlots();
                gMain_apFrameServiceCallbacks[2] = 0;
                {
                    void *p = (void *)state->loading_icon;

                    gGraphics_bSyncVBlankTarget = 0;
                    DisplayObject_SetResourceVariant(p, 2);
                }
                state->motion.card_view_phase = 5;
                return;
            }
            break;
        }
        break;
    case 5:
        if (gInput_awPadPressed[0] & LIBRARY_CARD_VIEW_CANCEL_BUTTON) {
            H(rec->primary_object, 8) |= 4;
            state->motion.card_view_phase = 6;
            state->motion.model_load_phase = 0;
            DisplayObject_ReleaseIfPresent((void *)state->loading_icon);
            File_CancelTransfers();
            SD_SEPlayFull(0x31);
            return;
        }
        if (state->motion.model_load_phase == 0 &&
            (gInput_awPadPressed[0] & LIBRARY_CARD_VIEW_ACTION_MASK)
            && ((gDuel_adwCardStats[(u16)state->motion.selected_card_id - 1] >> 0x1A) & 0x1F) < 0x14
            && state->motion.card_view_slide_phase == 0) {
            state->motion.card_view_phase = 4;
            return;
        }
        break;
    case 6:
        o = rec->primary_object;
        o->field_20.b.rotation_y += 8;
        if ((s8)o->field_20.b.rotation_y < 0) {
            W(rec->secondary_object, 4) |= 0x80000000;
            o->attribute &= 0xF7FFFFFF;
            DisplayObject_SavePosition((void *)o);
            i = 0;
            o->field_60 = 0;
            do {
                state->motion.display_objects.all[i]->flags |= 0x40;
                i++;
            } while (i < 9);
            H(TEXT_CHANNEL(3)->render_object, 8) |= 0x40;
            gMain_apFrameServiceCallbacks[3] = Library_DrawCardGrid;
            Fade_StartInKeepOverlay();
            state->motion.card_view_slide_phase = 2;
            state->motion.card_view_phase = 7;
            SD_SEPlayFull(0x33);
            return;
        }
        break;
    case 7:
        o = rec->primary_object;
        if (o != 0) {
            Library_CreateFadingCardOutline(DISPLAY_OBJECT_VIEW(o), 1);
            o->field_20.b.rotation_z += 0xC;
            o->field_20.b.rotation_y += 6;
            H(o, 0x60) += 0x66;
            DisplayObject_InterpolatePositionCosine((void *)o,
                state->motion.x - o->field_48.pivot.x - gGraphics_sViewportX,
                state->motion.y - o->field_48.pivot.y - gGraphics_sViewportY,
                o->field_60);
            H(o, 0x46) = H(o, 0x44) -= 0xCC;
            if (o->field_60 >= 0x800) {
                DuelEffect_ReleaseResourceObjects(0);
                state->cards[(u16)state->motion.selected_card_id].flags |= 0x80;
                return;
            }
        } else if (state->motion.card_view_slide_phase == 0) {
            DisplayObject_ReleaseIfPresent((void *)state->card_view);
            state->card_view = 0;
            TextBox_Destroy(&gText_aChannels[0]);
            state->motion.card_view_phase = 8;
            return;
        }
        break;
    case 8:
        state->motion.state = 1;
        break;
    }
}

#undef B
#undef H
#undef S
#undef W

void Library_EnterGridState(LibraryState *state)
{
    state->motion.state = 1;
}

void Library_NoOpScreenState3(u8 *value)
{
}

void Library_UpdateScreen(void)
{
    LibraryState *state;
    ViewState *model;
    s32 one;
    s32 r;
    s32 a;
    s32 step;
    s32 cur;
    s32 target;
    s32 count;
    s32 mode;
    GsRVIEW2 *dst;
    ViewState *p;

    state = &gLibrary_State;
    one = 1;
    mode = gLibrary_State.motion.state;
    mode &= 0xF;
    switch (mode) {
    case 0:
        Library_EnterGridState(state);
        break;
    case 1:
        Library_UpdateGridCursor(state);
        break;
    case 2:
        Library_UpdateCardView(state);
        model = &gViewState_Current;
        r = Model_GetSlotReadiness(0);
        if (r == one) {
            if (Model_GetSlotAnimationIndex(0) != r) {
                Model_CopySlotPosition(0, (u16 *)0x80181010);
                r = gLibrary_sSampledReferenceY - gLibrary_sCapturedReferenceY;
                a = r >= 0 ? r : -r;
                step = a / 24;
                step += 1;
                cur = model->view.vry;
                if (gLibrary_sSampledReferenceY < cur) {
                    model->view.vry = cur - step;
                    cur = *(volatile long *)&model->view.vry;
                }
                if (cur < gLibrary_sSampledReferenceY) {
                    model->view.vry = cur + step;
                }
            } else {
                r = model->view.vry;
                target = gLibrary_sCapturedReferenceY;
                if (r != target) {
                    if (target < r) {
                        r -= 8;
                        if (r < target) {
                            r = target;
                        }
                    } else {
                        r += 8;
                        if (target < r) {
                            r = target;
                        }
                    }
                    model->view.vry = r;
                }
                count = state->motion.animation_countdown - 1;
                state->motion.animation_countdown = count;
                if (count <= 0) {
                    state->motion.animation_countdown = 0x12C;
                    r = 2;
                    if (rand() & 1) {
                        r = 7;
                    }
                    Model_ControlSlotAnimation(0, r, 1);
                }
            }
        }
        p = &gViewState_Current;
        p->angle += 0xC;
        ViewState_ApplyOrbit();
        dst = Model_GetCameraViewBuffer();
        ((LibraryViewQuad *)dst)[0] = *(LibraryViewQuad *)&model->view.vpx;
        ((LibraryViewQuad *)dst)[1] = *(LibraryViewQuad *)&model->view.vry;
        Model_UpdateViewMetrics(0);
        break;
    case 3:
        Library_NoOpScreenState3((u8 *)state);
        break;
    }
}

/* The latter three Library functions in address order: the package-transfer
   callback, the pass that marks every owned card in the screen's state, and
   Library_InitializeScreen, which sets the screen up, installs that callback and hands
   control to the dispatcher above.

   The owned-card pass was recorded at gcc_2_8_1_g0_split. It compiles to an
   identical object at gcc_2_8_1_g8_split, which the entry needs, so the
   unit builds there. */

#ifdef VERSION_EUROPE
void Library_LoadPackageStage(FileTransferDescriptor *object, s32 mode)
{
    /* The European build loads one more stage, case 2: a 64x16 image picked
       by the language (gText_bLanguageIndex) into (896, 352). The US cases 2-6 follow
       as 3-7, case 1 transfers five sectors rather than four, and the image
       rows sit at x 0x280 (y 0xE0 and 0xE6). */
    switch (mode) {
    case 0:
        object->field_30.h.counter = 0x300;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        gFile_dwTransferState |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 64 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = 5 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 2:
        gGraphics_aImageTransferRects[0].x = 0x380;
        gGraphics_aImageTransferRects[0].y = 0x160;
        gGraphics_aImageTransferRects[0].w = 0x40;
        gGraphics_aImageTransferRects[0].h = 0x10;
        LoadImage2(&gGraphics_aImageTransferRects[0],
                   (u32 *)(gFile_TransferBufferBase + gText_bLanguageIndex * FILE_SECTOR_SIZE));
        object->phase_size = 4 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 3:
        gGraphics_aImageTransferRects[0].x = 0x280;
        gGraphics_aImageTransferRects[0].y = 0xE0;
        gGraphics_aImageTransferRects[0].w = 0x100;
        gGraphics_aImageTransferRects[0].h = 0x10;
        LoadImage2(&gGraphics_aImageTransferRects[0], (u32 *)gFile_TransferBufferBase);
        object->field_30.h.counter = 0x240;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        gFile_dwTransferState |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 48 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 5:
        gGraphics_aImageTransferRects[0].x = 0x280;
        gGraphics_aImageTransferRects[0].y = 0xE6;
        gGraphics_aImageTransferRects[0].w = 0x100;
        gGraphics_aImageTransferRects[0].h = 2;
        LoadImage2(&gGraphics_aImageTransferRects[0], (u32 *)gFile_TransferBufferBase);
        object->transfer_mode = 3;
        object->field_30.word = 0x26810;
        object->phase_size = 20 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 4:
    case 6:
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 7:
        SD_LoadSequenceBankPair(1, (u32 *)gFile_TransferBufferBase);
        break;
    }
}
#else
void Library_LoadPackageStage(FileTransferDescriptor *object, s32 mode)
{
    switch (mode) {
    case 0:
        object->field_30.h.counter = 0x300;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        gFile_dwTransferState |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 64 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = 4 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 2:
        gGraphics_aImageTransferRects[0].x = 0x100;
        gGraphics_aImageTransferRects[0].y = 0xF0;
        gGraphics_aImageTransferRects[0].w = 0x100;
        gGraphics_aImageTransferRects[0].h = 0x10;
        LoadImage2(&gGraphics_aImageTransferRects[0], (u32 *)gFile_TransferBufferBase);
        object->field_30.h.counter = 0x240;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        gFile_dwTransferState |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 48 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 4:
        gGraphics_aImageTransferRects[0].x = 0x100;
        gGraphics_aImageTransferRects[0].y = 0xF6;
        gGraphics_aImageTransferRects[0].w = 0x100;
        gGraphics_aImageTransferRects[0].h = 2;
        LoadImage2(&gGraphics_aImageTransferRects[0], (u32 *)gFile_TransferBufferBase);
        object->transfer_mode = 3;
        object->field_30.word = 0x26810;
        object->phase_size = 20 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 3:
    case 5:
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 6:
        SD_LoadSequenceBankPair(1, (u32 *)gFile_TransferBufferBase);
        break;
    }
}
#endif

void Library_MarkOwnedCards(void)
{
    u8 *p = gLibrary_abCardChest;
    s32 i = 0;
    u16 *q;
    do {
        if (*p != 0)
            Library_UpdateCardUsedFlag(i + (CAMPAIGN_FLAG_LIBRARY_CARD_BASE + CARD_ID_FIRST));
        i++;
        p++;
    } while (i < CARD_COUNT);
    q = gDuel_awPlayerDeck;
    i = 0;
    do {
        if (*q != 0)
            Library_UpdateCardUsedFlag(*q + CAMPAIGN_FLAG_LIBRARY_CARD_BASE);
        i++;
        q++;
    } while (i < DECK_SIZE);
}

/* The Library package's first sector and length; the Japanese and European
   WA.MRG place it elsewhere, so a regional build supplies its own. */
#ifndef LIBRARY_PACKAGE_FIRST_SECTOR
#define LIBRARY_PACKAGE_FIRST_SECTOR 0x1DCD
#endif
#ifndef LIBRARY_PACKAGE_SECTOR_COUNT
#define LIBRARY_PACKAGE_SECTOR_COUNT 0x8A
#endif
/* The resource record's clut_x (0x200 in the US build, 0x280 in the
   European one) and the y of the sprite at x 0x10 (0xD8, 0xE8). */
#ifndef LIBRARY_RESOURCE_CLUT_X
#define LIBRARY_RESOURCE_CLUT_X 0x200
#endif
#ifndef LIBRARY_SELECTOR_SPRITE_Y
#define LIBRARY_SELECTOR_SPRITE_Y 0xD8
#endif

void Library_InitializeScreen(void) {
    s16 *q;
    DuelEffectResourceRecord *b;
    LibraryState *r;
    DisplayObject *o;
    u8 *e;
    DuelEffectChannel *m;
    s32 n;
    s32 x;
    s32 y;
    s32 v;
    s32 c;
    s32 d;
    s32 k;
    s32 off;
    /* Retain the retail index-controlled palette loop and register frame. */
    s32 rb;

    gGraphics_sViewportY = 0;
    gGraphics_sViewportX = 0;
    Model_SetFogDrawFlags(0);
    DuelEffect_ClearResourceObjectPointers(0);
    n = CARD_COUNT - 1;
    q = (s16 *)0x801805A2;
    b = &gCardPreview_Resources[0];
    b->src_x = 0x100;
    b->src_y = 0x100;
    b->clut_x = LIBRARY_RESOURCE_CLUT_X;
    b->clut_y = 0xF0;
    do {
        *q = n + 1;
        n--;
        q--;
    } while (n >= 0);
    File_RequestAsyncTransfer(0, (u8 *)0, LIBRARY_PACKAGE_FIRST_SECTOR,
                              LIBRARY_PACKAGE_SECTOR_COUNT,
                              (FileTransferCallback)Library_LoadPackageStage, 0, 0);
    File_WaitForTransfers();
    Library_MarkOwnedCards();
    gMain_apFrameServiceCallbacks[3] = Library_DrawCardGrid;
    Library_CreateGridSectionSprites();
    r = &gLibrary_State;
    gLibrary_State.motion.state = 0;
    c = gCardGrid_bCursorColumn;
    d = c;
    if (c >= CARD_GRID_SECTION_SIDE_LENGTH) {
        x = (c % CARD_GRID_SECTION_SIDE_LENGTH) * 0xE + 0xAE;
    } else {
        x = (d % CARD_GRID_SECTION_SIDE_LENGTH) * 0xE + 0xE;
    }
    r->motion.rest_x = x;
    r->motion.x = x;
    k = gCardGrid_bCursorRow;
    y = (k / CARD_GRID_SECTION_SIDE_LENGTH) * 0xB2
        + (k % CARD_GRID_SECTION_SIDE_LENGTH) * 0x10 + 0xE;
    r->motion.rest_y = y;
    r->motion.y = y;
    Library_UpdateGridScroll(&r->motion);
    o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(o, 0x10, LIBRARY_SELECTOR_SPRITE_Y,
                                            0, 2, 1, 0x1B, 0x127);
    ((u8 *)&o->resource_uv)[1] = 0x80;
    DisplayObject_SelectOrderingTable1(o);
    n = CARD_ID_FIRST;
    o->flags = o->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    r->selector_sprite = o;
    o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        o, r->motion.x, r->motion.y,
        0, 2, 2, 0x1B, 0x147);
    rb = (s32)r;
    ((u8 *)&o->resource_uv)[1] = 0x80;
    r->motion.display_objects.by_role.render = o;
    r->cards[0].flags = 0;
    r->cards[0].clut_x = 0;
    do {
        ((LibraryState *)rb)->cards[n].flags = 0;
        r->cards[0].clut_x = 0;
        off = n * 4 - 4;
        v = (*(s32 *)((u8 *)gDuel_adwCardStats + off) >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
        switch (v) {
        case CARD_TYPE_MAGIC:
        case CARD_TYPE_EQUIP:
            ((LibraryState *)rb)->cards[n].clut_x = LIBRARY_CARD_SELECTOR_MAGIC_EQUIP;
            break;
        case CARD_TYPE_TRAP:
            ((LibraryState *)rb)->cards[n].clut_x = LIBRARY_CARD_SELECTOR_TRAP;
            break;
        case CARD_TYPE_RITUAL:
            ((LibraryState *)rb)->cards[n].clut_x = LIBRARY_CARD_SELECTOR_RITUAL;
            break;
        default:
            ((LibraryState *)rb)->cards[n].clut_x = LIBRARY_CARD_SELECTOR_DEFAULT;
            break;
        }
        n++;
    } while (n < CARD_ID_END);
    gText_StagedValues[0].library_count = 0;
    n = CARD_ID_FIRST;
    do {
        if (Campaign_TestStoryFlag(n + CAMPAIGN_FLAG_LIBRARY_CARD_BASE) != 0) {
            gText_StagedValues[0].library_count += 1;
            r->cards[n].flags = LIBRARY_CARD_FLAG_KNOWN;
            if (Library_CheckCardOwned(n) < 0) {
                r->cards[n].flags |= LIBRARY_CARD_FLAG_UNOWNED;
            }
        }
        n++;
    } while (n < CARD_ID_END);
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    /* The Japanese and European builds make neither this call nor the two
       field stores below. */
    DuelEffect_SetEntryRangeLayout(3, 1);
#endif
#ifdef VERSION_EUROPE
    /* The European build creates the box with flags 3. */
    m = TextBox_CreateFlagged(3, 0xF8, 0x58, -0x18, 0x90, 0x10, 3);
#else
    m = TextBox_Create(3, 0xF8, 0x58, -0x18, 0x90, 0x10);
#endif
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    m->glyph_advance_x = 0x10;
    m->line_advance_y = 0x10;
#endif
    TextBox_BuildBlocking(m);
    DisplayObject_SelectOrderingTable3(m->render_object);
    m->render_object->flags &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    Library_RefreshSelectedCardText((const LibraryState *)r);
    SD_BGMPlay(0x72D0);
}
