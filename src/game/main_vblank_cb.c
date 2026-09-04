#include "../types.h"

extern volatile s32 D_8009B09C;
extern volatile s32 D_8009B0C4;
extern volatile s32 D_8009B0C8;
extern u8 D_8009B0C3;
extern u8 D_8009AF0C;
extern void func_8003CC38(void);
extern void func_80047050(void);

void Main_VBlankCB(void)
{
    D_8009B09C++;
    D_8009B0C4++;
    D_8009B0C3 = 1;
    D_8009B0C8++;
    func_8003CC38();

    if (D_8009AF0C == 0) {
        D_8009AF0C = 1;
        func_80047050();
        D_8009AF0C = 0;
        D_8009B0C3 = 0;
    }
}
