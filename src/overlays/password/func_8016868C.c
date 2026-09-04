#include "../../types.h"

extern u8 D_800EB0F8[];

u8 *func_8016868C(s32 index, s32 x, s32 y)
{
    u8 *entry = D_800EB0F8 + index * 100;
    u8 *node = *(u8 **)(entry + 0x24);
    s16 *fields = (s16 *)(node + 0xE);

    for (;;) {
        if ((((u8 *)fields)[3] & 0x80) == 0) {
            return 0;
        }
        if (fields[-1] == x) {
            if (fields[0] == y) {
                return node;
            }
        }
        fields = (s16 *)((u8 *)fields + 0x1C);
        node += 0x1C;
    }
}
