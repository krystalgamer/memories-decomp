#include "../types.h"

extern unsigned char D_800117C8;
extern void func_8005B64C(void);
extern void File_RequestAsyncTransfer(int, void *, int, int, void (*)(void), int, int);
void func_8005B85C(void)
{
    File_RequestAsyncTransfer(1, &D_800117C8, 0, 0x73, func_8005B64C, 0, 0);
}
