#include "../types.h"
#include "main_frame.h"
#include "file_transfer.h"
#include "file_transfer_state.h"

void File_WaitForTransfers(void) {
    for (;;) {
        if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState) == 0) {
            break;
        }
        if ((gFile_dwTransferState & FILE_TRANSFER_STATE_SECONDARY_PENDING) == 0) {
            File_StopActiveRangeTransfer();
        }
        Main_AdvanceFrame();
    }

    while (gFile_dwCancelRequestState != 0) {
        Main_AdvanceFrame();
    }
}
