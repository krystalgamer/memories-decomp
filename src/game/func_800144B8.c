#include "../types.h"
#include "file_transfer.h"

extern volatile int D_8009B134;extern void func_80015010(void);
void func_800144B8(void){D_8009B0F4&=0x60;if((D_8009B0F4&FILE_TRANSFER_STATE_SECONDARY_PENDING)&&!(D_8009B0F4&0x40)){File_ActivateTransfer();if(D_8009B134){int v=0x80;if((D_8009B0F4&FILE_TRANSFER_STATE_PRIMARY_ACTIVE)&&(D_8009B0F4&FILE_TRANSFER_FLAG_SECTOR_RANGE))func_80015010();D_8009B134=v;}}else D_8009B134=0;}
