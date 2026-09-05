#include "../types.h"

extern u8 D_800F5750[];
extern s32 File_Exists();
extern s32 CdPosToInt_8007E710();
extern s32 func_8005B8A0();

s32 func_8005C464(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
{
    u8 *p = D_800F5750;
    if (File_Exists(arg0, p) != 0)
        return -1;
    return func_8005B8A0(
        p, arg1, arg2,
        CdPosToInt_8007E710((s32)p) + ((u32)(*(s32 *)(p + 4) + 0x7FF) >> 11),
        arg3, arg4);
}
