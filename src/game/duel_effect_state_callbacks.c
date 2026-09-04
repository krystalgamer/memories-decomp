#include "../types.h"

extern u32 D_8009B0CC[];
extern u32 D_8009B0F4[];
extern u32 D_8009B134[];
extern u16 D_8009B146[];
extern u16 D_8009B148[];
extern s16 D_8009B322;
extern u8 *D_8009B328;
extern u16 D_8009B348[2];
extern int func_80036D3C(u8 *);
extern int func_80049120(void *);
extern int rand(void);

void func_800378D8(u8 *object)
{
    u8 flags = object[0x51];

    if ((flags & 0x80) == 0) {
        object[0x51] = flags | 0x80;
    }
    if (D_8009B328[0x33] == 0) {
        object[0x51] = 0;
    }
}

void func_80037914(u8 *object)
{
    u8 flags = D_8009B328[0x32];

    if ((flags & 3) == 0) {
        D_8009B328[0x32] = flags | 0x10;
        D_8009B328[0x33] = 6;
        object[0x51] = 8;
    }
}

void func_80037950(u8 *object)
{
    u8 flags = D_8009B328[0x32];

    if ((flags & 3) == 0) {
        D_8009B328[0x32] = flags | 0x10;
        D_8009B328[0x33] = 4;
        object[0x51] = 8;
    }
}

void func_8003798C(u8 *object)
{
    if (((D_8009B0F4[0] & 0x02000030) | D_8009B134[0]) == 0) {
        object[0x51] = 0;
    }
}

void func_800379C4(u8 *object)
{
    if (func_80049120(object) != 1) {
        object[0x51] = 0;
    }
}

void func_800379F8(u8 *object)
{
    u8 flags = object[0x51];

    if ((flags & 0x80) == 0) {
        object[0x51] = flags | 0x80;
        D_8009B322 = func_80036D3C(object);
    }
    D_8009B322--;
    if (D_8009B322 == 0) {
        object[0x51] = 0;
    }
}

void func_80037A58(u8 *object)
{
    u8 flags = object[0x51];

    if ((flags & 0x80) == 0) {
        object[0x51] = flags | 0x80;
        D_8009B322 = func_80036D3C(object);
        D_8009B348[0] = D_8009B146[0];
        D_8009B348[1] = D_8009B148[0];
    }
    if (D_8009B0CC[0] & 1) {
        D_8009B146[0] = D_8009B348[0] + ((rand() & 7) - 4);
        D_8009B148[0] = D_8009B348[1] + ((rand() & 3) - 2);
    }
    D_8009B322--;
    if (D_8009B322 == 0) {
        D_8009B146[0] = D_8009B348[0];
        D_8009B148[0] = D_8009B348[1];
        object[0x51] = 0;
    }
}
