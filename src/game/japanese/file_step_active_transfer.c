#include "../../types.h"

#define VERSION_JAPAN
#define VERSION_JAPAN_FILE_STEP_ACTIVE_TRANSFER
#define D_8009B0E8 gJapanese_D_8009B0E8
#define D_8009B0F0 gJapanese_D_8009B0F0
#define D_8009B0F4 gJapanese_FileTransferFlags
#define D_8009B100 D_8009AFF0
#define D_8009B11D gJapanese_D_8009B11D
#define D_8009B120 gJapanese_D_8009B120
#define D_8009B124 gJapanese_D_8009B124
#define D_8009B128 D_8009B018
#define D_8009B130 gJapanese_D_8009B130
#define D_8009B134 gJapanese_FileSecondaryRequest
#define File_HandleSetFilterEvent Japanese_func_80014294

/* Same-file functions this one uses but that stay outside its guard. */
void File_HandleSetFilterEvent(u8 event);
void File_HandleRangeReadEvent(u8 event);
void File_HandleLogicalPositionEvent(u8 event, u8 *result);
void File_FinishActiveTransfer(void);

#include "../file_transfer_control.c"
