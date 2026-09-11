#include "../types.h"
#include "../psyq/libds.h"
#include "file_cd_helpers.h"

s32 File_Exists(s32 first, s32 second)
{
    s32 result = (s32)DsSearchFile((DslFILE *)second, (char *)first);
    s16 failure = -1;

    if (result == 0 || result == failure)
        return failure;
    return 0;
}
