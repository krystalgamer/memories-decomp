#include "../types.h"
#include "file_cd_helpers.h"
#include "file_constants.h"
#include "func_8005B8A0.h"
#include "movie_stream_requests.h"
#include "movie_stream_ranges.h"

extern u8 D_800F5750[];

extern s32 CdPosToInt_8007E710();

s32 func_8005C464(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
{
    u8 *p = D_800F5750;
    if (File_Exists(arg0, (s32)p) != 0)
        return -1;
    return func_8005B8A0(
        p, arg1, arg2,
        CdPosToInt_8007E710((s32)p) +
            ((u32)(*(s32 *)(p + 4) + (FILE_SECTOR_SIZE - 1)) >>
             FILE_SECTOR_SHIFT),
        arg3, arg4);
}
