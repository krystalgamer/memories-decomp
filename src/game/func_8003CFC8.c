#include "../types.h"
#include "save_data.h"
extern u32 D_8009AF64,D_8009AF68;void func_8003CFC8(u8*p){register s32 i;register u32*dst;u16 v=SaveData_CalcCrc16(p+SAVE_DATA_TERTIARY_OFFSET,SAVE_DATA_TERTIARY_LENGTH);dst=(u32*)(p+SAVE_DATA_TERTIARY_MASK_LAST_OFFSET);i=SAVE_DATA_TERTIARY_MASK_WORD_COUNT;*(u16*)(p+SAVE_DATA_TERTIARY_CHECKSUM_OFFSET+sizeof(u16))=v;*(u16*)(p+SAVE_DATA_TERTIARY_CHECKSUM_OFFSET)=v;{u32 seed=v|(v<<16);D_8009AF68=seed;D_8009AF64=seed;}do{--i;*dst=SaveData_NextMaskWord();dst--;}while(i);}
