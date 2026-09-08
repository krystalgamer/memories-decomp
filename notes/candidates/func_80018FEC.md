## `func_80018FEC` at 0x80018FEC

`gcc_2_8_1_g8_split`, 280 instructions against a target of 280, **opcode
multiset distance 0**, 114 differing words unshifted.

*Corrected.* The headline previously read "distance 6", and the section below
attributed that to "one `nop` plus the register renaming it drags along". The
multiset is in fact **identical** - every instruction retail has, this build
has, including the `nop`. It is at `+0x1BC` here and `+0x1B4` there, which is
the same one-slot shift that produces all 114 positions.

That correction changes what this candidate is. Distance 6 reads as six
instructions of shape work plus a scheduling problem; distance 0 with a single
displaced slot means **one `dbr` decision separates this from an exact match**,
and the 114 positions are one fault reported 114 times rather than a large
residual. It should be picked up on that basis. Written from scratch: no
stored candidate, no rows in `external_attempts.csv`, empty inventory note.

### The one decision, located exactly

The entry above says one `dbr` decision separates this from a match. The
instruction and the slot are these:

```
        build                                retail
+0x1B0  beqz  $v0, +0x1E0                    beqz  $v0, +0x1E4
+0x1B4  lui   $v0, %hi(D_8009B260)   <-slot  nop                        <-slot empty
+0x1B8  lbu   $v0, %lo(D_8009B260)           lui   $v0, %hi(D_8009B260)
+0x1BC  nop                          <-load  lbu   $v0, %lo(D_8009B260)
+0x1C0  andi  $v0, $v0, 1                    nop                        <-load delay
```

The branch is `if (flags & 0x2000)` failing. **Retail leaves its delay slot
empty and the build fills it with the first instruction of the fall-through**,
which shifts everything after by one slot and is why 114 of 280 positions differ
from a single cause.

**What makes this odd, and where the next attempt should go.** Retail's
fall-through begins with that same `lui`, and `$v0` is dead at the branch target
- the target block's first instruction is `lw $v0, 628($gp)`, which writes it -
so the fill looks legal in both. `reorg` will only *move* an instruction out of
a thread into a slot when the branch owns that thread exclusively
(`own_thread_p`); a block with a second predecessor can only be *copied* from,
and only from the taken side. So the most likely difference is not liveness but
the CFG: in retail that block appears to have another predecessor, and in this
candidate it does not.

That is a testable prediction rather than a description, and it says what to
try: a source shape that gives the `D_8009B260` test block a second entry.

Three shapes that do **not** change it, all 280/280 and 114:

| shape | result |
| --- | --- |
| the two tests merged into one `&&` condition | 114 |
| the byte read hoisted into a block-scoped local | 114 |
| the guard inverted into a `goto` with a label after the arm | 114 |

Seven profiles were swept: four tie at 114, and the three non-split ones are far
worse at 234, 234 and 327 with four to twenty-four extra instructions. So the
profile is settled and the residue is not a code-generation choice.

The duel-end sequencer, a five-state machine on the top bits of `D_8009B23A`.

* **first entry (0x8000 clear)** parks both side objects at x -0x40 and 0x180
  with `func_8001EC70` as their update callback, then walks the five
  `D_800EA030` slots: each card object's `+0x6A` record index selects a
  three-byte pose row in `D_80090918` through the card record's `+0x36C0`
  animation id, and the row's bytes become the object's x, y and its
  `D_800E9EF0` slot. It then releases the losing side's object, sets
  `D_8009B162` to 8, clears the counters and fades the music out.
* **0x4000** either waits out the 0x2000 hold, or spawns a `func_8002C604(0x13)`
  sparkle at a random x and a `Rand_GetInterval(0xB0)` y with its colour word
  stepped by the low two bits of the counter.
* **0x2000** counts `D_8009B1D0` down and fires one sparkle per `D_800E9EF0`
  slot until the counter reaches five, then hands over to 0x1000 with a
  centre-screen burst.
* **0x1000** waits for that burst to finish and starts the result screen
  through `func_8002C68C(0x18)` and `func_8003FF88(0x8021)`.
* **the fall-through** writes the winner side, clears the loser's two counters
  and arms the post-duel sound and state bytes.

**What is left is a single delay slot.** Retail leaves the slot of
`beqz $v0, .L800191D0` (the 0x2000 test) empty; this build steals the
`lui $v0, HI(D_8009B260)` that starts the fall-through block into it, so every
instruction after 0x1b4 sits one slot early, and the opcode distance of 6 is
that one `nop` plus the register renaming it drags along. `dbr`'s eager fill
takes an insn from the fall-through only when it predicts the branch not
taken, and nothing reachable from the source changes that prediction:
`volatile` on `D_8009B260`, the `.data` attribute form, a plain scalar, an
`else` around the 0x2000 arm and inverting the `& 1` test were all tried and
none of them left the slot alone. Everything before that point is byte-exact.

**Six more approaches to that slot, all byte-identical.** The entry already
records `volatile`, the `.data` attribute, a plain scalar, an `else`, and
inverting the `& 1` test. Added since:

| approach | result |
| --- | --- |
| inner test inverted with the stores guarded and one `return` | byte-identical |
| the byte read into a local before the test | byte-identical |
| `goto` to a shared `return` inside the arm | byte-identical |
| the read bound to a local pinned across nine registers | byte-identical, all nine |

The pin sweep is the informative one. On `func_8005C1F4` a pin closed a branch
delay slot that had been classified as beyond the source's reach, because it
changed which register the constant at the branch target landed in and so
changed whether `reorg` could steal it. That is the closest precedent to this
residual and it does not transfer: nine registers for the stolen `lui`'s
destination all leave the slot filled. So the steal here is not conditioned on
the register, and the remaining lever has to be something that changes `dbr`'s
prediction of the branch itself, or makes the fall-through's first instruction
unmovable - a store, a call, or a clobber of something live on the taken path -
none of which the existing statements can be rearranged into.

**Why `volatile` was never going to work, and what that leaves.** The stolen
instruction is the `lui` that materialises `&D_8009B260`, not the `lbu` that
reads it. `volatile` constrains the *load*; the address computation in front of
it stays an ordinary register write and remains a legal delay-slot fill. That
explains the entry's earlier `volatile` result rather than leaving it as a bare
negative, and it predicts the three further spellings tried since - a
`volatile` cast on the access, a `volatile u8 *` local holding the address, and
an explicit `!= 0` on the outer test - which are all byte-identical.

Eight profiles were also crossed. Every `_split` variant, including
`--use-comm-section` and `-fno-strength-reduce`, gives the same 114; the
non-split profiles are four to fifteen instructions long and 234 or worse. So
split addressing is required and the slot behaves identically across the
cohort.

**The requirement, stated precisely.** `dbr` will fill the slot from the
fall-through unless the first instruction there cannot legally execute on the
taken path. A `lui` into a dead register always can. Reaching this slot
therefore needs the block to begin with a store, a call, or a write to a
register that is live at the branch target - and the three statements in the
arm cannot be rearranged into any of those without changing what the function
does. That is a sharper statement than "no source change moves it", and it says
the next attempt should look at what is live across the branch rather than at
how the read is spelled.

**CFG shape does not reach it either, which closes the last open direction.**
The requirement stated above - that the fall-through block begin with a store,
a call, or a write to a register live at the branch target - points at
liveness rather than at spelling, so three restructurings were tried that
change the control-flow graph rather than the statement:

| restructuring | result |
| --- | --- |
| flattened to a combined `(flags & 0x6000) == 0x6000` test | 172, two instructions long |
| inverted inner test with a `goto` past the arm | byte-identical |
| the flag word re-read inside the arm instead of carried in `flags` | byte-identical |

The negative is conclusive rather than merely another failure, because the
block's first statement *is* the condition. `D_8009B260[0] & 1` has to be read
before anything else in the arm can be decided, so no legal rearrangement puts
a store or a call ahead of it, and the read's own address materialisation is
the stealable `lui`. The three statements after the test are all stores, and
every one of them is dominated by the test.

**So the function is bounded at 114 positions, all of them one displaced
delay slot.** It remains the closest large candidate in the store - the opcode
multiset is identical and a single `dbr` decision separates it from an exact
match - but nothing in the source reaches that decision. Anything that does
will have to come from outside the C: a compiler cohort whose `dbr` predicts
this branch differently, since every profile in the current set fills the slot
identically.

### Levers that got it here

- *Hand-hoist the loop's address constants and use a `goto` loop.* Written as
  a `do`/`while`, loop.c hoists the two one-instruction constants `0xB4` and
  `1` out of the body as well, which retail rematerialises per iteration. A
  `goto` loop hoists nothing, so the four addresses (`D_8015C424`,
  `D_80090918`, `func_8001EC70`, `D_800E9EF0`) have to be assigned to locals
  by hand, which is what puts them in `$t1`-`$t4`.
- *The `0x48000` base must be its own local.* `cards + k * 0x1C + 0x4B6C0`
  folds into one relocation and comes out as `lui 5` with a zero displacement;
  the `g = base + k * SIZE + 0x48000;` form that `duel_draw_resolution.c`
  already uses keeps `0x48000` as a materialised constant and `0x36C0` as the
  load displacement, which is retail.
- *Index-first address arithmetic.* `(u32)cards + k * 0x1C` gives
  `addu $v0, $v0, $t4`; the `cards[k * 0x1C]` spelling gives the operands the
  other way round.
- *One pointer local for the two pre-loop objects and the loop entry.* Retail
  keeps `$a2` across all three; separate locals put the loop's entry in `$a1`
  and cost the whole loop body's register naming.
- *The function address needs a pinned intermediate.* Retail materialises
  `func_8001EC70` into `$v0` and copies it to `$t1` at the top of the block.
  Unpinned, the scheduler sinks the materialisation to its first use and
  coalesces the copy away; `register void (*fn)(void) __asm__("$2");` with a
  separate `s32` holding `(s32)fn` reproduces both the position and the copy.
- *Read a value into a local before the stores that precede its use.* The pose
  row's second byte, the `Rand_GetInterval` result and the sparkle's `+0x14`
  word all have to be read into locals, because gcc will not schedule a load
  across a store through an unrelated pointer and retail issues these loads
  first.
- *`(u8)` on an `s8` local is what produces the `andi 0xFF`* on the winner
  side index; a plain `s32` copy of the `u8` global folds the mask away.

### Source

```c
#include "../types.h"
#include "duel_card_layout.h"

extern u16 D_8009B23A;
extern u16 D_8009B162;
extern u16 D_8009B1D0;
extern u8 D_8009B1B9;
extern u8 D_8009B1D5;
extern u8 gDuel_bWinnerSide;
extern u8 *D_8009B214;
extern u8 *D_8009B21C;
extern u8 *D_8009B1B4;
extern u8 *D_8009B17C;

extern u8 D_8015C424[];
extern u8 D_80090918[];
extern u8 *D_800E9EF0[];
extern u8 D_800E9F10[];
extern s32 D_800E9F04[];
extern u8 D_800E9FF0[];
extern u8 D_800EA030[];
extern u8 D_800E9ECF[];
extern u8 D_8009B260[];
extern u16 D_800EF658[];
extern u8 D_8009B369 __attribute__((section(".data")));
extern u8 D_8009B269 __attribute__((section(".data")));
extern u8 D_8009B26C __attribute__((section(".data")));

extern void func_8001EC70(void);
extern void func_8004036C(u8 *);
extern void func_80015C84(void);
extern void SD_BGMFadeOutWithStep(s32);
extern void SD_SEPlayFull(s32);
extern s32 func_80042B40(s32);
extern u8 *func_8002C604(s32);
extern s32 rand(void);
extern s32 Rand_GetInterval(s32);
extern void func_8002C68C(s32);
extern void func_8003FF88(s32);
extern void func_800156DC(void);
extern void func_800472A8(s32);
extern void func_80059C18(s32);

void func_80018FEC(void)
{
    u16 flags;
    u8 *obj;
    u8 *pose;
    u8 *rec;
    u8 *fx;
    s32 i;
    s32 anim;
    s32 n;
    s32 r;
    s32 t14;
    u8 *cards;
    u8 *poses;
    u8 **objs;
    register void (*fn)(void) __asm__("$2");
    s32 fnv;
    u8 *g;
    s32 py;
    s32 t;
    s32 k;
    u8 **slot;
    s8 side;
    u8 *other;

    flags = D_8009B23A;
    if ((flags & 0x8000) == 0) {
        i = 0;
        cards = D_8015C424;
        poses = D_80090918;
        fn = func_8001EC70;
        fnv = (s32)fn;
        objs = D_800E9EF0;
        D_8009B23A = flags | 0x8000;
        obj = D_8009B214;
        D_8009B1B4 = &D_800E9F10[D_8009B1D5 * 0x70];
        *(s16 *)(obj + 0x28) = -0x40;
        *(u16 *)(obj + 0x2C) = 0x10;
        obj[0x6C] = 1;
        *(s32 *)(obj + 0x24) = fnv;
        *(u16 *)(obj + 0x2A) = *(u16 *)(obj + 0x32);
        obj = D_8009B21C;
        *(s16 *)(obj + 0x28) = 0x180;
        *(u16 *)(obj + 0x2C) = 0x10;
        obj[0x6C] = 1;
        *(s32 *)(obj + 0x24) = fnv;
        *(u16 *)(obj + 0x2A) = *(u16 *)(obj + 0x32);
        rec = D_800EA030;
next_obj:
        obj = *(u8 **)rec;
        g = (u8 *)(obj[0x6A] * DUEL_CARD_RECORD_SIZE + (u32)cards + 0x48000);
        anim = *(s16 *)(g + 0x36C0) - 0x11;
        pose = (u8 *)(anim * 3 + (u32)poses);
        *(s16 *)(obj + 0x28) = pose[1] - 0x1A;
        py = pose[2];
        *(u16 *)(obj + 0x2C) = 0xB4;
        obj[0x6C] = 1;
        *(s32 *)(obj + 0x24) = fnv;
        *(s16 *)(obj + 0x2A) = py - 0x1E;
        objs[pose[0]] = obj;
        *(u8 **)rec = 0;
        i++;
        rec += 0xC;
        if (i < 5) {
            goto next_obj;
        }
        D_800E9F04[0] = 0;
        func_8004036C(*(u8 **)(D_8009B1B4 + 4));
        D_8009B162 = 8;
        D_8009B1D0 = 0;
        D_8009B1B9 = 0;
        D_8009B17C = 0;
        D_8009B23A = D_8009B23A | 0x4000;
        func_80015C84();
        D_800E9ECF[0] = 2;
        SD_BGMFadeOutWithStep(2);
        return;
    }
    if (flags & 0x4000) {
        if (flags & 0x2000) {
            if ((D_8009B260[0] & 1) != 0) {
                return;
            }
            D_8009B23A = flags & 0xBFFF;
            D_8009B1B9 = 0;
            D_8009B1D0 = 0;
            return;
        }
        if (D_8009B17C != 0) {
            if (D_8009B17C[0x1D] == 0) {
                return;
            }
            SD_SEPlayFull(0x1D);
            if (func_80042B40(1) == 0) {
                D_8009B23A = D_8009B23A | 0x2000;
                return;
            }
        }
        fx = func_8002C604(0x13);
        D_8009B17C = fx;
        *(u16 *)(fx + 0) = (rand() & 0xFF) + 0x20;
        r = Rand_GetInterval(0xB0);
        n = D_8009B1B9;
        t14 = *(s32 *)(fx + 0x14);
        *(u16 *)(fx + 2) = r + 0x20;
        *(s32 *)(fx + 0x14) = t14 + ((n & 3) << 13);
        D_8009B1B9 = n + 1;
        return;
    }
    if (flags & 0x2000) {
        t = D_8009B1D0 - 1;
        D_8009B1D0 = t;
        if ((s16)t > 0) {
            return;
        }
        if ((s8)D_8009B1B9 >= 5) {
            D_8009B23A = (flags & 0xDFFF) | 0x1000;
            fx = func_8002C604(0x13);
            *(u16 *)(fx + 0) = 0xA0;
            *(u16 *)(fx + 2) = 0x78;
            *(s32 *)(fx + 0x14) = *(s32 *)(fx + 0x14) + 0x8000;
            D_8009B17C = fx;
            return;
        }
        D_8009B1D0 = 4;
        fx = func_8002C604(0);
        k = (s8)D_8009B1B9;
        slot = &D_800E9EF0[k];
        *(u16 *)(fx + 0) = *(u16 *)(*slot + 0x30) + 0x1A;
        *(u16 *)(fx + 2) = *(u16 *)(*slot + 0x32) + 0x1E;
        *(s32 *)(fx + 0x14) = *(s32 *)(fx + 0x14) + ((k << 12) + 0xA000);
        *(u16 *)(fx + 0x1A) = 9;
        SD_SEPlayFull(0x17);
        D_8009B1B9 = D_8009B1B9 + 1;
        return;
    }
    if (flags & 0x1000) {
        if (D_8009B17C[0x1D] == 0) {
            return;
        }
        D_8009B23A = flags & 0xEFFF;
        SD_SEPlayFull(0x1D);
        func_8002C68C(0x18);
        func_8003FF88(0x8021);
        return;
    }
    func_800156DC();
    D_800EF658[0] = 0x309;
    side = D_8009B1D5;
    gDuel_bWinnerSide = side;
    D_800E9FF0[(u8)side * 0x20] = 0x28;
    other = &D_800E9FF0[(D_8009B1D5 ^ 1) * 0x20];
    *(u16 *)(other + 0x14) = 0;
    *(u16 *)(other + 0x12) = 0;
    func_800472A8(0x7310);
    func_80059C18(0x7310);
    D_8009B369 = 1;
    D_8009B269 = 3;
    D_8009B26C = 1;
}
```
