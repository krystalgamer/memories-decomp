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

`gcc_2_8_1_cc_g8_as_g0_split`, 23 of 23 instructions, opcode distance 0, 6
differing positions.

Same 6 as the previous record, but in plain C and in the same shape as its
sibling `func_8004A6F8`. Two things were wrong with the stored source rather
than with the result.

The `section(".data")` attribute was doing nothing: GCC reports "section
attribute ignored for uninitialized variable" for it, so the addressing it was
credited with came from the profile, not the attribute. Using `D_8009B458`
straight from `sound.h`, as the matched sibling `func_8004A7C0` does, reaches
the identical 6.

More importantly the `extern ... asm("D_8009B458")` alias made this candidate
impossible to ship. `record_external_attempt.py` rejects any asm extension, and
`--allow-register-pins` only rewrites `register x asm("$n")` pins, not symbol
aliases, so the stored source would have been refused at the moment it finally
matched. Any candidate carrying an alias has the same latent problem.

The store order fix found for `func_8004A6F8` applies here too: hoisting the
table word into a local and writing the mask at `+0x4C4` before the voice word
at `+0x4C0` reaches 6 from this shape as well, so both siblings now share one
source shape and one residual.

That residual is byte-identical between the two functions, at the same six
positions, and the multiset is exact, so it is placement only. The target
orders the prologue as [complete table address] [load state] [read table] and
emits `ori` before `sw $ra`; both candidates load the state first.

This is not reachable by reordering statements, and that is now a measured
fact rather than a guess. Under `gcc_2_8_1_g8_no_sched2` and
`gcc_2_8_1_g0_no_sched2` the natural order is identical for every source shape
tried -- `sw $ra`, the mask `lui`, the `sll`, the state load, the table
address, the table read, the `ori` -- including the variants that split the
table access into a separate address local. Per the rule recorded in
matching-evidence, an order that does not move under `no_sched2` will not move
by rewriting the statements, so the next attempt should look for something that
changes what the back end produces, not the order it is written in.

Crossed without improving on 6, across sweeps of 420, 624, 64 and 770 variants
plus a full profile sweep of all 29 profiles on both siblings: statement order
with the value local at every legal position, the mask inline against a named
local, `s32`/`int` element types, `s32`/`u32` values, index spellings by array,
by pointer, by multiply and by shift, splitting the table access into a
separate `s32 *`, `u8 *` or offset local, and four spellings of the state
pointer including none at all. The address split is consistently worse at 9.

```c
#include "../../src/types.h"
#include "../../src/psyq/libspu.h"
#include "../../src/game/sound.h"

extern s32 D_80011434[];

#define MASK (SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_ADSR1 | \
              SPU_VOICE_ADSR_ADSR2)

void func_8004A764(s32 index)
{
    u8 *p;
    s32 value;
    p = (u8 *)D_8009B458;
    value = D_80011434[index];
    *(u32 *)(p + 0x4C4) = MASK;
    *(u32 *)(p + 0x4C0) = value;
    *(u16 *)(p + 0x4FA) = 0;
    *(u16 *)(p + 0x4FC) = 0;
    *(s32 *)(p + 0x4E4) = SPU_VOICE_EXPIncN;
    SpuSetVoiceAttr((SpuVoiceAttr *)(p + 0x4C0));
}
```

## `func_8004A6F8` at 0x8004A6F8

`gcc_2_8_1_cc_g8_as_g0_split`, 27 of 27 instructions, opcode distance 0, 6
differing positions.

The second member of the `SpuVoiceAttr` family at `+0x4C0`. It copies `adsr1`,
`adsr2` and `a_mode` out of a second argument at `+0x20`, `+0x22` and `+0x24`
rather than using constants, which is what the `0x60100` mask says it does.
The argument is not a Psy-Q `VagAtr` -- that struct is `0x20` bytes and its
`adsr1` is at `+0x10` -- so it stays a `u8 *` with hex offsets until something
names it.

Two things brought this from 8 to 6, and they only work together. Hoisting the
table word into a local and storing the mask at `+0x4C4` before the voice word
at `+0x4C0` reproduces both stores exactly. The previous entry recorded that
writing `mask` first costs 11, and that is true while the table read is left
inline in the voice store; once the read is a named local, mask-first is the
better of the two. The two stores now agree, so that negative should not be
carried forward on its own.

Neither the `section(".data")` alias nor an `asm` label is needed. The matched
sibling `func_8004A7C0` uses `D_8009B458` straight from `sound.h`, and doing
the same here reaches the same 6 while keeping the source plain C, which is
also what the attempt recorder requires.

What remains is positions 5 to 10, and the multiset there is already exact, so
it is placement only. The target completes the `D_80011434` address before
loading `D_8009B458` and emits `ori $a2` before `sw $ra`; this build loads
`D_8009B458` first and sinks the `ori` one position later. `no_sched2` shows
the natural order puts `sw $ra` first and the state load ahead of the table
address, so sched2 is what moves them and the source is not choosing it.

Crossed without improving on 6, on this base rather than the old one: five
statement orders including the value local at every position, the mask inline
against a named local introduced at four points, `s32`/`int` table element
types, `s32`/`u32` for the value, three index spellings including the multiply
that closed `func_80047788`, `u8 *` against `SDSecondaryState *` against
`SpuVoiceAttr *` against no pointer local at all, and six profiles, across
sweeps of 420, 624 and 64 variants.

This is the same residual as `func_8004A764`, which also sits at 6 with the
multiset exact, so one prologue insight would close both.

```c
#include "../../src/types.h"
#include "../../src/psyq/libspu.h"
#include "../../src/game/sound.h"

extern s32 D_80011434[];
#define MASK (SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_ADSR1 | SPU_VOICE_ADSR_ADSR2)

void func_8004A6F8(s32 index, u8 *tone)
{
    u8 *p;
    s32 value;
    p = (u8 *)D_8009B458;
    value = D_80011434[index];
    *(u32 *)(p + 0x4C4) = MASK;
    *(u32 *)(p + 0x4C0) = value;
    *(u16 *)(p + 0x4FA) = *(u16 *)(tone + 0x20);
    *(u16 *)(p + 0x4FC) = *(u16 *)(tone + 0x22);
    *(u32 *)(p + 0x4E4) = *(u16 *)(tone + 0x24);
    SpuSetVoiceAttr((SpuVoiceAttr *)(p + 0x4C0));
}
```

## `func_8004A27C` at 0x8004A27C

`gcc_2_8_1_cc_g8_as_g0_split`, 31 of 31 instructions, opcode distance 0, 9
differing positions.

The third member of the `SpuVoiceAttr` family, and the one whose field names
the reading confirms most directly: the mask is `0xF`, which is
`SPU_VOICE_VOLL | VOLR | VOLMODEL | VOLMODER`, and the four fields written are
`+0x4C8`/`+0x4CA` and `+0x4CC`/`+0x4CE`, which are `volume.left`/`right` at
`+0x08`/`+0x0A` and `volmode.left`/`right` at `+0x0C`/`+0x0E`. So this is a
volume set, the other two are ADSR sets, and the `0x4C0` window is one
`SpuVoiceAttr` shared by all three.

The two volumes are `(x * state[0x514]) >> 7` and `(y * state[0x516]) >> 7`,
a fixed-point scale by a pair of halfwords rather than a signed division --
there is no rounding bias in the target.

Same 9 as the previous record, but in plain C. The stored source carried an
`extern ... asm("D_8009B458")` alias, which `record_external_attempt.py`
rejects outright: `--allow-register-pins` only rewrites `register x asm("$n")`
pins, not symbol aliases, so that source could not have been promoted at the
moment it matched. Using `D_8009B458` from `sound.h`, as the matched sibling
`func_8004A7C0` does, reaches the identical 9. All three siblings are now
stored in a form that can actually be shipped.

What remains is allocation, not order. The schedule already agrees: the target
loads the second scale into `$a1`, reusing the register that held `x` once the
first multiply has consumed it, and lands the two products in `$a3` and `$t1`;
this build loads the scale into `$v0` and lands the products in `$a1` and
`$t0`. The `lhu` and `mflo` sit at the same positions in both. Separately the
target emits the `0x4C0` store two positions earlier, before the `0x4C8` pair
rather than after.

The allocation is fixed before scheduling and does not depend on the source.
Under `gcc_2_8_1_g8_no_sched2` every shape tried produces the identical natural
form -- both scales loaded into `$v0`, then `mflo $t0` and `mflo $a1` -- so by
the rule recorded in matching-evidence this will not move by rewriting
statements. The next attempt should change what the back end produces rather
than the order it is written in.

Crossed without improving on 9, across a sweep of 1920 variants: all 24
orderings of the four `0x4C0` window stores, the table word inline against a
named local, the two products inline against named locals, the two scales
inline against named locals, `>> 7` against `/ 128`, and five profiles. The
mask-before-voice ordering that closed `func_8004A6F8` and `func_8004A764` is
inert here, which is worth knowing: the family shares a shape but not this
lever.

```c
#include "../../src/types.h"
#include "../../src/psyq/libspu.h"
#include "../../src/game/sound.h"

extern s32 D_80011434[];

#define MASK (SPU_VOICE_VOLL | SPU_VOICE_VOLR | \
              SPU_VOICE_VOLMODEL | SPU_VOICE_VOLMODER)

void func_8004A27C(s32 index, s32 x, s32 y)
{
    u8 *p;
    p = (u8 *)D_8009B458;
    *(u16 *)(p + 0x4CC) = 0;
    *(u16 *)(p + 0x4CE) = 0;
    *(u32 *)(p + 0x4C0) = D_80011434[index];
    *(u32 *)(p + 0x4C4) = MASK;
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

`gcc_2_8_1_g0`, 63 of 63 instructions, opcode distance 0, 28 differing
positions.

Supersedes the 62-instruction record. The missing instruction was the register
copy `addu $a0, $a1, $zero`, and the previous entry read it as a copy that
coalescing kept removing. It is not a copy in the source at all -- it is a
*read*.

The target reads `c->music_track` back five times: once before the `0xFFFF`
store and once before each of the four that follow. Only the last four are
loads. The first is emitted as a register copy because GCC forwards the value
it has just stored to that field, and the four after it cannot be forwarded
because the intervening store *through* the pointer may alias the field
itself. So the shape that produces the copy is an ordinary fifth read, written
through a second local so it is not confused with the constant already held.

That is worth carrying: when one redundant-looking `move` is missing and the
same field is reloaded nearby, look for a read that was folded into a copy
rather than for a way to defeat coalescing. Reaching for a barrier or a
register pin cannot produce it, which is why every spelling tried before kept
one register.

The instruction multiset is now exact. What remains is placement of the two
large constants: the target builds `0x801EA800` before the first `g_SDValue`
load and splits `0x801E2000` with its `lui` at `0x4c` and its `ori` at `0x78`,
while this build loads `g_SDValue` first and materialises both constants after.
The three `g_SDValue` reloads then land in different registers, which is what
most of the 28 positions are.

Crossed without improving on 28, over 100 variants: introducing the
`0x801EA800` constant at four points from the top of the function to just
before its use, the same four positions for `0x801E2000` as a named local, that
constant written inline instead, and five profiles. Statement position does not
move either constant, so the next attempt needs something that changes what the
back end emits rather than where the assignment is written.

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
    u16 *q;

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
    q = c->music_track;
    *q = 0xFFFF;
    q = c->music_track;
    *(s16 *)((u8 *)q + 2) = 0;
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

`gcc_2_8_1_g0`, 91 of 91 instructions, opcode distance 0, 17 differing
positions.

A GPU packet builder in the same family as `func_8005B260`. Built on the shape
that matched that sibling in #1592: `D_800FE240` declared
`extern u32 * __attribute__((section(".data")))` and indexed directly rather
than cached in a local, `P_TAG` for the length byte, and `addPrim` on a `GsOT`
for the ordering-table link.

Same 17 as the previous record, with two of the four pins removed. The entry
required the ordering table in `$11` and the index in `$10`, and recorded that
either one assigned early costs 28 or more positions. Neither pin is needed:
with both left free, `index` assigned early and `table` assigned late, the
build reaches the identical 17 at distance 0. Only the walking pointer in `$8`
and the copy-loop source in `$4` still earn their place.

The early-assignment cost was real but belonged to the pins rather than to the
placement. Holding `$10` across the body is what the 28 measured; once the
index is an ordinary local the allocator places it freely and early assignment
is free. Worth keeping in mind generally -- a pin and a statement position are
not independent axes, and a cost measured for one while the other is fixed can
be charged to the wrong one.

The one non-pin lever remains the `0xE2000000` association. Written with the
constant third in the `|` chain the body is 88 instructions; moving it to the
front or second gives 89.

What is left is two allocation facts. The target emits the argument copies as
`s`, `table`, `index`, interleaving the `lbu` and `lw` of the packet tag
between the first pair and the third, where this build emits them back to
back. And the target reads `D_800FE240` into `$v1` and then into `$a2`, where
this build reloads into `$a1` both times. The instruction multiset is exact,
so neither is structural.

Crossed without improving on 17, over 144 variants: three pins for the table
including none, three for the index including none, four placements of the two
assignments, the second `D_800FE240` read as its own local against indexed
directly, and two profiles. Splitting the pair so the table goes early costs
22; keeping both late with the table pinned is what holds 17.

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
    GsOT *table;
    s32 len;
    s32 i;
    u32 *dst;
    s32 index;

    s = src;
    index = idx;
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
    setlen(D_800FE240, len + 2);
    addPrim(&table->org[index & 0xFFFF], D_800FE240);
    D_800FE240 = D_800FE240 + (len + 3);
}
```

## `SD_SEPlay` at 0x80048658

`gcc_2_8_1_g0`, 66 of 68 instructions, opcode distance 2.

Supersedes the 65-instruction record at distance 3. Two changes account for the
gain, and the opcode census now names exactly what is left: one `addu` and one
`andi` missing, with nothing extra.

**A temporary for `e[2]` keeps its mask.** The sixth argument is
`e[2] & 0xFF` on a value that came from `lbu` and is therefore already 8-bit,
so GCC folds the mask away. Reading the byte into an `s32` local first and
masking the local restores it. The previous entry recorded a `u8` prototype, no
prototype, a `u8` temporary and an explicit cast as all failing, and they do --
the width of the temporary is not the lever. What matters is that the value is
read into a separate variable before the mask, which is the same shape that
supplied the missing copy in `func_80048F14`.

**Masking `vol` in place inside each arm.** Written as `vol & 0xFF` at both
call sites, GCC cross-jumps the identical argument setup into a shared tail and
emits one mask; the target has one in each arm. Writing `vol &= 0xFF;` as a
statement in each branch changes it from an expression into a side effect on a
pinned register, which the two arms no longer share.

The four argument copies at entry still need `register` pins on `$8`, `$9`,
`$6` and `$7`, in the same style as the matched sibling
`src/game/func_80048920.c`, which shares this function's `(arg0 & 0x8000)` and
`(arg0 & 0xF000) == 0x4000` structure and its `struct SoundState` view.

What remains is one `addu` and one `andi`. The `addu` is the copy at `0x20`:
the target moves `id` into `$a0` and then masks `$a0` in place for the
`func_800451E0` call, where this build emits `andi $a0, $t0, 0xFFFF` in one
instruction. Introducing a named local for that argument and masking it in
place does not reproduce it, so it is the same redundant-move class as
`func_80047DB0` rather than a spelling of the mask.

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
    s32 t2;

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
        t2 = e[2];
        vol &= 0xFF;
        func_800482B0(v, 0, vol, (s16)pan, e[3], t2 & 0xFF);
    } else {
        struct SoundState *b = g_SDValue;

        n = b->p43C[idc & 0xFFFF];
        if (n == 0xFFFF) {
            return;
        }
        e = b->p444 + n * 8;
        t2 = e[2];
        vol &= 0xFF;
        func_800482B0(idc & 0xFFFF, 0, vol, (s16)pan, e[3], t2 & 0xFF);
    }
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

## `func_80025028` at 0x80025028

`gcc_2_8_1_g8_split`, 40 of 40 instructions, opcode distance 0, 2 differing
positions.

Searches one side's five-slot row for an occupied card whose `+0x0C` field
equals the argument, and on a hit records the argument in `D_8009B22A`, copies
the owning object's `+0x6A` byte to `D_8009B1B8`, and returns the argument.
Returns 0 when no slot matches. `D_8009B22A` is cleared before the loop, so a
miss leaves it zero.

Nothing new is declared. All five globals already exist in sibling sources, the
record stride is `DUEL_CARD_RECORD_SIZE`, the row stride is
`DUEL_FIELD_SIDE_GRID_SLOT_COUNT`, the trip count is `DUEL_FIELD_ROW_SIZE` and
the tested bit is `DUEL_CARD_FLAG_OCCUPIED`, each matching the emitted constant
exactly. The record is a `u8 *` with hex offsets, the view
`duel_card_turn_animations.c` takes of `D_801A7AD8`.

The row index has to be written `D_800907D8[i + base]`; written `[base + i]`
both `addu` come out with the operands reversed, which is the grouping rule
recorded in matching-evidence.

**Two delay slots decide the rest, and `volatile` is what holds one open.**
The hit block is five instructions -- `lw`, `sh`, `lbu`, `nop`, `sb` -- and the
question is only what fills each load's delay. Reading the record pointer
before storing `D_8009B22A` lets the `sh` fill the `lw` slot, which is the
target's order, but GCC then fills the `lbu` slot with the following `sb` and
the build is 39 instructions. Declaring `D_8009B1B8` `volatile` stops that
store being moved into a delay slot, so the `nop` survives and the count is
exact. `extern volatile` is already used this way in
`dialog_read_choice_input.c` and `display_object_fade_callbacks.c`.

What remains is two positions, and they are the same swap seen from the other
side: GCC fills the `lbu` delay with the return value's `addu $v0, $a0, $zero`
and leaves `jr`'s slot a `nop`, where retail pads the `lbu` and puts the return
value in `jr`'s slot. Something has to occupy the `lbu` slot so the return copy
survives for the branch.

Crossed without improving on 2: `volatile` on the record pointer, on the byte
read and on the `D_8009B22A` store, the return value through a temporary
assigned before or after the block, and all 29 profiles. Only the `D_8009B1B8`
`volatile` moves anything.

```c
#include "../../src/types.h"
#include "../../src/game/duel_card_layout.h"
#include "../../src/game/duel_grid.h"

extern u8 D_8009B1D5;
extern u8 D_800907D8[];
extern u8 D_801A7AD8[];
extern  s16 D_8009B22A;
extern volatile u8 D_8009B1B8;

s32 func_80025028(s32 arg0)
{
    s32 i;
    s32 base;
    u8 *e;
    u8 *p;

    base = D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    D_8009B22A = 0;
    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        e = D_801A7AD8 + D_800907D8[i + base] * DUEL_CARD_RECORD_SIZE;
        if (*(u16 *)(e + 0x16) & DUEL_CARD_FLAG_OCCUPIED) {
            if (*(s16 *)(e + 0xC) == arg0) {
                p = *(u8 **)e;
                D_8009B22A = arg0;
                D_8009B1B8 = p[0x6A];
                return arg0;
            }
        }
    }
    return 0;
}
```

## `func_80012DB4` at 0x80012DB4

`gcc_2_8_1_g8_split`, 42 of 42 instructions, opcode distance 0, 9 differing
positions.

The frame-sync step. Calls `DrawSync(0)` unless bit `0x8000` of `D_8009B098`
is set, spins until `D_8009B0C8` reaches `D_8009B0C0`, clamps the low byte of
`D_8009B0C8` into `D_8009B0C1` at 1, publishes `D_8009B0C1 + 1` to
`D_8009B0D8`, writes either 2 or that count to `D_8009AFA3` depending on
`D_8009AFA4`, clears `D_8009AFA4`, resets `D_8009B0C8` to -1, calls `VSync(0)`
and bumps the frame counter `D_8009B0CC`.

Two things were needed for the exact count, both already recorded levers.

**The spin loop needs `volatile` on both globals it tests.** `D_8009B0C0` and
`D_8009B0C8` are re-read on every iteration, which only happens if the
compiler is forbidden from hoisting them; without it the loop reads them once.

**`D_8009AFA3` and `D_8009AFA4` need `section(".data")`.** Under `-G8` both are
small enough to become gp-relative, and the target reaches both through
`lui`/`lbu` and `lui`/`sb`. This is the addressing lever in
matching-evidence, and it is what took the build from 39 instructions to the
exact 42. Note the plain `extern volatile unsigned char D_8009AFA3` used by
`func_80058E1C.c` is not enough on its own here.

Everything else reuses existing declarations; only `D_8009B0C1` had none.
`D_8009B0C8` is declared as an array elsewhere but is a signed scalar here --
the comparison is `slt`, so the type is signed.

What remains is nine positions with the multiset already exact, so it is
placement only. The clamp block wants a `nop` in the load-delay slot before
`sb $v0, D_8009B0C1` where this build fills it with the store itself, and the
`D_8009AFA3` value sits in `$a1` in the target against `$v0` here, which shifts
the surrounding constant materialisation.

```c
#include "../../src/types.h"
#include "../../src/psyq/libgte.h"
#include "../../src/psyq/libgpu.h"
#include "../../src/psyq/libetc.h"

extern u16 D_8009B098;
extern volatile u8 D_8009B0C0;
extern volatile s32 D_8009B0C8;
extern u8 D_8009B0C1;
extern s32 D_8009B0D8;
extern volatile unsigned char D_8009AFA3 __attribute__((section(".data")));
extern unsigned char D_8009AFA4 __attribute__((section(".data")));
extern s32 D_8009B0CC;

void func_80012DB4(void)
{
    s32 v;

    if ((D_8009B098 & 0x8000) == 0) {
        DrawSync(0);
    }
    while (D_8009B0C8 < D_8009B0C0) {
    }
    v = D_8009B0C8;
    D_8009B0C1 = v;
    if ((v & 0xFF) != 0) {
        D_8009B0C1 = 1;
    }
    D_8009B0D8 = D_8009B0C1 + 1;
    if (D_8009AFA4 != 0) {
        D_8009AFA3 = 2;
    } else {
        D_8009AFA3 = D_8009B0D8;
    }
    D_8009AFA4 = 0;
    D_8009B0C8 = -1;
    VSync(0);
    D_8009B0CC = D_8009B0CC + 1;
}
```


## `func_8002FD10` at 0x8002FD10

`gcc_2_8_1_g8_split`, 112 of 112 instructions, opcode distance 0, 8 differing
positions.

The card-art loader. It stores its argument in `D_8009B2A4`, zeroes both
viewport halves, requests the 0x1E57/0x31 async transfer with `func_8002FB78`
as the completion callback, clears three 0x14-byte records at `D_800EAE98` and
marks the fourth `-1`, waits on `IsIdleGPU(10)`, then walks 25 tiles uploading
two VRAM rectangles each -- the first stepping across a 5-wide grid of 24x48
cells from x=0x380, the second stepping down a 0x40-wide column from y=0xF0.

All six canonical attempts were the automatic first pass; none of them was a
reconstruction, and the best of them differed at +0x0. Everything below is new.

**The whole body is exact.** The eight differing positions are all in the
prologue, at +0x4 through +0x20, and they are one permutation of one window:

```
target   sh a0,gp / move a0,0 / move a1,a0 / lui v0,fn / addiu v0,fn / sw s0 / lui s0,D / addiu s0,s0,D
build    lui v0,D / sw s0 / addiu s0,v0,D / sh a0,gp / move a0,0 / move a1,a0 / lui v0,fn / addiu v0,fn
```

Same instructions, same count, same multiset. Two facts decide it and they are
worth separating, because chasing either one alone wastes a sweep:

- **The register.** The target forms `&D_800EAE98` as `lui $s0` then
  `addiu $s0, $s0` -- the `HIGH` temporary coalesced with the destination. Under
  every `_split` profile this build emits `lui $v0` then `addiu $s0, $v0`. That
  is not a scheduling artefact: it holds under `gcc_2_8_1_g0_split_no_sched1`
  and `gcc_2_8_1_g0_no_sched2_split` too.
- **The order.** Because the uncoalesced form leaves the `HIGH` in `$v0`, and
  the callback address also wants `$v0`, there is an anti-dependence between the
  two pairs, and the scheduler has to run the `D_800EAE98` pair first. Fix the
  register and the order should follow; fix the order without the register and
  it cannot.

`_split` is not optional. Non-split profiles do coalesce the pair into one
register, which is the shape wanted here, but they then emit only one
`%lo(D_800E9D70)` -- the address computation -- and lose the target's
`sh $v1, %lo(D_800E9D70)($s3)`, which needs the `HIGH` kept in a register and
reused as a store base. So the function needs split addressing for one global
and coalescing for another, and no profile currently gives both.

Crossed without moving it, about thirty-five variants: five placements of the
`slot = D_800EAE98` assignment, all twenty-four declaration orders of the four
locals, `extern u8 D_800EAE98[]` with a cast against `extern Slot D_800EAE98[]`
against a sized `[4]` against a `.data` section attribute, `register` pins on
`slot`/`i`/`src` singly and together, the callback passed as `func_8002FB78`
against `&func_8002FB78` against a named local, `volatile` on four subsets of
the global stores, array subscripting instead of the walking pointer, and every
profile.

Three things that *were* load-bearing and should not be rewritten:

- `D_80010000` and both `gGraphics_sViewport` halves need
  `__attribute__((section(".data")))`. Declared plainly they go gp-relative
  under `-G8`; the target addresses all three with `%hi`/`%lo`. The viewport
  pair additionally has to be a **scalar**, not `extern u16 x[]`: the array
  spelling makes GCC materialise the address into a register, and the target
  uses the assembler macro form through `$at`.
- The second rectangle needs a pointer local for its `y`/`w`/`h` initialisers
  while `x` is written through the array. Writing all four through the array is
  one instruction short; writing all four through the pointer is the same 112
  but reorders the stores.
- The tile loop increments `i` **in the middle of the body**, between the two
  `LoadImage2` calls and the grid arithmetic, so the rectangle positions are
  computed from the incremented value. Written as a `for` with the increment in
  the header the count still comes out 112 but every instruction from the
  multiply onwards shifts by one.

```c
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

typedef struct {
    s32 unk00;
    s16 unk04;
    s16 unk06;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
} Slot;

extern u16 D_8009B2A4;
extern void *D_8009B2A0;
extern s16 gGraphics_sViewportX __attribute__((section(".data")));
extern s16 gGraphics_sViewportY __attribute__((section(".data")));
extern s32 D_80010000 __attribute__((section(".data")));
extern RECT D_800E9D70[];
extern Slot D_800EAE98[];

extern void File_RequestAsyncTransfer(s32, s32, s32, s32, void *, s32, s32);
extern void func_8002FB78(void);
extern void func_80039E9C(void);
extern void func_800137E4(void);
extern void *func_8002E3FC(void);

void func_8002FD10(s16 arg0)
{
    Slot *slot;
    u32 *src;
    RECT *second;
    s32 i;

    slot = D_800EAE98;
    D_8009B2A4 = arg0;
    gGraphics_sViewportY = 0;
    gGraphics_sViewportX = 0;
    D_8009B2A0 = 0;
    File_RequestAsyncTransfer(0, 0, 0x1E57, 0x31, func_8002FB78, 0, 0);
    func_80039E9C();

    *(s16 *)&slot[3] = -1;
    for (i = 0; i < 3; i++) {
        slot->unk00 = 0;
        slot->unk04 = 0;
        slot++;
    }
    func_800137E4();

    while (IsIdleGPU(10)) {
        ;
    }

    src = (u32 *)D_80010000;
    D_800E9D70[0].x = 0x380;
    D_800E9D70[0].y = 0;
    D_800E9D70[0].w = 0x18;
    D_800E9D70[0].h = 0x30;
    second = &D_800E9D70[1];
    D_800E9D70[1].x = 0x380;
    second->y = 0xF0;
    second->w = 0x40;
    second->h = 1;

    i = 0;
    do {
        LoadImage2(&D_800E9D70[0], src);
        LoadImage2(&D_800E9D70[1], src + 0x240);
        i++;
        D_800E9D70[1].y = D_800E9D70[1].y + 1;
        D_800E9D70[0].x = (i % 5) * 24 + 0x380;
        D_800E9D70[0].y = (i / 5) * 48;
        if (D_800E9D70[1].y >= 0x100) {
            D_800E9D70[1].x = D_800E9D70[1].x + 0x40;
            D_800E9D70[1].y = 0xF0;
        }
        src += 0x260;
    } while (i < 25);

    D_8009B2A0 = func_8002E3FC();
}
```


## `func_8004A0FC` at 0x8004A0FC

`gcc_2_8_1_g0`, 96 of 96 instructions, opcode distance 0, 39 differing
positions, first difference at +0xA4.

Sound spatialisation for one secondary object, called from
`func_8004A2F8` as `func_8004A0FC(state + object_offset, state + value * 24)`,
so its two arguments are an `SDSecondaryObject` and an `SDSecondaryRecord`. It
forms a pan value, clamps it to 0..0x7F, derives a level from the transfer and
record scalars, splits the level into left and right channels about the 0x40
centre, and writes both back scaled by the two `field_07E4`/`field_07E6` trims
and the object's 7-bit `field_000E`.

All six canonical attempts were profile sweeps of one reference source that is
not in this tree; none was a reconstruction and the best differed at +0x0.
Everything below is new, and the first 41 instructions are byte-exact.

**Field names.** This entry needs eight members that `sound.h` had inside
`SDSecondaryObject`'s pads, so the pads are split in this branch: `field_0008`
and `field_0009` are the two level multipliers, `field_000A`/`field_000B` the
two pan contributions, `field_000C` the clamped pan, `field_000E` the 7-bit
output trim, and `field_0014`/`field_0016` the two `u16` outputs. The struct is
still 0x28 and `make match` is unchanged.

**What is settled.** Four shapes were each worth a measurable step and should
not be rewritten:

- The state pointer must **not** be a local. `SDSecondaryState *state =
  D_8009B458;` puts the pointer in `$a0`, which forces the parameter copy
  `addu $a3, $a0, $zero` to the top of the function; the target has that copy in
  the first branch's delay slot. Writing `D_8009B458->...` at each of the three
  places emits the same three loads and fixes the whole prologue. Worth 36 of
  the 91 positions the first reconstruction had.
- The pan test is `if (state->field_0815 != 0)`, and the three-way channel split
  has to be spelled `if (pan >= 0x40) { if (pan == 0x40) ... } else { ... }`.
  GCC emits `slti $v0, $a2, 0x40` either way, but only this nesting puts the
  low-pan arm out of line, which is where the target has it.
- Inside the `>= 0x40` arms, `right` is assigned **before** `left`, and the
  equal case is `left = right`, not `left = level`. That is what lets the
  `right = level` copy sink into the `bne` delay slot and be shared by both
  arms.
- The level chain is three statements: the four-factor product, then
  `((level >> 14) * field_0008 * field_0009) >> 14`. Splitting the shift into
  its own statement, or folding the whole chain into one expression, each costs
  two instructions.

**What is left** is one register-allocation family. At +0xA4 the target takes
the four-factor product into `$v1` and shifts it in place; this build takes it
into `$a0` and shifts into `$v0`. Every later difference follows from that: the
level chain, the `left`/`right` pair, and the two output chains are all the same
instructions with the same operands in permuted registers, which is why the
opcode distance is 0.

The tail has a second, smaller consequence. The target issues both
`left * field_07E4` and `right * field_07E6` before scaling either, so both
`lbu 0xE($a3)` reads happen before the first `sh`. Written as two independent
statements this build emits them sequentially; written as two temporaries stored
afterwards, GCC common-subexpressions the two `field_000E & 0x7F` reads into one
and the function comes out three instructions short. The target has both reads
and no CSE, so the sequential spelling is right and the interleave has to come
from the scheduler.

Crossed without moving it, about forty variants: five spellings of the state
pointer, eight arrangements of the three-way split, six of the level chain,
`register` pins on `level`/`left`/`right`/`pan` singly and in combination (every
pin removes a copy the target has and comes out short), two spellings of the
output tail, and all eighteen viable profiles -- every one of which measures the
same 45 or worse, with `gcc_2_8_1_g0` and its cohort tied at 39.

```c
#include "../types.h"
#include "sound.h"


void func_8004A0FC(SDSecondaryObject *object, SDSecondaryRecord *record)
{
    s32 pan;
    s32 level;
    s32 left;
    s32 right;

    if (D_8009B458->field_0815 != 0) {
        pan = 0x40;
    } else {
        pan = D_8009B458->transfer.field_001B + object->field_000A + object->field_000B
            + record->field_0001 - 0xC0;
    }
    if (pan < 0) {
        pan = 0;
    }
    if (pan >= 0x80) {
        pan = 0x7F;
    }
    object->field_000C = pan;

    level = D_8009B458->transfer.field_0018 * (u16)D_8009B458->field_0512
          * record->field_0005 * record->field_0003;
    level = ((level >> 14) * object->field_0008 * object->field_0009) >> 14;

    if (pan >= 0x40) {
        if (pan == 0x40) {
            right = level;
            left = right;
        } else {
            right = level;
            left = ((0x40 - (pan & 0x3F)) * (level * 2)) >> 7;
        }
    } else {
        left = level;
        right = (pan * (level * 2)) >> 7;
    }

    object->field_0014 = (((left * D_8009B458->field_07E4) >> 7) * (object->field_000E & 0x7F)) >> 7;
    object->field_0016 = (((right * D_8009B458->field_07E6) >> 7) * (object->field_000E & 0x7F)) >> 7;
}
```

## `func_8002E5AC` at 0x8002E5AC

`gcc_2_8_1_g8_split`, 68 instructions against a target of 67, opcode
distance 3.

The script opcode that opens a dialogue box. When `func_8002E3B4` reports
idle it reads a little-endian `u16` out of the `D_8009B290` script stream,
advances the stream by two, sets bit `0x4000` of `D_8009B2A4`, calls
`func_8003B6AC(0)`, and creates a text box with the low 12 bits as its id.
The new object gets bit `8` at `+0x34`; when the script word has bit `0x8000`
the box is instead marked in `D_8009B27C` and that bit is cleared again. The
other path is the teardown: it returns while `D_8009B2A4` still has `0x4000`,
otherwise destroys the box unless `D_8009B27C` has `0x4000`, and clears both
`D_8009B28C` and `D_8009B27C`.

The stream read reuses the idiom from the matched neighbour
`func_8002E470.c`, which shares the `func_8002E3B4` guard and the same
`D_8009B290` cursor: take the cursor into a local, form the next pointer as
its own local, publish that, then index the original. Every global except
`D_8009B28C` already had a declaration in a sibling.

What remains is one instruction and three opcodes: one `addu` missing against
two extra `nop`. Note that `nop` is `sll $zero, $zero, 0`, so an opcode census
reports these as extra `sll` -- they are padding, not arithmetic.

Both differences are the same fact. The target fills the load-delay slot after
`lw $v1, D_8009B290` by splitting the stream advance into
`addiu $a1, $zero, 0x2` and `addu $v0, $v1, $a1`, spending an extra
instruction to keep the slot busy; this build emits `addiu $v0, $v1, 2` and
pads with `nop`. Giving the 2 its own local does not reproduce the split, and
neither does advancing the cursor inline.

Crossed without improving on distance 3: the stream advance as a named local,
inline, and through a separate next pointer; the `+0x34` read-modify-write
with and without a temporary for the second read; and three profiles. Reading
`+0x34` into a temporary reaches 66 instructions rather than 68, at the same
distance.

```c
#include "../../src/types.h"

extern u8 *D_8009B290;
extern u16 D_8009B2A4;
extern u16 D_8009B27C;
extern u16 D_8009B28C;
extern u8 D_800EB0F8[];

extern s32 func_8002E3B4(void);
extern void func_8003B6AC(s32);
extern void *TextBox_Create(s32, s32, s32, s32, s32, s32);
extern void TextBox_Destroy(void *);
extern void DuelEffect_MarkObjectIfActive(void *);

void func_8002E5AC(void)
{
    u8 *script;
    u8 *next;
    s32 value;
    u8 *box;

    if (func_8002E3B4() == 0) {
        script = D_8009B290;
        next = script + 2;
        D_8009B290 = next;
        value = script[0] | (script[1] << 8);
        D_8009B2A4 |= 0x4000;
        func_8003B6AC(0);
        box = (u8 *)TextBox_Create(0, value & 0xFFF, 0x10, 0xB0, 0x120, 0x30);
        DuelEffect_MarkObjectIfActive(box);
        *(u16 *)(box + 0x34) = *(u16 *)(box + 0x34) | 8;
        if ((value & 0x8000) != 0) {
            D_8009B27C |= 0x4000;
            *(u16 *)(box + 0x34) = *(u16 *)(box + 0x34) & 0xFFF7;
        }
        D_8009B28C = D_8009B27C;
        return;
    }
    if ((D_8009B2A4 & 0x4000) != 0) {
        return;
    }
    if ((D_8009B27C & 0x4000) == 0) {
        TextBox_Destroy(D_800EB0F8);
    }
    D_8009B28C = 0;
    D_8009B27C = 0;
}
```
