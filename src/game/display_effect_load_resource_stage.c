#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "../unmatched.h"
#include "file_transfer_buffer.h"
#include "display_effect_load_resource_stage.h"

/* Regional value: the load descriptor's x (0x200 -> 0x280). The European
 * build (src/game/european/) defines its own. */
#ifndef DISPLAY_EFFECT_RESOURCE_LOAD_X
#define DISPLAY_EFFECT_RESOURCE_LOAD_X 0x200
#endif

#define FILE_TRANSFER_PHASE_SIZE_WORD(descriptor) \
    (*(s32 *)&(descriptor)->phase_size)

void DisplayEffect_LoadResourceStage(FileTransferDescriptor *p, s32 mode)
{
    s32 v_0;
    s32 v_1;
    s32 v_2;
    s32 w;
    s32 n;
    s32 b;
    s32 t;
    s32 c;
    s32 k;
    s32 m;
    s32 m2;

    switch (mode) {
    case 0:
        m = 0xFFDDFFFF;
        v_0 = gFile_dwTransferState;
        n = p->position;
        p->field_30.h.field_32 = 0x100;
        p->w = 0x40;
        p->phase_size = 48 * FILE_SECTOR_SIZE;
        p->h = 0x10;
        v_0 &= m;
        gFile_dwTransferState = v_0;
        w = gFile_dwTransferState;
        w |= 0x10000;
        gFile_dwTransferState = w;
        p->transfer_mode = 2;
        b = gFile_TransferBufferBase;
        p->field_30.h.counter = 0x340 - n * 192;
        p->sector_destination = b;
        p->alternate_sector_destination = b + FILE_SECTOR_SIZE;
        break;

    case 1:
        m2 = 0xFFDCFFFF;
        FILE_TRANSFER_PHASE_SIZE_WORD(p) = FILE_SECTOR_SIZE;
        v_1 = gFile_dwTransferState;
        t = gFile_TransferBufferBase;
        v_1 &= m2;
        gFile_dwTransferState = v_1;
        p->alternate_sector_destination = t;
        p->sector_destination = t;
        p->transfer_mode = 1;
        break;

    case 2:
        p->x = DISPLAY_EFFECT_RESOURCE_LOAD_X;
        k = p->position;
        p->h = 2;
        c = gFile_TransferBufferBase;
        p->w = 0x100;
        p->y = k * 2 + 0xF0;
        LoadImage2((RECT *)p, (u32 *)c);
        m2 = 0xFFDCFFFF;
        FILE_TRANSFER_PHASE_SIZE_WORD(p) = FILE_SECTOR_SIZE;
        v_2 = gFile_dwTransferState;
        t = (s32)p->callback_data;
        v_2 &= m2;
        gFile_dwTransferState = v_2;
        p->alternate_sector_destination = t;
        p->sector_destination = t;
        p->transfer_mode = 1;
        break;
    }
}
