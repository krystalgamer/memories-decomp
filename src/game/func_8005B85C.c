#include "../types.h"

extern u8 gFile_szSuMrgPath[];
extern void func_8005B64C(void);
extern void File_RequestAsyncTransfer(int, void *, int, int, void (*)(void), int, int);
void func_8005B85C(void)
{
    File_RequestAsyncTransfer(1, gFile_szSuMrgPath, 0, 0x73, func_8005B64C, 0, 0);
}
