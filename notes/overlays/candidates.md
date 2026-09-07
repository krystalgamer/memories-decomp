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

At the time of writing the rows below are the unmatched functions whose claimed
state is not stored here, in the order worth recovering:

- `func_8016A37C`, claimed at 365 of 365 with five differing positions. The
  closest unstored state in the project.
- `func_80181728`, claimed within four instructions of the target, with the
  residual attributed to four callee-saved hoists.
- `FreeDuel_Init`, claimed at 446 of 468, with the variable-identity lever
  already probe-verified.
- `func_801821DC`, no claimed state.

## password `func_8016913C` at 0x8016913C

`gcc_2_8_1_g0_split`, 384 instructions against 382, opcode distance 2
with the unconditional jump count exact, and the first 91 instructions
agree.

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
#include "../../src/types.h"

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
    } else if ((D_8009B398[0] & 0x800) != 0) {
        D_8016D401 = 14;
        D_8016D402 = 8;
    } else {
        goto select;
    }
    func_8003FEE0(47);
    row = (s8)D_8016D402;
    col = (s8)D_8016D401;
    cell = D_8016AB38[row][col];
    if (cell < 0) {
        rp = D_8016AB38[row];
    again:
        col = col + cell;
        cell = rp[(s8)col];
        D_8016D401 = col;
        if (cell < 0) {
            goto again;
        }
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


select:
    if ((D_8009B394[0] & 0xC0) == 0) {
        if ((D_8009B394[0] & 0x20) != 0) {
            func_8003FEE0(NameEntry_AdjustLength(-1, 6) != 0 ? 12 : 9);
        }
        return;
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
    func_8003FEE0(NameEntry_AdjustLength(d, 6) != 0 ? 12 : 9);
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
}
```
## main_menu `func_80180390` at 0x80180390

`gcc_2_8_1_g0_split`, 495 instructions against 495, opcode distance 0
with every mnemonic count exact. The longest common subsequence of
mnemonics is 479 of 495, the first 192 instructions agree, and 232
positions differ.

The instruction mix is now exact. Every mnemonic appears the same number of
times as in the target, the instruction count is 495 against 495, and the
opcode distance is zero. What is left is register assignment and ordering.

Two changes took it from six, and the second is the interesting one.

The store of the new menu index has to be `volatile`. The target stores the
computed byte to `gMain_bMenuID` and then *reloads* it with `lbu` to index the
entry table, where this build kept the value in a register and masked it with
`0xff`, because the truncation to `u8` is known to the compiler. Qualifying the
store, and only the store, forces the reload: the surplus `andi` and the
missing `lbu` are the same instruction seen from two sides, and both go to zero
together. Qualifying the whole global instead is much worse, because it also
forces every other read of it in the function. Six spellings were measured,
including naming the value in a local before storing it, casting the assignment
to `u8`, naming the entry pointer, and reading the global back through a
volatile pointer at one or both uses; only the volatile store reaches it.

The base and count selection is two separate `if` statements on the same
condition, and the count comes first. The target evaluates `(u32)gMain_bMenuID
< 5` once and branches on the result twice, with `move s1,zero` in the first
delay slot and `li s0,6` in the second, so both assignments are made in slots
and no unconditional jump is needed. A single `if` with two assignments in each
arm cannot produce that: it gives one conditional branch and a `j` around the
else arm, which is the surplus `j` and the missing `beqz`.

Splitting into two `if` statements is worth two on its own. Splitting them and
putting the count first is worth all six. The order matters because the
assignment that lands in the first delay slot has to be the one whose register
is free at that point; with the base first, GCC fills the slots the other way
round and keeps a `bnez` where the target has a `beqz`. Writing the pair as two
conditional expressions reaches the same two as the unordered split, and
inverting the second test is worse.

All twenty-nine profiles were re-run at the new base and none matches; only
`gcc_2_8_1_cc_g0_as_g8_split` ties, as it has throughout.


The failure returns funnel through the single exit label, but only the first
eleven of them. That takes the distance to six, brings the `li` count to exactly
the target's, and moves the agreeing prefix from 181 to 192.

The function returns -1 from twenty places and -2 from one. The target
materialises the -1 once, in a shared exit block, and reaches the -2 return by
jumping one instruction past that block with the constant in the jump's delay
slot. This build materialised the constant at each return site, because the
register was free there, and paid two surplus `li` for it.

The number of sites to funnel matters and is not monotone, which is why a
single "use a shared exit" experiment would have missed it. Funnelling all
twenty is much worse, at thirteen, because GCC then merges too much and the `li`
count overshoots by four while two surplus `j` and two surplus `nop` appear.
Funnelling the first nine or ten leaves one surplus `li`. Eleven through fifteen
are all at six and identical. Sixteen brings a second surplus `j` back. So the
boundary sits between the tenth and eleventh site and again after the fifteenth,
and the whole span between them is flat; the twenty-one cells of the sweep over
how many to funnel were needed to see that.

Two axes were re-measured against the new base and are closed. The
thirty-two-cell product of `volatile` over the five remaining stores through
the menu entry pointer, which was measured one site at a time last cycle and is
measured as a product here, is byte-identical in every cell; adding the
qualifier to any subset of the frame countdown as a decrement, the countdown as
a negation, the clear of `+0x36`, and the set and the clear of bit `0x40` does
nothing. Repeating the sixteen-cell subset of it against the funnelled base is
byte-identical too, so the two levers do not interact.

What remains is one surplus `andi`, one surplus `j`, one missing `lbu`, two
surplus `lui` and one missing `beqz`.


Two stores through the menu entry pointer want the `volatile` qualifier, and
between them they take the distance from twelve to eight and bring the `nop`
count from three short to exact.

The mechanism is the same in both places and it is worth stating generally,
because it is the opposite of the delay-slot duplication this function already
exploits. GCC fills a branch delay slot by speculating an instruction from the
branch target when the target block begins with one that is safe to execute
either way. A plain store to memory is not safe that way, so a block that
begins with a store leaves the slot empty, which is what the target does. When
this build sank the store further down and left an `andi` at the head of the
block, the `andi` became the speculation candidate: it was copied into the slot
and kept where it was, so the mask appeared twice where the target has it once.
Qualifying the store stops it moving, the block starts with the store again,
and the slot goes back to a `nop`.

The store of the tween position to `+0x30` is the important one, worth four on
its own: one surplus `andi`, one `bnez` and the three missing `nop`. The store
of the accumulated value to `+0x36` in the pad-hold path is worth one more,
and it also wants its comparison spelled as the low side, `< 0xBB8` returning
-1 with -2 falling through, rather than the high side; that pairing is what
takes the `bnez` count to exact.

The qualifier is specific to those two. It was measured against five other
stores through the same pointer, the frame countdown at `+0x60` both as a
decrement and as a negation, the clear of `+0x36`, and both the set and the
clear of bit `0x40` at `+8`, and every one of the five is byte-identical. This
is not a general instruction to qualify stores; it is two places where the
scheduler moves a store the target does not move.

What remains is one surplus `andi`, one surplus `j`, one missing `lbu`, two
surplus `li` and two surplus `lui`. The two `li` are the return plumbing
already recorded: the target funnels its returns through a shared exit block
whose default is `li v0,-1` and jumps one instruction past it for the `-2`
return, materialising each constant in a delay slot, where this build
materialises them early because the register is free.


The distance is unchanged at twelve and the candidate is very much closer, so
the header now also quotes the prefix. Two changes did it, and neither works
without the other: measured on their own they cost three and eleven of the
distance respectively, and together they cost nothing while taking the agreeing
prefix from one instruction to 181, the mnemonic subsequence from 445 to 474
and the differing positions from 485 to 326.

The first is that the flag clear belongs in both arms of the poll dispatch, not
after them. Each of the three identical poll blocks ends

    if (value == 1) {
        Input_ResetPads();
        func_80180D2C(1);
    } else {
        Input_ResetPads();
    }
    D_80184598.f9B = 0;

and the target has that store twice per block, once in each arm's `j` delay
slot on the way to the exit. Writing it once and letting GCC share it produces
a third block holding the store, reached by a jump from the first arm and by
fall-through from the second, which is the surplus `j`. Writing it in both arms
gives the target's thirty-four `sb` exactly. It is worth three of the distance
on the store count and costs six elsewhere, because with the object addressed
as a struct GCC then shares an address computation the target re-materialises.

The second is the flag object split into six separate byte globals. On its own
that is much worse, at 23, because distinct symbols cannot share a `%hi` the
way one symbol at several offsets can, so the high half is re-materialised at
every access: eight surplus `lui` filling five load-delay slots the target
leaves as `nop`. This was measured and recorded as a closed axis two cycles
ago, with the struct confirmed.

That conclusion was correct about the struct and wrong about the axis. The two
faults are opposite in sign. Duplicating the store removes six `lui`; splitting
the object adds eight. Applied together the surplus is two, the three missing
`sb` and the two surplus `addiu` both disappear, and the front of the function
lines up for 181 instructions. Neither is visible while the other is held
fixed, which is exactly the failure mode of measuring one axis at a time.

The profile sweep on the new base has a trap worth recording.
`gcc_2_8_1_g0_no_sched2_split` reports distance 10, two better than the chosen
profile, with a prefix of two and 475 differing positions. It disables the
second scheduling pass, so nothing is scheduled; the target has filled delay
slots throughout and cannot have been built that way. The lower distance is the
multiset cancelling again, and it is not a candidate.

The remaining twelve are two surplus `andi`, two surplus `li`, two surplus
`lui`, one missing `lbu`, one surplus `j`, one missing `bnez` and three missing
`nop`. The two `li` are decoded: the target funnels its returns through a
shared exit block whose default is `li v0,-1`, and jumps one instruction past
it for the `-2` return, so the constant is materialised once; this build
materialises it at the return sites.


The sixth `slti` is a comparison the target computes twice and this build
computed once, and recovering it needs the copy to land in a local that has
other definitions.

The visibility test reads

    if ((u32)gMain_bMenuID < 5) {
        if (i >= 5) goto hide_entry;
    } else {
        if (i < 5) goto hide_entry;
    }

and both arms compare `i` against 5. GCC canonicalises `i >= 5` into the
negation of `i < 5`, so the two arms hold the same expression and it is
computed once and the result reused across the join. The target emits
`slti v0,s2,5` in both arms, off the same register, so the value is not
carried between them.

Assigning `i` to a local in the second arm and comparing the local defeats the
reuse, but only if that local is one the function already defines elsewhere.
A freshly declared local for the purpose is worth nothing at all: it has a
single definition and a single use, so it is coalesced away and the common
subexpression comes straight back. `count`, which the menu-wrap block below
assigns in both of its arms before any read, is not coalescable in the same
way, and with it the second `slti` appears with no `move` introduced to pay
for it. Worth one of the distance, and the assignment is provably dead
because every later read of `count` is dominated by one of that block's two
assignments.

The general form is worth stating: a copy inserted to break a common
subexpression has to be a copy the compiler cannot see through, and in this
compiler that means a variable with other live definitions. This also suggests
the original reused a small pool of scratch locals across unrelated purposes,
which is ordinary for the period and is why the reuse is the natural spelling
rather than a trick.

The addressing form was re-examined at the same time and the struct is
confirmed correct, which is worth recording because the disassembly argues the
opposite at first reading. The target holds only the high half of the address,
`lui s0,0x8018`, and folds the low half and the member offset into each
displacement as `17819(s0)`, `17820(s0)` and `17821(s0)`. This build
materialises the whole address once, `lui` then `addiu`, and uses `3(s0)` and
`4(s0)`, which costs the two surplus `addiu` and looks like the wrong shape.
Splitting the object into six separate globals, one per byte, does remove
those two `addiu`, and it raises the common subsequence from 444 to 464
because the front of the function then lines up. It is still much worse:
distinct symbols cannot share a `%hi` the way one symbol at several offsets
can, so GCC re-materialises the high half at every access and the distance
goes from 13 to 24, eight surplus `lui` filling five load-delay slots the
target leaves as `nop`. Declaring it as a `u8` or `s8` array indexed by
constants is byte-identical to the struct. All twenty-nine profiles were
re-run and only `gcc_2_8_1_cc_g0_as_g8_split` ties the current one; the
non-split profiles cost twenty-three instructions, so `-msplit-addresses` is
not the lever either.


`D_8009B398` is volatile. The target reads the two pad words at 0x8009B394
and 0x8009B398 more often than a non-volatile declaration allows: aligning
the `lhu` sequences shows four reads of those two addresses in the last
region where this build had three, and GCC had folded one away as a common
subexpression. Marking the second one volatile restores the read and also
removes a stall, so the missing `lhu` and the spare `nop` both go at once.

Only that one word wants it. `D_8009B394`, `D_8009B0D8`, `D_8009B3EA` and
`D_8009B3ED` are inert as volatile in all sixteen combinations, so this is a
fact about the one address rather than a blanket rule for the pad block. The
sibling `func_8016913C` already declares its three pad words volatile, which
is where the shape came from.

The cross-jump the previous entry decoded can be reproduced from the source,
and reproducing it properly is worth more than the conditional expression
that stood in for it.

That conditional expression fixed the call count but introduced a second
`sltiu` against 5. Listing every `slti` and `sltiu` on both sides shows the
target testing the menu id once in that region and this candidate testing it
twice, so the expression was buying the right call count at the cost of a
comparison the target does not make.

Routing the `SD_SEPlay(6, ...)` block's `return -1` through a shared label at
the end of the function is what actually lets GCC merge the two call sites,
because cross-jumping needs the two tails to reach the same place rather than
merely to look alike. With the two sounds written as separate branches again
and only that one `goto` added, the distance falls from 21 to 14 and the call
count stays at 29.

Which return goes through the label matters and is not symmetric: routing the
ninth sound's return through it as well gives 17, and routing only the ninth
gives 17 too. Only the sixth belongs there.

The structural defect is fixed. This candidate used to emit thirty calls
against the target's twenty-nine, which made every scheduling measurement on
it meaningless. Counting the target's calls by callee shows only four to
`SD_SEPlay` where the source had five, and reading the argument register at
each of the four gives 7, 6, 8 and 7 -- there is no call with 9 at all.

The target reaches the ninth sound by cross-jumping. At the menu-id test it
issues `sltiu v0,v0,5` then `bnez` into the middle of the `SD_SEPlay(6, ...)`
call site, with `li a0,9` in the branch delay slot, so one call site serves
both. The two blocks have identical tails -- a call and `return -1` -- which
is what makes them mergeable.

Writing the two sounds as one call with the id chosen by a conditional
expression reproduces the single call site and takes the distance from 21 to
15, with the call count now exactly 29. Putting the `>= 5` case first in an
`else` reaches 19, so the conditional-expression form is the better of the
two restructurings and both beat the original.
opcode distance 21.

The six consecutive bytes from 0x80184598 to 0x8018459D are one struct, not
six scalar globals. That is what lets the compiler hold the high half of the
address in a callee-saved register across the poll calls and fold the low half
plus the member offset into each access, which is the form the target uses and
the one the row had recorded as unreachable.

```c
#include "../../src/types.h"

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

s32 func_80180390(void)
{
    u8 *entry;
    u8 **slot;
    s32 step;
    s32 level;
    s32 value;
    s32 frame;
    s32 delta;
    s32 moved;
    s32 i;
    s32 base;
    s32 count;

    if (D_8018459B != 0) {
        value = SaveData_PollLoad();
        if (value != 0) {
            if (value == 1) {
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
        value = func_8003FCD8();
        if (value != 0) {
            if (value == 1) {
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
        value = func_8003FD14();
        if (value != 0) {
            if (value == 1) {
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
            entry = D_80184560;
            entry[0xE] = 0x80;
            entry[0xD] = 0x80;
            entry[0xC] = 0x80;
            *(u16 *)(entry + 8) |= 0x40;
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
            value = entry[0xE] + entry[0x60];
            entry[0xE] = value;
            entry[0xD] = value;
            entry[0xC] = value;
            entry = D_80184560;
            value = entry[0xC];
            if ((u32)(value - 0x41) >= 0x3F) {
                if ((s8)value < 0) {
                    entry[0x6C] = 0x3C;
                }
                entry = D_80184560;
                value = *(s16 *)(entry + 0x60);
                *(s16 *)(entry + 0x60) = -value;
            }
        }
        if ((D_8009B398 & 0x800) != 0) {
            SD_SEPlay(7, 0xFF, 0);
            entry = D_80184560;
            *(u16 *)(entry + 8) &= 0xFFBF;
            func_80180D2C(0);
            D_80184598 = 1;
            goto ret_m1;
        }
        entry = D_80184560;
        value = *(u16 *)(entry + 0x36) + D_8009B0D8;
        *(volatile s16 *)(entry + 0x36) = value;
        if ((s16)value < 0xBB8) {
            goto ret_m1;
        }
        return -2;
    }

    if (D_80184599 != 0) {
        moved = 0;
        slot = gMain_apMenuEntries;
        i = 0;
    entry_loop:
        entry = *slot;
        if (entry == 0) {
            goto next_entry;
        }
        if (*(s16 *)(entry + 0x60) <= 0) {
            goto next_entry;
        }
        *(s16 *)(entry + 0x60) = *(u16 *)(entry + 0x60) - 1;
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
        entry = *slot;
        delta = *(s16 *)(entry + 0x38) - *(s16 *)(entry + 0x36);
        frame = 0x10 - *(s16 *)(entry + 0x60);
        value = *(u16 *)(entry + 0x38);
        if (frame != 0x10) {
            value = rsin(frame << 6) * delta / 0x1000;
            entry = *slot;
            value = *(u16 *)(entry + 0x36) + value;
        }
        *(volatile s16 *)(entry + 0x30) = value;
        if ((frame & 1) != 0) {
            func_80180E6C(*slot);
        }
        entry = *slot;
        *(u16 *)(entry + 8) = *(u16 *)(entry + 8) | 0x40;
        goto tick_entry;
    hide_entry:
        entry = *slot;
        *(u16 *)(entry + 8) = *(u16 *)(entry + 8) & 0xFFBF;
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
            goto ret_m1;
        }
        value = D_80184596;
        D_80184599 = 0;
        if (value == 0) {
            return -1;
        }
        if (D_80184595 != 0) {
            if ((u32)gMain_bMenuID < 5) {
                for (i = 0; i < 0xB; i++) {
                    entry = gMain_apMenuEntries[i];
                    if (entry != 0) {
                        *(u16 *)(entry + 8) &= 0xFFBF;
                    }
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
        if ((u32)gMain_bMenuID < 5) {
            count = 5;
        } else {
            count = 6;
        }
        if ((u32)gMain_bMenuID < 5) {
            base = 0;
        } else {
            base = 5;
        }
        func_80040410(gMain_apMenuEntries[gMain_bMenuID], (gMain_bMenuID << 1) | 1);
        if ((D_8009B394 & 0x1000) != 0) {
            value = gMain_bMenuID - base + count - 1;
        } else {
            value = gMain_bMenuID - base + count + 1;
        }
        *(volatile u8 *)&gMain_bMenuID = value % count + base;
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
#include "../../src/types.h"

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

## main_menu `func_80181728` at 0x80181728

`gcc_2_8_1_g0_split`, 352 instructions against 356, opcode distance 4.
The longest common subsequence of mnemonics is 273 of 356.

This function had no stored candidate. Its row recorded a best of 352 of 356
from an attempt that was never kept, so every later cycle had to take the
figure on trust and could not build on it. This entry restores it: rebuilt from
the disassembly against the row's own description, it reaches the same 352 and
the same opcode distance of 4 on the first compile, which is independent
confirmation that the recorded figure was real and that the description is
accurate enough to reconstruct from.

The shape the row describes is confirmed instruction by instruction. The marker
in `D_801845B0[2]` takes its x from the toggle in `D_801845BC[2]` compared
against zero with 116 first and 220 second, so that 220 lands in the branch
delay slot, and its y is 74. Each side reads its widget from `D_801845B0` at
index 0 or 1 and tests the short-circuit or of the busy word at `+0x24` being
non-zero with that side's toggle byte being 2. The true side sets bit `0x40` of
the halfword at `+8`, re-reads the pointer, and writes the bar position and the
y, 111 and 139, only when the busy word is zero; the false side clears the same
bit and copies the marker's coordinates. Both paths converge on a single store
of y, which GCC cross-jumps without being asked. The bar position is the
display value times 128 divided by 8000 plus 176.

The two primitives are ordinary locals. The bar is a `POLY_G4` at `sp+72` with
`len` 8 at offset 3 and `code` 0x38 at offset 7; the digit is a `POLY_GT4` at
`sp+16` with `len` 0xC, `code` 0x3C, `tpage` 11 and `clut` 0x3EA9. The digit x
comes from a walking offset that starts at -126 and gains 8 a pass, and the u
coordinate is the value modulo ten times eight, less 128 for the left edge and
120 for the right; one division supplies both the remainder and the next value.

The four are now separated rather than described as one pressure difference,
and one of them has a price attached.

The missing `lui` is the third `%hi` base. Three distinct symbols are read in
the opening block, the pair of display values at `D_801845C0`, the widget
pointers at `D_801845B0` and the toggle bytes at `D_801845BC`, and under
`-msplit-addresses` each gets its own high-half pseudo. The target materialises
all three in the prologue, at instructions 1, 3 and 5. This build materialises
two there and defers the third, because it has already spent a register
spilling the second display value: the spill is visible as `sw v0,112(sp)` at
candidate instruction 20, in the window where the target is still setting up
addresses.

Two of the missing `sw` are spills the target makes and this build does not.
The target stores the field width to `sp+112`, reloads it, shifts it by three
and stores the result to `sp+116`, then reloads that on every pass of the first
digit loop, repeating the pattern at `sp+120` for the second. This build keeps
the width in a callee-saved register and the shifted value in another, so it
never spills either.

Hoisting the shift into its own local before the loop, which is the shape the
target's spill implies, does fix the `lui` and one of the `sw`. It is still one
worse overall, because it buys them with two surplus `lw`: GCC reloads the
hoisted value where the target reloads a spill slot, and the counts do not
line up. That is the first time either of those two has been moved at all, so
the axis is not closed, but the price is now known.

Two independent changes reach exactly the same barrier, which says the four
remaining instructions are one obstacle rather than several.

Hoisting the field-width shift into a local, already recorded, gives no surplus
`lui`, one missing `sw` instead of two, two surplus `lw` and a second missing
`move`. Hoisting the loop constants into locals in the *first* digit loop only,
which is a different change in a different part of the function, gives the same
five numbers. Two or more constants reach it; one is not enough. The two
candidates are not byte-identical, so these are genuinely different codes that
land on the same counts, and that is stronger evidence of a single obstacle
than two routes producing the same code would be.

Hoisting the constants in both loops rather than one degrades steadily with how
many are hoisted, at six, seven, eight and nine for one through four, as the
`li` count falls further below the target's with each one. That is the opposite
of the pattern on `func_80180390`, where the count of sites to change had a flat
optimum in the middle; here the optimum is at the edge, and the sweep was worth
running for that reason alone.

Two more axes are inert. The loop written as a `while` with the counter
incremented at the end is byte-identical to the `for`. Qualifying the draw
context `D_800E9D90` volatile is byte-identical, in all six of its cells
against the loop form and the shift hoist. Writing the loop as a countdown from
the digit count is catastrophic at twenty-two, because the induction variable
then runs the wrong way and the walking offset has to be recomputed.

Two axes measured alongside it are inert. Three spellings of the digit
division, including taking the quotient into a temporary before computing the
remainder and computing the remainder by subtraction rather than by the modulus
operator, are byte-identical to each other and to the current form in all six
cells of their product with the hoist; GCC canonicalises all three to the same
single division. Hoisting the four per-loop constants, which the target does
into `a3`, `s5`, `s4` and `s3`, remains much worse and is worse still in
combination with the shift hoist, at eleven.

The remaining four instructions are one missing `lui`, one missing `move` and
two missing `sw`, and they are the register-pressure difference the row already
records rather than a shape error. The target's frame is 176 bytes against this
build's 160, it saves ten callee-saved registers against nine, and it keeps
both display values in `s7` and `s8` for the whole function. This build spills
the second one to `sp+112` in the prologue, which is visible at candidate
instruction 20 as `sw v0,112(sp)` where the target is still setting up address
registers.

The four constants the target hoists, 106, 114, 112 and 120, are exactly where
the row says: `li a3,106`, `li s5,114`, `li s4,112`, `li s3,120` immediately
after the guard on the first digit loop, and re-hoisted as 134, 142, 112 and
120 for the second. Writing them as locals assigned before each loop to force
that hoist is much worse, at 24, because GCC then keeps them live across the
call rather than rematerialising, and the frame grows further. Making the draw
context `D_800E9D90` volatile, to force the three-instruction address
rematerialisation the target performs inside both loops, is inert; the load is
already repeated each pass and the qualifier does not reach the address
computation.

```c
#include "../../src/types.h"

typedef struct {
    u8 t0, t1, t2, len;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 r1, g1, b1, p1;
    s16 x1, y1;
    u8 r2, g2, b2, p2;
    s16 x2, y2;
    u8 r3, g3, b3, p3;
    s16 x3, y3;
} POLY_G4;

typedef struct {
    u8 t0, t1, t2, len;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 u0, v0;
    u16 clut;
    u8 r1, g1, b1, p1;
    s16 x1, y1;
    u8 u1, v1;
    u16 tpage;
    u8 r2, g2, b2, p2;
    s16 x2, y2;
    u8 u2, v2;
    u16 pad2;
    u8 r3, g3, b3, p3;
    s16 x3, y3;
    u8 u3, v3;
    u16 pad3;
} POLY_GT4;

typedef struct {
    u8 pad0[8];
    u16 f8;
    u8 pad0A[36 - 10];
    s32 f24;
    u8 pad28[48 - 40];
    s16 f30;
    s16 f32;
} Widget;

extern Widget *D_801845B0[];
extern u8 D_801845BC[];
extern u16 D_801845C0[];
extern u32 *D_800E9D90[];

extern s32 MainMenu_CountDecimalDigits(s32);
extern void func_80084320(void *, u32 *, s32);

void func_80181728(void)
{
    POLY_GT4 digit;
    POLY_G4 bar;
    Widget *w;
    s32 first;
    s32 second;
    s32 x;
    s32 i;
    s32 count;
    s32 width;
    s32 walk;

    first = D_801845C0[1];
    second = D_801845C0[7];
    if (D_801845BC[2] == 0) {
        x = 116;
    } else {
        x = 220;
    }
    D_801845B0[2]->f30 = x;
    D_801845B0[2]->f32 = 74;

    w = D_801845B0[0];
    if (w->f24 != 0 || D_801845BC[0] == 2) {
        w->f8 |= 0x40;
        w = D_801845B0[0];
        if (w->f24 == 0) {
            w->f30 = first * 128 / 8000 + 176;
            D_801845B0[0]->f32 = 111;
        }
    } else {
        w->f8 &= 0xFFBF;
        D_801845B0[0]->f30 = D_801845B0[2]->f30;
        D_801845B0[0]->f32 = D_801845B0[2]->f32;
    }

    w = D_801845B0[1];
    if (w->f24 != 0 || D_801845BC[1] == 2) {
        w->f8 |= 0x40;
        w = D_801845B0[1];
        if (w->f24 == 0) {
            w->f30 = second * 128 / 8000 + 176;
            D_801845B0[1]->f32 = 139;
        }
    } else {
        w->f8 &= 0xFFBF;
        D_801845B0[1]->f30 = D_801845B0[2]->f30;
        D_801845B0[1]->f32 = D_801845B0[2]->f32;
    }

    bar.len = 8;
    bar.code = 56;
    bar.r0 = 64;
    bar.g0 = 32;
    bar.b0 = 32;
    bar.r1 = 255;
    bar.g1 = 32;
    bar.b1 = 32;
    bar.r2 = 64;
    bar.g2 = 32;
    bar.b2 = 32;
    bar.r3 = 255;
    bar.g3 = 32;
    bar.b3 = 32;
    bar.x0 = 176;
    bar.x2 = 176;
    bar.y0 = 107;
    bar.y1 = 107;
    bar.y2 = 115;
    bar.y3 = 115;
    bar.x1 = first * 128 / 8000 + 176;
    bar.x3 = bar.x1;
    func_80084320(&bar, D_800E9D90[2], 2048);

    bar.r0 = 32;
    bar.g0 = 32;
    bar.b0 = 64;
    bar.r1 = 32;
    bar.g1 = 32;
    bar.b1 = 255;
    bar.r2 = 32;
    bar.g2 = 32;
    bar.b2 = 64;
    bar.r3 = 32;
    bar.g3 = 32;
    bar.b3 = 255;
    bar.x0 = 176;
    bar.x2 = 176;
    bar.y0 = 135;
    bar.y1 = 135;
    bar.y2 = 143;
    bar.y3 = 143;
    bar.x1 = second * 128 / 8000 + 176;
    bar.x3 = bar.x1;
    func_80084320(&bar, D_800E9D90[2], 2048);

    digit.len = 12;
    digit.code = 60;
    digit.tpage = 11;
    digit.clut = 16041;
    digit.r0 = 255;
    digit.g0 = 255;
    digit.b0 = 255;
    digit.r1 = 255;
    digit.g1 = 255;
    digit.b1 = 255;
    digit.r2 = 255;
    digit.g2 = 255;
    digit.b2 = 255;
    digit.r3 = 255;
    digit.g3 = 255;
    digit.b3 = 255;

    width = MainMenu_CountDecimalDigits(8000);
    count = MainMenu_CountDecimalDigits(first);
    walk = -126;
    for (i = 0; i < count; i++) {
        digit.x0 = width * 8 - walk;
        digit.x2 = digit.x0;
        digit.x1 = digit.x0 + 8;
        digit.x3 = digit.x1;
        walk = walk + 8;
        digit.y0 = 106;
        digit.y1 = 106;
        digit.y2 = 114;
        digit.y3 = 114;
        digit.v0 = 112;
        digit.v1 = 112;
        digit.v2 = 120;
        digit.v3 = 120;
        digit.u0 = (first % 10) * 8 - 128;
        digit.u1 = (first % 10) * 8 - 120;
        digit.u2 = digit.u0;
        digit.u3 = digit.u1;
        func_80084320(&digit, D_800E9D90[2], 2048);
        first = first / 10;
    }

    count = MainMenu_CountDecimalDigits(second);
    walk = -126;
    for (i = 0; i < count; i++) {
        digit.x0 = width * 8 - walk;
        digit.x2 = digit.x0;
        digit.x1 = digit.x0 + 8;
        digit.x3 = digit.x1;
        walk = walk + 8;
        digit.y0 = 134;
        digit.y1 = 134;
        digit.y2 = 142;
        digit.y3 = 142;
        digit.v0 = 112;
        digit.v1 = 112;
        digit.v2 = 120;
        digit.v3 = 120;
        digit.u0 = (second % 10) * 8 - 128;
        digit.u1 = (second % 10) * 8 - 120;
        digit.u2 = digit.u0;
        digit.u3 = digit.u1;
        func_80084320(&digit, D_800E9D90[2], 2048);
        second = second / 10;
    }
}
```
