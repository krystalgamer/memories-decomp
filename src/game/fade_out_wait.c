#include "../types.h"

extern void Fade_InitOut(void);
extern void Fade_StartOut(void);
extern void func_80015944(void);
extern void func_80015998(void);

void func_80015AD8(void)
{
    Fade_InitOut();
    func_80015998();
}

void Fade_WaitOut(void)
{
    Fade_StartOut();
    func_80015998();
}

void func_80015B28(void)
{
    func_80015944();
    func_80015998();
}
