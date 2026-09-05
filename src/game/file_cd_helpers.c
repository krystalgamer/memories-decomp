#include "../types.h"

typedef struct {
    int words[6];
} Local;

extern int func_8007D3F0(int, int);
extern int func_8005BE3C(void);
extern void func_8005BB7C(int);
extern void func_800803F4(Local *);
extern int func_80082324(int);
extern int func_80081FC0(Local *, int, int);

int File_Exists(int first, int second)
{
    register int result asm("$3") = func_8007D3F0(second, first);
    register int output asm("$2") = -1;

    if (result == 0) {
        goto negative;
    }
    if (result != output) {
        output = 0;
        goto done;
    }
negative:
    output = -1;
done:
    return output;
}

int func_8005C530(void)
{
    int result = func_8005BE3C();

    if (result != 0) {
        func_8005BB7C(0);
    }
    return result;
}

int func_8005C568(int first, int second)
{
    Local local;

    func_800803F4(&local);
    while (func_80082324(3) != 0) {}
    while (func_80081FC0(&local, first, second) != 0) {}
    return 0;
}
