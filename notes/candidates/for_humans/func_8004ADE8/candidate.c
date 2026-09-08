#include "../../../../src/types.h"
#include "../../../../src/psyq/libspu.h"
#include "../../../../src/game/sound_sequence_constants.h"

extern u8 *D_8009B458;
extern s32 D_80011434[];
/* Second name for the same symbol: retail re-reads the pointer here rather
   than reusing the base it just stored through. */
extern u8 *D_8009B458_r asm("D_8009B458");

extern s32 func_8004A854(s32);
extern s32 func_8004A940(s32, s32);
extern void func_8004A0FC(u8 *, u8 *);
extern s32 func_8004A3BC(u8 *, s32);
extern s32 func_80049FB4(s32, s32, s32, s32);

void func_8004ADE8(s32 arg0, s32 note, u8 velocity)
{
    register s32 channel asm("$23");
    u8 *prog;
    s32 ch;
    u32 tidx;
    u32 key;
    u8 *rec;
    u8 *vab;
    u8 *hdr;
    u8 *tone;
    u8 *obj;
    s32 idx;
    s32 used;
    s32 i;
    s32 sum;
    s32 vag;
    u16 *sizes;
    u8 program;
    u8 stolen;
    s32 level;
    s32 pitch;
    s32 amode;
    s32 *voice;
    u16 adsr1;
    u16 adsr2;

    channel = arg0;
    rec = D_8009B458 + (channel & 0xFF) * SD_SEQUENCE_CHANNEL_RECORD_SIZE;
    hdr = *(u8 **)(D_8009B458 + 0x4A8);
    program = rec[0];
    vab = hdr;
    used = 0;
    i = 0;
    if (program != 0) {
        do {
            if (vab[i * 0x10 + 0x20] != 0) {
                used++;
            }
            i++;
        } while (i < program);
    }
    prog = &vab[program * 0x10 + 0x20];
    if (prog[0] == 0) {
        return;
    }
    tidx = 0;
    key = note & 0xFF;
    ch = channel & 0xFF;
    level = note & 0x7F;
    do {
        tone = &vab[(used * 16 + (tidx & 0xFFFF)) * 32 + 0x820];
        if (key < tone[6]) {
            goto next;
        }
        if (tone[7] < key) {
            goto next;
        }
        {
            u32 limit = rec[6] & 0xF0;

            stolen = 0;
            if (limit != 0) {
                if ((rec[6] & 0xF) >= (limit >> 4)) {
                    idx = func_8004A854(ch);
                    if (idx == -1) {
                        idx = func_8004A940(ch, key);
                        goto have;
                    }
                    stolen = 1;
                    goto have;
                }
            }
        }
        idx = func_8004A940(channel & 0xFF, key);
    have:
        if (idx == -1) {
            goto next;
        }
        i = 0;
        sum = 0;
        vag = *(s16 *)(tone + 0x16);
        sizes = (u16 *)(*(u8 **)(D_8009B458 + 0x4A8)
                        + (*(u16 *)(hdr + 0x12)) * 512 + 0x820);
        obj = &D_8009B458[idx * SD_SECONDARY_OBJECT_SIZE + 0x180];
        if (vag > 0) {
            do {
                sum += sizes[i];
                i++;
            } while (i < vag);
        }
        sum <<= 3;
        voice = &D_80011434[idx];
        *(s32 *)(D_8009B458 + 0x4C4) = 0x6019F;
        *(s16 *)(D_8009B458 + 0x4CC) = 0;
        *(s16 *)(D_8009B458 + 0x4CE) = 0;
        *(s32 *)(D_8009B458 + 0x4C0) = *voice;
        *(s32 *)(D_8009B458 + 0x4DC) = *(s32 *)(D_8009B458 + 0x4B8) + sum;
        adsr1 = *(u16 *)(tone + 0x10);
        *(s16 *)(D_8009B458 + 0x4FA) = adsr1;
        *(u16 *)(obj + 0x20) = adsr1;
        adsr2 = *(u16 *)(tone + 0x12);
        *(s16 *)(D_8009B458 + 0x4FC) = adsr2;
        *(u16 *)(obj + 0x22) = adsr2;
        if ((*(u16 *)(tone + 0x10) & 0x80) == 0) {
            amode = 1;
        } else {
            amode = 5;
        }
        *(s32 *)(D_8009B458 + 0x4E4) = amode;
        *(u16 *)(obj + 0x24) = *(u16 *)(D_8009B458_r + 0x4E4);
        rec[4] = velocity;
        obj[0x12] = tone[4];
        obj[0x13] = tone[5];
        obj[0x11] = tone[0xC];
        obj[0] = idx;
        obj[4] = tidx;
        obj[2] = program;
        obj[5] = level;
        obj[3] = channel;
        obj[0xD] = 1;
        obj[0x10] = tone[0xD];
        obj[6] = level;
        obj[8] = prog[1];
        obj[0xA] = prog[4];
        obj[9] = tone[2];
        obj[0xE] = velocity;
        *(u16 *)(obj + 0x1E) = 0xFFFF;
        obj[0xB] = tone[3];
        func_8004A0FC(D_8009B458 + idx * SD_SECONDARY_OBJECT_SIZE + 0x180,
                      D_8009B458 + ch * SD_SEQUENCE_CHANNEL_RECORD_SIZE);
        *(s16 *)(D_8009B458 + 0x4C8) =
            (*(u16 *)(obj + 0x14) * *(u16 *)(D_8009B458 + 0x514)) >> 7;
        *(s16 *)(D_8009B458 + 0x4CA) =
            (*(u16 *)(obj + 0x16) * *(u16 *)(D_8009B458 + 0x516)) >> 7;
        obj[5] = level;
        *(s16 *)(obj + 0x1A) = -1;
        *(s16 *)(obj + 0x1C) = rec[7];
        pitch = func_8004A3BC(obj, rec[7]) + obj[6] * 128;
        *(s16 *)(D_8009B458 + 0x4D4) =
            func_80049FB4((s16)pitch >> 7, pitch & 0x7F, tone[4], tone[5]);
        SpuSetKeyOnWithAttr((SpuVoiceAttr *)(D_8009B458 + 0x4C0));
        if (stolen == 0) {
            if ((rec[6] & 0xF) < 0xF) {
                rec[6] = rec[6] + 1;
            }
        }
        obj[0xF] = 1;
        if (rec[0x10] == 0) {
            if ((tone[1] & 4) != 0) {
                s32 *m = &D_80011434[idx];
                do {
                    SpuSetReverbVoice(SPU_ON, *m);
                } while ((SpuGetReverbVoice() & *m) == 0);
            } else {
                s32 *m = &D_80011434[idx];
                do {
                    SpuSetReverbVoice(SPU_OFF, *m);
                } while ((SpuGetReverbVoice() & *m) != 0);
            }
        } else if (rec[0x10] != 1) {
            s32 *m = &D_80011434[idx];
            do {
                SpuSetReverbVoice(SPU_ON, *m);
            } while ((SpuGetReverbVoice() & *m) == 0);
        } else {
            s32 *m = &D_80011434[idx];
            do {
                SpuSetReverbVoice(SPU_OFF, *m);
            } while ((SpuGetReverbVoice() & *m) != 0);
        }
    next:
        tidx++;
    } while ((tidx & 0xFFFF) < prog[0]);
}
