#include "../types.h"
#include "card_type_icon_table.h"

struct Obj {
    u8 pad[11589];
    u8 field11589;
    s8 field11590;
    u8 field11591;
};

void func_80033CC4(struct Obj *a0) {
    int a1;
    int v1;
    u8 *p;
    u8 v0;
    a1 = a0->field11591;
    v1 = a0->field11590;
    a1 = a1 << 4;
    v1 = v1 << 1;
    p = D_80090DD8 + v1;
    a1 = a1 + (int)p;
    v0 = *(u8 *)(a1 + 1);
    v0 = v0 & 0xf;
    a0->field11589 = v0;
}
