#ifndef MEMORIES_DECOMP_CARD_PREVIEW_H
#define MEMORIES_DECOMP_CARD_PREVIEW_H
#include "../types.h"
#include "display_object.h"
#include "file_transfer.h"

/* Draws the paired preview's frame, image, stat rows and card-detail tiles. */
void CardPreview_DrawDetailPanel(DisplayObject *object, s32 ordering_table);
/* Uses the stored variant for nonnegative reference-triangle facing area,
 * otherwise variant 2, then redraws and requests the display-list pass again. */
void CardPreview_UpdateVariant(DisplayObject *source, s32 facing_area);

struct DisplayObject;

/* Builds the paired objects in one effect-resource slot and returns its
 * primary object, allocated second. Default-path attack/defense overrides
 * are narrowed to s16 before testing: a negative low halfword selects the
 * corresponding packed base stat. Special card types keep their preset
 * setup. Callers retain their explicit partial-view casts. */
struct DisplayObject *CardPreview_CreateResourceObjects(s32 index, s32 attack, s32 defense);

/* Releases both display-object words of one record through DisplayObject_ReleaseIfPresent and
 * zeroes them. */
void DuelEffect_ReleaseResourceObjects(s32 index);

/* Clears both stored object pointers in one gCardPreview_Resources resource slot without
 * releasing them. Takes the slot index, not a record pointer. */
void DuelEffect_ClearResourceObjectPointers(s32 index);

/* Starts the async read of one card's effect artwork into slot `slot` of the
 * gCardPreview_Resources record array. The low halfword of `card_id` is stored at +0x30;
 * the original full argument supplies the build's card-art disc position.
 * US/Japan use seven sectors per card; Europe uses eight.
 *
 * It hands back the transfer descriptor it started, with `slot` already
 * stored in the descriptor's callback_data for CardPreview_HandleResourceTransfer to pick up, and
 * it has published the descriptor's status through gFile_dwTransferState with
 * FILE_TRANSFER_STATE_PRIMARY_ACTIVE set. Every caller ignores the value,
 * which is why they can: the descriptor is reachable without it. The password
 * overlay's shop.c calls it too. */
FileTransferDescriptor *CardPreview_RequestResourceTransfer(s32 slot, s32 card_id);

/* Async phase callback for CardPreview_RequestResourceTransfer's card-art request. Phase 0
 * points both buffer words at gFile_TransferBufferBase for 0x3800 bytes; the next phase
 * uploads the four rects of the gCardPreview_Resources entry whose index CardPreview_RequestResourceTransfer
 * left in callback_data. */
void CardPreview_HandleResourceTransfer(FileTransferDescriptor *descriptor, s32 mode);

/* PAL language selection shared by text, artwork and localized resources. */
extern u8 gText_bLanguageIndex;

/* The value gText_bLanguageIndex takes at boot and wraps back to in the debug menu:
   the European release starts at 0 and asks, the single-language releases
   fix their own index. */
#ifndef BUILD_LANGUAGE_INDEX
#define BUILD_LANGUAGE_INDEX 0
#endif

#endif
