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

`gcc_2_8_1_g8_split`, 23 of 23 instructions, opcode distance 0, 6 differing
positions. Supersedes the 8-position state previously recorded here; the
structural reading below is that entry's and is unchanged.

Fills the secondary sound state's `SpuVoiceAttr` at `+0x4C0` and hands it to
`SpuSetVoiceAttr`. `voice` comes from `D_80011434[index]`, `mask` is `0x60100`,
`a_mode` is 5 and both ADSR words are cleared. The `pad04C0[0x40]` member in
`sound.h` is exactly `sizeof(SpuVoiceAttr)`, which is what the offsets confirm:
`+0x4C4` is `mask`, `+0x4E4` is `a_mode` at `+0x24`, and `+0x4FA`/`+0x4FC` are
`adsr1`/`adsr2` at `+0x3A`/`+0x3C`.

Two things had to be right together, and both still hold. The stores go through
the **state** pointer at `0x4C0` offsets, not through an `attr` pointer, because
the target keeps the state in `v1` and only computes `a0 = v1 + 0x4C0` as the
call argument. And `D_8009B458` needs a local `section(".data")` alias, because
the shared declaration in `sound.h` is small enough for `-G8` to make it
gp-relative while the target uses `lui`/`lw`.

**Two positions come from the store order.** The target emits `voice`, `mask`,
`adsr1`, `adsr2`, `a_mode`, with the `a_mode` store landing in the `jal` delay
slot. Writing `a_mode` third, where it reads most naturally next to `mask`,
costs those two. Writing it last matches. The `SPU_VOICE_*` names for the mask
and `SPU_VOICE_EXPIncN` for the mode also decode `0x60100` and `5`, which is
worth having in the source rather than in prose.

What remains is scheduling only: the target finishes the `D_80011434` address
arithmetic before materialising the state pointer, and sinks `sw $ra` two
positions later than this build does.

Do not treat the following as settled. Crossed without improving on the earlier
8: five attr-pointer register pins, three index spellings, four store orders,
two declaration orders, and ten profiles, across sweeps of 120, 96, 630, 144,
150 and 40 variants. `gcc_2_8_1_g0_split`, which the matched neighbour
`func_8004A7C0` uses, is consistently three positions worse here. Crossed
without improving on 6: reordering the mask before the voice store, moving the
table read into a local, naming the table base, and an early local for the mask
constant.

```c
#include "../../src/types.h"
#include "../../src/psyq/libspu.h"

extern s32 D_80011434[];
extern u8 *D_8009B458_d asm("D_8009B458") __attribute__((section(".data")));

void func_8004A764(s32 index)
{
    u8 *p;

    p = D_8009B458_d;
    *(u32 *)(p + 0x4C0) = D_80011434[index];
    *(u32 *)(p + 0x4C4) = SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_ADSR1 |
                          SPU_VOICE_ADSR_ADSR2;
    *(u16 *)(p + 0x4FA) = 0;
    *(u16 *)(p + 0x4FC) = 0;
    *(s32 *)(p + 0x4E4) = SPU_VOICE_EXPIncN;
    SpuSetVoiceAttr((SpuVoiceAttr *)(p + 0x4C0));
}
```

## `func_8004A6F8` at 0x8004A6F8

`gcc_2_8_1_g8_split`, 27 of 27 instructions, opcode distance 0, 8 differing positions.

The second member of the `SpuVoiceAttr` family at `+0x4C0`, and the same two
requirements as `func_8004A764`: stores through the state pointer at `0x4C0`
offsets, and a `section(".data")` alias so `D_8009B458` is not gp-relative.

This one copies `adsr1`, `adsr2` and `a_mode` out of a second argument at
`+0x20`, `+0x22` and `+0x24` rather than using constants, which is what the
`0x60100` mask says it does. The argument is not a Psy-Q `VagAtr` -- that
struct is `0x20` bytes and its `adsr1` is at `+0x10` -- so it stays a `u8 *`
with hex offsets until something names it.

The store order matters here too, in the opposite direction from
`func_8004A764`: writing `voice` before `mask` gives 8, writing `mask` first
gives 11, and the target emits `mask` first. Emission order is not source
order for these two, and neither reading is guessable from the disassembly
alone.

What remains is the same prologue scheduling as its sibling.

```c
#include "../../src/types.h"
#include "../../src/psyq/libspu.h"

extern s32 D_80011434[];
extern u8 *D_8009B458_d asm("D_8009B458") __attribute__((section(".data")));

void func_8004A6F8(s32 index, u8 *tone)
{
    u8 *p;

    p = D_8009B458_d;
    *(u32 *)(p + 0x4C0) = D_80011434[index];
    *(u32 *)(p + 0x4C4) = SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_ADSR1 |
                          SPU_VOICE_ADSR_ADSR2;
    *(u16 *)(p + 0x4FA) = *(u16 *)(tone + 0x20);
    *(u16 *)(p + 0x4FC) = *(u16 *)(tone + 0x22);
    *(u32 *)(p + 0x4E4) = *(u16 *)(tone + 0x24);
    SpuSetVoiceAttr((SpuVoiceAttr *)(p + 0x4C0));
}
```

## `func_8004A27C` at 0x8004A27C

`gcc_2_8_1_g8_split`, 31 of 31 instructions, opcode distance 0, 9 differing positions.

The third member of the family, and the one whose field names the
`SpuVoiceAttr` reading confirms most directly: the mask is `0xF`, which is
`SPU_VOICE_VOLL | VOLR | VOLMODEL | VOLMODER`, and the four fields written are
`+0x4C8`/`+0x4CA` and `+0x4CC`/`+0x4CE`, which are `volume.left`/`right` at
`+0x08`/`+0x0A` and `volmode.left`/`right` at `+0x0C`/`+0x0E`. So this is a
volume set, the previous two are ADSR sets, and the `0x4C0` window is one
`SpuVoiceAttr` shared by all three.

The two volumes are `(x * state[0x514]) >> 7` and `(y * state[0x516]) >> 7`,
which is a fixed-point scale by a pair of halfwords in the state block rather
than a signed division -- there is no rounding bias in the target.

Store order again: `volmode` pair, then `voice`, then `mask`, then the two
volumes gives 9. Writing `mask` before `voice`, which is the target's emission
order, gives 14. Naming the two products in locals is inert.

What remains is scheduling and the register naming of the two `mflo` results.

```c
#include "../../src/types.h"
#include "../../src/psyq/libspu.h"

extern s32 D_80011434[];
extern u8 *D_8009B458_d asm("D_8009B458") __attribute__((section(".data")));

void func_8004A27C(s32 index, s32 x, s32 y)
{
    u8 *p;

    p = D_8009B458_d;
    *(u16 *)(p + 0x4CC) = 0;
    *(u16 *)(p + 0x4CE) = 0;
    *(u32 *)(p + 0x4C0) = D_80011434[index];
    *(u32 *)(p + 0x4C4) = SPU_VOICE_VOLL | SPU_VOICE_VOLR |
                          SPU_VOICE_VOLMODEL | SPU_VOICE_VOLMODER;
    *(u16 *)(p + 0x4C8) = (x * *(u16 *)(p + 0x514)) >> 7;
    *(u16 *)(p + 0x4CA) = (y * *(u16 *)(p + 0x516)) >> 7;
    SpuSetVoiceAttr((SpuVoiceAttr *)(p + 0x4C0));
}
```

## `func_80045334` at 0x80045334

`gcc_2_8_1_g0`, 70 of 70 instructions, 2 differing positions.

The instruction mix is exact and every register agrees. What is left is a
scheduler tie-break between two independent stack stores: the target writes
`f04` at `0xCC` and puts `f08` in the `jal` delay slot, and this build does the
reverse.

`gcc_2_8_1_g0_no_sched2` reads the order out directly, and it is the fact that
matters here: the natural store order is `f08`, `f0C`, `f04` -- `kind`,
`second`, `first` -- and the target needs `f04`, `f0C`, `f08`. Everything else
in the function, including `addiu $a0, $sp, 0x10` at `0xC0`, is already
correct on this base.

That natural order is invariant. About 1800 variants were compiled crossing
assignment order, local declaration order, `volatile` subsets, the lvalue
spelling of each store (member, through a `struct Request *`, and through a
cast address), named local against inline expression for `first` and `second`,
register pins on `first`, `second` and `kind`, the position of `kind` within
each switch arm, and every profile. None moved it off `f04`/`f08` swapped, so
statement order is not the input the order is computed from.

`volatile` is the one control that does reach it, and it is worth knowing as a
lever rather than as a failed experiment: marking the fields `volatile` makes
the emitted store order exactly the source order. It cannot finish the job
here because a `volatile` store may not sink into the `jal` delay slot, so the
last store stays put and `addiu $a0, $sp, 0x10` takes the slot instead. All
three `volatile` lands at 5, not the 6 recorded previously; `f04`+`f0C`
`volatile` with the source ordered `f04`, `f0C`, `f08` -- the shape the target
implies -- lands at 4. So the remaining move is something that orders these
two stores without making the third one immovable.

Three register pins are needed and each corrects an allocation on a sequence
that is already exact: the `g_SDValue` pointer to `$a1` (unpinned it goes to a
callee-saved register and shifts everything), the table pointer to `$v0`, and
the request code to `$s3`. Two source-level facts also matter and are not
pins: the three-way test on `arg0 & 0xF000` is a `switch`, not an `if`/`else if`
chain -- the chain builds 67 instructions with `bne` where the target has `beq`
plus a `j` to the default -- and the default arm reads `code += 0x6000` rather
than `code = arg0 + 0x6000`, which is what keeps `$s3` live across it.

```c
#include "../../src/types.h"
#include "../../src/game/sound.h"

struct Request {
    u8 tag;
    u8 pad01;
    s16 f02;
    s32 f04;
    s32 f08;
    s32 f0C;
    u8 pad10[0x30 - 0x10];
};

extern void func_800464F0(void);
extern s32 func_80045BE8(struct Request *);

void func_80045334(s32 arg0)
{
    struct Request req;
    register SDValue *a asm("$5");
    SDValue *b;
    SDValue *c;
    s32 value;
    register s32 code asm("$19");
    s32 kind;
    register u8 *first asm("$16");
    u8 *second;
    register u32 *table asm("$2");

    a = g_SDValue;
    code = arg0;
    if ((a->flags_004A & 0x80) == 0) {
        return;
    }
    if ((a->flags_004A & 0x40) == 0) {
        if ((u32)(code & 0xFFFF) > 0x9FFF) {
            return;
        }
    }
    if ((arg0 & 0x8000) == 0) {
        return;
    }
    value = arg0 & 0xF000;
    *(s16 *)((u8 *)a + 0x534) = arg0;
    switch (value) {
    case 0x8000:
        code = arg0 + value;
        table = *(u32 **)((u8 *)a + 0x51C);
        kind = 0x50;
        break;
    case 0x9000:
        code = arg0 + 0x7000;
        table = *(u32 **)((u8 *)a + 0x518);
        kind = 0x60;
        break;
    default:
        code += 0x6000;
        kind = 0x70;
        b = g_SDValue;
        table = *(u32 **)((u8 *)b + 0x520);
        break;
    }
    first = *(u8 **)table;
    second = (u8 *)table + 8;
    func_800464F0();
    req.tag = 0x21;
    req.f02 = code;
    req.f04 = (s32)first;
    req.f08 = kind;
    req.f0C = (s32)second;
    func_80045BE8(&req);
    c = g_SDValue;
    c->flags_0040 = (c->flags_0040 | 1) & 0xFFFB;
}
```

## `func_80048F14` at 0x80048F14

`gcc_2_8_1_g0`, 62 of 63 instructions.

One instruction short, and it is a register copy: the target materialises
`0x801EA800` in `$a1`, stores it to `music_track`, and then copies it to `$a0`
with `addu $a0, $a1, $zero` for the `0xFFFF` store. Every spelling tried keeps
one register -- two pointer locals, reassigning one local, reusing a single
local for both constants, and pinning the destination to `$4` -- because the
ranges do not overlap and GCC coalesces the copy away.

Everything else is exact, including the four reloads of `music_track` and the
two load-delay `nop`s. Reading the field back into the same local before each
of the four stores is what produces those reloads; caching it once leaves three,
and a `volatile` view of the field does not help.

```c
#include "../../src/types.h"
#include "../../src/psyq/libspu.h"
#include "../../src/game/sound.h"

typedef struct {
    int first;
    int second;
    short third;
    short fourth;
    u8 padC[12];
} Packet;

extern void func_80049594(s32);
extern void func_80049600(s32);
extern void func_80049544(void);

void func_80048F14(void)
{
    Packet packet;
    SDValue *a;
    SDValue *b;
    SDValue *c;
    u16 *p;

    SpuReserveReverbWorkArea(1);
    SpuSetReverb(1);
    packet.first = 7;
    packet.second = 2;
    packet.third = 0x7FFF;
    packet.fourth = 0x7FFF;
    SpuSetReverbModeParam((SpuReverbAttr *)&packet);
    a = g_SDValue;
    a->field_1586 = 0;
    a->field_1588 = 0;
    ((u8 *)a)[0x158A] = 0;
    b = g_SDValue;
    *(s16 *)((u8 *)b + 0x1580) = 0xFF;
    b->field_1584 = 0xFF;
    c = g_SDValue;
    b->field_1582 = 0;
    p = (u16 *)0x801EA800;
    c->music_track = p;
    c->field_1560 = (u8 *)0x801E2000;
    c->field_1578 = -1;
    c->field_157A = -1;
    *(s16 *)((u8 *)c + 0x157C) = -1;
    c->field_157E = -1;
    *p = 0xFFFF;
    p = c->music_track;
    *(s16 *)((u8 *)p + 2) = 0;
    p = c->music_track;
    *(s32 *)((u8 *)p + 4) = 0;
    p = c->music_track;
    *(s32 *)((u8 *)p + 8) = 0;
    p = c->music_track;
    *(s32 *)((u8 *)p + 0xC) = 0x40000;
    func_80049594(2);
    func_80049600(0x14);
    func_80049544();
}
```

## `func_8005B36C` at 0x8005B36C

`gcc_2_8_1_g0`, 91 of 91 instructions, 17 differing positions.

A GPU packet builder in the same family as `func_8005B260`. Rebuilt on the
shape that matched that sibling in #1592: `D_800FE240` declared
`extern u32 * __attribute__((section(".data")))` and indexed directly rather
than cached in a local, `P_TAG` for the length byte, and `addPrim` on a `GsOT`
for the ordering-table link. That took it from 89 of 91 with the opcode
histogram short by two `addu` to the exact count with the whole body after the
prologue already correct.

What is left is the prologue: the three argument copies come out in the order
`table`, `index`, `s` where retail has `s`, `table`, `index`, and the two
`D_800FE240` reads land in one register where retail uses two. Reordering the
assignments makes it worse (47 and 76 differing), and splitting the second read
into its own local is inert.

Four pins are in place and each one corrects an allocation on a sequence that
is otherwise exact: the walking pointer to `$8`, the copy-loop source to `$4`,
the ordering table to `$11` and the index to `$10`. The last two only work when
they are assigned late, immediately before `setlen`; assigning either early
costs 28 or more positions.

The one non-pin lever worth keeping is the `0xE2000000` association. Written
with the constant third in the `|` chain the body is 88 instructions; moving it
to the front or second gives 89 and drops the differing positions from 84 to
69.

```c
#include "../../src/types.h"
#include "../../src/psyq/libgte.h"
#include "../../src/psyq/libgpu.h"
#include "../../src/psyq/libgs.h"

extern u32 *D_800FE240 __attribute__((section(".data")));

void func_8005B36C(u32 *src, GsOT *ot, s32 idx, s32 offx, s32 offy,
                   s32 maskx, s32 masky)
{
    register u32 *s __asm__("$8");
    register u32 *from __asm__("$4");
    register GsOT *table __asm__("$11");
    s32 len;
    s32 i;
    u32 *dst;
    register s32 index __asm__("$10");

    s = src;
    len = ((P_TAG *)s)->len;
    D_800FE240[0] = *s++;
    D_800FE240[1] = 0xE2000000
                  | ((((-maskx) & 0xFF) / 8) & 0x1F)
                  | (((((-masky) & 0xFF) / 8) & 0x1F) << 5)
                  | ((((offx & 0xFF) / 8) & 0x1F) << 10)
                  | ((((offy & 0xFF) / 8) & 0x1F) << 15);
    dst = D_800FE240 + 2;
    from = s;
    for (i = len - 1; i != -1; i--) {
        *dst++ = *from++;
    }
    D_800FE240[len + 2] = 0xE2000000;
    table = ot;
    index = idx;
    setlen(D_800FE240, len + 2);
    addPrim(&table->org[index & 0xFFFF], D_800FE240);
    D_800FE240 = D_800FE240 + (len + 3);
}
```

## `SD_SEPlay` at 0x80048658

`gcc_2_8_1_g0`, 65 of 68 instructions.

Three short. Two are `andi` -- the mask on the volume argument, which the
target computes separately in each of the two call paths where GCC cross-jumps
it into the shared tail, and a redundant `andi $v0, $v1, 0xFF` on a value that
came from `lbu` and is therefore already 8-bit. Neither a `u8` prototype, no
prototype at all, a `u8` temporary nor an explicit cast reproduces the second
one. The third is the `addu $a0, $t0, $zero` copy before the first call.

The four argument copies at entry need `register` pins on `$8`, `$9`, `$6` and
`$7`, in the same style as the matched sibling `src/game/func_80048920.c`, which
shares this function's `(arg0 & 0x8000)` / `(arg0 & 0xF000) == 0x4000`
structure and its `struct SoundState` view.

```c
#include "../../src/types.h"

struct SoundState {
    u8 pad0[0x43C];
    u16 *p43C;
    u8 pad1[0x444 - 0x440];
    u8 *p444;
    u8 pad2[0x44C - 0x448];
    u16 tbl44C[64];
};

extern struct SoundState *g_SDValue;
extern void func_800451E0(s32, s32);
extern void func_800482B0(s32, s32, s32, s32, s32, s32);

void SD_SEPlay(s32 arg0, s32 arg1, s32 arg2)
{
    register s32 id asm("$8");
    register s32 idc asm("$9");
    register s32 vol asm("$6");
    register s32 pan asm("$7");
    s32 lo;
    s32 hi;
    s32 n;
    u8 *e;

    id = arg0;
    pan = arg2;
    idc = id;
    vol = arg1;
    if (id & 0x8000) {
        func_800451E0(id & 0xFFFF, 0);
        return;
    }
    if ((id & 0xF000) == 0x4000) {
        struct SoundState *a = g_SDValue;
        u16 v;

        lo = (id & 0x1F) << 1;
        hi = id & 0x100;
        hi = (hi != 0) << 6;
        v = *(u16 *)((u8 *)a + (lo + hi) + 0x44C);
        if (v == 0xFFFF) {
            return;
        }
        n = a->p43C[v];
        if (n == 0xFFFF) {
            return;
        }
        e = a->p444 + n * 8;
        func_800482B0(v, 0, vol & 0xFF, (s16)pan, e[3], e[2] & 0xFF);
    } else {
        struct SoundState *b = g_SDValue;

        n = b->p43C[idc & 0xFFFF];
        if (n == 0xFFFF) {
            return;
        }
        e = b->p444 + n * 8;
        func_800482B0(idc & 0xFFFF, 0, vol & 0xFF, (s16)pan, e[3], e[2] & 0xFF);
    }
}
```

## `func_80047788` at 0x80047788

`gcc_2_8_1_g8_split`, 55 of 55 instructions, opcode distance 0, 4 differing
positions.

Issues a type-32 sound request for one entry of the `field_0448` table and then
advances `field_0438` by that entry's second word. The `0x51` request built on
the stack afterwards carries the *pre-advance* `field_0438` and the same
`0x801E6800` address the first call is given.

Three things had to be right, and only the first is specific to this function.

**The frame count fixed the divide.** `((field_0002 << 3) + 0x7FF) / 0x800`
written as one expression folds to `(field_0002 + 0xFF) / 0x100` -- identical
arithmetic, one instruction shorter, no `sll` -- which is the reassociation
already recorded under "Split a multiply and a divide across three assignments".
Three assignments to one variable keep the shift, both bias arms and the
`sra $t0, $v0, 11` exactly as retail has them.

**`g_SDValue` needs the `section(".data")` alias**, the same as the
`SpuVoiceAttr` family: the `sound.h` declaration is small enough for `-G8` to
make it gp-relative, and this target uses `lui`/`lw` at all three reads.

**The tail is two statements, not one.** Written as
`c->field_0438 += entry->field_0004;` the build is 54 instructions and reads
`field_0438` before `field_0448`; splitting the entry pointer into its own
statement first gives the exact 55 and retail's read order. That is the same
shape as the per-block-locals rule -- the pointer wants to be a named local at
the point it is used.

What remains is two allocation pairs, both of which reuse an input register as
the destination where this build takes a fresh one: `lw $v1, 0x448($a3)`
followed by `addu $v1, $s0, $v1`, and the tail's `addu $s0, $s0, $v0`.
Assigning the tail's entry back into `off` reproduces the second pair exactly
and costs the prologue's constant placement instead, landing at 5. Crossed
without improving on 4: both operand orders at each of the two sites, a `u8 *`
entry pointer, pinning `off` to `$16`, pinning the entry to `$3`, splitting the
`off` shift, and a named local for the `0x801E6800` constant, which is much
worse at 57 instructions.

```c
#include "../../src/types.h"
#include "../../src/game/sound.h"

extern SDValue *g_SDValue_d asm("g_SDValue") __attribute__((section(".data")));

struct Request {
    u8 tag;
    u8 pad01[3];
    s32 f04;
    s32 f08;
    s32 f0C;
    u8 pad10[0x30 - 0x10];
};

extern void func_800471D0(s32, s32, s32, s32, s32, s32);
extern s32 func_80045BE8(struct Request *);

void func_80047788(s32 arg0)
{
    struct Request req;
    SDValue *a;
    SDValue *b;
    SDValue *c;
    s32 step;
    s32 off;
    SDValueLink *entry;

    a = g_SDValue_d;
    step = (s32)(*(u16 *)((u8 *)a + 2) << 3);
    step = (step + 0x7FF) / 0x800;
    step = step + 1;
    off = (arg0 & 0xFFFF) << 3;
    entry = (SDValueLink *)((u8 *)a->field_0448 + off);
    func_800471D0(a->field_0438, 0x801E6800,
                  step + *(u16 *)entry, entry->field_0004, 0x800, 0x10);
    b = g_SDValue_d;
    req.tag = 0x51;
    req.f04 = b->field_0438;
    req.f0C = 0x801E6800;
    func_80045BE8(&req);
    c = g_SDValue_d;
    entry = (SDValueLink *)((u8 *)c->field_0448 + off);
    c->field_0438 = c->field_0438 + entry->field_0004;
}
```

## `func_80047DB0` at 0x80047DB0

`gcc_2_8_1_g0`, 68 of 69 instructions, one `addu` short.

Keys off whichever of the four voices is playing the given id, clearing its bit
in `+0x434` and calling `func_80047C70` per voice, then issues one `SpuSetKey`
for all of them. Structurally it is `func_80048920`'s twin -- the same
`arg0 & 0x8000` early exit, the same `(arg0 & 0xF000) == 0x4000` table lookup
through `+0x44C`, and the same four-iteration `do`/`while` -- so that matched
file is the right template and its `struct SoundState` view is reused here.

**A known-constant local became a variable shift.** `mask = 1;` written before
the `0x4000` block puts the constant 1 in a register that is still live at
`lo = (in & 0x1F) << 1`, and GCC uses it: the shift comes out `sllv v1,v1,s2`
instead of `sll`. Moving the initialisation after the block removes it. That is
"A known-constant local can become a variable shift amount" in
`matching-evidence.md`, and it is worth knowing it fires across an `if` and not
just adjacent to the shift. It costs nothing to check -- one `sllv` in the
build where the target has `sll` names the cause exactly.

**Two pins hold the argument copies.** The target copies `arg0` into `$a1` and
then `$a1` into `$a0`, keeping the tests on `$a1` while `$a0` receives the
table value in the `0x4000` branch. `register` on `$5` and `$4` reproduces the
first copy; the second is the one still missing, and it is the same redundant
round trip that GCC coalesces in this family.

Crossed without producing the second copy: assigning both from the parameter
rather than in sequence, swapping the declaration order, and an
`__asm__ volatile("" : "+r"(...))` barrier on either variable. A barrier placed
on `value` inside the `0x4000` branch does reach 69 of 69, but it adds a
different instruction rather than that copy, and statement-level inline
assembly is rejected at integration in any case, so it is not stored here.

```c
#include "../../src/types.h"
#include "../../src/psyq/libspu.h"

struct SoundState {
    u8 pad0[0x404];
    u16 ids[4];
    u8 pad1[0x434 - 0x40C];
    u8 active;
    u8 pad2[0x44C - 0x435];
    u16 tbl44C[64];
};

extern struct SoundState *g_SDValue;
extern void func_800464F0(void);
extern void func_80045114(void);
extern void func_80047C70(s32);

void func_80047DB0(s32 arg0)
{
    register s32 i __asm__("$16");
    register s32 bit __asm__("$17");
    register s32 mask __asm__("$18");
    register s32 keys __asm__("$19");
    register s32 id __asm__("$20");
    s32 lo;
    s32 hi;
    register s32 value __asm__("$4");
    register s32 in __asm__("$5");

    in = arg0;
    value = in;
    if (in & 0x8000) {
        func_800464F0();
        func_80045114();
        return;
    }
    if ((in & 0xF000) == 0x4000) {
        struct SoundState *a = g_SDValue;
        u16 v;

        lo = (in & 0x1F) << 1;
        hi = in & 0x100;
        hi = (hi != 0) << 6;
        v = *(u16 *)((u8 *)a + (lo + hi) + 0x44C);
        if (v == 0xFFFF) {
            return;
        }
        value = v;
    }
    mask = 1;
    keys = 0;
    bit = 0x100000;
    i = keys;
    id = value & 0xFFFF;
    do {
        struct SoundState *b = g_SDValue;

        if (b->ids[i] == id) {
            keys |= bit;
            b->active = b->active & ~mask;
            func_80047C70(bit);
        }
        mask <<= 1;
        i++;
        bit <<= 1;
    } while (i < 4);
    if (keys != 0) {
        SpuSetKey(0, keys);
    }
}
```
