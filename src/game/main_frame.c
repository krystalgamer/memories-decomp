#include "../types.h"

extern volatile s32 D_8009B09C;
extern volatile s32 D_8009B0C4;
extern volatile s32 D_8009B0C8;
extern u8 D_8009B0C3;
extern u8 D_8009AF0C;
extern void func_8003CC38(void);
extern void func_80047050(void);
extern void func_8001306C(void);
extern void func_80012DB4(void);
extern void func_80012E5C(void);
extern void Input_UpdatePads(void);

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

void func_80012D4C(void)
{
    func_8001306C();
    func_80012DB4();
    func_80012E5C();
    Input_UpdatePads();
}

void func_80012D84(int count)
{
    do {
        func_80012D4C();
    } while (--count != 0);
}
