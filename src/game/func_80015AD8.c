#include "../types.h"

extern void Fade_InitOut(void);
extern void func_80015998(void);

void func_80015AD8(void)
{
    Fade_InitOut();
    func_80015998();
}
