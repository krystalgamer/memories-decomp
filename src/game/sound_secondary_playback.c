#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

typedef struct {
    s32 image;
    s32 type;
    s16 x;
    s16 y;
    u8 pad_0C[52];
} Packet;

extern s32 D_80011434[];
extern int func_8004A0FC();
extern int func_8004A27C();
extern void func_8004A2F8(void);
extern void func_8004A518(void);
extern int func_8004ACE4();
extern void func_8004C77C(SDSecondaryState *);
/* Preserve the no-argument call form used by the playback-start variant. */
extern void func_8004C77C_no_arg(void) asm("func_8004C77C");
extern s32 func_8004CABC(void);
extern void SD_ResetSequenceTracks(void);

int func_80049A64(void *input, short value)
{
    unsigned int tag;
    SDSecondaryState *state;
    D_8009B458->flag_0500 = 1;
    tag = *(unsigned int *)input;
    if (tag != 0x53455170 && tag != 0x6468544D &&
        tag != 0x2054444B && tag != 0x3154444B)
        return -1;
    state = D_8009B458;
    if (state->field_07E0 == -1) {
        state->field_07E8 = input;
        state->field_07E0 = value;
        state->field_07E2 = 2;
        state->flag_0500 = 0;
        return 0;
    }
    state->flag_0500 = 0;
    return -1;
}

void func_80049AF4(s32 arg0)
{
    register s32 one asm("$17") = 1;

    D_8009B458->flag_0500 = one;

    if (D_8009B458->field_07E0 == -1) {
        D_8009B458->flag_0500 = 0;
        return;
    }

    D_8009B458->field_07EC = 0x10000;
    D_8009B458->field_07DC = D_8009B458->field_07E8;
    func_8004C77C(D_8009B458);

    if ((arg0 & 0xFF) == 0) {
        D_8009B458->field_07E2 = 4;
    } else {
        D_8009B458->flag_0502 = one;
        D_8009B458->field_07E2 = 1;
    }

    D_8009B458->flag_0500 = 0;
}

void func_80049BAC(s32 value)
{
    SDSecondaryState *state;

    D_8009B458->flag_0500 = 1;
    state = D_8009B458;
    if (state->field_07E0 == -1) {
        state->flag_0500 = 0;
    } else {
        SDSecondaryState *first;
        SDSecondaryState *second;
        state->field_07EC = 0x10000;
        state->field_07DC = state->field_07E8;
        func_8004C77C_no_arg();
        first = D_8009B458;
        first->flag_0502 = 1;
        second = D_8009B458;
        first->field_080C = value;
        second->field_07E2 = 1;
        second->flag_0500 = 0;
    }
}

void func_80049C40(void)
{
    SDSecondaryState *initial = D_8009B458;

    if (initial->field_07E0 != -1) {
        SDSecondaryState *state;
        SDSecondaryState *other;
        initial->flag_0500 = 1;
        SD_ResetSequenceTracks();
        func_8004A518();
        state = D_8009B458;
        state->flag_0500 = 0;
        other = D_8009B458;
        state->field_07E6 = SD_SECONDARY_LEVEL_MAX;
        state->field_07E4 = SD_SECONDARY_LEVEL_MAX;
        state->field_07E2 = 2;
        other->flag_0502 = 0;
    }
}

void func_80049CB0(void)
{
    SDSecondaryState *state = D_8009B458;

    if (state->field_07E0 != -1) {
        state->flag_0500 = 1;
        D_8009B458->field_07E0 = -1;
        D_8009B458->field_07E2 = 0;
        D_8009B458->field_07E6 = SD_SECONDARY_LEVEL_MAX;
        D_8009B458->field_07E4 = SD_SECONDARY_LEVEL_MAX;
        D_8009B458->flag_0500 = 0;
    }
}

void func_80049CF8(void)
{
    Packet packet;
    register u8 *initial asm("$2") = (u8 *)D_8009B458;
    register u8 *state asm("$3");
    register s32 i asm("$16");
    register s32 offset asm("$17");
    register s32 *image asm("$18");

    initial[0x500] = 1;
    state = (u8 *)D_8009B458;
    if (*(s16 *)(state + 0x510) > 0) {
        s32 count;

        i = 0;
        asm volatile(
            "lui $2,%%hi(D_80011434)\n\t"
            "addiu %0,$2,%%lo(D_80011434)"
            : "=r"(image)
            :
            : "$2"
        );
        offset = i;
        do {
            u8 *entry = state + offset;

            if ((entry[0x183] >> 4) == 0 && entry[0x18D] != 0) {
                register s32 type asm("$2") = 3;

                packet.type = type;
                packet.image = *image;
                packet.x = 0;
                packet.y = 0;
                SpuSetVoiceAttr((SpuVoiceAttr *)&packet);
            }
            image++;
            asm volatile("" : "+r"(image));
            state = (u8 *)D_8009B458;
            count = *(s16 *)(state + 0x510);
            asm volatile("" : "+r"(count));
            i++;
            offset += SD_SECONDARY_OBJECT_SIZE;
        } while (i < count);
    }
    state = (u8 *)D_8009B458;
    *(s16 *)(state + 0x7E2) = 4;
    state[0x500] = 0;
}

void func_80049DD8(void)
{
    Packet packet;
    register u8 *initial asm("$2") = (u8 *)D_8009B458;
    register u8 *state asm("$3");
    register s32 i asm("$16");
    register s32 offset asm("$17");
    register s32 *image asm("$18");

    initial[0x500] = 1;
    state = (u8 *)D_8009B458;
    if (*(s16 *)(state + 0x510) > 0) {
        s32 count;

        i = 0;
        asm volatile(
            "lui $2,%%hi(D_80011434)\n\t"
            "addiu %0,$2,%%lo(D_80011434)"
            : "=r"(image)
            :
            : "$2"
        );
        offset = i;
        do {
            u8 *entry = state + offset;

            if ((entry[0x183] >> 4) == 0 && entry[0x18D] != 0) {
                register s32 type asm("$2") = 3;

                packet.type = type;
                packet.image = *image;
                packet.x = *(u16 *)(entry + 0x194);
                packet.y = *(u16 *)(entry + 0x196);
                SpuSetVoiceAttr((SpuVoiceAttr *)&packet);
            }
            image++;
            asm volatile("" : "+r"(image));
            state = (u8 *)D_8009B458;
            count = *(s16 *)(state + 0x510);
            asm volatile("" : "+r"(count));
            i++;
            offset += SD_SECONDARY_OBJECT_SIZE;
        } while (i < count);
    }
    state = (u8 *)D_8009B458;
    *(s16 *)(state + 0x7E2) = 1;
    state[0x500] = 0;
}

void func_80049EC8(s32 arg0, s32 arg1)
{
    SDSecondaryState *state = D_8009B458;
    s32 mode = state->field_07E2;

    state->field_07E4 = arg0 & SD_SECONDARY_LEVEL_MASK;
    state->field_07E6 = arg1 & SD_SECONDARY_LEVEL_MASK;
    if (mode != 2)
        func_8004A2F8();
}

void func_80049F10(s16 first, s16 second)
{
    SDSecondaryState *state = D_8009B458;

    state->field_0514 = first;
    state->field_0516 = second;
    if (state->field_07E2 != 2)
        func_8004A2F8();
}

s32 func_80049F50(void)
{
    s32 value;

    if (D_8009B458->field_07E2 == 1) {
        value = (s16)func_8004CABC();
        if (value == 3)
            D_8009B458->field_07E2 = value;
    }
    return D_8009B458->field_07E2;
}
