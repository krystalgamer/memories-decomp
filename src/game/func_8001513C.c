#include "../types.h"
#include "file_transfer.h"

void func_8001513C(FileTransferDescriptor *object)
{
    object->phase_remaining -= 0x800;
    if (object->phase_remaining <= 0) {
        object->mode = 0;
        if (object->phase_callback != 0) {
            s32 count = object->result++;

            object->phase_callback(object, count);
        }
        object->phase_remaining = object->mode;
    }
}
