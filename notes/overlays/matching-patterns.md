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
*(s16 *)(a + 0x18) = D_8016D428 * 16 + 0xA3;

/* matches */
s32 value = D_8016D428;
*(s16 *)(a + 0x1A) = 0x63;
*(s16 *)(a + 0x18) = value * 16 + 0xA3;
```

Verified by `func_8016A00C` in the password module.

## A stall the compiler would have filled

The converse also happens, and it is a stopping condition rather than a
technique. Where two accesses share a base pointer at different offsets, GCC
can prove independence and will reorder freely to fill a delay slot. If the
target leaves the stall in place, no ordinary source ordering reproduces it.

`func_8016A02C` in the password module is the recorded example: the target
keeps a `nop` after `lhu $v1, 0x8($v0)` where GCC hoists the load above the
neighbouring `sb`. It remains assembly rather than being forced.

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

<<<<<<< HEAD
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
=======
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
>>>>>>> e876f3e9 (decomp: match func_80168588 in both overworld overlays)

## Known unresolved residual

`FreeDuel_PlaceCursor` is the only overlay function left in this state, and it
is close: a candidate already emits the same instructions in the same count as
the target under `gcc_2_8_1_g0_split`, with **only the two leading `lui`
instructions transposed**. Reading `D_8009B366` and `D_8009B367` into locals
before the coordinate stores is what fixed the load placement, since GCC will
not hoist a global load across a store through a `u8` pointer. Declaration
order does not affect what is left.

Its inventory note also records that `-fno-schedule-insns` and
`-fno-schedule-insns2` both move it further away. **That measurement predates
`gcc_2_8_1_g0_split_no_sched1` and `gcc_2_8_1_g0_no_sched2_split`, and it is
not recorded whether it was taken with those flags added to the split profile
or through the non-split profiles that were the only ones available.** Two
functions have already been unblocked by exactly that ambiguity, so repeat it
unambiguously before treating the flags as ruled out.

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
