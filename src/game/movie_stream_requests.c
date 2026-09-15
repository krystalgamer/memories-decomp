#include "../types.h"
#include "file_cd_helpers.h"
#include "file_constants.h"
#include "func_8005B8A0.h"
#include "movie_stream_requests.h"
#include "movie_stream_ranges.h"
#include "../unmatched.h"

void func_8005C388(s32 index, s32 arg1, s32 end_frame, s32 arg3, s32 arg4)
{
    CdlLOC position;
    MovieStreamRange *base;
    MovieStreamRange *entry;
    s32 i;
    s32 total;

    base = gMovie_aStreamRanges;
    entry = &base[index];
    if (entry->end_frame != 0)
        end_frame = entry->end_frame;
    i = 0;
    total = 0;
    if (index > 0) {
        do {
            total += base[i].sector_count;
            i++;
        } while (i < index);
    }
    CdIntToPos_8007E600(D_800E9EB4 + total, &position);
    {
        MovieStreamRange *table;
        s32 value;

        value = CdPosToInt_8007E710(&position);
        table = gMovie_aStreamRanges;
        value += table[index].sector_count;
        func_8005B8A0((u8 *)&position, arg1, end_frame, value, arg3, arg4);
    }
}

s32 Movie_StartFileStream(s32 path, s32 start_frame, s32 end_frame, s32 arg3,
                          s32 arg4)
{
    CdlFILE *file = &gMovie_FileLookupResult;

    if (File_Exists(path, (s32)file) != 0)
        return -1;
    return func_8005B8A0(
        (u8 *)file, start_frame, end_frame,
        CdPosToInt_8007E710(&file->pos) +
            ((u32)(file->size + (FILE_SECTOR_SIZE - 1)) >>
             FILE_SECTOR_SHIFT),
        arg3, arg4);
}
