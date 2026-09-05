#include "../types.h"

#include "sound.h"

extern int SD_ReadSequenceByte();
extern int SD_ReadVariableLengthValue();
extern int func_8004BC2C();

void func_8004BE6C(int *value, int amount)
{
    *value += amount;
}

void func_8004BE80(void)
{
}

void func_8004BE88(u8 *p, s32 arg1)
{
    u8 *e;
    u8 *f;
    u32 v;
    u32 i;
    s32 off;
    s32 mode;
    s32 t;
    s32 z;

    mode = arg1 & 0xFF;

    switch (mode) {
    case 0x2F:
        p[0x24] = 1;
        goto one;

    case 0x51:
        z = 0;
        v = SD_ReadSequenceByte(p) << 16;
        v = v | (SD_ReadSequenceByte(p) << 8);
        v = v | SD_ReadSequenceByte(p);
        e = (u8 *)D_8009B458;
        ((SDSecondaryState *)e)->field_0808 = v;
        v = 0x3938700 / v;
        v = v * 100 / 115;
        if (v >= 0x100) {
            v = 0xFF;
        }
        t = ((SDSecondaryState *)e)->timebase;
        switch (t) {
        case 0x3C:
        case 0x18:
            v = v >> 1;
            break;
        case 0x1E:
            v = v >> 2;
            break;
        }
        f = (u8 *)D_8009B458;
        if (((SDSecondaryState *)f)->field_07FA != 0) {
            off = z;
            i = z;
            do {
                *(s16 *)(f + off + 0x52E) = v;
                *(s16 *)(f + off + 0x52C) = v;
                off += SD_SEQUENCE_TRACK_RECORD_SIZE;
                i++;
            } while (i < ((SDSecondaryState *)f)->field_07FA);
        }
        break;

    case 0x54:
        SD_ReadSequenceByte(p);
        SD_ReadSequenceByte(p);
        SD_ReadSequenceByte(p);
        goto three;

    case 0x58:
        SD_ReadSequenceByte(p);
        func_8004BC2C(p);
        break;

    case 0x59:
    three:
        SD_ReadSequenceByte(p);
        SD_ReadSequenceByte(p);
    one:
        SD_ReadSequenceByte(p);
        break;

    case 0 ... 0xF:
    default:
        func_8004BE6C((s32 *)p, SD_ReadVariableLengthValue(p));
        break;
    }
}

void func_8004C0AC(void *input)
{
    unsigned int i = 0;
    unsigned int count = SD_ReadVariableLengthValue(input);

    do {
        i++;
        if ((u8)SD_ReadSequenceByte(input) == 247)
            break;
    } while (i < count);
}
