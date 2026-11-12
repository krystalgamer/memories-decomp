#ifndef MEMORIES_DECOMP_LIBRARY_H
#define MEMORIES_DECOMP_LIBRARY_H
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "library_state.h"
#include "file_transfer.h"
#include "../ygo_types.h"

#define CARD_GRID_SECTION_SIDE_LENGTH 10
#define CARD_GRID_SECTIONS_PER_ROW 2
#define CARD_GRID_SECTION_CARD_COUNT \
    (CARD_GRID_SECTION_SIDE_LENGTH * CARD_GRID_SECTION_SIDE_LENGTH)
#define CARD_GRID_SECTION_ROW_CARD_COUNT \
    (CARD_GRID_SECTION_CARD_COUNT * CARD_GRID_SECTIONS_PER_ROW)

/* The card grid's cursor position.
 *
 * Library_UpdateGridCursor navigates the grid and reads both into s32 locals;
 * Library_InitializeScreen places the cursor sprite within a section from them, deriving
 * x from the column remainder and y from the row quotient/remainder. The
 * cursor spans two side-by-side 10x10 sections per section row.
 *
 * Both must stay small-data eligible, and that is not a style question here:
 * Library_UpdateGridCursor's retail loads are gp-relative. A plain s8 scalar
 * is eligible under its uniform -G8 profile; an oversized array or a .data
 * attribute would change that address formation.
 *
 * The s8 spelling is measured rather than preferred. #3054 established that
 * what this code requires is a signed read: declaring them u8 and dropping
 * the casts builds an executable sixteen bytes short, while u8 with an
 * explicit `(s8)` cast, s8 with the cast, and s8 without it all produce the
 * same instructions. Both sources now take the last of those.
 */
extern s8 gCardGrid_bCursorColumn;
extern s8 gCardGrid_bCursorRow;

/* Returns the flags of the directly indexed card record, including ID zero. */
u32 Library_GetCardFlags(const LibraryState *base, s32 index);

/* Draws the Library card-list grid and its cursor box. Never called from C:
 * Library_InitializeScreen (library_runtime.c) and Library_UpdateCardView install its address in
 * gMain_apFrameServiceCallbacks[3], the per-frame draw callback slot. */
void Library_DrawCardGrid(void);

s32 Library_UpdateCursorMotion(void);
void Library_StartCursorMotion(s32 column, s32 row, s32 frames);

/* Fade/projection callback installed by Library_CreateFadingCardOutline in
 * the same (DisplayObject *, GsOT *) draw-callback slot BuildDeck_DrawCardListPage's
 * header describes, and hands the table straight to Graphics_SubmitDrawModePacket. */
void Library_DrawFadingCardOutline(DisplayObject *object, GsOT *ordering_table);

/* Creates one fading outline snapshot using selected source transform and
 * dimension words. Colour starts at zero; the draw callback supplies its blue
 * fade. The returned pool object uses the original narrowed depth delta. */
DisplayObject *Library_CreateFadingCardOutline(DisplayObject *src, int add);

/* Returns the card id under the current Library grid cursor. The state pointer
 * is part of the caller ABI but is not read by the calculation. */
s32 Library_GetGridCursorCardId(const LibraryState *state);

/* Updates Library grid selection, movement, cancellation and scroll state. */
void Library_UpdateGridCursor(LibraryState *state);

/* Refreshes text-box slot 1 from the cursor-selected card. Clearing the
 * published selection for an unknown card does not clear the staged ID or
 * selector 5. The existing color_index_54 byte view, unowned override and blocking
 * channel build remain unchanged. The motion-completion caller passes its
 * enclosing LibraryState through the first-member address. */
void Library_RefreshSelectedCardText(const LibraryState *state);

typedef struct {
    s32 values[4];
} LibraryViewQuad;

typedef char LibraryViewQuad_size_must_be_0x10[
    sizeof(LibraryViewQuad) == 0x10 ? 1 : -1
];

/* Library screen state 2, the card view: opens the view, slides the model
 * and text box in, rotates the model light with the view angle and walks the
 * close sequence back to the grid. Library_UpdateScreen passes the owned LibraryState;
 * internal byte/scalar views retain the retail access and scheduling shape. */
void Library_UpdateCardView(LibraryState *state);

/* Selects grid state 1 with a whole-byte write, not grid setup or a reset. */
void Library_EnterGridState(LibraryState *state);
/* The dispatcher's unchanged empty state-3 handler. */
void Library_NoOpScreenState3(u8 *state);

void Library_UpdateScreen(void);

/* Package-transfer phase callback installed by Library_InitializeScreen before it
 * initializes the Library screen state and hands control to Library_UpdateScreen. */
void Library_LoadPackageStage(FileTransferDescriptor *object, s32 mode);

/* Walks the save's card table and marks every owned card in the Library
 * screen's own state, which is what makes the grid draw them as obtained. */
void Library_MarkOwnedCards(void);

/* The Library screen's entry: sets the screen up, installs Library_LoadPackageStage as
 * the package-transfer phase callback and hands control to Library_UpdateScreen. */
void Library_InitializeScreen(void);

/* Keeps the vertical scroll following a record's y, and pins the horizontal
 * scroll to zero.
 *
 * It reads one halfword, at +0xA, three times: once signed to get the
 * distance from the current gGraphics_sViewportY, then twice unsigned to
 * rebuild the viewport when that distance leaves the 0x40..0xB0 band. The
 * band is a dead zone, so the viewport only moves when the record has left
 * it, and it is then placed 0x40 above or 0xB0 below the record.
 *
 * The record is the library motion state. Library_InitializeScreen passes gLibrary_State and
 * writes that same halfword itself on the line before the call --
 * `((LibraryMotionState *)r)->y = y` -- so the offset is that record's y,
 * which ygo_types.h's LibraryMotionState names at exactly 0xA.
 * Library_UpdateGridCursor is the second caller and passes &state->motion
 * after handling cursor movement.
 *
 * Both callers supply the asserted LibraryMotionState prefix. The record
 * is read-only; its signed first read and unsigned follow-up reads remain
 * distinct around the viewport store. */
void Library_UpdateGridScroll(const LibraryMotionState *record);
#endif
