#include "../types.h"

extern void func_80015780(void);
extern void func_800157DC(void);
extern void func_8001581C(void);
extern void func_80015998(void);

void func_800159D8(void)
{
    func_80015780();
    func_80015998();
}

void func_80015A00(void)
{
    func_800157DC();
    func_80015998();
}

void func_80015A28(void)
{
    func_8001581C();
    func_80015998();
}
