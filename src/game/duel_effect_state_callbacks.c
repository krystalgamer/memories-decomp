#define D_8009B0CC_IN_DATA
#include "../types.h"
#include "func_80036D3C.h"
#include "../psyq/rand.h"
#include "file_transfer.h"
#include "graphics_frame.h"
#include "display_effect_lifecycle.h"

extern s16 D_8009B322;
extern u16 D_8009B348[2];
extern int func_80049120(void *);

extern u16 gGraphics_uViewportX[] asm("gGraphics_sViewportX");
extern u16 gGraphics_uViewportY[] asm("gGraphics_sViewportY");

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
    if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
         D_8009B134_abs) == 0) {
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
        D_8009B348[0] = gGraphics_uViewportX[0];
        D_8009B348[1] = gGraphics_uViewportY[0];
    }
    if (D_8009B0CC & 1) {
        gGraphics_uViewportX[0] = D_8009B348[0] + ((rand() & 7) - 4);
        gGraphics_uViewportY[0] = D_8009B348[1] + ((rand() & 3) - 2);
    }
    D_8009B322--;
    if (D_8009B322 == 0) {
        gGraphics_uViewportX[0] = D_8009B348[0];
        gGraphics_uViewportY[0] = D_8009B348[1];
        object[0x51] = 0;
    }
}
