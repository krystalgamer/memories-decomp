#include "../types.h"

extern u8 gLibrary_aCardArtRecord[];
extern s32 D_8009B10C;
extern u8 D_800E9DF0[];
extern u8 D_8009B0E0;
extern s32 gFile_anLba[];
extern u8 *gFile_apszName[];
extern u8 D_80010038[4];

extern void func_80013898(s32);
extern void func_800137E4(void);
extern void File_GetPosition(s32 *, s32);

void File_SetPositionTable(void)
{
    s32 *position;
    u8 **name;
    u8 *current;
    s32 i;
    u8 *state;

    func_80013898((s32)gLibrary_aCardArtRecord);

    D_8009B10C = (s32)func_800137E4;
    state = D_800E9DF0;
    *(s16 *)(state + 4) = 0x120;
    *(s16 *)(state + 6) = 0xD0;
    *(s16 *)(state + 0xC) = 0xB;
    *(s32 *)D_800E9DF0 = 0x8000000;
    D_8009B0E0 = 0;
    *(s32 *)(state + 0x14) = 0x808080;
    *(u16 *)(state + 0xE) = 0xA000;
    *(s32 *)(state + 8) = 0x180018;
    *(s32 *)(state + 0x10) = 0xFC0230;

    for (i = 0, position = gFile_anLba, name = gFile_apszName;;) {
        current = *name;
        if (current == (u8 *)0) {
            break;
        }
        File_GetPosition(position, (s32)current);
        printf(D_80010038, current, *position);
        position++;
        name++;
        i++;
        if (i >= 7) {
            break;
        }
    }
}
