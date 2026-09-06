# Resident near-miss candidates

Candidate sources for unmatched resident functions live in `tmp/`, which is not
tracked, so they are lost when a session ends. `notes/overlays/candidates.md`
already solves this for the overlays and records that the same near miss had
been rebuilt at least six times before it existed. The resident side had no
equivalent, so the same loss was happening here: a candidate would reach a
measured state, the state would be described in prose, and the next run would
rebuild the code from that prose.

This file is the durable copy for functions in `src/game/`. The rules are the
overlay file's rules, unchanged, because the problem is the same one.

Rules:

- Store the exact source that produces the recorded state, not a tidied
  version. These are measurements, and reformatting them can change the output.
- Record the profile and the measured result in the heading, and re-verify
  before trusting a stored candidate. If it no longer reproduces, say so in the
  function's inventory row rather than silently editing it here.
- Delete an entry once the function matches and its source is promoted into
  `src/game/`.
- The inventory row and `notes/research/matching-evidence.md` remain the place
  for findings, negatives and levers. This file holds only code.

Opcode distance below is the encoding-based multiset distance described in
`notes/research/matching-evidence.md`, not a positional diff. A large
positional count on top of a small opcode distance is an allocation or
scheduling problem, not a structural one, and the stored candidate is then the
right base to continue from rather than something to rewrite.


## `func_80046294` at 0x80046294

`gcc_2_8_1_g8_split`, 151 of 151 instructions, opcode distance 0, 17 differing
positions.

The instruction multiset is exact. Three differences remain and all three are
placement or register choice rather than shape:

- the `k` and `j` initialisers are emitted two instructions earlier than in the
  target, which puts them before the jump table hoist rather than after;
- `addu v1,v1,t0` sits one position earlier;
- the `0x20` arm computes its address as `addu a0,a2,a1` where the target has
  `addu a0,a1,a2`. `addu` is commutative and GCC canonicalises the operand
  order, so this one is probably not reachable from C at all.

Do not treat the following as settled, because each was measured against one
configuration and two of them have already reversed once. Pinning the state
pointer to `v1` helped when the copy used my own expression form and hurt once
it used the sibling's, so it is unpinned here. Four declaration orders, three
initialiser orders, four address spellings for the `0x20` arm, and seven
profiles were crossed at 17 without moving it.

```c
#include "../types.h"

typedef struct {
    u8 pad00[0x40];
    u16 flags;
    u8 pad42[0x4C - 0x42];
    s16 count;
    u8 pad4E[0x7C - 0x4E];
    u8 f7C;
    u8 f7D;
    u8 pad7E[0x80 - 0x7E];
    u8 entries[0x157E - 0x80];
    s16 f157E;
} SoundState;

typedef struct { u32 words[12]; } SoundEntry;

extern SoundState *g_SDValue __attribute__((section(".data")));
#define SOUND_STATE (g_SDValue)

extern s16 func_80049F50(void);
extern void func_80049C40(s16 arg0);

void func_80046294(void)
{
    SoundState *p;
    register s32 i asm("a3");
    register s32 j asm("a2");
    register s32 k asm("t0");
    register SoundState *q asm("a1");
    s32 tag;
    register SoundEntry *dst asm("v0");
    register u8 *src_base asm("v1");
    register SoundEntry *src asm("a0");

    p = SOUND_STATE;
    i = 0;
    if (p->count <= 0) {
        goto tail;
    }
    k = 0x30;
    j = i;

    do {
    if (p->count == 0) {
        goto tail;
    }
    tag = p->entries[j];
    switch (tag) {
    case 0x42:
    case 0x43:
    case 0x45:
    case 0x46:
    case 0x48:
        p = SOUND_STATE;
        dst = (SoundEntry *)((u8 *)p + j);
        dst = (SoundEntry *)((u8 *)dst + 0x80);
        src_base = (u8 *)p + k;
        src = (SoundEntry *)(src_base + 0x80);
        *dst = *src;
        p = SOUND_STATE;
        p->count = (u16)p->count - 1;
        goto test;
    case 0x20:
        q = SOUND_STATE;
        if (*(s32 *)&q->entries[j + 0x10] != 0x20) {
            goto test;
        }
        p = q;
        p = SOUND_STATE;
        dst = (SoundEntry *)((u8 *)p + j);
        dst = (SoundEntry *)((u8 *)dst + 0x80);
        src_base = (u8 *)p + k;
        src = (SoundEntry *)(src_base + 0x80);
        *dst = *src;
        p = SOUND_STATE;
        p->count = (u16)p->count - 1;
        goto test;
    default:
        k += 0x30;
        j += 0x30;
        i += 1;
        goto test;
    }

test:
    p = SOUND_STATE;
    } while (i < p->count && i >= 0);
    goto tail;

tail:
    switch (SOUND_STATE->f7C) {
    case 0x42:
    case 0x43:
    case 0x45:
    case 0x46:
    case 0x48:
        SOUND_STATE->f7C = 0;
        SOUND_STATE->f7D = 0;
        break;
    }

    p = SOUND_STATE;
    if ((p->flags & 0x80) == 0) {
        return;
    }
    if (p->f157E == -1) {
        return;
    }
    if (func_80049F50() == 1) {
        func_80049C40(SOUND_STATE->f157E);
    }
    SOUND_STATE->flags &= 0xFF7F;
}
```

## `Duel_LoadPackageStage` at 0x800171A8

`gcc_2_8_1_g8_split`, 242 instructions against a target of 237, opcode
distance 7.

The 13-phase duel package callback of the `func_8003B808` family. Table
`jtbl_800100C0` at `0x8C0` spanning `0x34` bytes, thirteen distinct bodies
whose addresses increase monotonically, so source order is numeric here.

Every phase byte count documented in `notes/overlays/runtime-loader.md`
reads back out of this code, and phases 2, 3 and 4 pin `gDuel_awEquipTable`
to `0x8017A1D8`, `gDuel_aFusionTable` to `0x8017C2D8` and
`gDuel_awRitualData` to `0x801799D8`, each matching its documented
destination.

Remaining difference: phases 0 and 10 merge from the pointer store where
the target merges one statement earlier, from the size store. Aligning
phase 6's tail with phase 12's already took this from 255 instructions to
242; the same kind of alignment is likely what closes the rest.

No word store at `0x30`, so plain `s16` fields rather than the union that
`func_8002BD0C` and `func_800577B0` need.

```c
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
    s32 field08;
    s32 field0C;
    u8 pad10[0xC];
    s32 field1C;
    u8 pad20[0x10];
    s16 field30;
    s16 field32;
    u8 pad34[0x12];
    u8 field46;
} Object;

extern volatile u32 D_8009B0F4 __attribute__((section(".data")));
extern s32 D_8009B118 __attribute__((section(".data")));
extern s32 D_80010000 __attribute__((section(".data")));
extern s32 D_800101DC __attribute__((section(".data")));
extern u8 D_800E9D70[100];
extern u16 gDuel_awEquipTable[];
extern u16 gDuel_aFusionTable[];
extern u16 gDuel_awRitualData[];
extern u8 D_801A8000[];
extern u8 D_801A9800[];

#define gStageRect (*(RECT *)D_800E9D70)

void Duel_LoadPackageStage(Object *object, s32 phase) {
    switch (phase) {
    case 0:
        object->field30 = 0x300;
        object->field32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        D_8009B0F4 |= 0x10000;
        object->field46 = 2;
        object->field1C = 0x20000;
        object->field08 = D_8009B118;
        object->field0C = D_8009B118 + 0x800;
        break;

    case 1:
        object->field1C = 0x2000;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field0C = D_8009B118;
        object->field08 = D_8009B118;
        object->field46 = 1;
        break;

    case 2:
        gStageRect.x = 0x100;
        gStageRect.y = 0xF0;
        gStageRect.w = 0x100;
        gStageRect.h = 0x10;
        LoadImage2(&gStageRect, (u32 *)D_8009B118);
        object->field0C = (s32)gDuel_awEquipTable;
        object->field08 = (s32)gDuel_awEquipTable;
        object->field1C = 0x2800;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field46 = 1;
        break;

    case 3:
        object->field0C = (s32)gDuel_aFusionTable;
        object->field08 = (s32)gDuel_aFusionTable;
        object->field1C = 0x10000;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field46 = 1;
        break;

    case 4:
        object->field0C = (s32)gDuel_awRitualData;
        object->field08 = (s32)gDuel_awRitualData;
        object->field1C = 0x800;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field46 = 1;
        break;

    case 5:
        object->field1C = 0x1000;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field0C = D_8009B118;
        object->field08 = D_8009B118;
        object->field46 = 1;
        break;

    case 6:
        gStageRect.x = 0;
        gStageRect.y = 0xF0;
        gStageRect.w = 0x100;
        gStageRect.h = 8;
        LoadImage2(&gStageRect, (u32 *)D_8009B118);
        object->field30 = 0x200;
        object->field32 = 0x100;
        object->w = 0x40;
        D_8009B0F4 &= 0xFFDDFFFF;
        D_8009B0F4 |= 0x10000;
        object->field46 = 2;
        object->field1C = 0x10000;
        object->h = 0x10;
        object->field08 = D_8009B118;
        object->field0C = D_8009B118 + 0x800;
        break;

    case 7:
        object->field1C = 0x16000;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field0C = D_800101DC;
        object->field08 = D_800101DC;
        object->field46 = 1;
        break;

    case 8:
        object->field0C = (s32)D_801A8000;
        object->field08 = (s32)D_801A8000;
        object->field1C = 0x1800;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field46 = 1;
        break;

    case 9:
        object->field0C = (s32)D_801A9800;
        object->field08 = (s32)D_801A9800;
        object->field1C = 0x1800;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field46 = 1;
        break;

    case 10:
        object->field30 = 0x340;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        object->field32 = 0;
        D_8009B0F4 |= 0x10000;
        object->field46 = 2;
        object->field1C = 0x4000;
        object->field08 = D_8009B118;
        object->field0C = D_8009B118 + 0x800;
        break;

    case 11:
        object->field1C = 0x2800;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field0C = D_80010000;
        object->field08 = D_80010000;
        object->field46 = 1;
        break;

    case 12:
        object->field30 = 0x280;
        object->field32 = 0x100;
        object->w = 0x40;
        D_8009B0F4 &= 0xFFDDFFFF;
        D_8009B0F4 |= 0x10000;
        object->field46 = 2;
        object->field1C = 0x10000;
        object->h = 0x10;
        object->field08 = D_8009B118;
        object->field0C = D_8009B118 + 0x800;
        break;
    }
}
```

## `func_8004A764` at 0x8004A764

`gcc_2_8_1_g8_split`, 23 of 23 instructions, opcode distance 0, 8 differing
positions.

Fills the secondary sound state's `SpuVoiceAttr` at `+0x4C0` and hands it to
`SpuSetVoiceAttr`. `voice` comes from `D_80011434[index]`, `mask` is `0x60100`,
`a_mode` is 5 and both ADSR words are cleared. The `pad04C0[0x40]` member in
`sound.h` is exactly `sizeof(SpuVoiceAttr)`, which is what the offsets confirm:
`+0x4C4` is `mask`, `+0x4E4` is `a_mode` at `+0x24`, and `+0x4FA`/`+0x4FC` are
`adsr1`/`adsr2` at `+0x3A`/`+0x3C`.

Two things had to be right together. The stores go through the **state**
pointer at `0x4C0` offsets, not through an `attr` pointer, because the target
keeps the state in `v1` and only computes `a0 = v1 + 0x4C0` as the call
argument. And `D_8009B458` needs a local `section(".data")` alias, because the
shared declaration in `sound.h` is small enough for `-G8` to make it
gp-relative while the target uses `lui`/`lw`.

What remains is scheduling only: the target finishes the `D_80011434` address
arithmetic before materialising the state pointer, and sinks `sw $ra` two
positions later than this build does.

Do not treat the following as settled. Crossed without improving on 8: five
attr-pointer register pins, three index spellings, four store orders, two
declaration orders, and ten profiles, across sweeps of 120, 96, 630, 144, 150
and 40 variants. `gcc_2_8_1_g0_split`, which the matched neighbour
`func_8004A7C0` uses, is consistently three positions worse here.

```c
#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

extern int D_80011434[];
extern SDSecondaryState *D_8009B458_d asm("D_8009B458")
    __attribute__((section(".data")));

#define ATTR(s) (*(SpuVoiceAttr *)((u8 *)(s) + 0x4C0))

void func_8004A764(int index)
{
    SDSecondaryState *s = D_8009B458_d;
    ATTR(s).voice = D_80011434[index];
    ATTR(s).mask = 0x60100;
    ATTR(s).a_mode = 5;
    ATTR(s).adsr1 = 0;
    ATTR(s).adsr2 = 0;
    SpuSetVoiceAttr(&ATTR(s));
}
```
