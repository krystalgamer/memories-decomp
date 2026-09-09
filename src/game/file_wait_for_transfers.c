#include "../types.h"
#include "main_frame.h"
#include "file_transfer.h"

void File_WaitForTransfers(void) {
    for (;;) {
        if (((D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             D_8009B134) == 0) {
            break;
        }
        if ((D_8009B0F4 & FILE_TRANSFER_STATE_SECONDARY_PENDING) == 0) {
            func_80015038();
        }
        func_80012D4C();
    }

    while (D_8009B134 != 0) {
        func_80012D4C();
    }
}
