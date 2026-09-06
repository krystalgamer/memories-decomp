# Overlay Matching Patterns

Source-shape rules recovered while matching overlay functions with
`gcc_2_8_1_g0_split`. Each one is stated as the observable difference in the
target, followed by the source construct that reproduces it, and each is
backed by a function that now matches exactly.

These are about recovering what the original author wrote. They are not
tricks for defeating the optimiser: where the difference cannot be expressed
in ordinary C, the function stays as assembly.

To compare a candidate against the target cheaply enough to try several
shapes, use `tools/project/overlay_diff.py`, documented in
[`README.md`](README.md). Instruction count alone is a weak signal — several
of the rules below were only visible in a byte-level diff of candidates that
all had the right length.

## A store fills a load delay slot

If the target issues a global load and fills its delay slot with an unrelated
store, the source read that global into a local **before** the store.

GCC will not hoist a load of a global across a store through a pointer
parameter, because it cannot prove the two do not alias. So it only appears
above the store if it was already above it in the source.

```c
/* one instruction too long: the load lands after the store, with a nop */
*(s16 *)(a + 0x1A) = 0x63;
*(s16 *)(a + 0x18) = gPassword_nDigitIndex * 16 + 0xA3;

/* matches */
s32 value = gPassword_nDigitIndex;
*(s16 *)(a + 0x1A) = 0x63;
*(s16 *)(a + 0x18) = value * 16 + 0xA3;
```

Verified by `func_8016A00C` in the password module.

## A stall can point to access syntax

Where two accesses share a base pointer at different offsets, GCC can prove
independence and reorder them to fill a delay slot. If the target leaves the
stall in place, source statement order alone may not reproduce it.

`func_8016A02C` in the password module is the recorded example: the target
keeps a `nop` after `lhu $v1, 0x8($v0)` where GCC hoists the load above the
neighbouring `sb` when both fields are written as byte-pointer casts. The
function was later matched by using structure members, as described below.

## Index loops, not pointer walks

Write the loop over an index and let GCC strength-reduce it. Pre-optimising
into a pointer walk folds the array base and the starting offset into a
single `addiu` and comes out an instruction short of the target, which forms
the base and the offset separately.

```c
/* matches: counter set first, base and offset formed separately */
for (i = 15; i >= 0; i--) {
    if (gFreeDuel_apSparklePool[i] == 0) {
        return &gFreeDuel_apSparklePool[i];
    }
}
```

Verified by `FreeDuel_GetSparkleSlot` in the Free Duel module.

### Which one is the pointer, when the target has both

That rule is about a loop with **one** induction variable. When the target
keeps a counter *and* a walking pointer, the two spellings build the same
length and differ only in which register plays which role, so the instruction
count will not tell you and the register assignment has to:

```c
/* counter in a0, pointer in v1 -- and the counter init fills the
   delay slot of the branch that jumps into the loop */
for (index = 0; index < 0x28; index++) {
    if (D_801D0200[index] == 0) { ... }
}

/* pointer in a0, counter in v1 -- and the %hi of the base fills that
   delay slot instead, which is what the target had */
entry = D_801D0200;
for (index = 0; index < 0x28; index++) {
    if (*entry == 0) { ... }
    entry++;
}
```

The tell is in the preheader rather than the loop: an explicit pointer is
initialised before the counter, so its `%hi` is what is available early enough
to be hoisted into the preceding delay slot.

Verified by `FreeDuel_UpdateScreen` in the Free Duel module.

## A materialised base under a constant offset means a second use

A constant member offset normally folds into the relocation, so seeing the
base formed on its own and the offset left on the load is a positive signal:

```
lui   $v0, %hi(D_800EB15C)
addiu $a0, $v0, %lo(D_800EB15C)     # base on its own
lhu   $v0, 0x34($a0)                # offset left on the load
```

GCC only does this when the plain base is needed for something else, so look
down the block for another use of that register. Here `$a0` is still live at
the `jal` two instructions later, which says the base is that call's argument:

```c
/* builds 205 against 206 -- the offset folds and the base vanishes */
if ((*(u16 *)(D_800EB15C + 0x34) & 8) == 0) {
    TextBox_Destroy(D_800EB15C);
}

/* matches */
panel = D_800EB15C;
if ((*(u16 *)(panel + 0x34) & 8) == 0) {
    TextBox_Destroy(panel);
}
```

Naming the base in a local is not by itself enough -- with only one use GCC
propagates the local away and folds anyway. It needs the *second* use to
survive. Passing the bare symbol to the call is worse than either: GCC then
folds the load, materialises the folded address and recovers the argument as
`addiu $a0, $v1, -52`, which is one instruction longer and obviously wrong.

Verified by `FreeDuel_UpdateScreen` in the Free Duel module.

## A redundant register copy means a separate variable

A copy that looks pointless usually means the source used its own local for
an intermediate rather than assigning back in place. The extra variable also
changes what the scheduler puts in the delay slot.

```c
/* one instruction short: result goes straight back into the loop variable */
value = value / 10;

/* matches: quotient gets its own local, and the copy frees the delay slot */
next = value / 10;
value = next;
```

Verified by `func_80181EEC` in the main menu module.

The rule is about matching the original's locals, not about avoiding locals.
Read the target both ways:

- If the target does **not** materialise something in a register, do not give
  it a local. `func_80169D10` writes `D_800EB0F8` base-relative at absolute
  offsets, and adding a local for the element makes GCC fold the base and the
  offset into one `addiu`. `FreeDuel_Entry` is the same in miniature: hoisting
  a repeated expression into a local displaces an address load out of a branch
  delay slot.
- If the target **does** materialise one, the local is required.
  `func_801681E8` forms the `D_800F2848` base in its own register and stores
  at constant offsets from it; without a local for that base, GCC walks the
  address incrementally instead.

Both of those are the same question asked in opposite directions: does a
register in the target hold a value the source named?

### The same question about an address

Naming the base of an indexed access is the addressing form of this rule, and
it is worth checking separately because the symptom does not look like a
redundant copy at all. `func_80168FCC` indexes the 66-byte location table:

```c
/* seven instructions differ: the multiply is evaluated first, the address is
   materialised where it is used, and the sum lands in the base register */
record = gCampaignMap_aLocationTable + gCampaignMap_Location * 66;

/* matches */
table = gCampaignMap_aLocationTable;
record = table + gCampaignMap_Location * 66;
```

Naming the base is what splits `%hi` from `%lo`. As one expression the address
is a two-instruction constant emitted next to its use, after the index
scaling. As a named local it becomes a value live across the preceding branch,
so the `lui` can be hoisted — here the delay-slot filler pulled it six
instructions earlier, into the delay slot of an `if` that the merge block
follows, and only the `addiu` stayed behind.

Two consequences follow, and either can be the visible symptom:

- The `lui` and the `addiu` are **separated** in the target, often by
  unrelated instructions, and the `lui` may be duplicated on a path where a
  call clobbered it. One expression always emits them adjacent.
- The sum lands in the **index** register rather than the base register,
  which then cascades into the temporaries of every load that uses it.

The second is easy to misread as a register-allocation mystery. It is not:
it follows from which pseudo dies at the add.

## An offset inside the relocation means the symbol was indexed directly

When the two directions above are hard to call, the relocation itself
usually settles it. Reaching a member through the symbol lets GCC fold the
member offset into the relocation, and every other member is then reached
relative to that fold:

```c
/* does not match */
*(u16 *)(*(u8 **)(D_801845C0 + 4)) = *(u16 *)(D_801845C0 + 0);
```

```
lui   $3, %hi(D_801845C0+4)     # offset folded into the relocation
lw    $4, %lo(D_801845C0+4)($3)
addiu $2, $3, %lo(D_801845C0+4)
lhu   $3, -4($2)                # and the base is now reached backwards
```

A local pins the address so each member stays an ordinary load offset, which
is what the target does:

```c
u8 *state = D_801845C0;
*(u16 *)(*(u8 **)(state + 4)) = *(u16 *)state;
```

Two signals, either of which is enough:

- `%hi(sym+N)` in the build where the target has a plain `%hi(sym)`. This is
  not a relocation quirk to normalise away; it is the source having indexed
  the symbol.
- A **negative** load offset. A struct member should never need one, so it
  means the base register points at some other member.

Verified by `func_80181E30` in the main menu module.

## Signed and unsigned short tests

A zero test compiled as `sll $rX, $rX, 16` followed by a branch is a **signed**
short; `andi $rX, $rX, 0xffff` is an unsigned one. When the field is loaded
with `lhu` but tested with `sll`, the field is `u16` and the variable holding
it is `s16`.

Observed in `FreeDuel_UpdateSparkle`, where this closed the semantic gap
though that function does not yet match in full.

## A global reloaded for every test is volatile

The single most expensive thing to get wrong, because it is invisible in the
logic and shows up only as size. `CampaignMap_MoveCameraDpad` tests one pad
word twelve times:

```
lhu  v0,%lo(gInput_wPad1Held)(a1)
nop
andi v0,v0,0x40
beqz v0,...
...
lhu  v0,%lo(gInput_wPad1Held)(a1)     <- reloaded, two instructions later
nop
andi v0,v0,0x8000
```

Written as a plain `extern u16`, GCC folds all twelve into one load and the
function builds **95 instructions against 128** — a quarter of the body gone,
with every branch and every constant still correct. Adding `volatile` to the
declaration matches it exactly, with no other change.

Read the sign this way round: a load of the same global repeated in blocks
that a single load would dominate is not the scheduler failing to CSE. GCC
2.8.1 at `-O2` will hoist that without hesitation. The reload *is* the
qualifier, and no amount of statement reordering will reproduce it.

The stall is part of the tell. Each reload is followed by its own `nop`,
because a volatile load cannot be moved to cover its own delay slot either.

Two practical notes:

- Check the resident sources before deciding. `gInput_wPad1Held` was already
  `volatile u16` in four of them, so the overlay was the outlier, not the
  discovery. Grep for the symbol first — a match elsewhere in the repo is
  cheaper evidence than a rebuild.
- Getting it wrong is *not* symmetrical. A missing `volatile` loses
  instructions in bulk; a spurious one adds them. A build that is short by
  roughly the number of times a global is tested is this, almost always.

### First rule out the ordinary explanation

A reload is only evidence of `volatile` when **nothing is written between the
two reads**. The same symptom has a second, far more common cause: a store the
compiler cannot prove misses the location.

| between the two reads | what the reload means |
|---|---|
| nothing | `volatile`, and only the qualifier reproduces it |
| a store it cannot disambiguate | ordinary aliasing, already explained |

`func_80168AB4` in the password module is the second case. It divides twice by
the frame count at `+0x60` and reloads it for the second division — but the
store to `+0x36` sits between them, through the same `u8 *`, so the reload is
forced with no qualifier involved. Writing the two results into locals and
storing them afterwards removes the obstruction, GCC folds the two reads into
one, and the body comes out two instructions **short**.

Which is the point: in that second case reaching for `volatile` would add
instructions, the failure direction this rule warns about just above. Check
what is between the reads before reaching for the qualifier.

See the obstruction rule below for the general habit this is an instance of.

## You arrange the obstruction, not the order

When a build comes out **short**, the compiler has merged something the target
keeps separate. You cannot fix that by trying to place instructions — there is
no source form that says "emit this load again". What you can place is the
thing that *prevents the merge*, and then the compiler produces the target's
shape on its own.

Two independent instances, in different modules and different optimisations:

| function | what merged | the obstruction |
|---|---|---|
| `func_80168AB4` (password) | two loads of one field, folded by CSE | a store between them the compiler cannot disambiguate |
| `func_80183514` (main menu) | two copies of one value, coalesced by the register allocator | an `if` between them, so the copies are not live over the same range |

In both, the short build is the *natural* way to write the code, and the
matching build looks slightly laboured:

```c
/* 136 of 138: one load, because nothing separates the two reads */
stepX = ((tx - x) << 8) / frames;
stepY = ((ty - y) << 8) / frames;
w->stepX = stepX; w->stepY = stepY;

/* 138 of 138: the first store obstructs the reuse of `frames` */
*(s16 *)(w + 0x36) = ((...) << 8) / *(s16 *)(w + 0x60);
*(s16 *)(w + 0x38) = ((...) << 8) / *(s16 *)(w + 0x60);
```

```c
/* 119 of 120: both copies of DEF live over the same range, so they coalesce */
hiA = defA;
loA = defA;
if (defA < atkA) hiA = atkA;
if (atkA < defA) loA = atkA;

/* 120 of 120: the first `if` splits their live ranges */
hiA = defA;
if (defA < atkA) hiA = atkA;
loA = defA;
if (atkA < defA) loA = atkA;
```

The practical consequence is a diagnosis order. A build that is **short** by a
small number of instructions is a merge, so look for what the target has
separating the two things and put it back. A build that is the **right length
but in the wrong order** is scheduling, and *that* is where statement order
and the load-delay rules apply. Reaching for reordering on a short build wastes
measurements, because no arrangement of the same instructions changes how many
there are.

The corollary is the one worth remembering: once the obstruction is in place,
**do not also try to arrange the result**. In `func_80168AB4` the target's two
stores sit together at the end, which reads like evidence for grouped stores in
the source — but grouping them is exactly what removes the obstruction. The
scheduler sank them there by itself.

## A flat comparison chain is if/else, and the last arm is the else

GCC 2.8.1 lowers even a small `switch` to a **balanced comparison tree**: it
tests the midpoint with `slt` and halves the range. So a target that compares
against its constants **in a flat sequence**, one `beq` after another, was not
a `switch` in the source no matter how switch-shaped the logic reads.

Given a flat chain, the block layout then says which arm was written last:

- Each test branching **forward** to a block that the chain also reaches by
  **fall-through** means that block is the source's final `else`.
- So if two values share an arm and that shared arm is the fall-through
  target, the source tested the **negated** condition:
  `if (x != a && x != b) { rest } else { shared }`, not
  `if (x == a || x == b) { shared } else { rest }`.

Writing it the natural way round emits the shared arm first and reaches it
with an extra jump, which is the whole difference.

Verified by `func_80184344` in the main menu module, where the type values
`0x14` and `0x17` share a palette. The same function also shows the narrower
tell that a value shifted with `srl` rather than `sra` was held in an
**unsigned** local.

**Where the arms are laid out says which form to write.** The two comparators
in the same module make the contrast cleanly:

- `func_80184254` sends every arm straight to the shared exit, with the value
  in the branch's delay slot and one arm falling through at the end. That is
  the plain `if / else if / else` chain, and it matches unchanged.
- `func_8018416C` instead lays two arms out as separate blocks *after* the
  tests. That needs every test negated, so each arm becomes an `else`. Written
  plainly it comes out three instructions short, because GCC cross-jumps the
  duplicated arm into an earlier one.

So read the block layout before choosing: arms that are delay-slot values on
branches to a common exit mean the plain chain; arms that are blocks of their
own at the bottom mean the negated one.

`func_801836F4` is a third instance and confirms the count exactly: written as
a flat six-way `else if` chain it builds three instructions short, and the
negated nesting that `func_80183A14` uses matches. It also gives the shortfall
a second, easier-to-spot symptom. The three instructions are not lost where
the chain is; they are lost at the **epilogue**:

```
  target                        flat chain
  j    <return>                 li   v1,-1
  li   v1,-1                    lw   ra,16(sp)
  li   v1,1                     move v0,v1
  move v0,v1                    jr   ra
  lw   ra,16(sp)                addiu sp,sp,24
  nop
  jr   ra
  addiu sp,sp,24
```

Merging the `-1` arm into the fall-through also removes the branch target in
front of `move v0,v1`, so the return copy stops starting a basic block and
`lw ra` is free to hoist into its place. That takes the `nop` with it. A
target epilogue that still stalls after `lw ra` is therefore evidence in its
own right: something above it is a branch target that the build has merged
away.

### Which sentinel is assigned first

When several keys share one `else` arm that sets them all to the same
sentinel, the order of those assignments decides the register pair, with no
change to the instruction count. It is worth knowing the direction, because
otherwise it is a coin flip you resolve by rebuilding.

`func_801836F4` and `func_80183884` are the same comparator body with the two
outer comparison levels swapped, so between them they answer it:

| function | compares first | `else` arm assigns first |
|---|---|---|
| `func_801836F4` | ATK | DEF |
| `func_80183884` | DEF | ATK |

**The sentinel written first is the key compared *second*.** The `if` arm is
the other way round in both: there the keys are assigned in the order their
fields sit in the packed word, and only the sentinel arm flips.

Getting it backwards in `func_801836F4` cost eight differing positions, all of
them a two-register permutation (`a3`/`t0` on one side, `v1`/`a0` on the
other) with the length already correct. That signature — a clean register
transposition with nothing else wrong — is the cue to look at assignment order
in a shared arm rather than at allocation.

## Two loop invariants: the second one emitted becomes the pointer

When a loop indexes a two-dimensional array with one invariant index and one
running index, as in `table[slot][i]`, GCC hoists two values into the loop
preheader: the address of `table`, and the row offset `slot * sizeof(row)`.
It then strength-reduces exactly one of them into the induction variable and
adds the other on every iteration, so the loop body keeps a visible
`addu $rDest, $rInvariant, $rInduction`.

Which one becomes the induction variable follows the order they are emitted:
the invariant emitted **second** is the one that increments. This is
observable without a full match, because the two setup values also land in
swapped registers.

```
sll   $t0, $v0, 3           # row offset emitted first  -> stays invariant
lui   $v0, %hi(table)
addiu $a0, $v0, %lo(table)  # table address emitted second -> increments
.Lloop:
addu  $v1, $t0, $a0
...
addiu $a0, $a0, 4
```

Writing the row offset as a statement **before** the loop does move it ahead
of the address, but it also lets GCC prove `table + offset` invariant, so the
two fold into a single pointer before the loop and the per-iteration `addu`
disappears. That is the same folding described under the pointer-walk rule
above.

Computing the offset **inside the loop body** resolves both. GCC then creates
the induction variable itself rather than seeing a variable the source
already placed, so the offset stays a separate integer, and its initialiser
is emitted in the loop preheader instead of ahead of it:

```c
for (i = 0; i < count; i++) {
    offset = i * stride + base_offset;  /* body: GCC creates the giv */
    entry = record + offset;
```

Measured on `func_80168050`, shared by both overworld overlays, where this
reproduces the target exactly. The same source with `offset` initialised
before the loop, or folded into `record + i * stride + base_offset`, fails in
the two ways described above.

The mirror case is when the **address** is the one that must increment, so the
loop body reads `addu $rDest, $rOffset, $rTable` with `addiu $rTable, $rTable,
stride` at the bottom. Letting GCC create that giv folds the row offset into
its initial value, because both halves are invariant. Walking an explicit
source pointer instead keeps them apart:

```c
offset = slot * row_bytes;
p = table;
while (i < count) {
    entry = ...p and offset...;
    p++;
    i++;
}
```

Here `p` varies, so `p + offset` is not invariant and survives into the body.
Measured on `func_801840F8` in the main menu module. Initialising the counter
**before** `offset` and `p` also matters: with the counter left to a `for`
initialiser it is emitted after them and the two loop registers come out
swapped.

## Which operand comes first in a two-register add

`addu $rD, $rA, $rB` and `addu $rD, $rB, $rA` are different words, so operand
order is part of the match. Two rules decide it.

**Integer addition keeps source order; pointer addition does not.** The C front
end rewrites `int + pointer` back to `pointer + int`, so a target that adds an
invariant offset *before* a pointer cannot be written as `offset + p`. Making
the addition integer-typed preserves the order:

```c
entry = (Type *)(offset + (s32)p);   /* addu $rD, $rOffset, $rP */
entry = (Type *)((u8 *)p + offset);  /* addu $rD, $rP, $rOffset */
```

**Accumulating into a variable pins it to the first operand.** `x += v` emits
`addu $rX, $rX, $rV`, because the destination is also the left operand. When
the target reads `addu $rX, $rV, $rX` the sum went into a **different**
variable, and the operands then follow source order:

```c
total = entry->count + amount;       /* addu $rTotal, $rCount, $rAmount */
```

This is the same evidence as the locals rule — a register holding a value the
source named — read through the operand fields rather than through an extra
copy.

Both measured on `func_801840F8` in the main menu module.

## Store order decides which value reuses a base register

When two values are derived from one base and each is stored, one of them
gets a fresh register and the other overwrites the base. Which is which is
**not** controlled by the order of the assignments — it follows the order the
results are **stored**.

`func_80183E8C` computes two x edges from one base:

```
target : addiu v1,v0,-12      addiu v0,v0,-4       (base in v0; -4 reuses it)
wrong  : addiu v0,v1,-4       addiu v1,v1,-12      (base in v1; -12 reuses it)
```

Measured on a reduced loop, varying only where the two results are stored:

| store order | value that takes a fresh register |
|---|---|
| `r,r,l,l` | `r` |
| `l,l,r,r` | `l` |
| `r,l,r,l` | `r` |
| `l,r,l,r` | `l` |

**The value stored first takes the fresh register**; the other is computed
last and inherits the base. Swapping the two assignments changes nothing, and
neither does deriving one edge from the other — GCC folds that away.

The catch is that you usually cannot simply reorder the stores, because their
order is itself fixed by the target. `func_80183E8C` needs the *left* edge to
take the fresh register while the *right* edge's stores are emitted first, and
moving both left stores ahead costs nine positions. The resolution is to
interleave — issue **one** store of the first value, then compute the second:

```c
left = base - 12;
sprite.x0 = left;        /* one store, before the other edge exists */
right = base - 4;
sprite.x1 = right;
sprite.x3 = right;
...
sprite.x2 = left;        /* the second left store stays where it was */
```

So the lever is the position of the *first* store of each value, not of all of
them. That is a much narrower thing to vary than "reorder the stores", and it
is why four recorded orderings had missed it.

Verified by `func_80183E8C` in the main menu module, which sat at three
differing positions until this.

## An address computed as a flat integer sum

When a target builds an address from several terms and no array-indexed form
reproduces the order, the source added the terms as plain integers with the
symbol cast into the sum:

```c
func(row * stride + (s32)table + element * 4, ...);
```

Two things follow, and they pull in opposite directions from what looks
natural.

**Source order sets emission order.** Any form that starts from the symbol —
`&table[element] + row * stride`, `((u8 *)table + element * 4) + row * stride`,
`&table[element + row * count]` — emits that group first, and no
reassociation moves the row term ahead of it. If the target computes the row
offset before the lookup that produces `element`, the row term is written
first.

**Do not parenthesise the grouping you see at runtime.** GCC re-associates the
sum, so the pair that ends up grouped in the emitted code is not the pair the
source bracketed. Writing `row * stride + ((s32)table + element * 4)` — the
grouping the target actually computes — is wrong; the flat left-associated sum
is what produces it.

Measured on `func_801844D8` in the main menu module across 24 variants that all
had the target's 32 instructions. Every symbol-first form differed at 21 or
more of the 32 positions, the parenthesised sum at 14, and only the flat sum in
emission order matched. A named local for the lookup also fails, at 18: it
moves the load ahead of the multiply chain.

## Emission order distinguishes hand-written from generated code

The preheader gives a reliable reading of which values the source named.
Ordinary assignments are emitted where the source puts them; loop-invariant
addresses and induction-variable initialisers are emitted in the preheader,
after them. So in

```
jal   <call>
...   record         # before the hoisted addresses -> written by hand
addu  $s4, $zero, $zero   # loop counter -> written by hand
lui   ...            # hoisted address
lui   ...            # hoisted address
addiu $s2, $zero, 0x12    # after them -> compiler-generated giv
```

`record` and the counter must be source variables assigned after the call,
and the offset must not be. Reading the order this way pinned every saved
register in `func_80168050` before the first full build.

The corollary is that a value emitted *before* a hoisted address cannot be
made to move after it by reordering statements, because no source position
maps there. When a setup value sits in the wrong place relative to the
hoisted addresses, the fix is to stop naming it, not to move it.

Measured on `func_801840F8` in `main_menu`, where `table[slot][i]` reproduces
the instruction count exactly, including the `361 << 3` expansion of the
`2888`-byte row stride, and on `func_80168050` in the overworld overlays,
where the body-computed offset produces a full match.

## Struct members, not casts through a byte pointer

Two ways of writing the same field access are not equivalent to the optimiser.
Reaching a field as `*(u16 *)((u8 *)p + 0x8)` lets GCC hoist that load above an
unrelated `sb` to a different offset, which fills the load-delay slot and
loses an instruction. Declaring the object and using `p->field` keeps the
order the source had.

```c
*(u16 *)((u8 *)obj + 0x8) |= 4;   /* load hoists above the earlier sb */
obj->flags |= 4;                  /* stays put, and the nop survives */
```

The register allocation moves with it: in the cast form the `%hi` of the
following global landed in `$v1`, in the member form it is `$a0` as the target
has.

Verified by `func_8016A02C` in the password module, where the cast form builds
20 instructions against 21. Statement order does not substitute for it —
moving the `|=` to the front still matches, and moving it does not rescue the
cast form. Reading the field into a local first also matches, so the lever is
the access, not the expression.

This one is worth knowing because both of its symptoms invite the wrong
diagnosis. A load hoisted above a store looks like scheduling, and a register
that will not move looks like allocation; both were recorded here as a sched1
problem needing a profile that did not exist. The stock `gcc_2_8_1_g0_split`
matches once the accesses are members.

### The same lever pushes the other way

Reading a wide field off a `u8 *` base can also make a load *sink* below a
store instead of hoisting above one, for the same reason: the byte-pointer
`MEM` carries no type, so GCC cannot rule out an alias with the global being
stored between the two loads.

`func_801688BC` snapshots five camera channels. The last two are 32-bit and
sit at `+0x1C` and `+0x24`; the target loads them back to back:

```
sw   t0,%lo(D_80169610)(v0)
lw   t0,28(a3)
lw   a0,36(a3)          <- fills the load delay of the first
sll  v0,t0,0x10
```

Off the `u8 *` base, the second load sinks past the intervening store and the
delay slot goes to a `lui` instead:

```c
/* three slots wrong: the second lw lands after the D_801695CC store */
pitch = *(s32 *)(camera + 0x1C);
dist  = *(s32 *)(camera + 0x24);

/* matches: a second base, typed to the field being read */
s32 *cameraLong = (s32 *)D_800F2848;
pitch = cameraLong[7];
dist  = cameraLong[9];
```

Both bases are the same symbol, so this costs nothing at runtime and adds no
instruction — it only tells GCC what is being read. Note the base has to be
the symbol itself: `(s32 *)(D_800F2848 + 0x10)` with indices `3` and `5`
needs its own `addiu` and comes out a word long.

The tell is a load that appears *later* than its source position, separated
from a sibling load by exactly one store. Do not read that as scheduling; ask
first what type the pointer had.

## A register copy before the last store may be the return value

A tail that reads

```
lhu  $v1, 0xE($v0)
move $v0, $s0
sh   $v1, 0x32($v0)
```

looks like two temporaries being swapped, and the store using a fresh copy of
the object pointer looks like an allocation quirk. It is neither. The `move`
is the **return value copy** — the function returns that pointer — and the
scheduler placed it in the load's delay slot, after which the store naturally
uses `$v0` because the two registers already hold the same value.

So when a value is copied into `$v0` shortly before the end and nothing else
explains it, try returning it before reaching for a profile.

Verified by `func_80168588`, shared by both overworld modules. Adding the
`return` and giving the location-table base a named local shared by both record
computations matches on the stock `gcc_2_8_1_g0_split`. The
`-fno-schedule-insns` that had been recorded as necessary for its
saved-register allocation makes it worse, at 58 instructions against 57.

The shared base matters on its own: naming it is what puts its `%hi` ahead of
the location load, which is the emission-order rule again — the term the source
named first is emitted first.

## Known unresolved residual

`FreeDuel_PlaceCursor` is the only overlay function left in this state, and it
is close: a candidate already emits the same instructions in the same count as
the target under `gcc_2_8_1_g0_split`, with **only the two leading `lui`
instructions transposed**. Reading `D_8009B366` and `D_8009B367` into locals
before the coordinate stores is what fixed the load placement, since GCC will
not hoist a global load across a store through a `u8` pointer. Declaration
order does not affect what is left.

That ambiguity about the scheduling flags is now closed. All four are much
worse against a candidate that is otherwise five positions from the target, so
this is a source question and not a profile one:

| Profile | Differing positions |
|---|---:|
| `gcc_2_8_1_g0_split` | **5** |
| `gcc_2_8_1_g0_split_no_sched1` | 57 |
| `gcc_2_8_1_g0_no_sched2_split` | 54 |
| `gcc_2_8_1_g0_no_sched2` | 66 |
| `gcc_2_8_1_g0_no_sched1` | 72 |

Three further `lui` transpositions of the same kind were reachable from the
source, which is what leaves only the leading pair. Each was fixed by naming
the symbol in a local **before** the arithmetic that uses it, so the `%hi`
is emitted ahead of it rather than after:

```c
base = D_801D0000;              /* not &D_801D0000[index * 4] in one go */
row = &base[index * 4];

pFlag = &D_8009B32E;            /* not a bare store to the global */
stored = index - 31960;
*pFlag = stored;
```

The remaining pair resists this, because the symbol that must come first,
`D_8009B366`, is already read into a local as the first statement of the
function, while the one that must come second, `D_800EB0F8`, is held in a
callee-saved register across two calls and so is set up early regardless.

**Naming a base merges `%hi` materialisations, so do not reach for it when the
target keeps two.** The obvious next move on the remaining pair is to name the
coordinate base as well, `grid = &D_8009B366` then `grid[0]` and `grid[1]`.
That builds one instruction **short**, because it gives both coordinate reads a
single `%hi`. The target deliberately materialises `%hi` for that page twice:
once transiently for `D_8009B366`, and once into a callee-saved register for
`D_8009B367`, which survives the call and serves a later read of the same
symbol for the grid index. Two reads of neighbouring symbols through separate
`lui`s are evidence that the source names the two globals separately, and the
lever above must be withheld there. It is a lever for a base that is genuinely
used as a base, not for two adjacent scalars.

### What the previous occupant of this section taught

`FreeDuel_UpdateSparkle` sat here with a transposition of two independent
chains and was not unresolvable. The note recorded that swapping the two source
statements produced byte-identical output, and concluded that the scheduler
chose the order rather than the source.

The inference does not follow. Swapping two **adjacent** statements changes no
dependence, so of course it changes nothing. Moving one of them past the three
intervening stores does, and matches exactly:

```c
level = obj->r - 4;
obj->b = level;                  /* the three stores */
obj->g = level;
obj->r = level;
timer = obj->timer - 1;          /* moved down past them */
obj->timer = timer;
if (timer == 0) {
```

So before concluding that a transposition is the scheduler's choice, move a
statement far enough to change what depends on what. Adjacent swaps are not
evidence.

Profiles measured against `FreeDuel_UpdateSparkle` while it was still open.
None of them mattered to its match, but the last row is worth keeping:

| Profile | Result |
|---|---|
| `gcc_2_8_1_g0_split` | correct size, single transposition |
| `gcc_2_8_1_g0` | further |
| `gcc_2_8_1_g0_no_split` | further |
| `gcc_2_8_1_g0_no_sched2` | further |
| `gcc_2_7_2_g0` | furthest, `0xC4` instead of `0xC8` |

The 2.7.2 result is positive evidence that these modules belong to the GCC
2.8.1 cohort, reached through the recorded escalation path rather than by
assumption.

## Statement splitting controls evaluation order

Where the target evaluates the left side of a binary operator before the
right, the source used two statements. C leaves the order unspecified and GCC
will otherwise load the operand first.

```c
/* loads the digit first, and fills the branch delay slot with the address */
packed = (packed << 4) | gPassword_abDigits[i];

/* matches: shift first, address formed inside the loop, delay slot empty */
packed <<= 4;
packed |= gPassword_abDigits[i];
```

## A rotated loop means the source did not use while

`while (cond) { ... }` lets GCC peel the first test and rotate the loop, which
moves the condition to the bottom and changes which branch is inverted. A
target that tests at the top and returns to it with an unconditional `j` was
written as an endless loop with explicit exits.

```c
/* matches: test at top, j back-edge, increment in its delay slot */
for (;;) {
    if (entry[0] == -1) {
        return 0;
    }
    if (packed == entry[1]) {
        return index;
    }
    entry += 2;
    index++;
}
```

Both verified by `func_8016A304` in the password module.

`func_8016868C` in the same module confirms it, and shows how far the damage
travels. Its recorded blocker was about induction variables, not loops: GCC
was said to refuse a negative load offset and to split one walker into two,
giving three induction variables where the target has two. That was entirely
downstream of the rotation. Peeling the first test gives GCC a different
address to build the combined giv from — the peeled load's, at `+0x11`, rather
than the target's `+0xE` — and the offsets that follow are the consequence, not
the cause. Writing the loop as `for (;;)` with an explicit `return` fixed the
giv base and the offsets at the same time and matched on the first try.

Prefer suspecting the loop shape over the addressing when a target reads one
walker at a **negative** offset and a candidate reads two at non-negative ones.

The exit arm also has to `return` rather than `break`:

```c
if (!(node->flags & 0x80)) {
    return 0;         /* fills the exiting branch's delay slot with the value */
}
```

Breaking to a shared `return 0` after the loop assembles to the same 31
instructions but leaves that delay slot empty and emits the zero after `jr
$ra`, which is a two-position miss with everything else identical.

## Callee-saved register numbers follow death order, not declaration order

When several parameters are live across the same calls, GCC 2.8.1 does not hand
out `s2`, `s3`, `s4` in declaration order. `global.c` ranks allocnos by roughly
`n_refs / live_length` and assigns the highest-ranked one the first free
callee-saved register, so the parameter whose **last use comes earliest** gets
the lowest-numbered register.

A candidate that puts the parameters in `s2/s3/s4` where the target has
`s3/s4/s2` is therefore not a register-allocation mystery: the target simply
finishes with the third parameter before the other two. Move that parameter's
uses to the top of the trailing block and the numbering follows.

```c
/* target saves s3, s4, then s2, i.e. a0 -> s3, a1 -> s4, a2 -> s2 */
D_801845D8 = toggle;              /* third parameter dies first  -> s2 */
D_801845BC[2] = (*toggle == 0);
*(u16 **)(state + 4) = first;     /* second to die               -> s3 */
*(u16 **)(state + 0x10) = second; /* last to die                 -> s4 */
```

This is worth checking before blaming the scheduler, because a wrong s-register
assignment reorders the whole tail and looks like many independent misses.

Verified by `func_80180FD8` in the main menu module.

## A chained assignment loads once and stores right to left

`a = b = *p;` evaluates `*p` a single time and assigns `b` before `a`. Written
as two statements the first store kills the load, because a `u16` store through
one pointer may alias a `u16` load through another, and GCC reloads:

```c
/* two loads, ascending stores -- two instructions too many */
*(u16 *)state = *first;
*(u16 *)(state + 2) = *first;

/* one load, +2 stored before +0 -- matches */
*(u16 *)state = *(u16 *)(state + 2) = *first;
```

So the tell is a single load feeding two stores **with the higher offset stored
first**. Reaching for a temporary local gives the single load but leaves the
store order free, and then it has to be argued separately; the chain gives both
at once and is what the original almost certainly said.

The same shape appears on plain constants: `D_801845BC[0] = D_801845BC[1] = 2;`
emits `[1]` before `[0]`.

Verified by `func_80180FD8` in the main menu module.

## Passing the global rather than the local sinks the first argument

After `global = local;`, passing `local` and passing `global` to the following
call produce the same code except for **where the `a0` load is emitted**:

```c
D_801845B0[0] = object;
if (object != 0) {
    /* move a0,v0 emitted first, ahead of a1/a2/a3 */
    func_800404CC(object, 0, 0, 3, 4, 0, 0xB, 0x20C);

    /* move a0,v0 emitted last, after a1/a2/a3 */
    func_800404CC(D_801845B0[0], 0, 0, 3, 4, 0, 0xB, 0x20C);
}
```

The value is identical either way -- CSE folds the reload back to the same
register -- so this is invisible except as a four-position permutation of the
argument setup, and it is not reachable by reordering anything. Read the
argument spelling off the target rather than assuming the local.

The two forms can legitimately coexist in one function: in `func_80180FD8` the
two objects held in scalar globals pass the local and keep `a0` first, while
the three held in an array pass `D_801845B0[i]` and sink it. That is consistent
with the array elements also being re-read for the field update that follows,
where a scalar global is read once and an array element twice.

Verified by `func_80180FD8` in the main menu module.

## A value moved through a scratch register is a conditional expression

An `if`/`else` whose arms both assign the same variable lets each arm write
that variable's register directly. A conditional expression evaluates both arms
into **one** pseudo and then copies it, so the copy is visible:

```c
/* two instructions short: each arm assigns s0 itself */
if (value - 0x1F4 > 0) { value = value - 0x1F4; } else { value = 1; }

/* matches: both arms land in v0, then one move */
value = (value - 0x1F4 > 0) ? (value - 0x1F4) : 1;
```

```
bgtz  $v0, .L        # v0 already holds value - 0x1F4
 move $s0, $v0       # then-arm, in the delay slot
li    $v0, 1         # else-arm builds the constant in the *scratch* register
j     .L
 move $s0, $v0       # and copies it, rather than li $s0, 1
```

`li $v0, 1` followed by `move $s0, $v0` where `li $s0, 1` would have done is
the whole tell. The same shape appears with a non-constant arm as
`li $v1, 8000` / `move $s0, $v1`.

It reads per arm, so a chain can mix the two forms, and that is worth using
rather than smoothing over:

```c
if (down) {
    value = (value - 0x1F4 > 0) ? (value - 0x1F4) : 1;   /* through v0  */
} else if (value < 2) {
    value = 0x1F4;                                       /* direct: li s0,500 */
} else {
    value = (value + 0x1F4 < 0x1F41) ? (value + 0x1F4) : 0x1F40;  /* through v1 */
}
```

The middle arm assigning `$s0` directly is what says only the outer two share a
value, so it should stay a statement.

Verified by `func_801812B4` in the main menu module.

## A mask before every use means the local is narrow

`andi $reg, $reg, 0xffff` in front of each arithmetic use of a local, where the
local was loaded once with `lhu`, means the local is declared `u16` rather than
`s32`. GCC keeps the wide value in the register and masks at the use sites
instead of truncating at each assignment, so the masks appear scattered rather
than next to the stores.

The second, independent tell is the comparison: `sltiu` against a small
constant where an `s32` local would give `slti`. GCC knows a `u16` is
non-negative and picks the unsigned form.

Declaring the same local `s32` drops both the masks and the `sltiu`, which cost
six instructions across two copies of the block in `func_801812B4`.

Note this only holds for a local. The masks are about the declared width of the
variable, not about how the value was loaded — the `lhu` that produced it
already zero-extended, so the masks would be redundant if GCC were tracking the
value rather than the type.

Verified by `func_801812B4` in the main menu module.

## Declare the table, do not hand-write the address

A two-dimensional table should be declared `u16 sym[][N]` and the row
arithmetic left to GCC. Hand-writing the flat sum reaches the same instructions
but **fixes their order**, and that is usually what blocks the last few
positions:

```c
/* generated: GCC forms slot*3, *4 - slot, doubled; reuses the row offset for
   both the element and the count; hoists the base itself */
while (i < D_80185C9C[slot][0]) {
    func_801840F8(slot, D_80185C9C[slot][i + 1], amount);
    i++;
}

/* hand-written: same fifty instructions, order pinned, four positions off */
offset = slot * 22;
index  = i * 2;
value  = *(u16 *)(index + offset + (s32)D_80185C9C);
```

The generated form also gets the loop-carried partial product for free: the
target recomputes `slot * 22` from a `slot * 3` that survives in a register
across the call, which is not something you would think to write.

## A measured negative is only a negative against the shape it was measured on

`func_80184030` carried a recorded result that incrementing the counter after
the call, with `i + 1` in the index, "was measured and not helping". That was
true, and it was still the missing lever.

It did not help against the hand-written address because the row chain is
pinned there, so moving the increment cannot change which instruction leads the
loop body. Against the generated address it decides exactly that: the row
offset no longer depends on the increment, so it becomes the first instruction
of the body — which is what the target rotates into the back-edge delay slot,
where the earlier builds rotated the increment.

So a negative is a fact about a **pair**: lever *and* surrounding shape. When
the shape changes, the negatives recorded against the old one are not evidence
any more, and the cheap move is to retry them rather than to trust the list.
Both of these levers had been measured separately and neither worked; together
they matched on the first build.

Verified by `func_80184030` in the main menu module, using the declaration
proved by `func_80183B2C` in the same module.

### Declare the row shape, not just the row

The rule above is about *whether* to hand-write the address. There is a second
question once you decide to index: **what shape you declare the element as.**

`func_80181F68` walks three per-slot tables. Indexing them as flat arrays and
letting GCC strength-reduce had been measured and rejected, because it produced
the right hoisting split but built fifteen instructions over — GCC made one
induction variable per *field* touched:

```c
/* two givs for this table, and the same again for the next */
D_801845EC[i * 2]     = (s32)object;
D_801845EC[i * 2 + 1] = 0;
```

Declaring the element gives one giv per *table*, which is what the target has:

```c
typedef struct { u8 *object; s32 unk4; } Slot;
extern Slot D_801845EC[];
D_801845EC[i].object = object;
D_801845EC[i].unk4   = 0;
```

The count matters beyond the instruction total, because each table that becomes
a giv stops being an invariant address and so stops competing for the
four-invariant hoisting budget. Getting the element shape right is therefore
what decides which *other* symbols end up hoisted.

Verified by `func_80181F68` in the main menu module.

### Declaring the shape usually subsumes the address tricks

When the row shape is right, levers that had to be discovered separately stop
being levers and fall out of the declaration. `func_8018338C` carried three
recorded findings — build the count base in two statements or GCC folds the
`+80` into the symbol's `%lo`; index the comparator table from the base symbol
rather than naming the one four bytes in; and a register-numbering shift
through a six-word copy. Declaring

```c
typedef struct { s16 id; u16 count; } Card;
extern Card  D_801845FC[][722];      /* one giv for the row, not two */
typedef struct { s32 entries[6]; } Comparators;
Comparators c = *(Comparators *)&D_80180000[1];   /* the block move */
```

reproduced all three at once and took the function from 33 differing positions
to 2.

The general point: reach for the **declaration** before reaching for a way to
write the arithmetic. An address trick that compensates for a wrong declaration
tends to fix one position and pin several others, which is why those notes
accumulate.

The two that remained were both already-recorded rules rather than anything new
— the row address and the count base are separate locals because the target
keeps both in registers, and the sort mode is read once into a local.

Verified by `func_8018338C` in the main menu module.

## Per-occurrence reloads mean the local is volatile

The strongest tell that a local is `volatile` is that the compiler reloads it
from its stack slot once for **every occurrence in the source**, rather than
once per statement or once per basic block. Three uses of the same pointer in
one chained assignment become three `lw` from the same offset into three
different registers, with no store in between that could have invalidated a
cached copy.

`func_80183B2C` in the main menu module keeps two object pointers this way:

```
lw   a1,16(sp)
lw   a0,16(sp)
lw   v1,16(sp)
li   v0,64
sb   v0,14(v1)
sb   v0,13(a0)
sb   v0,12(a1)
```

Nothing between the three loads can alias, so ordinary common subexpression
elimination would have collapsed them. That qualification is the whole rule:
**a reload only means volatile when no store separates it.** A store through any
pointer may alias what the load reads, so a reload after one is forced rather
than chosen and says nothing about the source. `func_80180390` has three
identical loads of a global in one call-free block and needs no qualifier at
all, because each one follows a store. Use
`tools/project/overlay_scan_reloads.py` to apply the test rather than reading it
off by eye; scanned across all five modules, `func_80183B2C` is the only
function that shows the signature.

Declaring the pair as

```c
u8 *volatile o[2];
```

reproduces it. Plain locals keep both pointers in callee-saved registers and
build a smaller frame; a non-volatile aggregate local puts them on the stack
but still loads each one once. Only the qualifier gives one load per
occurrence.

### The qualifier also explains unfilled load delay slots

The instruction scheduler may not move anything across a volatile access, so
every block that touches the volatile object keeps its load-use hazards as
`nop`s while the rest of the function is scheduled normally. A function whose
first half is full of unfilled load delay slots and whose loops are tightly
scheduled is not evidence of a lower optimisation level. It is evidence of a
volatile object in the first half. Measure before concluding: `func_80183B2C`
looked like `-O1` on both counts at once — no elimination and no scheduling —
and building it at `-O1` did get closer, 201 of 216 against 162, which made the
wrong explanation look right. The volatile qualifier at the ordinary `-O2`
profile reached 216 of 216 with the whole first half exact, because it is the
one cause that produces both effects.

## A compound assignment evaluates the destination address once

`*(s16 *)(p + 0x60) *= -1;` and `*(s16 *)(p + 0x60) = -*(s16 *)(p + 0x60);`
compute the same value, but the first mentions `p` once and the second mentions
it twice. With a volatile `p` that is the difference between one reload and
two, and it also changes the load width: the compound form loads `lh`, while
the written-out form loads `lhu` because only the low half of the negation is
stored back.

Use the compound form when the target evaluates the address once. This is the
same rule as the chained assignment above, seen from the other side: the
number of times a name appears in the source is the number of times a volatile
or otherwise un-eliminable object is read.

## Two differently signed reads of one byte are two source expressions

A range test that emits `lbu` with `sltiu` and then `lb` with `bgez` is reading
the same byte twice with two different signednesses, not once with one
comparison. In `func_80183B2C` the colour channel leaves the range `0x41` to
`0x7F`, written as

```c
if (obj[0][0xC] < 0x41 || (s8)obj[0][0xC] < 0) {
```

through a `u8 *`. The unsigned compare catches the low end and the signed test
catches the high end.

**The high test is free; the low one is not.** Measured by perturbing the
matched source: writing the high end as an unsigned `obj[0][0xC] > 0x7F`
instead of the signed test is **byte-identical**, so the two spellings are the
same to the compiler and the earlier claim here that a two-sided comparison
changes the shape was wrong. What does break it is making the *low* test
signed, `(s8)obj[0][0xC] < 0x41`, which costs two positions, or collapsing both
ends into one unsigned subtract and compare, `(u8)(obj[0][0xC] - 0x41) > 0x3E`,
which builds 211 instructions against 216.

So the content of the rule is narrower than it first looked: the low test must
be unsigned and the two ends must stay two tests. How the high end is spelled
does not matter.

## Inline the call argument when locals lose the register contest

When the instruction counts already agree and the only difference is which
callee-saved register holds which value, check whether the target assigns
arguments through named locals at all. Coordinates computed into `x` and `y`
and then passed to two calls give the same instructions as the expressions
written inline in both argument lists, but not the same registers: the named
locals create their pseudos earlier and win the allocation priority contest
against the loop's own induction and address values.

In `func_80183B2C` the loop bodies were 216 of 216 with 52 differing positions,
every one of them the same instruction with a different register. The target
puts `s0` on the x coordinate and `s2` on the element address; named locals put
`s3` on x and `s0` on the element address. Writing

```c
func_80183E8C((i % 5) * 28 + 0x10, (i / 5) * 17 + 0xBC, D_80185C9C[0][i + 1]);
func_80184344((i % 5) * 28 + 0x10, (i / 5) * 17 + 0xBC, D_80185C9C[0][i + 1]);
```

instead of assigning `x` and `y` first matched. Common subexpression
elimination still computes each coordinate once and keeps it across both calls,
so this costs nothing; it only moves the pseudos later.

Declaration order was measured on the same function and has no effect at all —
three permutations of `i`, `x` and `y` all gave the identical 52 positions. The
lever is whether the value is a named local, not where it is declared.

Verified by `func_80183B2C` in the main menu module.

## A subtraction's destination follows the variable, not the statement

An earlier version of this section claimed that `subu $rD, $rA, $rB` with `$rD`
holding `B` means the result was **assigned back** in a separate statement:

```c
d = *(s16 *)(widget + 0x30);   /* claimed to be required */
d = target - d;
```

Measured by perturbing the matched source, that is wrong. Collapsing the two
statements into `d = target - *(s16 *)(widget + 0x30);` is **byte-identical**.
The intermediate assignment buys nothing.

What does matter is *which variable* receives the result. Giving the difference
a fresh local instead of reusing `d` costs two positions, and that is the whole
of the effect. So this is not a separate rule about subtraction at all — it is
the live-range rule below, seen from one side. Read that one instead.

### One variable can have to carry three values in a row

Fixing the register alone was not enough there. With the coordinate read as its
own statement, the scheduler hoisted that load into the load delay slot left by
the grid coordinate's `lb`, which removed a `nop` the target keeps and built one
instruction short.

The target keeps the slot empty because the register is still busy: it holds the
grid coordinate, then the widget's current coordinate, then the difference, all
in the same register across the whole block. Reusing one variable for all three
reproduces that:

```c
d = D_8009B36C;                /* the grid coordinate  */
tx = d * 56 + 20;
d = *(s16 *)(widget + 0x30);   /* the current position */
d = tx - d;                    /* the difference       */
sx = (d << 8) / 8;
```

That built the match. Two perturbations of the matched source bound it from
both sides: dropping the grid coordinate from the chain, so `tx` is computed
straight from `D_8009B36C`, builds 119 against 120; and keeping the chain but
giving the difference a fresh local costs two positions. So all three values
have to pass through the one variable, and it is the chain that matters rather
than economy of locals.

This is the rule the subtraction section above collapses into. A register that
holds several values in sequence is evidence that the source named them with
one variable — and that is the whole of it. How the individual assignments are
split into statements does not matter.

The general point is the same one the volatile rule makes from the other side:
**a register that holds several values in sequence is evidence that the source
named them with one variable.** Look at the whole live range before deciding
what a register means.

## A test by sll 16 rather than andi means the value went through a signed local

`andi $v0, $v0, 0xffff` before a branch tests the low half of a value the source
treated as unsigned. `sll $v0, $v0, 16` in the same position tests it as
`s16`. The load can still be `lhu`, because the field is unsigned and only the
local is signed:

```c
s16 left;
left = *(u16 *)(widget + 0x60) - 1;
*(u16 *)(widget + 0x60) = left;
if (left == 0) {
```

Reading, storing and testing the field directly gives `andi` and no local.

**The tell only discriminates on an equality test.** A signed relational test
needs `sll 16` whatever the source does, so `sll 16` in front of a `bgtz`,
`blez`, `bltz` or `bgez` proves nothing about a local. `func_80168AB4` in the
password module has exactly that shape on its frame count and builds
byte-identical code with or without the local, which was measured both ways.
Only `sll 16` where `andi` would otherwise be expected — in front of a `beqz`
or `bnez` — is evidence.

Both rules verified by `FreeDuel_UpdateCursorTween` in the free duel module;
the limit on the second measured on `func_80168AB4` in the password module.

## A re-read that must land early needs its own local

When a global pointer is stored through and then read again, the store may
alias the global, so the compiler must reload — and the scheduler cannot hoist
that reload back above the store. If the target issues the reload *earlier*
than the statement that consumes it, the only way to get it there is to write
it earlier, which means giving it its own local.

`func_8018001C` writes three fields of `D_80184560` and the target uses two
loads for them, pairing `+0x6C` and `+0x60` on the first and `+0x36` on the
second:

```c
third = D_80184560;
third[0x6C] = 0x3C;
fourth = D_80184560;               /* the re-read, written early */
*(s16 *)(third + 0x60) = -2;
*(u16 *)(fourth + 0x36) = 0;
```

Spelling the global in the third statement instead of naming `fourth` produces
the same two loads and the same pairing, but issues the second one immediately
before its own store, where it stalls: one extra `nop`, one instruction over.

Two rules combine here and are worth separating:

- **Which statement re-reads** decides the load-to-store pairing. A local for
  the first group and the global for the last gives one load per group.
- **Where the re-read is written** decides its position, and therefore whether
  its use stalls. A store through the earlier local pins it.

The general form: a reload's position in the output is the position of the
source expression that caused it, not the position of the value's use. When
those differ in the target, the source named the value.

Verified by `func_8018001C` in the main menu module.
