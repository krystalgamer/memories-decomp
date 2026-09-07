# Overlay near-miss candidates

Candidate sources for unmatched overlay functions live in `tmp/`, which is not
tracked, so they are lost when a session ends. Every run that wants to continue
a near miss has had to rebuild its candidate from the prose in the function's
inventory row first. That has happened at least six times, three of them on
`func_80168CDC` alone, and each rebuild costs several minutes and can drift
from the state the row describes.

This file is the durable copy. A candidate belongs here once it is close enough
that the next run would otherwise rebuild it: roughly, when it reaches the
target's instruction count, or when its recorded state is the base for further
measurement.

Rules:

- Store the exact source that produces the recorded state, not a tidied version.
  These are measurements, and reformatting them can change the output.
- Use the include an integrated overlay source uses, `#include "../../types.h"`,
  which is what every file under `src/overlays/<module>/` has. A stored
  candidate is meant to be promotable as-is, so an include that only resolves
  from a scratch directory makes it fail to build the moment someone integrates
  it. A sweep harness that compiles from somewhere else should rewrite the
  include as it writes its temporary file rather than change what is stored.
- Record the profile and the measured result in the heading, and re-verify with
  `overlay_diff.py` before trusting a stored candidate. If it no longer
  reproduces, say so in the function's inventory row rather than silently
  editing it here.
- Delete an entry once the function matches and its source is promoted into
  `src/overlays/`.
- The inventory row remains the place for findings, negatives and levers. This
  file holds only code.
- If a row claims a near-miss state, the source that produces it belongs here.
  A claimed state with no stored source is not a result, because nobody can
  reproduce it. That has happened three times so far and each one cost a full
  reconstruction to recover: `func_80169734`, which the row had at 309 of 309
  with two differing positions, `CampaignMap_UpdateLocationTransition` at 217
  of 217 with seventy, and `func_8016A37C` at 365 of 365 with five. The first
  two were rebuilt and then matched; the third is still being recovered.
  `FreeDuel_Init` was recovered the same way and has now matched, so a lost
  reconstruction is worth rebuilding from the row's own prose every time.

At the time of writing the rows below are the unmatched functions whose claimed
state is not stored here, in the order worth recovering:

- `func_8016A37C`, claimed at 365 of 365 with five differing positions. The
  closest unstored state in the project.
- `func_80181728`, claimed within four instructions of the target, with the
  residual attributed to four callee-saved hoists.
- `func_801821DC`, no claimed state.

## main_menu `func_801821DC` at 0x801821DC

`gcc_2_8_1_g0_split`, 85 instructions against 1132, so this is an opening
fragment rather than a near miss. It is stored anyway because the function had
no candidate at all, and because the part that is written reproduces the
target's shape closely enough to be worth building on rather than rediscovering.
The mnemonic subsequence is 79 and the first four blocks are structurally right;
everything after the per-entry init loop is still missing.

What the opening decodes to, with the evidence:

The first two instructions load `D_801845E0` before the stack adjust, and the
prologue saves every callee-saved register into a 72-byte frame, so the source
needs enough simultaneously live values to use `s0` through `s8`. This fragment
uses far fewer and gets a 40-byte frame, which is the main reason the agreeing
prefix is still zero: the prologue cannot match until the register pressure
does.

Two paired assignments are visible and are worth preserving. Words 4 and 6 are
`move s3,zero` then `move s2,s3`, and words 8 and 10 are `li s8,1` then
`move s6,s8`, which is one C variable initialised from another rather than two
literals. Word 20 puts `D_801845E0->f69 - 4` in `s5` and word 21 stores the same
value to `24(sp)`, so that value is both a register variable and a stack
temporary.

The two block copies at words 31 to 61 are pointer-comparison walks, not counted
loops: the increment is `addiu v1,v1,16` and the exit test is `bne v1,a0`
against an end pointer, with no induction variable. Writing them as
`for (i = 0; i < 64; i++)` leaves a `slti` and a counter that the target does
not have, and costs nine instructions. Written as `do { *dst = *src; src++;
dst++; } while (src != end);` the loop body matches the target instruction for
instruction, differing only in which temporaries the copy uses.

All four pointers come from one base register: the target emits one
`lui`/`addiu` pair for `D_801D1200 + 4608` and then four `addiu` offsets from
it. Assigning the base to a local first and deriving the others from it, with
the second copy's pointers assigned before the first copy runs, reproduces that
and takes the subsequence from 75 to 79; indexing the array directly instead
emits a fresh `%hi`/`%lo` pair per pointer.

The remaining work is the body after word 79, which is the bulk of the
function: a menu input and navigation state machine with 53 calls, 27 of them
`SD_SEPlay`.

```c
#include "../../types.h"

typedef struct {
    u32 w0;
    u32 w4;
    u32 w8;
    u32 wC;
} E16;

typedef struct {
    u8 pad0[105];
    u8 f69;
} Host;

extern Host *D_801845E0;
extern u8 D_80185CD1;
extern u8 D_80185CC8[];
extern u16 D_80185C9C[];
extern E16 D_801D1200[];

extern s32 func_8003F70C(void);
extern void func_8018338C(s32, s32);
extern void func_801844D8(s32);
extern void func_8003CB7C(void);

s32 func_801821DC(void)
{
    s32 i;
    s32 zero1;
    s32 zero2;
    s32 one1;
    s32 one2;
    s32 lim;
    s32 base;
    s32 r;
    u8 *slot;
    u16 *hslot;
    E16 *src;
    E16 *dst;
    E16 *end;
    E16 *src2;
    E16 *dst2;

    zero1 = 0;
    zero2 = zero1;
    one1 = 1;
    one2 = one1;
    base = 715;
    lim = D_801845E0->f69 - 4;
    if (D_80185CD1 != 0) {
        r = func_8003F70C();
        if (r == 0) {
            goto out;
        }
        if (r != one2) {
            goto other;
        }
        end = D_801D1200;
        dst2 = end + 256;
        src2 = end + 360;
        dst = end;
        src = end + 104;
        do {
            *dst = *src;
            src++;
            dst++;
        } while (src != end + 168);
        dst = dst2;
        src = src2;
        do {
            *dst = *src;
            src++;
            dst++;
        } while (src != end + 424);
        i = 0;
        slot = D_80185CC8;
        hslot = D_80185C9C;
        do {
            *hslot = 0;
            func_8018338C(i, 1);
            func_801844D8(i);
            slot[i] = 0;
            i = i + 1;
            hslot = (u16 *)((u8 *)hslot + 22);
        } while (i < 2);
        func_8003CB7C();
    }
other:
    zero2 = zero1;
out:
    return lim + base + one1 + zero2;
}
```

## password `func_8016913C` at 0x8016913C

Writing the `? 12 : 9` argument as an explicit assignment in each arm, at the
`arm4` call site only, takes the opcode distance from 4 to 2 and the subsequence
from 362 to 363, so it improves both keys at once. The target materialises 9 and
12 straight into `a0` after `NameEntry_AdjustLength` returns; the conditional
expression makes GCC produce the constant before the call instead, hold it in a
callee-saved register across the call and copy it into `a0`, which is where the
two surplus `move` instructions came from. Assigning through a named local in
each arm removes one of them.

Applying the same rewrite at both call sites reaches a subsequence of 364 but
returns the distance to 4, so only the first site is taken. Nine earlier cells
covering ternary, if-else and a named intermediate at both sites were all
byte-identical, and the difference here is that the assignment happens in the
arms rather than in the argument, which is what separates the constant's
materialisation from the call.

The `D_8009B398[0] & 0x800` arm also belongs out of line, and moving it takes
the longest common mnemonic subsequence from 351 to 362. The evidence is the
same alignment argument as the select-screen block above: the target ends that
arm with `j 0x169360` into word 137, so the arm is laid out after the common
tail and jumps back to it, while the inline `else if` makes it fall through.
Rewriting it as `goto alt800` with the arm placed immediately before the
`select` label reproduces the layout, and eleven more instructions fall into
the target's relative order.

This candidate is stored at that state deliberately, and the choice is worth
explaining because the two headline metrics disagree. Out of line the opcode
distance is 4 against 3 and the count is 384 against 383, both one worse,
because the arm no longer sits next to code that has already loaded
`%hi(0x8017)` and has to load its own; inline, eleven instructions are in the
wrong order but that single `lui` is absent. Opcode distance is an L1 multiset
distance and cancels, so it under-reports a block in the wrong place, and this
repository's own methodology note records that distance mis-ranked this
particular function before. The subsequence cannot cancel, so it is the better
key here, and the residual it leaves is one identified instruction rather than
a layout question. Three spellings of the arm, covering the negated guard, the
positive guard and the two assignments in the other order, all give the same
362 and the same extra `lui`, so the cost is inherent to the placement rather
than to how the arm is written.

The residual is `{lui +1, beqz +1, move +2, bnez -1, addu -1}`. That surplus
`lui` is not a property of the arm, which is emitted identically to the target
instruction for instruction, but of what precedes it. In the target the arm's
first load reads `D_8009B398[0]` through a register that already holds the right
high half, inherited from the block laid out immediately before it, which ends
in a jump and leaves that value live; wherever this candidate puts the arm, no
such value is live on entry and it loads its own. Four placements were measured,
before `select`, before `arme`, before `sel_ret` and before `join`, and none
both keeps the subsequence at 362 and drops the `lui`: the three alternatives
fall back to 351 or 352, and one of them costs ten instructions. So the `lui`
cannot be removed by moving the arm, and the remaining question is which block
the target lays out immediately before it rather than where the arm itself
goes.

The select-screen early return belongs out of line at the end of the function,
not inline where it is written. Aligning the two streams on mnemonic plus
immediates shows a thirteen-instruction block that tests `D_8009B394[0] & 0x20`
and calls `func_8003FEE0` sitting at words 233 to 247 in the candidate and at
words 360 to 372 in the target: the same code in two different places, which is
block placement rather than allocation. Writing it as `goto sel_ret` with the
block itself after the last statement of the function moves it, and takes the
longest common mnemonic subsequence from 341 to 351, the differing positions
from 309 to 294 and the instruction count from 384 to 383 against a target of
382. That is the first movement on this function in several runs, and it came
from aligning the streams rather than from another spelling.

The residual is now `{beqz +1, bnez -1, addu -1, move +2}`. The two surplus
`move`s are the `? 12 : 9` argument staged through a callee-saved register
instead of being written straight into `a0`: the target emits `li a0,9` and
`li a0,12` into the argument register on the two arms of a branch, and this
candidate computes the value into `s3` and copies it. Nine cells covering the
product of ternary, if-else and a named intermediate at both call sites are
byte-identical, so the spelling of the conditional is not the lever; GCC
canonicalises all three forms before the choice is made.

`D_8016D400`, `D_8016D401` and `D_8016D402` are three separate symbols, not one
grouped object, and that is settled rather than assumed. The target addresses
0x8016D401 and 0x8016D402 through `a0` with the displacements -11263 and -11262,
which looks like one symbol reached at two offsets, but the two `a0` values come
from different `lui` instructions either side of the reload at index 95, so each
byte carries its own `%hi`. Declaring them as one struct or one array, in four
variants covering signed and unsigned element types, takes the opcode distance
from 2 to 22 and the agreeing prefix from 91 to 57. Keep them separate.

The residual is a register-allocation inversion at index 91. The target leaves
the `bnez` delay slot empty because the register it would materialise the
constant 11 into is `v1`, which is still holding a `%hi` used by the load at
index 92; this candidate puts the constant in `v0` and the `%hi` in `v1`, so
nothing stops it filling the slot. Eight cells covering the position of the
`D_8016D401 = 11` assignment within its block against three spellings of the
flag expression leave the prefix at 91 in every case, which is consistent with
the cause being allocation rather than statement order.

`gcc_2_8_1_g0_split`, 384 instructions against 382, opcode distance 2
with the unconditional jump count exact, and the first 91 instructions
agree. The longest common subsequence of mnemonics is 341 of 382 and 309
positions differ.

The two surplus `lui` that this entry chased for several cycles are gone. The
walk that follows a negative cell was written with a label and a `goto`, and a
`goto` loop carries no `NOTE_INSN_LOOP_BEG`, so `loop.c` never scans it and the
`%hi` of the store target is rebuilt on every iteration. Writing the same walk
as a `while` gives the loop a begin note, the `%hi` becomes an ordinary
invariant and is hoisted once, and both surplus `lui` disappear together
because every global in this range shares the high half. The remaining fault is
two register copies and one `nop` against one `addu`, which is a different and
smaller problem than the one this row started with.

The caret's frame counter is written after its x position, not before. The
`kind == 1` block ends with a run of stores through the object returned by
`func_80168CDC`, and moving `obj[0x6C] = 6` to the end of that run, after the
store of the x position to `+0x44`, removes the last surplus `nop`.

The reason is a load-delay stall and it is worth stating because the fix looks
arbitrary. The x position is `D_8016D42C * 16 + 112`, so the block has to load
that byte and then shift it, and on this core the value is not available in the
instruction immediately after the load. With the `6` stored before the load
there is nothing left to put between them and the slot becomes a `nop`; with
the `6` stored after, the `li` and the `sb` that write it fill the gap, which
is exactly what the target does at instructions 353 to 356. Thirty-six cells of
the product of four spellings of the slot address, three of the x expression
and three placements of that store agree: every cell that puts the `6` last
measures two, and every cell that does not measures three, independently of the
other two axes.

Three more axes are closed at two, and one of the cells is a trap that the next
attempt should be warned off rather than drawn to.

Statement order in the caret block is entirely inert. Twenty-one cells of the
product of three orders for the flag store, the slot address and the null test
against seven orders for the five stores through the returned object, including
reversing them, are byte-identical to one another. The scheduler reaches the
same arrangement from all of them, so the `obj[0x6C] = 6` placement recorded
above is not a statement-order effect in general; it is specific to that store
sitting between a load and its use.

The spellings of the eleventh-column arm are inert too. Naming the constant
eleven in a local and using it for both the comparison and the assignment,
folding the flag test into the table subscript, and hoisting the column
assignment above the pad read all produce identical code.

The trap is reading `D_8016D42C` once into a local instead of twice. It looks
like the best cell on the board: the instruction count becomes exactly 382,
matching the target for the first time, and the differing positions fall from
310 to 297, which by the stated ordering would decide a tie on distance. It is
a false positive. The target loads that byte twice, once before the call into
`func_80168CDC` and once after it, and the single-read form reaches its numbers
by not performing the second load at all. It trades the surplus `lui` for a
missing `lb`, so the distance is unchanged at two and the mnemonic total is
unchanged at four; nothing was repaired. Declaring the global `const`, which
would let GCC rematerialise the load rather than keep the value, changes
nothing either way.

Both surplus `lui` are now located, and they are one fault in two places rather
than two faults.

The first is inside the cell-walk loop. The target holds the high half of
`D_8016D401` in `a2` for the whole loop, materialised once before it and used
both by the column read that precedes the loop and by the store of the new
column inside it, so the store is a single `sb` at `-11263(a2)`. This build
materialises a fresh high half inside the loop body, on every pass, for that
one store.

The second is the one already recorded: the high half of `D_8016D42C` in `s0`,
materialised once and read through both before and after the call into
`func_80168CDC`.

The two are the same decision. In each place the target hoists a high-half
pseudo into a register with a long life and this build re-emits it at the point
of use. The register assignment behind it is visible in the prologue: both
sides save nine callee-saved registers into a forty-eight byte frame, but the
target puts the widget pointer in `s1` and leaves `s0` for shorter-lived
values, using it first for the cell in the walk loop and then for the
`D_8016D42C` base. This build pins the widget in `s0` for the whole function,
so the two high halves never get a register at all.

Nothing reaches it. Twenty-four cells were measured against this base. Six
spellings of the walk that separate the row offset from the array base, which
is the shape the target's two loop `addu` imply, leave the loop-local pointer
in front at two with the nearest alternative at four. Ten cells of the product
of four placements for the store of the column inside the loop, before the load
instead of after, once after the loop instead of inside it, and both, against
the store being qualified `volatile`, leave the current placement in front at
two with everything else at three. Eight declaration orders of the seventeen
locals, including reversing them, grouping the pointers first and last, and
moving the widget pointer to the end, are byte-identical, which is the second
function on which that lever has now been shown inert.

That leaves two surplus `lui` and an `addu` that should be a `move`, and the
latter shares an opcode class with what it is measured against, so it cancels
and costs nothing. The instruction count is 384 against 382 and the
unconditional jump count is exact.

Four axes were re-measured against the new base and are closed. The sixteen-cell
`volatile` product over `D_8016D401`, `D_8016D402`, `D_8016D426` and
`D_8016D42C` leaves the base in front, with the nearest cell one worse and the
rest between six and twenty-nine. The six spellings of the caret slot store are
now decisively separated rather than nearly tied: the inverted null test is at
two and every other spelling, including the two that store zero first and were
within one before, is at five. The slot address spelling and the x expression
spelling are both inert across all twelve of their combinations.


The null test on the glyph node is written the other way round. The caret slot
is filled with

    if (node == 0) {
        *slot = 0;
    } else {
        *slot = *(u16 *)node;
    }

and not with the non-zero test first. It is worth one of the distance and it
takes both `beqz` and `bnez` to exactly the target's counts, which no other
spelling of that statement reaches. Four were measured: the non-zero test
first, a conditional expression, a plain store of zero followed by a guarded
overwrite, and the same with an empty then-arm. The two that store zero first
do remove one of the surplus `lui`, but they lose an unconditional jump, so
they are worse overall; the conditional expression is identical to the
non-zero test. Only the inverted test moves the branch counts.

The walk form and the dispatched-cell mask were re-measured against the new
base and both keep their existing settings, the loop-local pointer and the
four-bit mask, with the nearest alternative two worse. The twenty-nine
profiles are closed with only `gcc_2_8_1_cc_g0_as_g8_split` tying, and the
sixteen-cell product of `volatile` over `D_8016D401`, `D_8016D402`,
`D_8016D426` and `D_8016D42C` is closed at the new base as well; the qualifier
on `D_8016D400` that this row already records remains the only one worth
having.

Three remain and one of them is free. The `addu` that should be a `move` shares
an opcode class with what it is measured against, so it cancels and costs
nothing. The other two are a single surplus `lui` counted twice over, and the
mechanism is now exact rather than described. The target materialises the high
half of `D_8016D42C` into the callee-saved `s0` at instruction 329, in the
middle of the read-modify-write that sets bit `0x80`, and reads through it
twice, once at 333 before the call into `func_80168CDC` and once at 353 after
it. This build materialises a fresh base for each of the two reads, because
without another live value competing for it the allocator prefers two `lui` to
one callee-saved register and its save and restore. Both sides load
`D_8016D42C` exactly twice, so the difference is not the number of reads but
whether the address survives the call.


The select dispatch is now reproduced instruction for instruction, and the
whole residual is two surplus `lui`, one inverted branch and an `addu` that
should be a `move`. The distance is still four and the composition behind it is
much better: the mnemonic-level total falls from ten to six, the longest common
subsequence rises from 322 to 333, and the unconditional jump count, which is
the second metric in the ordering, goes from one short to exact.

Two changes did it and neither is worth anything without the other, which is
why previous cycles kept landing on the chain.

The three arms are reached out of line. `beq` to the `n == 4` arm, `beq` to the
`n == 6` arm, an unconditional `j` to the default, and the three bodies laid
out afterwards behind labels. Written that way the eight instructions from the
first comparison to the jump match the target exactly, including which constant
lands in each delay slot. On its own that layout is three worse, because it
inverts the test inside the fourth arm.

The fourth arm's test is a store of the default followed by a test for equality.
Writing `d = 1;` and then `if (col == 11) { d = -1; } else { gx = 20; }` puts
the common value where the target puts it and leaves only the exceptional
assignment in the branch, which is what produces the target's `bne` against the
literal eleven rather than a `beq` past the arm. Five other spellings of that
same test were measured under the same layout, including hoisting the default
with the comparison inverted, swapping the two assignments inside the arm, and
writing the whole thing with an explicit label and `goto`, and all five leave
the branch inverted. The combination measures every one of `beq`, `bne`, `lb`,
`j` and `nop` at exactly the target's count.

The measurement that made this findable was per-mnemonic rather than aggregate.
Opcode distance folds `beq` with `beqz` and `bne` with `bnez`, because they
share a primary opcode, so an inverted comparison against a register cancels
against a compensating comparison against zero and the aggregate cannot see it.
Reporting the signed count for each mnemonic separately showed that the chain
layout and the out-of-line layout were wrong in opposite directions on the same
pair, one at plus one and the other at minus one, with the target between them.

What remains is two surplus `lui` and the branch that goes with them. The
target keeps a `%hi` base in `s0` across the tail of the function and reads
`D_8016D42C` through it; this build rematerialises the base at that point and
again once more. It is the same register-pressure disagreement already recorded
for the `addu` in the walk loop, where the target keeps the row offset and the
array base in two registers and this build folds them into one.


A fourth came from declaring `D_8016D400` volatile. It is the pad-owned bit
field the select path sets with `|= 0x40` and `|= 0x80`, and without the
qualifier GCC keeps the read-modify-write in a register across the two arms
that touch it. The target reloads and restores it each time. Only that one of
the four candidate globals wants the qualifier: `D_8016D402` costs
twenty-five, `D_8016D401` costs thirty-one and drops the agreeing prefix from
91 to 61, and `D_8016D426` costs one. All sixteen combinations were measured
together rather than one at a time, which is how the single useful one was
separated from the three harmful ones it would otherwise have been averaged
with.

The sixty-four cell product of walk form, dispatched-cell mask, dispatch shape
and store placement was then re-run against the new base and is closed at four.
Nothing in it beats the current cell, the shared `D_8016D426` store beats the
per-arm form in every one of the thirty-two pairs, and folding the sign
extension into the loop's addition is byte-identical to casting at the use.
The twenty-nine profiles are closed too, with only
`gcc_2_8_1_cc_g0_as_g8_split` tying.

The remaining four are two missing `lb`, one missing `j`, one surplus `nop`,
and an `addu` that should be a `move`, the last of which cancels inside its
opcode class and so costs nothing. The block placement of the select dispatch
is decoded and is the reason for the first two. The target reaches all three
arms out of line: `beq` to the `n == 4` arm, `beq` to the `n == 6` arm, and an
unconditional `j` to the default, with the two constants for the comparison
materialised in between. This build's `else if` chain puts the `n == 4` arm
inline and branches past it with `bne`.

Writing that placement explicitly, with three labels and a `goto` for each arm,
does fix both. It gives the target's fourteen `lb` and its nine `j` exactly,
and it has the best mnemonic subsequence anything on this function has reached,
332 against the current 322. It is nonetheless three worse, because it inverts
two branch senses and adds two `lui`: the target has two `beq` and two `bne`
where the explicit form has three and one, and the chain has one and three.
The target sits exactly between the two, so one arm wants the branch-to-label
form and one wants the inline test. Inverting the `col != 11` test inside the
arm, which is the obvious candidate for the odd one out, does not do it: all
six cells of layout against that test's sense leave the chain in front. The
mixed placement that produces two of each is still to be found.


A fifth came from the order of the two side effects inside the walk loop. The
target computes the address of the next cell and loads it before it stores the
new column back to `D_8016D401`; this build stored first and computed after.
The two are independent, because the store is to a different object than the
load, so the swap is free semantically and worth one of the distance. Writing
the loop as

        col = col + cell;
        cell = rp[(s8)col];
        D_8016D401 = col;

is the shape. Folding the sign extension into the addition, as
`col = (s8)(col + cell)`, reaches the same number by the same route, so the
cast's position is not the point; the store's is.

With that in place four more axes were re-run and are closed at five. The three
hoist forms crossed with the mask still put the loop-local pointer first. The
cursor-x computation in five forms, including naming the sign-extended column
in a local, testing the eleventh column first, and adding twenty to the
already-stored value, is byte-identical in three of them and worse in the rest.
The select dispatch in five layouts, including reordering the two equality
arms, nesting them and writing them as a `switch`, does not improve on the
current chain; the `switch` is interesting only in that it has the best
mnemonic subsequence of anything tried, 333, while being three worse on
distance, which is a reminder that the subsequence is a diagnostic and not the
target.

The remaining five are two missing `lb`, one missing `addu`, one missing `j`
and one surplus `nop`, with the two branch senses cancelling within their
opcode classes. The two `lb` are the same class of problem as the `addu`: the
target reloads a byte this build keeps in a register.


Opcode distance halves from 12 to 6, and every step of it came from a product
rather than from an axis measured on its own. Two of the three levers had
already been measured as inert or harmful against the previous base.

The store of the cursor row into `D_8016D426` belongs after the arms, not
inside them. The three arms of the column dispatch each ended with
`D_8016D426 = D_8016D402;`, and GCC cross-jumps those into a single tail, so
the emitted code looked right. It is not the same code. The target reloads
`D_8016D402` from memory in that tail and stores the reloaded value, which is
what one assignment placed after the chain produces and what three assignments
inside it do not. Worth two of the distance on its own.

Hoisting the row pointer out of the cell-walk loop is then worth four more.
The same change measured against the previous base was worth nothing: it
removed four instructions the target keeps and left the distance at ten. With
the `D_8016D426` store moved it removes exactly the right ones. This is the
clearest expiry of a lever recorded on this function so far, and it is the
reason the two must be measured as a product.

The mask on the dispatched cell is now affordable. `D_8016AB38[row][col] & 0xF`
gives the `lbu` and the `andi` of `0xf` that the target has, because a sign
extension under a four-bit mask is dead; it cost two instructions against the
old base and pays for itself here. It leaves the candidate two instructions
short rather than two long, and the mnemonic subsequence rises from 322 to 325.

Three axes were re-run against the new base and are closed. The dispatch
structure around the pad tests, in three forms including the nested one and the
inverted-test one, is byte-identical. The `>= 11` test spelled as `> 10` or
with an `(s8)` cast is byte-identical, and so is nesting the `0x1000` test
instead of chaining it, across eighteen cells of their product. The
signedness of `D_8016D402` and `D_8016D426` is inert because the explicit
`(s8)` casts already decide it. Of the twenty-nine profiles only
`gcc_2_8_1_cc_g0_as_g8_split` ties the current one.

Two structural facts about the remaining six are decoded. The target keeps the
row offset and the array base in *separate* registers across the walk loop,
`a1` holding `row * 15` and `a3` holding the address of `D_8016AB38`, and does
two `addu` inside the loop. The pointer hoist folds them into one register and
does one, which is the missing `addu`. Rewriting the array as a flat `s8 []`
and hoisting `row * 15` into its own local separates them in the source but
measures worse, at 12, because GCC then recomputes the multiply; spelling the
multiply inline in both uses gives 10. So the separation is real and the
spelling that produces it has not been found.

The other is that the flag argument must go through the `n` local. Writing
`flag = (D_8009B3A4[0] & 0x4000) != 0;` in one statement, or as a shift and
mask, costs two. The two-step form is what the target's `andi` followed by
`sltu` against zero wants.


The opcode distance rose from 10 to 12 while the candidate got substantially
closer, which is the reason the header now also quotes a prefix. Opcode
distance is an L1 distance between two opcode multisets, so it cannot see
order, and it cancels: an error that adds an instruction and an error that
drops one net to zero. Once register allocation is the dominant residual that
cancellation is the normal case, and the metric then punishes a correction for
exposing the error that had been hiding behind it. The length of the leading
run of instructions whose mnemonics agree cannot cancel, because it stops at
the first disagreement. Steering by it took the agreeing prefix from 15
instructions to 91, and the differing-position count fell from 321 to 281 at
the same time, so the two honest metrics agree with each other and only the
multiset distance dissents.

Three shape corrections came out of reading that prefix, and the last two only
work together.

The transition's remaining distance is written as a subtraction. The target
computes `addiu v0,a0,-16` on the loaded `w->f3C` and then `subu` against
`w->f5E`, which is `home = w->f3C - 16; delta = w->f5E - home;`. The previous
source used `home = 16 - w->f3C; delta = w->f5E + home;`, which is the same
value and costs an extra `li` of the constant 16 because 16 has to be
materialised before it can be subtracted from. This function's own note used
to record the addition form as the better one, and it was, at a distance of
14; the levers expire when the base changes and this one had.

The frame countdown in `w->f60` is signed. The target stores the decremented
value and then tests it with `sll` by 16 followed by `bnez`, which is how GCC
tests a 16-bit signed value against zero; an unsigned one gives `andi` with
`0xffff`. Declaring the field `s16` and writing the plain
`w->f60 = w->f60 - 1; if (w->f60 != 0)` produces the target's sequence,
including the store landing before the branch rather than in its delay slot.
Six spellings reach the same code, so the field's type is what matters here
and not the shape of the statement.

The two cursor directions are separate `if` statements, not an `else if`
chain. The branch that skips the column wrap goes to `0x110`, which is the
`0x8000` test for the other direction, so a frame that has already
incremented the column still tests the decrement bit and re-reads the pad to
do it. Writing it as two independent tests is worth nothing on its own. The
upper wrap spelled `>= 15`, which the target's `slti` had already proved
correct, is worth *minus* two on its own, because the extra `li` of the
constant 15 that `== 15` needs was filling a load-delay slot that the target
leaves as a `nop`. Together they are worth 33 instructions of agreeing prefix
and they restore the exact instruction count. This is the clearest case yet
for measuring levers as a product: both were separately recorded as inert or
harmful, and both are correct.

The remaining prefix stops at a branch delay slot. The target leaves the slot
after `bnez v0,0x1b0` empty and puts the constant 11 in `v1` afterwards; this
build hoists `li v0,11` into the slot. The target cannot do that because it
schedules the pad load into `v0` first, so the constant has to go somewhere
else, and `v1` holds the pad's base address and is live. Eleven orderings of
the four statements in that arm all leave the prefix at 91, so the ordering is
not the handle; the scheduling of the pad load is.

One further target behaviour is decoded but not yet reproduced. The cell that
the select path dispatches on is loaded unsigned and masked, `lbu` then
`andi` with `0xf`, which is what GCC emits for `D_8016AB38[row][col] & 0xF`
because a sign extension under a four-bit mask is dead. Spelling the mask
costs two instructions here rather than the one it should, so the target is
saving those two somewhere this build still spends them, and the mask is
being held back until that is found.

Two of the declared locals were dead and neither the emitted code nor any
metric changed when they were removed, which is worth stating because the
declaration list is often assumed to be a register-allocation lever in this
compiler. Nine orders of the remaining sixteen, including reversing them,
grouping the pointers first and grouping them last, and moving the struct
pointer to the end, all produce byte-identical output. Declaration order is
inert here; the allocation that is still wrong is being decided by something
else. The twenty-nine compiler profiles are closed at this base too: only
`gcc_2_8_1_cc_g0_as_g8_split` ties the current profile and nothing beats it.


The instruction count is exact again and the last cursor-column scale is
written as a multiplication rather than a shift.

`(D_8016D42C << 4) + 112` and `D_8016D42C * 16 + 112` are the same value and
different code. The shift form makes GCC load the `s8` with `lbu` and then
sign-extend and scale in one go, `sll` by 24 followed by `sra` by 20, which
is three instructions. The multiplication form loads with `lb`, which is
already sign-extended, and scales with a single `sll` by 4. The target has
the `lb`, so the source multiplies. Worth one of the distance and one
instruction, and assigning the global to a local first reaches the same
place, which is the same effect by a different route.

This was found by aligning the sequence of `lb`, `lbu` and `sra` on both
sides with `difflib` rather than comparing their totals. The totals said two
`lb` missing and one `sra` extra, which is three separate-looking problems;
the alignment showed a single replacement at the end of the function and
named the statement.

Two findings, read off the narrow load and store widths rather than off the
positions, and they compose: 14 alone becomes 12 and 13, and 11 together.

The cursor's x position is stored straight into `D_8016D434` in both arms of
the eleventh-column test rather than computed into a local and stored once.
The target has two `sh` to that address and this build had one, which is
what a local produces. Worth two of the distance.

The y position is stored before `func_800429D8` rather than after it. The
target reloads both `D_8016D434` and `D_8016D436` with `lh` when it computes
the two tween deltas; this build reloaded only the first, because the second
store sat after the call and GCC could still see the stored value in a
register. Moving the store above the call makes the call clobber it and
forces the reload. Worth one more.

Both are the same observation from two directions: the target keeps these
two globals in memory rather than in registers across this block, and the
source shapes that produce that are a direct store and a store placed before
the call.

The instruction count is now exact. The change that got there is a single
subtraction written the other way round: the target computes the tween
distance with `addu`, so the constant 16 is subtracted from the widget's
current position and the result added, rather than 16 being folded into a
home value that is then subtracted. Writing

    home = 16 - w->f3C;
    delta = w->f5E + home;

instead of `home = w->f3C - 16; delta = w->f5E - home;` supplies the `addu`,
removes one `subu`, and takes the count from 381 to the target's 382 while
the distance falls from 15 to 14. The flat form, `delta = w->f5E - w->f3C +
16`, reaches the same count but measures 16, and negating the difference is
byte-identical to the original, so the improvement is specifically about
which operand carries the constant.
The frame countdown is then cheapest read as an `s32` local with the test
cast, `if ((u16)n != 0)`, which is worth one differing position over the
`u16` local and identical on every other measure.

Two content faults were read off the target's opcode classes rather than off
its positions, and both are about where a value is written rather than what
the code says.

The frame countdown is a `u16` local. The target stores the raw decremented
value with `sh` and then tests it with `sll v0,v0,0x10` followed by `bnez`,
which is the canonical way to ask whether the low sixteen bits are non-zero.
An `s32` local tests all thirty-two and needs no shift. Worth one of the
distance.

The `0x4000` flag is masked into a local before it is tested. Written as
`flag = (D_8009B3A4[0] & 0x4000) != 0` the compiler recognises a single-bit
test and folds it to `srl` by fourteen and `andi` by one. The target instead
has `andi` by `0x4000` followed by `sltu s0,zero,v0`, which is what a
comparison against a value it cannot see as one bit produces. Naming the
masked value defeats the fold. Worth two more.


The two `NameEntry_AdjustLength` sites always play a sound; they do not test
its result to decide whether to play one. The target reads
`bnez v0` selecting `a0 = 12` over `a0 = 9` and then falls into a single
`jal SD_SEPlayFull`, so the shape is
`SD_SEPlayFull(NameEntry_AdjustLength(...) ? 12 : 9)` rather than
`if (NameEntry_AdjustLength(...) == 0) SD_SEPlayFull(9)`. The stored candidate
had dropped the `12` path at both sites, which cost three instructions and
three of the distance. Correcting both is worth more than correcting either:
21 to 20 and 19 alone, 18 together.

This was found by `tools/project/overlay_arity_audit.py`, which reported that
the candidate did not make the target's last two calls, to
`NameEntry_AdjustLength` and `SD_SEPlayFull`. Reading the source against that
report is what exposed the dropped `12` path, since the calls were present but
conditional.

The audit still reports the same two calls as missing after the fix, so the
placement difference is a second and still-open fault rather than a symptom of
the first. The target lays this block out at the very end, after the main body
jumps over it to the epilogue at index 358; this candidate emits it inline at
the `select:` gate. Moving it behind a forward `goto` at the end of the
function was measured and is worse, 19 and 379 instructions, so the placement
is not controlled by statement position here.

Four spellings of the ternary at each site, sixteen cells, are all flat at 18,
and so are all 29 profiles. `gcc_2_8_1_g0_no_sched2_split` is worth noting as
the only near neighbour: distance 19 but 381 instructions and 334 differing
positions against this profile's 380 and 354.

The widget struct's tail fields sit at +0x5E and +0x60, so the padding
between +0x3E and them is 32 bytes, not 56. The earlier candidate put them
at +0x76 and +0x78, twenty-four bytes too far, and the field at +0x60 is
read unsigned. Neither the opcode distance nor the position count could see
that -- a `sh` is a `sh` at any offset, and those positions already differed
on their base register -- but the register-blind instruction multiset does,
and it falls by fourteen.

Three more source shapes were worth measuring. The frame counter's test
drops the `(s16)` cast, which is worth two of the distance. The cursor's
forward wrap compares `== 15` rather than `>= 15`, worth one. And the cell
lookup takes the table entry whole rather than masking it with `0xF`, worth
two more; the mask was never in the target.

Note that the differing-position count moves the wrong way across these,
311 to 352, because removing instructions displaces everything after them.
The distance and the register-blind measure both improve.

```c
#include "../../types.h"

typedef struct {
    u8 pad0[48];
    s16 f30;
    s16 f32;
    u8 pad34[2];
    s16 f36;
    s16 f38;
    u8 pad3A[2];
    u16 f3C;
    u8 pad3E[32];
    u8 f5E;
    u8 pad5F;
    s16 f60;
} W;

extern u16 D_8016D4D4;
extern W *D_8016D404;
extern s16 D_8016D434;
extern s16 D_8016D436;
extern s8 D_8016D401;
extern u8 D_8016D402;
extern u8 D_8016D426;
extern s8 D_8016D42C;
extern u16 *D_8016D418;
extern volatile u8 D_8016D400;
extern s8 D_8016AB38[][15];
extern u8 D_8016ABC0[][2];
extern volatile u16 D_8009B3A4[];
extern volatile u16 D_8009B394[];
extern volatile u16 D_8009B398[];

extern void func_80042A78(W *);
extern void func_800429D8(W *);
extern void func_8003FEE0(s32);
extern s32 NameEntry_AdjustLength(s32, s32);
extern u8 *TextBox_GetGlyphAt(s32, s32, s32);
extern u8 *func_80168CDC(s32, u8 *);
extern void func_80168708(void);
extern void func_80168AB4(void);

void func_8016913C(void)
{
    W *w;
    s32 home;
    s32 delta;
    s32 n;
    s32 cell;
    s32 col;
    s32 row;
    s32 flag;
    s32 y;
    u8 *node;
    u8 *obj;
    s32 kind;
    s32 second;
    s32 gx;
    s32 gy;
    s32 d;
    s8 *rp;

    w = D_8016D404;
    if ((D_8016D4D4 & 0x4000) != 0) {
        home = w->f3C - 16;
        delta = w->f5E - home;
        if (delta != 0) {
            w->f3C = (delta >= 0) ? (w->f3C + 2) : (w->f3C - 2);
        }
        func_80042A78(w);
        w->f60 = w->f60 - 1;
        if (w->f60 != 0) {
            return;
        }
        w->f3C = w->f5E + 16;
        w->f30 = D_8016D434;
        w->f32 = D_8016D436;
        D_8016D4D4 &= 0xBFFF;
    }
    if ((D_8009B3A4[0] & 0xF000) != 0) {
        if ((D_8009B3A4[0] & 0x2000) != 0) {
            D_8016D401 = D_8016D401 + 1;
            if (D_8016D401 >= 15) {
                D_8016D401 = 0;
            }
        }
        if ((D_8009B3A4[0] & 0x8000) != 0) {
            D_8016D401 = D_8016D401 - 1;
            if (D_8016D401 < 0) {
                D_8016D401 = 14;
            }
        }
        if ((D_8009B3A4[0] & 0x5000) != 0) {
            if (D_8016D401 >= 11) {
                n = D_8009B3A4[0] & 0x4000;
                flag = (n != 0);
                D_8016D401 = 11;
                D_8016D402 = D_8016ABC0[(s8)D_8016D402][flag];
            } else if ((D_8009B3A4[0] & 0x1000) != 0) {
                D_8016D402 = D_8016D402 - 1;
                if ((s8)D_8016D402 < 0) {
                    D_8016D402 = 8;
                }
            } else {
                D_8016D402 = D_8016D402 + 1;
                if ((s8)D_8016D402 >= 9) {
                    D_8016D402 = 0;
                }
            }
            D_8016D426 = D_8016D402;
        }
    } else {
        goto alt800;
    }
tail47:
    func_8003FEE0(47);
    row = (s8)D_8016D402;
    col = (s8)D_8016D401;
    cell = D_8016AB38[row][col];
    rp = D_8016AB38[row];
    while (cell < 0) {
        col = col + cell;
        cell = rp[(s8)col];
        D_8016D401 = col;
    }
    D_8016D402 = D_8016D426;
    w->f5E = 0;
    if (cell != 0) {
        if ((cell & 0x40) != 0) {
            w->f5E = 20;
        }
        D_8016D402 = cell & 0xF;
    }
    D_8016D434 = (s8)D_8016D401 * 20 + 22;
    if ((s8)D_8016D401 >= 11) {
        D_8016D434 = (s8)D_8016D401 * 20 + 42;
    }
    y = (s8)D_8016D402 * 18 + 24;
    D_8016D436 = y;
    func_800429D8(w);
    w->f60 = 8;
    w->f36 = ((D_8016D434 - w->f30) << 8) / 8;
    w->f38 = ((D_8016D436 - w->f32) << 8) / 8;
    D_8016D4D4 |= 0x4000;


alt800:
    if ((D_8009B398[0] & 0x800) == 0) {
        goto select;
    }
    D_8016D401 = 14;
    D_8016D402 = 8;
    goto tail47;
select:
    if ((D_8009B394[0] & 0xC0) == 0) {
        goto sel_ret;
    }
    kind = 0;
    second = kind;
    row = (s8)D_8016D402;
    col = (s8)D_8016D401;
    n = D_8016AB38[row][col] & 0xF;
    gx = kind;
    if (n == 4) {
        goto arm4;
    }
    if (n == 6) {
        goto arm6;
    }
    goto arme;
arm4:
    d = 1;
    if (col == 11) {
        d = -1;
    } else {
        gx = 20;
    }
    n = NameEntry_AdjustLength(d, 6);
    if (n != 0) {
        n = 12;
    } else {
        n = 9;
    }
    func_8003FEE0(n);
    gy = 36;
    goto join;
arm6:
    second = 2;
    gy = 72;
    D_8016D400 |= 0x40;
    goto join;
arme:
    kind = 1;
    gx = (s8)D_8016D401 * 20;
    gy = ((s8)D_8016D402 * 9) << kind;
    func_8003FEE0(41);
join:
    ;
    node = TextBox_GetGlyphAt(kind, gx, gy);
    obj = func_80168CDC(kind, node);
    obj[0x6C] = 1;
    *(void **)(obj + 0x24) = func_80168708;
    if (node == 0) {
        *(u16 *)(obj + 8) &= 0xFFBF;
    }
    if (second != 0) {
        *(s16 *)(obj + 0x48) = 20;
        node = TextBox_GetGlyphAt(kind, gx + 20, gy);
        obj = func_80168CDC(kind, node);
        obj[0x6C] = 1;
        *(void **)(obj + 0x24) = func_80168708;
        *(s16 *)(obj + 0x48) = 0;
    }
    if (kind == 1) {
        u16 *slot;
        D_8016D400 |= 0x80;
        slot = &D_8016D418[D_8016D42C];
        if (node == 0) {
            *slot = 0;
        } else {
            *slot = *(u16 *)node;
        }
        obj = func_80168CDC(1, node);
        *(s16 *)(obj + 0x60) = 8;
        *(void **)(obj + 0x24) = func_80168AB4;
        *(s16 *)(obj + 0x46) = 204;
        *(s16 *)(obj + 0x44) = D_8016D42C * 16 + 112;
        obj[0x6C] = 6;
    }
    return;
sel_ret:
    if ((D_8009B394[0] & 0x20) != 0) {
        func_8003FEE0(NameEntry_AdjustLength(-1, 6) != 0 ? 12 : 9);
    }
    return;
}
```
## main_menu `func_80180390` at 0x80180390

`gcc_2_8_1_g0_split`, 495 instructions against 495, opcode distance 0 with
every mnemonic count exact. The longest common subsequence of mnemonics is
494 of 495, the first 386 instructions agree, and 25 positions differ.

Every remaining difference is a register name. The instruction sequence is one
short of identical, no instruction is missing, extra or misplaced, and the 25
disagreeing positions are five small clusters where a value the target keeps in
`v0` this candidate keeps in `v1`, or the reverse. There is no longer any
structural question outstanding: what remains is allocation order alone.

One of those clusters was hidden rather than absent. A diff that normalises
displacements to compare registers alone reports the two `lh` instructions that
start the animation block as agreeing, because they differ only in their offsets:
the target loads `0x38` and then `0x36`, and this candidate loaded them the other
way round. Writing the difference as `value = *(s16 *)(eloop + 0x38); delta =
value - *(s16 *)(eloop + 0x36);` puts the loads in the target's order and takes
the differing positions from 28 to 25. The lesson is about the tool rather than
the function: normalising a field to isolate one kind of difference also hides
every difference in that field, so a register-level diff has to be read against
an unnormalised one before concluding that a region is register-only. Embedded
assignment and comma forms that ought to express the same evaluation order are
canonicalised back to the original and do not reproduce it.

The candidate reached this state through six independent levers, each of which
was found by decoding a specific instruction the target emits and asking what
source shape could produce it. They are recorded here with the mechanism rather
than the outcome, because each one generalises.

**A `volatile` store pins the scheduler and steals a delay slot.** The pad-hold
accumulation wrote its result through `*(volatile s16 *)(entry + 0x36)`. A
volatile memory reference is a scheduling barrier, so the `li v0,-2` belonging
to the return below it could not be moved down into the branch delay slot where
the target puts it; instead it drifted up into the load-delay slot after the
field read, which the target leaves as a `nop`. Dropping the qualifier let both
land where the target has them and moved the agreeing prefix from 190 to 204.
The qualifier is genuinely required on the two writes to `entry + 0x30` and
`gMain_bMenuID`, where removing it costs four and two instructions
respectively, so this is not a blanket rule: volatile is a scheduling
instruction as much as a semantic one, and each occurrence has to earn itself.

**`if (cond) return A; return B;` flips a branch polarity for free.** The
comparison against 3000 emitted `bnez` where the target emits `beqz`. Reaching
the `-2` return through a label costs `{j +1, nop +1}` because a `goto` to a
separate return block needs a real jump. Writing the test in its inverted form
with both returns inline costs nothing: GCC lays the `-2` path on the branch and
the `-1` path on the fall-through, and both jump straight to the shared
epilogue with the return value in their own delay slots. The same lever appears
twice more further down, and the choice is not uniform — the site after the
entry loop wants `return -1` so its value fills the `bnez s4` delay slot, while
the `vx3` test immediately after wants `goto ret_m1` so it branches to the
shared `li v0,-1` block. Sweeping all eleven return sites one at a time is
cheap and is the only reliable way to tell which is which.

**`MEM_IN_STRUCT_P` lets a store sink past a global load.** The per-entry
countdown at `entry + 0x60` had to move down into a branch delay slot, crossing
a `lbu` of `gMain_bMenuID`. GCC 2.x will not reorder a store through a plain
casted pointer against a global scalar, but it treats an aggregate member
reference as unable to conflict with a non-struct scalar, so writing the store
through a one-member struct type frees it. That single change moved the prefix
from 214 to 232. This is the same mechanism recorded for `FreeDuel_Init`, and
it is worth reaching for whenever a store and a nearby global access refuse to
interleave.

**One C variable is one pseudo, so variable identity is register allocation.**
GCC 2.8.1 has no SSA; every reference to a C local belongs to a single allocno
that receives a single hard register for the whole function. Three separate
gains came from this. Splitting the pointer used inside the entry loop away
from the pointer used before it raised the loop copy's frequency-weighted
priority and moved it from `a1` into `a0`, which is what the rest of the block
is allocated around. Giving the later clear-flags loop its own counter and its
own walking pointer shortened the live ranges of `i` and `slot` enough to undo
a three-way rotation in which `frame`, `slot` and `i` held `s2`, `s0` and `s1`
instead of the target's `s0`, `s1` and `s2`. Giving the final accumulation
region its own pointer rather than reusing a name already live in the fade
block took the differing positions from 43 to 28. The reverse direction matters
too: at the two sites that only set or clear a flag bit, the target holds the
pointer in `v1`, a block-local temporary, which is what dereferencing `*slot`
inline produces and what a named variable prevents.

**Merging roles into one variable can be the point.** The opposite move was
decisive around the animation block. The target's `v0` holds three unrelated
values in succession — the field at `0x36`, the countdown at `0x60`, then the
field at `0x38` — which is the signature of one C variable reused three times.
While these were three separate expressions the third load had a free register
and hoisted into a load-delay slot the target leaves empty; once they share
`value` the third load cannot move above the second's use and the `nop` is
restored. This took the prefix from 232 to 243 and, unusually, improved the
distance, the position count and the subsequence at the same time.

**A basic-block boundary decides what the scheduler may fill.** Signed division
by a power of two expands to a rounding `bgez`, which splits the block. The
target reloads the entry pointer between the `mult` and that branch, so in the
source the reload must precede the divide, and the product must live in its own
local so the divide's destination coalesces instead of paying a `move`. Written
as `step = rsin(...) * delta; eloop = *slot; value = step / 0x1000;` the prefix
goes from 244 to 283. The same reasoning applies one block later: splitting
`gMain_bMenuID = value % count + base` into an assignment to `value` followed by
the store keeps the following statement's address setup from being hoisted into
the `mfhi` hazard slot, and took the subsequence from 492 to 494 and the
differing positions from 188 to 70 in a single step.

**Two further orderings were read directly off the target.** The paired
conditionals that pick `base` and `count` must assign `base` first — the
polarities were already right, only the order was wrong, and swapping it moved
the prefix from 342 to 384. The clear-flags loop must be written as an explicit
pointer walk with the counter initialised before the loop rather than as an
indexed `for`, because `loop.c` appends the derived induction variable's
initialiser at `NOTE_INSN_LOOP_BEG`, after the loop initialiser, and the target
shows the counter being zeroed first; that took the prefix from 298 to 342.

What is left is five clusters of `v0`/`v1` inversion, at the countdown and
menu-id test, the delta and frame computation, the clear-flags counter, and the
modulo and store. Declaration order, the eight statement orderings around the
divide, all twenty-nine compiler profiles, and the guard shapes for the main
entry test have all been swept against this source and are inert; the profile
sweep confirms `gcc_2_8_1_g0_split` is uniquely best. The next thing to try is
the identity of the temporaries in those five clusters, on the evidence that
variable identity has produced every one of the last four gains.

```c
#include "../../types.h"

extern u8 *D_80184560;
extern u8 *gMain_apMenuEntries[];
extern u8 gMain_bMenuID;
extern u8 D_80184595;
extern u8 D_80184596;
extern u8 D_80184597;

extern s8 D_80184598;
extern u8 D_80184599;
extern u8 D_8018459A;
extern u8 D_8018459B;
extern u8 D_8018459C;
extern u8 D_8018459D;
extern u16 D_8009B0D8;
extern u16 D_8009B394;
extern volatile u16 D_8009B398;
extern u8 D_8009B3EA;
extern u8 D_8009B3ED;

extern void func_80180D2C(s32);
extern void func_80180E6C(u8 *);
extern void func_80040410(u8 *, s32);
extern void Input_ResetPads(void);
extern s32 SaveData_PollLoad(void);
extern void SaveData_RequestLoad(void);
extern s32 func_8003FCD8(void);
extern s32 func_8003FD14(void);
extern s32 func_8003F70C(void);
extern void func_8003F87C(void);
extern void SD_SEPlay(s32, s32, s32);
extern s32 rsin(s32);

typedef struct { s16 h; } H16s;

s32 func_80180390(void)
{
    u8 *ent3;
    u8 *entry;
    u8 *ent6;
    u8 **slot;
    s32 j;
    u8 **slot2;
    s32 step;
    s32 level;
    s32 value;
    s32 frame;
    s32 delta;
    s32 moved;
    s32 i;
    s32 base;
    s32 count;
    s32 lvl;
    s32 vx3;
    s32 acc;
    s32 neg;
    s32 chr;
    u8 *ent5;
    s32 poll;
    u8 *ent2;
    u8 *eloop;

    if (D_8018459B != 0) {
        poll = SaveData_PollLoad();
        if (poll != 0) {
            if (poll == 1) {
                Input_ResetPads();
                func_80180D2C(1);
                D_8018459B = 0;
            } else {
                Input_ResetPads();
                D_8018459B = 0;
            }
        }
        goto ret_m1;
    }

    if (D_8018459C != 0) {
        poll = func_8003FCD8();
        if (poll != 0) {
            if (poll == 1) {
                Input_ResetPads();
                func_80180D2C(1);
                D_8018459C = 0;
            } else {
                Input_ResetPads();
                D_8018459C = 0;
            }
        }
        goto ret_m1;
    }

    if (D_8018459D != 0) {
        poll = func_8003FD14();
        if (poll != 0) {
            if (poll == 1) {
                Input_ResetPads();
                func_80180D2C(1);
                D_8018459D = 0;
            } else {
                Input_ResetPads();
                D_8018459D = 0;
            }
        }
        goto ret_m1;
    }

    if (D_8018459A != 0) {
        if (func_8003F70C() == 0) {
            goto ret_m1;
        }
        Input_ResetPads();
        D_8018459A = 0;
        goto ret_m1;
    }

    step = D_80184598;
    if (step != 0) {
        level = D_80184597 + (step << 3);
        D_80184597 = level;
        if (step > 0) {
            if ((s8)level < 0) {
                goto fade_done;
            }
        }
        if (step >= 0) {
            goto ret_m1;
        }
        if ((u8)level != 0) {
            goto ret_m1;
        }
    fade_done:
        if (D_80184598 < 0) {
            ent2 = D_80184560;
            ent2[0xE] = 0x80;
            ent2[0xD] = 0x80;
            ent2[0xC] = 0x80;
            *(u16 *)(ent2 + 8) |= 0x40;
            D_80184560[0x6C] = 0x3C;
            *(s16 *)(D_80184560 + 0x36) = 0;
        }
        D_80184598 = 0;
        goto ret_m1;
    }

    entry = D_80184560;
    if (entry != 0 && (*(u16 *)(entry + 8) & 0x40) != 0) {
        if (entry[0x6C] != 0) {
            entry[0x6C] = entry[0x6C] - 1;
        } else {
            lvl = entry[0xE] + entry[0x60];
            entry[0xE] = lvl;
            entry[0xD] = lvl;
            entry[0xC] = lvl;
            entry = D_80184560;
            chr = entry[0xC];
            if ((u32)(chr - 0x41) >= 0x3F) {
                if ((s8)chr < 0) {
                    entry[0x6C] = 0x3C;
                }
                ent5 = D_80184560;
                neg = *(s16 *)(ent5 + 0x60);
                *(s16 *)(ent5 + 0x60) = -neg;
            }
        }
        if ((D_8009B398 & 0x800) != 0) {
            SD_SEPlay(7, 0xFF, 0);
            ent3 = D_80184560;
            *(u16 *)(ent3 + 8) &= 0xFFBF;
            func_80180D2C(0);
            D_80184598 = 1;
            goto ret_m1;
        }
        ent6 = D_80184560;
        acc = *(u16 *)(ent6 + 0x36) + D_8009B0D8;
        *(s16 *)(ent6 + 0x36) = acc;
        if ((s16)acc >= 0xBB8) {
            return -2;
        }
        return -1;
    }

    if (D_80184599 != 0) {
        moved = 0;
        i = 0;
        slot = gMain_apMenuEntries;
    entry_loop:
        eloop = *slot;
        if (eloop == 0) {
            goto next_entry;
        }
        if (*(s16 *)(eloop + 0x60) <= 0) {
            goto next_entry;
        }
        ((H16s *)(eloop + 0x60))->h = *(u16 *)(eloop + 0x60) - 1;
        if ((u32)gMain_bMenuID < 5) {
            if (i >= 5) {
                goto hide_entry;
            }
        } else {
            count = i;
            if (count < 5) {
                goto hide_entry;
            }
        }
        eloop = *slot;
        value = *(s16 *)(eloop + 0x38);
        delta = value - *(s16 *)(eloop + 0x36);
        value = *(s16 *)(eloop + 0x60);
        frame = 0x10 - value;
        value = *(u16 *)(eloop + 0x38);
        if (frame != 0x10) {
            step = rsin(frame << 6) * delta;
            eloop = *slot;
            value = step / 0x1000;
            value = *(u16 *)(eloop + 0x36) + value;
        }
        *(volatile s16 *)(eloop + 0x30) = value;
        if ((frame & 1) != 0) {
            func_80180E6C(*slot);
        }
        *(u16 *)(*slot + 8) = *(u16 *)(*slot + 8) | 0x40;
        goto tick_entry;
    hide_entry:
        *(u16 *)(*slot + 8) = *(u16 *)(*slot + 8) & 0xFFBF;
    tick_entry:
        moved++;
        func_80040410(*slot, (i << 1) | (gMain_bMenuID != i));
    next_entry:
        i++;
        slot++;
        if (i < 0xB) {
            goto entry_loop;
        }
        if (moved != 0) {
            return -1;
        }
        vx3 = D_80184596;
        D_80184599 = 0;
        if (vx3 == 0) {
            goto ret_m1;
        }
        if (D_80184595 != 0) {
            if ((u32)gMain_bMenuID < 5) {
                j = 0;
                slot2 = gMain_apMenuEntries;
                for (; j < 0xB; j++) {
                    ent3 = *slot2;
                    if (ent3 != 0) {
                        *(u16 *)(ent3 + 8) &= 0xFFBF;
                    }
                    slot2++;
                }
                D_80184598 = -1;
            } else {
                func_80180D2C(0);
                gMain_bMenuID = 1;
            }
            D_80184595 = 0;
            return -1;
        }
        if (gMain_bMenuID != 1) {
            return gMain_bMenuID;
        }
        func_80180D2C(0);
        gMain_bMenuID = 5;
        return -1;
    }

    if ((D_8009B394 & 0x5000) != 0) {
        if ((u32)gMain_bMenuID >= 5) {
            base = 5;
        } else {
            base = 0;
        }
        if ((u32)gMain_bMenuID < 5) {
            count = 5;
        } else {
            count = 6;
        }
        func_80040410(gMain_apMenuEntries[gMain_bMenuID], (gMain_bMenuID << 1) | 1);
        if ((D_8009B394 & 0x1000) != 0) {
            value = gMain_bMenuID - base + count - 1;
        } else {
            value = gMain_bMenuID - base + count + 1;
        }
        value = value % count + base;
        *(volatile u8 *)&gMain_bMenuID = value;
        func_80040410(gMain_apMenuEntries[gMain_bMenuID], gMain_bMenuID << 1);
        SD_SEPlay(6, 0xFF, 0);
        goto ret_m1;
    }

    if ((D_8009B398 & 0x8E0) == 0) {
        return -1;
    }
    if ((D_8009B398 & 0x20) != 0) {
        if ((u32)gMain_bMenuID < 5) {
            SD_SEPlay(9, 0xFF, 0);
            return -1;
        }
        SD_SEPlay(8, 0xFF, 0);
        D_80184595 = 1;
    } else {
        SD_SEPlay(7, 0xFF, 0);
        switch (gMain_bMenuID) {
        case 1:
            SaveData_RequestLoad();
            D_8018459B = D_8018459B + 1;
            return -1;
        case 3:
            D_8009B3ED = 0;
            D_8009B3EA = 0;
            D_8018459C = D_8018459C + 1;
            return -1;
        case 2:
            D_8009B3ED = 0;
            D_8009B3EA = 0;
            D_8018459D = D_8018459D + 1;
            return -1;
        case 0xA:
            func_8003F87C();
            D_8018459A = D_8018459A + 1;
            return -1;
        }
    }
    func_80180D2C(1);
ret_m1:
    return -1;
    return -1;
}
```

## password `func_8016A37C` at 0x8016A37C

The second of the three `lui` points is fully explained, and it is a register
conflict rather than a scheduling preference. At word 266 the target branches on
`flags & 0x8000` and fills the delay slot with `lui a2`, speculative work for the
branch target at word 279, which then reads through `a2`. This candidate cannot
do that because `a2` is already occupied: its `flags` local is allocated to `a2`,
where the target holds the same value in `v1` as a short-lived temporary. With
`a2` live across the branch there is no register for the speculative high half,
so the slot takes the `ori` from the fall-through path instead and the branch
target pays for its own `lui` at word 279, which is the third point.

So the second and third points are one fact seen twice: a single allocation
choice for `flags` both denies the delay slot its speculative load and forces the
rematerialisation nine instructions later. Sixteen subsets of replacing `flags`
with direct global reads in case 2 are byte-identical, and the three flags-split
variants recorded above do not move it either, so the remaining question is what
makes GCC prefer a callee-clobbered argument register for a value the target
keeps in `v1`.

The residual is three instruction placements, not the four large clusters an
unaligned position count suggests. Aligning the two streams on mnemonic plus
immediates, so that register renaming does not block the alignment, splits the
206 raw disagreements into 74 register renames, 51 masked relocations and 81
genuine ones, and reduces the genuine ones to three points. The candidate is
missing a `lui` at word 236, missing another at word 267, and materialises one
it should not at word 279; everything between those points is the same
instructions shifted by one position and renamed.

All three concern where a `%hi(0x8017)` lives. At word 236 the target
rematerialises the high half into `v1` for `D_8016D424 |= 0x4000` even though
`s1` already holds it from the top of the case, and this candidate reuses `s1`.
At word 267 the target fills the delay slot of the preceding `bnez` with a
`lui a2`, speculative work for the branch target, and then reuses `a2` at words
279 and 319; this candidate has no such register live and pays for a fresh one
at 279. Seven spellings of the `0x4000` update, covering the compound and
explicit forms, a volatile cast at the site, a volatile read, a volatile
declaration, a local copy and reuse of the already-loaded `flags`, all leave the
agreeing prefix at 236, and the two that change anything are worse.

`gcc_2_8_1_g0_split`, 365 of 365 instructions, 94 differing positions,
opcode distance 0.

The ninety-four differing positions are not evenly spread and they are not
noise. Measured as a leading run of agreeing mnemonics the candidate reaches
236 of 365 before the first disagreement, the longest common subsequence of
mnemonics is 358 of 365, and the differing words fall into exactly four
clusters: 197 to 203, 223 to 228, 236 to 271 and 279 to 326. Naming them turns
one large number into four bounded problems.

The first cluster is one register. The target holds the flags word read from
`D_8016D424` in `a0` and this build holds it in `a2`, and every other
difference in that range follows from it. The block is otherwise identical
instruction for instruction, including the `andi` of `0x4000` for the later
test being hoisted into the branch delay slot, which both sides do.

The second cluster is `v0` and `v1` exchanged between the two loads that feed
the `sltu`. It is tempting to read the target as comparing the two values the
other way round and to fix the source accordingly. That is wrong: writing
`D_801D0000[504] < D_801A8000[D_8016D49C * 2]` takes the cluster from six
positions to nine and the total from 94 to 96, so the existing spelling is
confirmed and the exchange is allocation rather than order.

The third and fourth clusters are one disagreement in two directions, and it
is the reason the opcode distance reads zero. At word 236 the target
re-materialises the high half of the flags address into a fresh `v1`, while
this build reuses the copy already sitting in `s1` and is one instruction
short there. At word 279 the reverse happens: the target reuses the `a2` it
already has and this build emits a new `lui` and is one instruction long. The
two cancel exactly, so the opcode multiset is perfect while the register
assignment is wrong in two places. Anyone reading the distance alone would
conclude the instruction mix is solved and only ordering remains; what is
actually happening is two errors of opposite sign.

The fourth cluster is the count-and-step division chain, and its register map
is uniformly displaced rather than scrambled: the target keeps `count` in `a0`
and `step` in the callee-saved `s0` and takes each `mfhi` into `a3`, while this
build keeps `count` in `a1`, `step` in `a0` and takes each `mfhi` into `t0`.

The obvious reading of that, which was recorded here first, is that the target
must have fewer values live across the block, because it can spare an argument
register where this build has to reach past them. That reading is wrong and
should not be inherited. Both sides hold exactly two long-lived `%hi` base
pseudos through the whole case, and they hold the same two: the high half of
`D_8016D424` and the high half of `D_8016D438`, which are separate pseudos
under `-msplit-addresses` even though the two symbols share a high half. The
target puts them in `a1` and `a2`, leaving `a3` free for the `mfhi`; this build
puts them in `a2` and `a3`, so the `mfhi` has to go to `t0` and everything else
shifts by one. The live count is identical. Only the order in which the
allocator reached the two bases differs.

`step` landing in the callee-saved `s0` is not evidence of a value living
across a call either. Its last use, the subtraction from `D_801D0000[504]`,
precedes the only call in the block, and moving that call earlier so that
`step` genuinely is live across it costs one of the distance and does not move
any of the four clusters.

Nothing found so far moves them. The tail of the case in six statement orders,
including three that put the call before the subtraction, leaves all four
windows at exactly five, six, thirty-six and forty-seven. Merging `flags4` into
`flags`, merging `msg` into `state`, and both together, leave them unchanged
while making the total worse. `count` and `step` must be unsigned: spelling
them `s32` costs thirty-eight, because the four divisions become signed and
each grows the sign-correction sequence. The residual is an allocator ordering
decision that none of the source axes tried so far can reach.


Two of those nine came from state 4, which is now correct apart from a
single position. Both are about *pseudo identity* rather than about what the
code says, and neither is visible in the source's meaning:

- The widget is read straight out of `D_8016D418` inside the `0x8000` arm
  rather than through the `widget` local that the following statements use.
  Worth six positions. Introducing a local there gives the value a longer
  live range than the target's, which changes its class.
- State 4 takes its *own* `flags` local instead of sharing the one that
  states 1, 2 and 3 use. Worth three more. The sharing is what couples the
  four live ranges into one pseudo and forces a common allocation.

The direction matters and is not uniform: giving state 3 its own local is
worth *minus* four, and giving every state its own is minus seven. Only
state 4 wants to be separate. All eight combinations were measured.

The instruction mix is exact, so only register choice and scheduling remain.

Six earlier findings still hold: the five-way dispatch is a `switch` whose
range check must not be duplicated; the starchip counter is unsigned; state 1
falls through into state 2 with an explicit `else` around its early exits; the
cursor's decrement arm stores before testing; the two message-box calls are one
call with the id chosen first; and `func_8002CCA8` takes the card id plus 1024.

Four more were needed to reach the exact mix.

`D_801D07E0` is not a scalar. The target forms a base at 0x801D0000 with
`lui` plus `addiu` and reads `2016(v0)`, which is the aggregate addressing
form; a scalar gives `lui %hi` plus `%lo(sym)(reg)` and no `addiu`. That one
instruction was the whole difference between distance 1 and distance 0.

The starchip step chain is four independent `if` statements over a
pre-initialised `step`, not an `if`/`else if` chain. The chain form makes the
compiler test before dividing and jump to the join; the sequential form lets it
divide, then test, then fall through, which is what the target does. Worth six
of the opcode distance and four instructions.

The `func_8002CCA8` result selects the 229 message when it is *non-zero*, and
the cost comparison is `cost < pool` selecting 228, with 227 in the `else`.
Both polarities were measured against both chain forms: they are not
independent, and the pair that wins was not the best of either axis alone.

The state-1 widget is read straight from the global for its two accesses
rather than through a local, which is worth six positions.

The starchip step divides by 10, 20, 30 and 40, not by 10, 100, 1000 and
10000, so the step grows with the magnitude of the count and the counter
drains in roughly constant time. The divisors are read off the magic
constants: an unsigned divide compiles to a multiply-high and a shift, and
0xCCCCCCCD shifted 3, 4 and 5 is division by 10, 20 and 40 while 0x88888889
shifted 4 is division by 30. The differing-position count cannot see this
error, because the whole block is displaced by one instruction and every
position in it differs either way; the register-blind instruction multiset
can, and it improves by sixteen.

```c
#include "../../types.h"

typedef struct {
    u8 pad0[8];
    u16 f8;
    u8 pad10[23];
    u8 f33;
} Widget;

typedef struct {
    u8 pad0[96];
    s16 f96;
    u8 pad98[10];
    u8 f108;
} Cursor;

typedef struct {
    u32 lo;
    u32 hi;
} Pair;

extern u16 D_8016D424;
extern u32 D_800EB12C;
extern u16 D_8016D49C;
extern Widget *D_8016D418;
extern s32 D_8016D428;
extern Cursor *D_8016D420;
extern u8 D_8016D410[];
extern u32 D_8016D438;
extern u32 D_801A8000[];
extern Pair D_801D5608;
extern u32 D_801D0000[];
extern volatile u16 D_8009B394;
extern volatile u16 D_8009B398;
extern volatile u16 D_8009B3A4;
extern u32 D_8009B0F4;
extern u32 D_8009B134;
extern u8 D_8009B269;
extern u8 D_8009B26C;
extern s8 D_8009B34D;

extern void func_80039794(void);
extern void SD_SEPlayFull(s32);
extern void Password_SetDigitCursorTarget(Cursor *);
extern void Password_RefreshDigitDisplay(void);
extern void func_8003FF34(void);
extern void Fade_WaitOut(void);
extern s32 Password_LookupCardID(void);
extern void func_80029164(s32, s32);
extern void func_8016A02C(s32);
extern s32 func_8002CCA8(s32);
extern void Password_CreateMessageBox(s32, s32);
extern void func_8002CCE4(s32);
extern void func_80021894(s32);
extern void Password_RefreshStarchipDisplay(void);

void func_8016A37C(void)
{
    Cursor *cursor;
    Widget *widget;
    s32 index;
    s32 digit;
    s32 state;
    u32 count;
    u32 step;
    u16 flags;
    u16 flags4;
    u16 card;
    s32 msg;

    func_80039794();
    if ((D_8016D420->f108 & 0x40) != 0) {
        return;
    }
    if ((D_800EB12C & 0x2008) != 0x2000) {
        return;
    }
    state = D_8016D424 & 0x1F;
    switch (state) {
    case 0:
        if ((D_8009B3A4 & 0xA000) != 0) {
            if ((D_8009B3A4 & 0x2000) != 0) {
                index = D_8016D428 + 1;
                D_8016D428 = index;
                if (index >= 8) {
                    D_8016D428 = 7;
                    return;
                }
            } else {
                index = D_8016D428 - 1;
                D_8016D428 = index;
                if (index < 0) {
                    D_8016D428 = 0;
                    return;
                }
            }
            SD_SEPlayFull(47);
            cursor = D_8016D420;
            Password_SetDigitCursorTarget(cursor);
            cursor->f96 = 8;
            cursor->f108 |= 0x40;
            return;
        }
        if ((D_8009B394 & 0x5000) != 0) {
            digit = D_8016D410[D_8016D428];
            if ((D_8009B394 & 0x1000) != 0) {
                digit = digit + 1;
                if (digit >= 10) {
                    digit = 0;
                }
            } else {
                digit = digit - 1;
                if (digit < 0) {
                    digit = 9;
                }
            }
            SD_SEPlayFull(7);
            D_8016D410[D_8016D428] = digit;
            Password_RefreshDigitDisplay();
            return;
        }
        if ((D_8009B398 & 0x20) != 0) {
            SD_SEPlayFull(8);
            func_8003FF34();
            Fade_WaitOut();
            D_8009B26C = D_8009B269;
            return;
        }
        if ((D_8009B398 & 0x40) != 0) {
            card = Password_LookupCardID();
            D_8016D49C = card;
            if (card == 0) {
                SD_SEPlayFull(9);
                return;
            }
            D_8016D424 = 1;
            SD_SEPlayFull(48);
        }
        return;
    case 1:
        flags = D_8016D424;
        if ((flags & 0x8000) == 0) {
            D_8016D424 = flags | 0x8000;
            func_80029164(0, D_8016D49C);
            return;
        }
        if ((flags & 0x4000) == 0) {
            if (((D_8009B0F4 & 0x2000030) | D_8009B134) != 0) {
                return;
            }
            D_8016D424 = flags | 0x4000;
            func_8016A02C(D_8016D49C);
            return;
        }
        D_8016D418->f33 = D_8016D418->f33 + 8;
        if (D_8016D418->f33 == 0) {
            D_8016D418->f8 &= 0xFFFB;
            SD_SEPlayFull(12);
            D_8016D424 = 2;
        } else {
            return;
        }
        /* fallthrough */
    case 2:
        flags = D_8016D424;
        if ((flags & 0x8000) == 0) {
            D_8016D424 = flags | 0x8000;
            D_801D5608.lo = D_801A8000[D_8016D49C * 2];
            D_801D5608.hi = D_8016D49C;
            if (func_8002CCA8(D_8016D49C + 1024) != 0) {
                Password_CreateMessageBox(229, 128);
                return;
            }
            if (D_801A8000[D_8016D49C * 2] < D_801D0000[504]) {
                msg = 228;
            } else {
                msg = 227;
            }
            Password_CreateMessageBox(msg, 0);
            D_8016D424 |= 0x4000;
            return;
        }
        if ((flags & 0x4000) != 0) {
            if (D_8009B34D == 0) {
                D_8016D424 = flags & 0xBFFF;
                func_8002CCE4(D_8016D49C + 1024);
                func_80021894(D_8016D49C);
                D_8016D424 = 3;
                return;
            }
        }
        D_8016D424 = 4;
        return;
    case 3:
        flags = D_8016D424;
        if ((flags & 0x8000) == 0) {
            D_8016D424 = flags | 0x8000;
            D_8016D438 = D_801A8000[D_8016D49C * 2];
        }
        count = D_8016D438;
        step = 1;
        if (count >= 10) { step = count / 10; }
        if (count >= 100) { step = count / 20; }
        if (count >= 1000) { step = count / 30; }
        if (count >= 10000) { step = count / 40; }
        if (step == 0) {
            step = 1;
        }
        count = count - step;
        D_8016D438 = count;
        D_801D0000[504] = D_801D0000[504] - step;
        if (count == 0) {
            D_8016D424 = 4;
            Password_RefreshStarchipDisplay();
        }
        return;
    case 4:
        flags4 = D_8016D424;
        if ((flags4 & 0x8000) == 0) {
            D_8016D424 = flags4 | 0x8000;
            D_8016D418->f8 |= 4;
        }
        widget = D_8016D418;
        widget->f33 = widget->f33 + 8;
        if ((s8)D_8016D418->f33 < 0) {
            Password_CreateMessageBox(226, 0);
            D_8016D424 = 0;
        }
        return;
    }
}
```

