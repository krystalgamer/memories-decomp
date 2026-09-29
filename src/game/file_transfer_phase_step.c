#include "../types.h"
#include "file_transfer.h"

/* Counts the current phase down one sector and fires its callback at zero. */
void func_8001513C(FileTransferDescriptor *object)
{
    object->phase_remaining -= FILE_SECTOR_SIZE;
    if (object->phase_remaining <= 0) {
        object->phase_size = 0;
        if (object->phase_callback != 0) {
            s32 count = object->result++;

            CALL32(FileTransferCallback, object->phase_callback)(object, count);
        }
        object->phase_remaining = object->phase_size;
    }
}
