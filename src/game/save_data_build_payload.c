#include "../types.h"
#include "save_data.h"

extern u8 D_801D4000[];
extern s32 D_8009B0C4[];
extern s8 gSD_bOutputType __attribute__((section(".data")));
extern s32 D_8009B3B8;

extern void Util_CopyWords(void *, void *, u32);
extern void func_8003CF14(void *);
extern void func_8003CFC8(void *);

void func_8003D03C(u8 *data)
{
    u8 *copy;
    register s32 output_type asm("$3");
    s32 value;
    register s32 saved_value asm("$2");

    Util_CopyWords(data, D_801D4000, 0x200);

    saved_value = D_8009B0C4[0];
    output_type = gSD_bOutputType;
    *(s32 *)(data + 0x600) = 0;
    *(s32 *)(data + 0x608) = saved_value;

    if (output_type < 0) {
        gSD_bOutputType = 0;
    }

    data[0x7DE] = gSD_bOutputType;
    copy = data + 0x200;
    value = D_8009B3B8 + 1;
    *(s32 *)(data + 0x604) = value;
    *(s32 *)(data + 0xC84) = value;

    func_8003CF14(copy);
    func_8003CFC8(copy);

    {
        u32 i;

        i = 0;
        do {
            *((u8 *)((u32)data + i) + 0x828) = 0;
            i++;
        } while (i < 0x58);
    }

    Util_CopyWords(data + 0x880, data + 0x200, SAVE_DATA_STATE_SIZE);
}
