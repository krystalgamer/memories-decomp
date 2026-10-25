#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "duel_effect_resource_record.h"
#include "duel_effect_resource_setup.h"
#define D_8009B118_IS_POINTER_IN_DATA
#include "../unmatched.h"
#include "file_transfer_buffer.h"

#define VRAM_UPLOAD_WORDS(address) ((u32 *)(address))

/* Regional values: the package size and the offset of its last image. The
   European package holds one 0x2A0-byte third image per text language
   (gText_bLanguageIndex, below), so the fourth image moves from 0x2AE0 to 0x3560.
   The European build (src/game/european/) defines its own. */
#ifndef FUNC_800289BC_PHASE_SIZE
#define FUNC_800289BC_PHASE_SIZE 0x3800
#endif
#ifndef FUNC_800289BC_FOURTH_IMAGE_OFFSET
#define FUNC_800289BC_FOURTH_IMAGE_OFFSET 0x2AE0
#endif

void CardPreview_HandleResourceTransfer(FileTransferDescriptor *object, s32 mode)
{
    DuelEffectResourceRecord *e;
    RECT *rect;
    u8 *image_source;
    s32 x;
#ifdef VERSION_EUROPE
    s32 offset;
#endif

    if (mode == 0) {
        object->phase_size = FUNC_800289BC_PHASE_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = (u32)gFile_TransferBufferBase;
        object->sector_destination = (u32)gFile_TransferBufferBase;
        object->transfer_mode = 1;
        return;
    }

    e = &gCardPreview_Resources[(s32)object->callback_data];

    rect = &e->rects[0];
    image_source = gFile_TransferBufferBase;
    e->rects[0].w = 0x33;
    e->rects[0].h = 0x60;
    e->rects[0].x = e->src_x;
    e->rects[0].y = e->src_y;
    LoadImage(rect, VRAM_UPLOAD_WORDS(image_source));

    rect = &e->rects[1];
    x = e->field_2C;
    image_source = gFile_TransferBufferBase + 0x2640;
    e->rects[1].w = 0x100;
    e->rects[1].h = 1;
    e->rects[1].x = x;
    e->rects[1].y = e->field_2E;
    LoadImage(rect, VRAM_UPLOAD_WORDS(image_source));

    rect = &e->rects[2];
#ifdef VERSION_EUROPE
    /* The third image of the current text language. */
    offset = gText_bLanguageIndex * 0x2A0 + 0x2840;
    image_source = gFile_TransferBufferBase + offset;
    e->rects[2].w = 0x18;
    e->rects[2].h = 0xE;
    e->rects[2].x = e->src_x;
#else
    x = e->src_x;
    image_source = gFile_TransferBufferBase + 0x2840;
    e->rects[2].w = 0x18;
    e->rects[2].h = 0xE;
    e->rects[2].x = x;
#endif
    e->rects[2].y = e->src_y + 0x60;
    LoadImage(rect, VRAM_UPLOAD_WORDS(image_source));

    rect = &e->rects[3];
    image_source = gFile_TransferBufferBase + FUNC_800289BC_FOURTH_IMAGE_OFFSET;
    e->rects[3].w = 8;
    e->rects[3].h = 0x58;
    e->rects[3].x = e->src_x + 0x38;
    e->rects[3].y = e->src_y;
    LoadImage(rect, VRAM_UPLOAD_WORDS(image_source));
}
