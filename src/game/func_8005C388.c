#include "../types.h"
#include "file_cd_helpers.h"
#include "func_8005B8A0.h"
#include "movie_stream_requests.h"
#include "movie_stream_ranges.h"

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
