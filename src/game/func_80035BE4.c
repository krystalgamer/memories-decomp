#include "../types.h"

extern void TextBox_SetRect(int, int, int, int, int);
extern void DuelEffect_InitEntry(int, int, int);
void func_80035BE4(int a, int b, int c, int d, int e, int f) {
    TextBox_SetRect(a, c, d, e, f);
    DuelEffect_InitEntry(a, b, 0);
}
