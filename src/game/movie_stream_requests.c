#include "../types.h"

typedef struct MovieStreamRange {
    u16 sector_count;
    u16 end_frame;
} MovieStreamRange;

extern MovieStreamRange gMovie_aStreamRanges[13];
extern s32 D_800E9EB4;
extern u8 D_800F5750[];

extern void CdIntToPos_8007E600(s32, void *, s32);
extern s32 CdPosToInt_8007E710();
extern s32 File_Exists();
extern s32 func_8005B8A0();

void func_8005C388(s32 index, s32 arg1, s32 end_frame, s32 arg3, s32 arg4)
{
    s32 buffer[2];
    MovieStreamRange *base;
    MovieStreamRange *entry;
    s32 i;
    s32 total;

    base = gMovie_aStreamRanges;
    entry = &base[index];
    if (entry->end_frame != 0) {
        end_frame = entry->end_frame;
    }
    i = 0;
    total = 0;
    if (index > 0) {
        do {
            total += base[i].sector_count;
            i++;
        } while (i < index);
    }
    CdIntToPos_8007E600(D_800E9EB4 + total, buffer, total);
    {
        register MovieStreamRange *table asm("a1");
        register void *buffer_arg asm("a0");
        s32 value;

        value = CdPosToInt_8007E710(buffer);
        buffer_arg = buffer;
        table = gMovie_aStreamRanges;
        value += table[index].sector_count;
        func_8005B8A0(buffer, arg1, end_frame, value, arg3, arg4);
    }
}

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
