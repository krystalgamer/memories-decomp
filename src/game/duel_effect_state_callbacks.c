#define D_8009B0CC_IN_DATA
#include "../types.h"
#include "func_80036D3C.h"
#include "../psyq/rand.h"
#include "file_constants.h"
#include "file_transfer.h"
#include "graphics_frame.h"
#include "display_effect_lifecycle.h"
#include "duel_effect.h"
#include "duel_effect_state_callbacks.h"

extern s16 D_8009B322;
extern u8 D_8009B335;
extern u16 D_8009B348[2];
extern int func_80049120(void *);

extern u16 gGraphics_uViewportX[] asm("gGraphics_sViewportX");
extern u16 gGraphics_uViewportY[] asm("gGraphics_sViewportY");

void func_800378D8(u8 *object)
{
    u8 flags = ((DuelEffectChannel *)object)->state_51;

    if ((flags & 0x80) == 0) {
        ((DuelEffectChannel *)object)->state_51 = flags | 0x80;
    }
    if (D_8009B328[0x33] == 0) {
        ((DuelEffectChannel *)object)->state_51 = 0;
    }
}

void func_80037914(u8 *object)
{
    u8 flags = D_8009B328[0x32];

    if ((flags & 3) == 0) {
        D_8009B328[0x32] = flags | 0x10;
        D_8009B328[0x33] = 6;
        ((DuelEffectChannel *)object)->state_51 = 8;
    }
}

void func_80037950(u8 *object)
{
    u8 flags = D_8009B328[0x32];

    if ((flags & 3) == 0) {
        D_8009B328[0x32] = flags | 0x10;
        D_8009B328[0x33] = 4;
        ((DuelEffectChannel *)object)->state_51 = 8;
    }
}

void func_8003798C(u8 *object)
{
    if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
         D_8009B134_abs) == 0) {
        ((DuelEffectChannel *)object)->state_51 = 0;
    }
}

void func_800379C4(u8 *object)
{
    if (func_80049120(object) != 1) {
        ((DuelEffectChannel *)object)->state_51 = 0;
    }
}

void func_800379F8(u8 *object)
{
    u8 flags = ((DuelEffectChannel *)object)->state_51;

    if ((flags & 0x80) == 0) {
        ((DuelEffectChannel *)object)->state_51 = flags | 0x80;
        D_8009B322 = func_80036D3C(object);
    }
    D_8009B322--;
    if (D_8009B322 == 0) {
        ((DuelEffectChannel *)object)->state_51 = 0;
    }
}

void func_80037A58(u8 *object)
{
    u8 flags = ((DuelEffectChannel *)object)->state_51;

    if ((flags & 0x80) == 0) {
        ((DuelEffectChannel *)object)->state_51 = flags | 0x80;
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
        ((DuelEffectChannel *)object)->state_51 = 0;
    }
}

/* The last of the eight, and the only one that waits on the file transfer
   itself: it drives the sector-range request through three D_8009B335 stages
   and, when byte 0x51 bit 0x40 was armed, repeats the whole run D_8009B33C
   times before clearing the state. The switch falls through deliberately -
   each stage re-arms the 0xFF countdown and drops into the next test in the
   same call. */
void func_80037B40(u8 *object)
{
    DuelEffectChannel *p = (DuelEffectChannel *)object;

    if ((p->state_51 & 0x80) == 0) {
        p->state_51 |= 0x80;
        p->delay_52 = 0xFF;
        D_8009B335 = 0;
        if (D_8009B33C != 0) {
            p->state_51 |= 0x40;
        }
    }

    p->delay_52--;

    if (p->delay_52 != 0) {
        switch (D_8009B335) {
        case 0:
            if ((D_8009B0F4_abs & FILE_TRANSFER_FLAG_SECTOR_RANGE) == 0) {
                return;
            }
            p->delay_52 = 0xFF;
            D_8009B335 = 1;
        case 1:
            if ((D_8009B112_abs & 0x4000) == 0) {
                return;
            }
            p->delay_52 = 0xFF;
            D_8009B335 = 2;
        case 2:
            if ((D_8009B112_abs & 0x4000) == 0) {
                break;
            }
            if ((p->state_51 & 0x40) == 0) {
                return;
            }
            D_8009B33C--;
            if (D_8009B33C > 0) {
                return;
            }
            break;
        }
    }

    p->state_51 = 0;
    p->delay_52 = 1;
}
