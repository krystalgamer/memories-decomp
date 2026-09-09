#include "../types.h"
#include "../psyq/libsn.h"
#include "func_80059908.h"

#define FILE_READ_CHUNK_SIZE 0x1000

s32 func_80059908(s32 path, s32 destination, s32 offset, s32 length)
{
    s32 handle;
    s32 total;
    s32 remaining;
    s32 chunk;

    if (path == 0)
        return 0;
    handle = PCopen((char *)path, 0, 0);
    if (handle < 0)
        return 0;
    if (length == 0)
        length = PClseek(handle, 0, 2) - offset;
    total = length;
    remaining = total;
    PClseek(handle, offset, 0);
    if (total > 0) {
        do {
            chunk = FILE_READ_CHUNK_SIZE;
            if (remaining <= FILE_READ_CHUNK_SIZE)
                chunk = remaining;
            if (PCread(handle, (char *)destination, chunk) != chunk) {
                total = 0;
                break;
            }
            remaining -= chunk;
            destination += chunk;
        } while (remaining > 0);
    }
    PCclose(handle);
    return total;
}
