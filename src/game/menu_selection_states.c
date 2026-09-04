#include "../types.h"

extern u8 D_80090D0C[];
extern u8 D_80090D28[];
extern u8 D_8009B268[];
extern u8 D_8009B26C[];
extern u8 D_8009B26D[];
extern u8 D_8009B2E8;
extern u8 D_8009B2EB;
extern u8 D_8009B363[];
extern u16 gDebug_nSceneOrSoundID;
extern void func_80030250(void *, int, int, int, int, int, int);
extern int func_80030294(void);

void func_80030C10(void)
{
    u8 flags = D_8009B2EB;
    int result;

    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
        gDebug_nSceneOrSoundID = D_8009B2E8;
        func_80030250(D_80090D0C, 0x14, 0, 0, 0xC, 2, 1);
    }
    result = func_80030294();
    if (result != 0) {
        if (result < 0) {
            D_8009B2EB = 0;
        } else {
            D_8009B363[0] = 0;
            D_8009B26C[0] = 5;
            D_8009B2E8 = *(u8 *)&gDebug_nSceneOrSoundID;
        }
    }
}

void func_80030CB0(void)
{
    u8 flags = D_8009B2EB;
    int result;

    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
        gDebug_nSceneOrSoundID = D_8009B26D[0];
        func_80030250(D_80090D28, 0x15, 0, 0, 0x12, 2, 1);
    }
    result = func_80030294();
    if (result != 0) {
        if (result < 0) {
            D_8009B2EB = 0;
        } else {
            D_8009B268[0] = 1;
            D_8009B26C[0] = 8;
            D_8009B26D[0] = *(u8 *)&gDebug_nSceneOrSoundID;
        }
    }
}
