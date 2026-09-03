#include "../types.h"

extern void Fade_StartOut(void);
extern void func_80015998(void);

void Fade_WaitOut(void)
{
    Fade_StartOut();
    func_80015998();
}
