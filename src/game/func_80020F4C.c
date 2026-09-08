#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "input.h"
#include "rand_get_interval.h"
#include "display_object_api.h"
#include "display_object_helpers.h"
#include "file_transfer.h"
#include "sound.h"

/* Duel-result outro sequence, driven from the scene state word D_8009B23A.

   The first call (bit 0x8000 clear) fades the BGM out, records the winning
   side in D_8009B362/D_8009B238, requests the outro package (0x1DAB, 0x22
   sectors) through func_80020BE4, picks the win or lose track for
   D_8009B1E0 -- 0x72F0 only when the player won against a real opponent --
   and hands the two banner objects D_8009B214/D_8009B21C to func_8001EC70
   before entering step 1 of D_8009B174.

   Afterwards the low nibble of D_8009B174 is the step and bit 0x80 marks
   "step already entered":

     1  wait for the DMA/queue flags to drain, then stream the per-opponent
        voice clip (0x1D33 + id*3) into the scratch below gDuel_awRitualData
        and start the track; once it has been kicked off, wait again and
        play D_8009B1E0.
     2  spawn the seven confetti sprites from the D_80090928 (real opponent)
        or D_80090960 (no opponent) table row for the winning side, each on
        func_80020D4C with a random radius and orbit key, and remember them
        in the gDuel_awRitualData slot table; then wait for func_80042B40.
     3  hold for 0x258 frames or until the player presses one of the 0xE0
        buttons, then retarget every spawned sprite at func_80020EE8 so it
        flies off, and wait for func_80042B40 again.
     4  once the fade at D_800E9ECE has finished, hand the scene over to
        state 0xD.  */

struct Obj {
    u16 unk0;
    u16 unk2;
    u32 unk4;
    u16 unk8;
    u8 pad0[0x16 - 0xA];
    u8 unk16;
    u8 pad1[0x24 - 0x17];
    void (*unk24)(void *);
    s16 unk28;
    u16 unk2A;
    s16 unk2C;
    s16 unk2E;
    s16 unk30;
    s16 unk32;
    u8 pad2[0x40 - 0x34];
    u16 unk40;
    u8 pad3[0x48 - 0x42];
    s16 unk48;
    s16 unk4A;
    u8 pad4[0x68 - 0x4C];
    u8 unk68;
    u8 unk69;
    u8 pad5[0x6C - 0x6A];
    u8 unk6C;
};

typedef struct {
    struct Obj *obj;
    u8 pad[0xC - 4];
} Entry;

typedef struct {
    u8 x;
    u8 y;
    u8 kind;
    u8 tag;
} Spec;

#define SPEC_COUNT 7

extern void func_800472A8(s32);
extern u32 func_8004703C(void);
extern s32 func_80042B40(s32);
extern s32 rand(void);
extern void func_80020BE4(void);
extern void func_80020D4C(void *);
extern void func_8001EC70(void *);
extern void func_80020EE8(void *);

extern u16 D_8009B23A;
extern u16 D_8009B162;
extern u16 D_8009B1E0;
extern u16 D_8009B1D0;
extern u8 D_8009B174;
extern s8 D_8009B238;
extern u8 gDuel_bWinnerSide;
extern struct Obj *D_8009B214;
extern struct Obj *D_8009B21C;

extern u8 D_8009B362 __attribute__((section(".data")));
extern s8 gDuel_bOpponentID __attribute__((section(".data")));
extern u8 D_800E9ECE[];
extern u8 D_801AF000[];
extern u16 gDuel_awRitualData[];
extern Spec D_80090928[][SPEC_COUNT];
extern Spec D_80090960[][SPEC_COUNT];

void func_80020F4C(void)
{
    Entry *slots;
    struct Obj *obj;
    Spec *spec;
    s32 i;
    s32 flags;
    s32 state;
    s32 id;
    s32 timer;
    s32 mode;
    s32 v;

    slots = (Entry *)gDuel_awRitualData;

    v = D_8009B23A;
    if ((v & 0x8000) == 0) {
        D_8009B23A = v | 0x8000;
        SD_BGMFadeOut();
        id = gDuel_bWinnerSide;
        D_8009B362 = 0;
        if (id != 0) {
            D_8009B362 = 1;
        }
        D_8009B238 = id;
        File_RequestAsyncTransfer(
            0, 0,
            FILE_WA_DUEL_RESULTS_START_SECTOR,
            FILE_WA_DUEL_RESULTS_SECTOR_COUNT,
            func_80020BE4, 0, 0
        );
        mode = 0x72E0;
        if (gDuel_bWinnerSide != 0) {
            if (gDuel_bOpponentID >= 0) {
                mode = 0x72F0;
            }
        }
        obj = D_8009B214;
        D_8009B1E0 = mode;
        obj->unk28 = -116;
        obj->unk2C = 0x30;
        obj->unk6C = 1;
        obj->unk24 = func_8001EC70;
        obj->unk2A = obj->unk32;
        obj = D_8009B21C;
        obj->unk28 = 0x198;
        obj->unk2C = 0x30;
        obj->unk6C = 1;
        obj->unk24 = func_8001EC70;
        D_8009B174 = 1;
        obj->unk2A = obj->unk32;
        return;
    }

    if (D_8009B162 != 0) {
        return;
    }

    flags = D_8009B174;
    state = flags & 0xF;
    switch (state) {
    case 1:
        if ((flags & 0x80) == 0) {
            if ((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                D_8009B134_abs) {
                return;
            }
            D_8009B174 = flags | 0x80;
            D_8009B162 = 8;
            id = gDuel_bOpponentID;
            if (id < 0) {
                id = 1;
            }
            File_RequestAsyncTransfer(0, 0, id * 3 + 0x1D33, 3, 0, 0,
                                      (s32)((u8 *)slots - 0x1800));
            func_800472A8(D_8009B1E0);
        } else {
            if ((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                D_8009B134_abs) {
                return;
            }
            D_8009B174 = 2;
            SD_BGMPlay(D_8009B1E0);
        }
        break;
    case 2:
        if ((flags & 0x80) == 0) {
            D_8009B174 = flags | 0x80;
            for (i = 0; i < SPEC_COUNT; i++) {
                if (gDuel_bOpponentID >= 0) {
                    spec = &D_80090928[gDuel_bWinnerSide][i];
                } else {
                    spec = &D_80090960[gDuel_bWinnerSide][i];
                }
                slots[i].obj = 0;
                if (spec->kind != 0) {
                    obj = func_800400AC(func_8004002C(), 2);
                    func_800428A8(obj, spec->x, spec->y, 0,
                                  gDuel_bWinnerSide, spec->kind, 0x11, 9,
                                  (s32)D_801AF000);
                    if (obj->unk69 >= 0x1A) {
                        obj->unk40 += 0x10;
                    }
                    obj->unk8 |= 0x28;
                    obj->unk4 |= 0x50000000;
                    obj->unk48 = spec->tag;
                    obj->unk4A = 0x18;
                    func_80042918(obj);
                    *(s32 *)&obj->unk2C = *(s32 *)&obj->unk30;
                    obj->unk28 = (rand() & 0x3F) + 0x140;
                    obj->unk2A = Rand_GetInterval(0x1000);
                    obj->unk6C = 1;
                    obj->unk24 = func_80020D4C;
                    slots[i].obj = obj;
                }
            }
        } else {
            if ((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                D_8009B134_abs) {
                return;
            }
            if (func_80042B40(1) == 0) {
                D_8009B174 = 3;
            }
        }
        break;
    case 3:
        if ((flags & 0x80) == 0) {
            D_8009B174 = flags | 0x80;
            D_8009B1D0 = 0x258;
        }
        if ((D_8009B174 & 0x40) == 0) {
            timer = D_8009B1D0 - 1;
            D_8009B1D0 = timer;
            if ((s16)timer > 0) {
                if ((gInput_wPad1Pressed & 0xE0) == 0) {
                    if ((func_8004703C() & 0x80) != 0) {
                        return;
                    }
                }
            }
            D_8009B174 = D_8009B174 | 0x40;
            for (i = 0; i < SPEC_COUNT; i++) {
                obj = slots[i].obj;
                if (obj != 0) {
                    obj->unk6C = 1;
                    obj->unk24 = func_80020EE8;
                }
            }
        } else {
            if (func_80042B40(1) == 0) {
                D_8009B174 = 4;
            }
        }
        break;
    case 4:
        if ((flags & 0x80) == 0) {
            D_8009B174 = flags | 0x80;
        }
        if ((D_800E9ECE[0] & 0x80) == 0) {
            D_8009B23A = 0xD;
        }
        break;
    }
}
