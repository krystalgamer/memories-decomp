#include "../types.h"

typedef struct {
    s16 id;
    u8 pad_02[4];
    s16 state;
} ModelTransferItem;

extern s32 D_8009B074;
extern u8 D_8009B078;
extern s8 D_8009B079;
extern s8 D_8009B07A;
extern s8 D_8009B07B;
extern s8 D_8009B07C;
extern void Model_CopySlotU16Values(int, void *);

void func_8005FAE4(void)
{
    D_8009B074 = 0;
    D_8009B078 = 0;
    D_8009B079 = 0;
    D_8009B07A = -1;
    D_8009B07B = 0;
    D_8009B07C = 0;
}

int func_8005FB08(void)
{
    return D_8009B078 == 0;
}

int func_8005FB14(void)
{
    int value = 0;

    if (D_8009B078 != 0) {
        value = D_8009B074;
    }
    return value;
}

void func_8005FB30(u8 *data)
{
    int i;

    if (!data) {
        data = (u8 *)D_8009B074;
    }
    if (!data) {
        return;
    }
    for (i = 0; i < 2; i++) {
        ModelTransferItem *item = (ModelTransferItem *)(data + i * 8);

        if (item->state < 4) {
            if (item->state >= 2) {
                Model_CopySlotU16Values(item->id, data + 0x10 + i * 8);
            }
        }
    }
}
