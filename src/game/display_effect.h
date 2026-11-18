#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_H

#include "../types.h"
#include "menu_record.h"
#include "../ygo_types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "display_object.h"

/* The lifecycle helpers take the MenuRecord they arm, reset and tear down:
 * the records they are handed are elements of gDisplayEffect_aMenuRecords
 * (script_scene.c, duel_effect_text.c), and gDisplayEffect_apfnSteps calls
 * DisplayEffect_FinishStepWhenIdle with the same MenuRecord *. */

/* Zero on the first mark; one when bit 0x80 was already present. */
s32 DisplayEffect_MarkInitialized(MenuRecord *record);
/* Partial state/position/delay reset, with one RNG call on every invocation. */
void DisplayEffect_ResetRecordState(MenuRecord *record);
/* Exactly three addressable slots, released and nulled from index 2 to 0. */
void DisplayEffect_ReleaseObjectRow(struct DisplayObject **objects);
/* Deactivate the fixed record and clear only its three primary object slots.
 * This does not free arena storage, clear the step or reset shared selectors. */
void DisplayEffect_ReleaseRecord(MenuRecord *record);

/* gText_DisplayEffectRecord moved to menu_record.h with the MenuRecord * type it now
 * carries; it points into gDisplayEffect_aMenuRecords, which is declared there too. */
void DisplayEffect_FinishStepWhenIdle(MenuRecord *record);

/* The file-transfer phase callback DisplayEffect_UpdateResourceAcquisition installs for a duel effect's
 * VRAM upload. `mode` selects the phase; the descriptor it is handed is the
 * one File_TryRequestAsyncTransfer returned, and every offset the function
 * touches is a named FileTransferDescriptor member.
 *
 * Phase 2 programs the descriptor's leading RECT with the clut rectangle --
 * x 512, y position * 2 + 240, w 256, h 2 -- and hands it straight to
 * LoadImage2. DisplayEffect_UpdateResourceAcquisition writes exactly those four values into its own
 * slot->clut_rect when it does the upload inline instead. Phase 0 likewise
 * puts 832 - position * 192 in the 0x30 halfword pair, which is the
 * slot->image_rect.x that function computes, and matches what
 * FileTransferDescriptor's own comment calls the VRAM position pair the
 * phase-2 paths write there.
 *
 * The two phase_size stores in phases 1 and 2 go through a local
 * `u32 *phase_size = &p->phase_size` deliberately. Written as the plain
 * member assignment,
 * GCC cross-jumps the two phases' tails into one shared
 * block and the executable comes out eight bytes short; retail keeps them
 * duplicated. Only that one statement does it -- `transfer_mode`, `sector_destination` and
 * `alternate_sector_destination` all convert plainly in the same tails, and phase 0's
 * `p->phase_size = 48 * FILE_SECTOR_SIZE;` is fine because the larger
 * constant needs its own
 * lui/ori either way. */
void DisplayEffect_LoadResourceStage(
    FileTransferDescriptor *descriptor, s32 mode);

/* Builds up to three display objects and writes them back through `out` as
 * out[0], out[1] and out[2], storing NULL in the slots it does not fill. That
 * is why the second parameter is DisplayObject **: callers pass one
 * three-object row of the MenuRecord grid. DisplayEffect_CrossfadeResources passes rows 0 through 3, and each call fills its
 * own three-slot group. Returns 1 when it built anything and 0 when
 * DisplayEffect_HasResourceEntry rejected the first index; every caller
 * discards that result.
 *
 * The first parameter is the record itself. menu_record.h already names the
 * three fields this reads -- slot_index selects which of the two buffer bases
 * and which layer constants to use, and x/y at 0x34/0x36 are the pair
 * that header documents as "the pair DisplayEffect_UpdatePositionTween and DisplayEffect_FadeInRecord hand to
 * the position helpers". They are u16 there and read signed here, so the two
 * reads keep an explicit (s16) at the use. */
s32 DisplayEffect_BuildResourceObjects(
    MenuRecord *record, DisplayObject **out, s32 resource_index);

/* Walks three levels of halfword offset tables rooted at `b`: entry `x` of the
 * root gives the offset of a second table, entry `y` of that gives a third,
 * and entry `z` of the third must be non-zero. Returns 1 when all three are
 * non-zero, 0 at the first zero. DisplayEffect_BuildResourceObjects asks it
 * about the table at gDisplay_abResourceBank0 or gDisplay_abResourceBank1. */
int DisplayEffect_HasResourceEntry(
    unsigned char *base, int group, int row, int entry);

/* Exactly three nullable slots, descending. Replaces blend bits, narrows
 * depth to s8 and adjusts CLUT y; zero retains GsALON, restores neutral RGB
 * and decrements CLUT y rather than selecting an opaque/idempotent mode. */
void DisplayEffect_SetObjectRowBlend(DisplayObject **objects, u32 attribute, s32 depth_offset);

#define DISPLAY_EFFECT_VRAM_SLOT_COUNT 5

typedef struct {
    /* Word buffers: LoadImage and StoreImage move them as u32 *. */
    u32 image[0x18000 / 4];
    u32 clut[0x400 / 4];
    u8 extra[0x800];
    RECT image_rect;
    RECT clut_rect;
} DisplayEffectVramSlot;

typedef char DisplayEffectVramSlot_size_must_be_0x18C10[
    sizeof(DisplayEffectVramSlot) == 0x18C10 ? 1 : -1
];

/* DISPLAY_EFFECT_VRAM_SLOT_COUNT bytes, one per DisplayEffectVramSlot of
 * the gRuntime_pSharedWorkBase buffer that DisplayEffect_UpdateResourceAcquisition walks: it starts slot at the last one, steps
 * i down to 0 and slot with it, takes i into gDisplayEffect_ResourceCacheWriteSlot when the entry is
 * negative, compares each entry with a->effect_id, stores a->effect_id at that
 * selected index and indexes the gRuntime_pSharedWorkBase slots with the same byte;
 * DisplayEffect_ResetMenuRecords stores -1 into all of them (menu_record_reset.c:28-35).
 * The declaration stays unsized: display_effect.c builds
 * with -G8, where a complete five-byte array is small data
 * and the link fails
 * (notes/research/matching-evidence.md:6684-6695). menu_record_reset.c used
 * to declare it `s8 [DISPLAY_EFFECT_VRAM_SLOT_COUNT]` while this consumer
 * used `s8 []`. */
extern s8 gDisplayEffect_ResourceCacheWriteSlot;
extern s8 gDisplayEffect_CachedResourceIds[];

/* Step two: restore a cache hit or request/capture a miss, then build row
 * zero on a later invocation. The write-slot byte is shared across calls.
 * Its bit 0x40 progress flag lives in display_effect_step. */
void DisplayEffect_UpdateResourceAcquisition(MenuRecord *record);

/* Step 4 rebuilds/crossfades old and new resource rows, with complementary
 * color and position updates; completion retains only the new primary row.
 * Step 5 is the separate portrait transition using only the first two
 * primary-row slots and its original keep-versus-release endpoints.
 * Both retain the MenuRecord callback ABI. */
void DisplayEffect_CrossfadeResources(MenuRecord *record);
void DisplayEffect_UpdatePortraitTransition(MenuRecord *record);

/* Incoming effect step 7: initialize the blend row and advance a stored
 * halfword ramp from zero by gGraphics_nBaseFrameStep * 8. Completion keeps the primary
 * row, restores its existing row attributes, releases grid[1] and clears
 * the activity-inhibit bit.
 *
 * Before canonical names, the table privately declared address 0x8003AAE4
 * as `void DisplayEffect_FadeInRecord(u8 *);` while the definition used MenuRecord *.
 * This owned header retains the definition's actual callback ABI. */
void DisplayEffect_FadeInRecord(MenuRecord *record);

/* Outgoing effect step 6: initialize a secondary blend row and ramp 0x80,
 * then subtract gGraphics_nBaseFrameStep * 8 in the same call. A nonpositive signed result
 * clears the step, releases grid[1] and then releases the record.
 *
 * Keep the MenuRecord callback type.
 * Primary objects do not survive this endpoint as they do for fade-in. */
void DisplayEffect_FadeOutRecord(MenuRecord *record);

/* gDisplayEffect_apfnSteps display effect step: the easing sweep. It runs the record's six
 * tail halfwords -- 0x40 and 0x42 the destination, 0x44 and 0x46 the distance
 * still to travel, 0x48 the eased angle and 0x4A its per-tick step -- and
 * hands the result to the position helpers as x and y.
 *
 * Every offset belongs to MenuRecord, the callback table's argument type. */
void DisplayEffect_UpdatePositionTween(MenuRecord *record);

/* Writes one coordinate pair into every non-null object of a three-slot
 * MenuRecord grid row, walking it from index 2 down to 0. Fewer than three
 * objects is normal rather than an error. */
void DisplayPositionGroup_SetChildPositions(
    struct DisplayObject **children,
    s16 x,
    s16 y
);

/* Sets the record's own x/y pair and then pushes it to its primary row. */
void DisplayPositionGroup_SetPosition(MenuRecord *record, s32 x, s32 y);

/* The callbacks sharing the implementation file retain their individually
 * measured argument views where old GCC's allocation depends on them. */

/* Ticks one menu record's display effect: variant_update_flags at 0x32,
 * object slots at 0x04 and 0x08, variant_countdown at 0x3E, and the
 * grid[0][2] requested_variant/variant_update_counter at 0x3A/0x3B.
 * The byte counter runs both down and up, not once per frame.
 *
 * This took u8 * because none of that was expressible. All of it is now:
 * 0x04 and 0x08 are grid[0][1] and grid[0][2] -- #3551 established that
 * grid[4][3] is four three-object rows -- and 0x3E is variant_countdown.
 * Both consumers hold the record as MenuRecord and no longer cast. */
void DisplayEffect_UpdateMenuRecord(MenuRecord *p, s32 n);

void DisplayEffect_ProcessMenuRecords(s32 value);

/* The display effect step table at 0x80090F68.
 *
 * Each callback receives the MenuRecord selected from gDisplayEffect_aMenuRecords. Left
 * unsized deliberately: its consumer indexes it with `& 0x1F`, which permits
 * thirty-two entries against the eight defined, so a bound here would
 * contradict the call site rather than describe it. */
extern void (*gDisplayEffect_apfnSteps[])(MenuRecord *);

#endif
