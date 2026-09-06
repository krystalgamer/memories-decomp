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

`gcc_2_8_1_g8_split`, 151 of 151 instructions, opcode distance 2, 74 differing
positions.

A sound-driver dispatcher with two jump tables. The instruction mix is nearly exact; what remains is register allocation and one delay-slot
placement. The target hoists the jump table base into `$t1` as a loop
invariant and this build does not, and the target leaves a load-delay `nop`
where this build fills it.

Measured and rejected: rewriting the back edge as `do { } while` to invite
the loop-invariant hoist changed nothing. Declaring `g_SDValue` as an array
caches `%hi` in `$s0` across the calls and adds a saved register the target
does not use; the scalar with `section(".data")` recorded here is correct.

Layout: `jtbl_800106C0` at file offset `0xEC0`, 41 entries ending `0xF64`;
four byte gap; `jtbl_80010768` at `0xF68`, 7 entries. Both table addresses are
8-aligned, so GCC's `.align 3` accounts for the gap without a `pad` entry.
The 41 entries reach only three bodies: tag `0x20` alone, tags
`{0x42, 0x43, 0x45, 0x46, 0x48}` together, and the rest default. The second
table keys on the same five tags.

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

typedef struct {
    u32 words[12];
} SoundEntry;

extern SoundState * volatile g_SDValue __attribute__((section(".data")));
#define SOUND_STATE (g_SDValue)

extern s16 func_80049F50(void);
extern void func_80049C40(s16 arg0);

void func_80046294(void)
{
    SoundState *p;
    register s32 i asm("a3");
    register s32 j asm("a2");
    register s32 k asm("t0");
    s32 tag;
    SoundEntry *dst;
    SoundEntry *src;

    p = SOUND_STATE;
    i = 0;
    if (p->count <= 0) {
        goto tail;
    }
    k = 0x30;
    j = i;

loop:
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
        dst = (SoundEntry *)&p->entries[j];
        src = (SoundEntry *)&p->entries[k];
        *dst = *src;
        p = SOUND_STATE;
        p->count = (u16)p->count - 1;
        goto test;

    case 0x20:
        p = SOUND_STATE;
        if (*(s32 *)&p->entries[j + 0x10] != 0x20) {
            goto test;
        }
        dst = (SoundEntry *)&p->entries[j];
        src = (SoundEntry *)&p->entries[k];
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
    if (i >= p->count) {
        goto tail;
    }
    if (i < 0) {
        goto tail;
    }
    goto loop;

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

## `func_8001D5B4` at 0x8001D5B4

`gcc_2_8_1_g8`, 45 instructions against a target of 47, opcode distance 6,
34 differing positions.

Computes a cursor step direction. It clears the direction, compares the
target row against the cursor row and then the target column against the
cursor column, and writes one of -1, 0, 1, 2 or 3 into `D_8009B160`.
Returns 0 only when both coordinates already match, so the caller reads the
return as "still moving". `D_8009B1D6` and `D_8009B1D7` are the destination
column and row; the cursor's own pair sits at `+0xF` and `+0x10`.

Missing two instructions against the target: one `sb` and one `j`. The
target keeps five stores to `D_8009B160` and merges the top `-1` case with
the bottom `2` case, both of which end `store; return 1`. This build merges
a different pair and emits four.

Measured and rejected, all three producing byte-identical output:

- the early-return spelling stored here;
- inverting the row test so the equality branch is taken rather than the
  inequality one;
- a direct transcription of the target's basic blocks using `goto` and a
  shared store label, which is as close to the emitted shape as C gets.

That all three normalise to the same 45 instructions is the useful part. The
residual is not reachable by rearranging the branches, so the next attempt
should look at the types rather than the control flow: the direction global
is `s8` here and the comparisons are between `s8` values, and a different
width or signedness for either would change the sign-extension work and the
store count together.

```c
#include "../types.h"

typedef struct {
    u8 pad00[0xF];
    s8 x;
    s8 y;
} Cursor;

extern u16 D_8009B162;
extern s8 D_8009B160;
extern s8 D_8009B1D6;
extern s8 D_8009B1D7;
extern s32 func_80024088(Cursor *cursor, s8 dir);

s32 func_8001D5B4(Cursor *cursor)
{
    s32 dir;

    if (D_8009B162 != 0) {
        dir = -1;
        goto store;
    }

    if (func_80024088(cursor, D_8009B160) != 0) {
        goto moved;
    }

    D_8009B160 = -1;
    if (D_8009B1D7 != cursor->y) {
        D_8009B160 = 1;
        if (D_8009B1D7 >= cursor->y) {
            goto moved;
        }
        D_8009B160 = 3;
        return 1;
    }

    if (D_8009B1D6 == cursor->x) {
        return 0;
    }

    D_8009B160 = 0;
    if (D_8009B1D6 >= cursor->x) {
        goto moved;
    }
    dir = 2;

store:
    D_8009B160 = dir;

moved:
    return 1;
}
```
