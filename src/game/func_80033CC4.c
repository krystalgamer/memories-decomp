#include "../types.h"
#include "card_type_icon_table.h"
#include "func_80033CC4.h"

void func_80033CC4(CardList *a0) {
    int a1;
    int v1;
    u8 *p;
    u8 v0;
    a1 = a0->kind;
    v1 = a0->sort_choice;
    a1 = a1 << 4;
    v1 = v1 << 1;
    p = D_80090DD8 + v1;
    a1 = a1 + (int)p;
    v0 = *(u8 *)(a1 + 1);
    v0 = v0 & 0xf;
    a0->sort_mode = v0;
}
