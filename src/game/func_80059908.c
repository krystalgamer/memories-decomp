#include "../types.h"

#define FILE_READ_CHUNK_SIZE 0x1000

extern s32 func_80073704();
extern s32 func_80073734();
extern s32 func_80073758();
extern void func_80073724();

s32 func_80059908(s32 path, s32 destination, s32 offset, s32 length)
{
    s32 handle;
    s32 total;
    s32 remaining;
    s32 chunk;

    if (path == 0)
        return 0;
    handle = func_80073704(path, 0, 0);
    if (handle < 0)
        return 0;
    if (length == 0)
        length = func_80073734(handle, 0, 2) - offset;
    total = length;
    remaining = total;
    func_80073734(handle, offset, 0);
    if (total > 0) {
        do {
            chunk = FILE_READ_CHUNK_SIZE;
            if (remaining <= FILE_READ_CHUNK_SIZE)
                chunk = remaining;
            if (func_80073758(handle, destination, chunk) != chunk) {
                total = 0;
                break;
            }
            remaining -= chunk;
            destination += chunk;
        } while (remaining > 0);
    }
    func_80073724(handle);
    return total;
}
