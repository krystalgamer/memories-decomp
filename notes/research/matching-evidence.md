# Matching Type and Code-Generation Evidence

## Purpose

This note preserves recurring evidence recovered while matching larger game
functions. It records concrete widths, strides, relocation behavior, and
compiler idioms that should guide untouched functions. Address-based names
remain in use because the original semantic names are unknown.

Exact matching functions are the strongest anchors. Layouts inferred only from
nonmatching candidates are identified as provisional and must be corroborated
before they become shared C types.

## GCC 2.8.1 code-generation patterns

### Data placement and address formation

- `%gp_rel` byte and halfword globals require a `gcc_2_8_1_g8` profile.
- Large arrays and structures referenced with `%hi`/`%lo` pairs require G0
  declarations or declarations whose size exceeds the small-data limit.
- Use a split-address profile when the target retains a symbol's high half
  across intervening instructions before applying the low relocation.
- Mixed absolute and GP-relative references can still require a G8 profile;
  classify each symbol independently instead of assigning a profile from one
  global.
- `-msplit-addresses` is a per-translation-unit compiler flag, not a maspsx
  setting; the three G0 profiles pass identical maspsx flags and differ only in
  `-msplit-addresses` versus `-mno-split-addresses`.
- A function *can* nevertheless carry both address forms, and one profile in the
  table supplies them: see "Targets that need both address forms" below.

#### Targets that need both address forms

Some functions require macro form for a direct scalar load and split form for a
symbol-indexed load in the same body. Choosing between `-msplit-addresses` and
`-mno-split-addresses` cannot give both, and each choice leaves a different
residual.

The two forms are easy to tell apart in the target:

- macro form reuses the destination register as its own address temporary, as in
  `lui $a0, %hi(sym)` followed by `lb $a0, %lo(sym)($a0)`. Because no spare
  register survives, a second read of the same global must re-materialise the
  address, which is itself a useful signal.
- split form keeps the symbol in a register of its own and adds an index to it,
  as in `lui $v0, %hi(sym)` / `addiu $v0, $v0, %lo(sym)` / `addu $v1, $v1, $v0`
  / `lb $v0, 0($v1)`, four instructions in total.

Under `-mno-split-addresses` GCC emits the indexed load as the macro
`lb $v1, sym($v1)`, which the assembler expands through `$at` in three
instructions rather than four.

The conflict shows up in two different ways, so both are worth recognising:

- `Duel_GetTerrainBoost` (`0x8002497C`) ends one instruction short, at 24 of 25.
  Its two `gDuel_bTerrain` reads are correct only under a non-split profile,
  which is also what forces the second read to re-materialise its address, but
  the indexed table load then collapses to the three-instruction `$at` form.
  The second half of that reading has since been corrected: see "Cross-block
  address CSE" below. The address form is not what costs the instruction.
- `Ai_GetHandSize` (`0x80070710`) keeps the correct instruction count of 10
  under a split profile, and differs only in which register carries the high
  half: the target reuses the load's destination, as macro form does, while
  split form allocates a separate register for it.

A shortfall in the instruction count is therefore not the only symptom of this
conflict; a same-length body whose only fault is the address temporary is the
same problem seen from the other side.

##### The conflict is resolvable: choose the form per symbol, with `-G`

The choice is not actually between the two address forms. It is between *sizes*.
`-msplit-addresses` governs only how GCC materialises the addresses it decides
to materialise; which symbols get materialised at all is governed by the
small-data threshold. Compiling at `-G8` puts a small scalar below the threshold
and leaves the large array above it, and the two are then emitted differently in
the same function: the scalar in macro form that reuses its own destination as
the address temporary, the array in the four-instruction split form.

`gcc_2_8_1_cc_g8_as_g0_split` is exactly that profile — `-G8 -mgas
-msplit-addresses` at compile time, `-G0` at assembly time, so nothing ends up
`%gp_rel` in the object. It resolves `Ai_GetHandSize` (`0x80070710`) exactly from
pure C, after six terminal attempts had settled on the address temporary as the
irreducible residual:

```c
s8 Ai_GetHandSize(void)
{
    return gDuel_aOpponentData[gDuel_bOpponentID].b[0];
}
```

The register pin the earlier candidates carried is not needed and does nothing;
the allocation follows from the two address forms once they are both available.

So when a target mixes the forms, read the *sizes* of the symbols involved
before concluding anything about the flag. A body that mixes a sub-threshold
scalar with an above-threshold aggregate wants a `cc_g8_as_g0` profile; the
conflict is only real when both symbols fall on the same side of the threshold.

Pinning a base pointer to a hard register does not help: GCC folds the pointer
back into a symbol-indexed load and the pin is optimised away.

#### Large constant offsets need a struct member and a split profile

A table reached at a large constant offset from a symbol has its own failure
mode, distinct from the two address forms above. Written as pointer
arithmetic, both halves of the offset are compile-time constants in one
expression, so GCC folds them together and emits a single oversized load
displacement. The assembler expands that through `$at` in three instructions:

```
lui $at, 5 ; addu $at, $v0, $at ; lw $v0, -18760($at)
```

The target instead spends four, materialising the round part of the offset and
keeping the remainder as the displacement:

```
lui $a0, 0x4 ; ori $a0, $a0, 0x8000 ; addu $v0, $v0, $a0 ; lw $v0, 0x36B8($v0)
```

Declaring the symbol as a padded struct object and reading the member directly
restores that form, because GCC splits a large struct member offset rather than
folding it. Measured against `gcc_2_8_1_g0_split` by varying only the pad:

| member offset | materialised | load displacement |
| --- | --- | --- |
| `0x1000` | none | 4096 |
| `0x7FF0` | none | 32752 |
| `0x8000` | `0x8000` | 0 |
| `0x9000` | `0x8000` | 4096 |
| `0x18000` | `0x18000` | 0 |
| `0x36B8` | none | 14008 |
| `0x4B6B8` | `0x48000` | 14008 |
| `0x4B9FC` | `0x48000` | 14844 |

Below `0x8000` the whole offset fits a signed displacement and nothing is
materialised. At or above it GCC materialises the largest multiple of `0x8000`
and leaves the remainder as the displacement. Beware that padding is subject to
the struct's alignment, so a pad of `0x8001` in front of a four-byte member
places it at `0x8004`, not `0x8001`.

Two conditions are easy to get wrong:

- the idiom needs a **split** profile. Under a non-split profile GCC folds the
  offset into the relocation instead, emitting `lui $at, %hi(sym+308920)`, and
  the `$at` form returns. No arrangement of the struct changes that.
- the offset must actually be large enough to split. Declaring a symbol as a
  padded struct object buys nothing when the member sits below `0x8000`, since
  there is no split to make. `Ai_GetHandSize` is the worked negative: rewriting
  `gDuel_aOpponentData` as a struct object leaves every non-split profile
  emitting the same `lui $at` / `addu $at` / `lb` sequence as before.

`func_80024D34` (`0x80024D34`, `0x48000` plus `0x36B8`) and
`duel_has_all_exodia_pieces.c` (`0x4B9FC` in the same blob) are the two worked
positives, both on `D_8015C424`.

Register pins are not a substitute here either. GCC constant-folds through the
pin, so pinning the base or the offset leaves the `$at` form in place.

#### How much the address temporary tells you about the profile

The two forms above are worth measuring rather than eyeballing, because the
signal is strong in one direction only. Scanning the matched corpus for every
`lui $X, %hi(sym)` immediately followed by a load whose base register is also
`$X`, and splitting those pairs by the profile each function is recorded
under, gives:

| Profile group | destination reused | separate register | pairs |
| --- | --- | --- | --- |
| non-split | 299 (99.3%) | 2 (0.7%) | 301 |
| split | 14 (60.9%) | 9 (39.1%) | 23 |

Read this asymmetrically:

- a **separate** register is roughly fifty times more likely under a split
  profile than a non-split one, so seeing one in the target is strong evidence
  for split before any C is written.
- **reuse** is only weak evidence for non-split. It is what non-split almost
  always does, but split still produces it in a clear majority of cases, so it
  cannot on its own rule split out.

The practical consequence is that reuse in the target does not justify
abandoning a split profile that is otherwise scoring well. `func_8004A27C`
(`0x8004A27C`) is the worked example: the target reuses the destination at its
`D_8009B458` load, yet the split profile reaches 12 differences at the correct
31 instructions with every register allocated as retail has it, while the
non-split profile that reproduces the reuse sits at 20 and misallocates the
`mflo` pair. Only the separate-register case should be treated as decisive.

Reproduce the table by pairing `config/slus_01411/matching_c.json` against the
built executable; file offset is VRAM minus `0x8000F800`.

#### Comparing profiles: count parity beats diff size

When choosing between profiles, rank candidates by whether the instruction
count equals the target's, not by how small a normalised diff is. The two
disagree, and the diff is the misleading one.

A profile that renders a `%gp_rel` load as `lui` plus `lw` adds an instruction
the target does not have. Every later instruction then shifts by one, but the
extra instruction also pads the body, and a diff that compares position by
position can report fewer differences for the profile that is further from a
match.

`func_800472A8` (`0x800472A8`) is the worked example: it needs `gcc_2_8_1_g8`
because `g_SDValue` is `%gp_rel`, yet a G0 profile scored better on a naive
diff while being strictly wrong. Once the correct profile was used the count
matched at 27 and every remaining difference was cosmetic.

Prefer instruction-count parity first, then compare the bodies. "The diff got
smaller" is the natural way to pick a profile and it will rank the wrong one
first whenever address form changes how many instructions a load costs.

##### The rule inverts when the source shape changes

Count parity ranks profiles against a *fixed* source. It is not a way to rank
source shapes against each other, and used that way it points the wrong way,
because a shape can shed instructions by dropping a structural element the
target actually has. When the source changes, require count and diff to agree
before preferring a shape, and treat a candidate that falls *below* the
target's count as a warning rather than as progress.

`func_8005A3D0` (`0x8005A3D0`, 38 instructions) produced four of these in a
row while the best honest candidate sat at 38 of 38 with 20 differences:

| shape | instructions | diffs |
| --- | --- | --- |
| self-pointer taken from the outer cursor | 39 | 38 |
| `while (1)` with the increment in the exit test | 37 | 32 |
| a `goto` into the middle of the `for` | 37 | 32 |
| dropping the redundant `if (n != 0)` guard | 35 | 32 |

The first looks like an improvement on a then-current 41 and is not: it deletes
the byte-offset cursor the target plainly carries, and re-forms the
self-pointer from the outer pointer instead of from a reloaded base. The last
three fall under the target count outright. In every case the diff moved the
opposite way to the count, which is the signal to distrust the count.

A later post-terminal result kept both structural requirements: an explicit
outer-loop offset and a body-first inner loop. Moving the outer-record
increment into the outer back edge, then pinning only the remaining allocation
roles, matched all 38 instructions under `gcc_2_8_1_g8_split`.

#### A `do`/`while` with a `break` pays a rotation fixup

GCC 2.8.1 rotates a search loop written body-first into a continue-form branch
with the induction variable bumped in the back-edge delay slot, then undoes the
bump on the way out:

```
addiu $a2, $a2, 0x1        in the delay slot of the back edge
addiu $a2, $a2, -0x1       immediately after the exit
```

That pair is pure overhead and the target does not have it. Writing the same
search as a `for` removes it:

```c
for (; j < n; j++, inner++) {
    if (inner->f4C == self) {
        break;
    }
}
```

On `func_8005A3D0` that was worth three instructions, taking the body from 41
to exactly 38. Every body-first spelling reintroduces the fixup and returns to
41: `do { … } while (j < n)`, `do { … } while (++j < n)`,
`do { … } while (inner++, j < n)`, and `for (;;)` with two `break`s all score
the same 41.

The awkward part is that the target's own loop *is* body-first: it relies on an
earlier guard and drops straight into the body, spending the two slots the
`for` form spends on its entry test on address setup instead. None of the
initial isolated loop spellings gave both properties at once.

The later exact source recovered the missing surrounding structure. It uses
explicit `outer`, `inner`, `after_inner`, and `next` labels, keeps the
byte-offset cursor separate from the typed record pointer, and advances the
record in the outer back-edge delay slot. Allocation-only register pins then
preserve the remaining retail roles. The result matches all `0x98` bytes and
relocations under `gcc_2_8_1_g8_split`.

#### Separate register-permutation residuals from schedule-permutation ones

A body whose instruction multiset, registers and relocations are all exact but
whose order is wrong is a different blocker from one whose order is exact and
whose registers are wrong, and the two want opposite fixes. Record which of
them a terminal residual is, because the pin advice under "Register pins" only
applies to the second.

`func_8004A764` (`0x8004A764`) is the schedule case. Its six terminal attempts
were all recorded under `gcc_2_8_1_g0`, but the matched neighbour
`func_8004A7C0` uses `gcc_2_8_1_g0_split`, and the target's
`lui`/`addiu %hi`/`%lo(D_80011434)` pair is the split form. Under the cohort
profile, and with the `Packet` type and the `func_80077450(Packet *)` prototype
already recovered in `src/game/sound_secondary_playback.c`, the body comes out
at 23 of 23 instructions, 0x5C of 0x5C bytes, with identical registers and
identical relocations. Everything from `sw $v0, 0x4C0($v1)` to the epilogue,
including the `sw $v0, 0x4E4($v1)` branch-delay store, is byte-exact. Only the
11-instruction prologue is ordered differently: retail finishes the
`D_80011434` address arithmetic before materialising the `D_8009B458` high
half, and sinks `sw $ra` below `ori $a1`.

Six source shapes were probed against it. Stores taken through the packet
pointer collapse to small offsets off `$a0` and are the wrong shape outright.
Stores taken through the state pointer, with the cast applied only at the call,
are the right shape; a typed overlay struct with a named `packet` member
compiles to the same thing and reads better. The two shapes that fix the
prologue order both break the allocation instead: hoisting the loaded value
into a local moves it to `$v1` and the state to `$a0`, and hoisting the element
address moves `addiu $a0, $a0, 0x4C0` into the delay slot. `-fno-schedule-insns`
and `-fno-schedule-insns2` each produce a third order that is no closer.

So every shape that fixes the schedule breaks the allocation, and every shape
that keeps the allocation keeps the schedule. Pins do not help here: the
allocation is already correct. A residual of this kind is compiler-side, and
the next attempt on it needs a `cc1` whose list scheduler tie-breaks
differently, not a seventh C variant.

`func_8004A6F8` (`0x8004A6F8`) is the same function with three `u16` fields read
from a second parameter instead of constants, and it lands in the same place
from the other direction. Reading the table entry into a local before the state
pointer gives its 27 instructions and 0x6C bytes; pinning that local to `$2` and
the state pointer to `$3` then makes every register correct. 23 of the 27 are in
place, and the four that are not are one permutation: the target issues the
state load immediately after its `lui` and the candidate fills that slot with
`sw $ra`. So pins can finish the allocation half of this residual even when the
schedule half stays out of reach — worth doing, because it narrows what the next
attempt has to explain.

#### Terminal histories recorded before the profile system are not terminal

`config/slus_01411/attempts.csv` has two eras. Later rows name a profile from
`compiler_profiles.json` in both the `compiler` and `flags` columns; earlier
rows name a toolchain and a free-form phrase, such as
`gcc-2.8.1-psx / -O2 -G8 early-return`. 268 addresses carry at least one row of
the older kind, and 161 of those are still `unmatched_asm`. Those histories
never tried their cohort's profile, so their six attempts do not mean what a
profile-era six means.

`func_80013B04` (`0x80013B04`) shows the difference. Its six rows are all
free-form, and every one of them blames the branch orientation: "inverted the
busy branch", "moved the null return to the shared epilogue". Both claims are
wrong. Under `gcc_2_8_1_g8_split` — the profile its matched neighbour
`func_80014A5C` uses, and the one the target's `%hi`/`%lo(gFile_anLba)` pair
requires — writing the guard positively puts the null return inline exactly
where the target has it:

```c
if (((D_8009B0F4 & 0x2000030) | D_8009B134) == 0) {
    transfer = &D_800E9E60;
    transfer->state = 0;
    D_8009B0F4 = 0x100010;
    transfer->field_24 = gFile_anLba[file_index] + sector_offset;
    return transfer;
}
return 0;
```

That is 25 of 25 instructions, 0x64 of 0x64 bytes, the target's registers, the
target's relocations, `%gp_rel` on both `D_8009B*` reads, and the target's block
layout. What is left is a `sched2` permutation: the target hoists
`addu $a3, $a1, $zero` to instruction 1 and fills the delay slot with
`lui $a1, 0x10`, and it interleaves the two address pairs in the body rather
than completing each one.

So the recorded blocker was an artefact of the wrong profile, and the real one
is the same schedule residual as above. When a pre-profile history is the only
history an address has, the cheapest new evidence is its cohort's profile, and
`matching_c.json` gives that for free from any matched neighbour.

#### Cross-block address CSE, and what it costs

`Duel_GetTerrainBoost` (`0x8002497C`) reads `gDuel_bTerrain` twice and
re-materialises its address the second time:

```
lui  $v1, %hi(gDuel_bTerrain)
lbu  $v1, %lo(gDuel_bTerrain)($v1)
```

GCC 2.8.1 will not do that. Its three blocks — the entry, the `slti` block, and
the body — each have a single predecessor, so they form one extended basic
block, `cse_main` sees the first read's address as still available, and the
second read becomes a bare `lbu $v1, 0x0($a1)`. That one missing `lui` is the
entire difference. Under `gcc_2_8_1_g0_split` the rest of the body is exact:
23 of the 24 emitted instructions are byte-identical to the target, at
positions 0..13 and, one earlier, 15..23.

Three levers are needed to get that far, and each is worth knowing on its own:

- a `goto` past the shared `return 0` produces the target's block layout. `if
  (c) return 0;` twice makes GCC duplicate the null return; a plain nested `if`
  makes it sink the return past the body. Only the `goto` puts the return
  between the two branches where the target has it.
- pinning the table base to `$4` is what produces `addu $v1, $a0, $zero`. The
  parameter copy exists because `$a0` is taken by the table address before
  `type` is used again; with the base anywhere else, `$a0` survives and the copy
  never appears.
- splitting `type * 6 - 1` into its own local keeps `addiu $v0, $v0, -0x1` out
  of the load displacement.

Nothing reaches the CSE itself. A 2-D `gDuel_aTerrainBoost[type][gDuel_bTerrain
- 1]`, an `extern u8 gDuel_bTerrain[9]` declaration with `[0]` subscripts, and
pinning the first read's value to `$2` all keep it, and the first two also fold
the `-1` back into the displacement.

##### `volatile` separates the value CSE from the address CSE

There are two CSEs in play, and they can be defeated separately. Declaring the
global `volatile` forces the *load* to be re-issued but does **not** force its
*address* to be re-materialised, so it recovers the missing instruction without
reaching the `lui`.

On `Duel_GetTerrainBoost` that takes the body from 24 of 25 instructions to the
correct 25 of 25 under `gcc_2_8_1_g0_split`. The second read reappears, but as
`lbu $v1, 0x0($a1)` against the address still held from the first, so the
`lui`/`lbu` pair above is still not reproduced. Instruction count parity is
therefore recovered while the residual stays.

The same lever settles the count on `func_8002DDFC` (`0x8002DDFC`), where the
target performs two independent read-modify-write sequences on `D_8009B0F4`:

```
lui/lw ; and ; lui/sw          clear
lui/lw ; or  ; lui/sw          set
```

Left non-volatile, GCC folds the second read into the first and collapses both
into one read-modify chain, which costs exactly two instructions and leaves the
body at 74 of 76. Declaring it `volatile` gives 76 of 76.

Two caveats keep this in proportion:

- the levers are not additive. On `Duel_GetTerrainBoost`, combining `volatile`
  with the `goto`, the `$4` pin, or the split `type * 6 - 1` local each scores
  worse than `volatile` alone. They are alternative routes to the instruction
  count, not stackable improvements.
- the pattern is uncommon, so this is not a reflex. Across the matched corpus
  only about 6% of functions whose source declares no volatile extern contain a
  re-materialised repeat load, against about 15% of the 48 whose source does —
  a real but weak association on small numbers. Reach for `volatile` when the
  target visibly reads one global twice, not on a whole-body mismatch.

Declaring these globals `volatile` is consistent with the tree rather than a
trick: 25 tracked sources already do it, and `D_8009B0F4` in particular is
declared `volatile` in ten places against seven plain ones.

The correction to the earlier reading: under `gcc_2_8_1_g0_no_split` the address
is *also* CSEd — materialised once at the top and reused at both reads — and the
table load additionally degrades to the `$at` macro, giving 23 of 25. So the
non-split profile does not force re-materialisation, the split profile is
strictly better, and this residual belongs with the compiler-side ones rather
than with the address-form conflict.

#### Two source levers that are worth trying before any profile change

Both come from `Duel_GetBaseCardStat` (`0x8002CBF4`) and both are general.

**Split a load-and-shift into two statements.** Written as one expression, the
two arms of an `if`/`else` that both read the same array get opposite register
assignments:

```c
value = gDuel_adwCardStats[card_id - 1] >> 9;   /* arms differ */
value = gDuel_adwCardStats[card_id - 1];
```

Split into a load and a `>>= 9`, both arms compile to byte-identical address
blocks, which is what the target has. The shift then lands in the `j`'s delay
slot on its own.

**Clamp by assignment, not by return.** These are not the same shape:

```c
if (stat < 10000) return stat;      /* bgez + j, result kept in $v0 */
return 9999;

if (stat >= 10000) stat = 9999;     /* bltz / slti / bnez, copies in delay slots */
return stat;
```

The second is the retail shape here, and switching to it made the last thirteen
instructions byte-exact in one step. The tell in a target is a comparison whose
result is copied into `$v0` from a delay slot rather than computed there.

#### The no-sched1 profile needs a split variant

`gcc_2_8_1_g0_no_sched1` (#537) is `gcc_2_8_1_g0` plus `-fno-schedule-insns`,
and it is not a split profile. A function that needs both — first-pass
scheduling off *and* split addresses — has no profile to name.

`Duel_GetBaseCardStat` is that function. Its three
`%hi`/`%lo(gDuel_adwCardStats)` blocks require split addresses; under
`gcc_2_8_1_g0_no_sched1` they collapse to the `$at` macro and the body drops
from 45 instructions to 42. Under the default `gcc_2_8_1_g0_split` the
`sll $s0, $v0, 1` that completes a `* 10` is deferred past the third address
block into its load-delay slot, so the body comes out at 44 and the multiply
temporary moves from `$v0` to `$a1`. With `-msplit-addresses` and
`-fno-schedule-insns` together the multiply stays whole, the load-delay `nop`
returns, and the body is 45 of 45 with a single register pair left over.

`gcc_2_8_1_g0_split_no_sched1` is added here for that reason. It is additive:
no source names it yet, and `make match` is unaffected.

#### Pins can cost an instruction near an incoming argument

The counterpart to the `%hi`-temporary and call-return rules. At this
snapshot, `func_8003201C` (`0x8003201C`) reached its 40 instructions with the
right multiset and a two-pair register permutation, and its own cohort file
(`build_deck_add_card.c`) uses pins freely, so pins are the obvious next step.
Every one tried makes it **41**: pinning the outer counter to `$6` makes GCC
copy `$a0` into `$a3` first because the pinned register collides with the
incoming argument, and pinning the inner counter or the entry pointer makes it
hoist an extra `addiu $t1, $a0, 0x2D54` and add a copy. When the permutation
you want involves a register adjacent to an incoming argument, a pin buys a
copy rather than moving one.

Separately, that function only reaches 40 instructions at all when the counter
address is written inline. Binding it to a local first —

```c
u8 *slot = (u8 *)(arg0 + id);
slot[0x5AC4] = 0;
... slot[0x5AC4]++;
```

— gives 39: GCC keeps one register for both uses. Writing both accesses as
`*(u8 *)(arg0 + id + 0x5AC4)` makes it strength-reduce `arg0 + id` into an
induction pointer and take a loop-invariant copy of it for the inner loop,
which is the 40th instruction.

A later post-terminal source changed the surrounding histogram structure
rather than adding a pin to the old shape. It initializes the inner record
index before assigning the output and record pointers, keeps the card ID in
`$a2` and output pointer in `$a3`, and uses a separate second loop for the
leading occupied records. That combination matches all `0xA0` bytes and
relocations under `gcc_2_8_1_g8`.

### Disassembly artifacts

Splat names any address-shaped literal as though it were a symbol. A `%hi`/`%lo`
pair against a symbol with no definition anywhere in the image is therefore a
constant that splat has guessed at, and its two instructions should be read
independently rather than as one materialisation.

`func_8005C5D4` (`0x8005C5D4`) was deferred after six attempts for this reason.
Its `lui $v1, 0x80` and `addiu $v1, $v1, -1` were read as a single `0x7FFFFF`
named `D_7FFFFF`, and asking GCC for `0x7FFFFF` yields `lui 0x7F` + `ori 0xFFFF`,
which no profile bends into the retail pair. The value is really `0x800000` with
the loop's first decrement peeled out ahead of the loop label, corroborated by
the compensating `addiu $v1, $v1, 1` on the early exit.

A later post-terminal reconstruction expressed that interpretation directly:
initialize the timeout to `0x800000`, test `--timeout <= 0`, and clear the
completion flag on every exit. That pure C now matches all `0x58` bytes under
`gcc_2_8_1_g8`.

### Signed values and arithmetic

- Signed bytes are commonly loaded with `lbu` followed by `sll 24` and
  `sra 24`. A plain `signed char` expression often reproduces this shape.
- Signed division by ten uses the `0x66666667` multiply-high sequence.
- Division of a signed halfword by two sign-extends the halfword, adds the sign
  correction, and shifts right to preserve C truncation toward zero.
- Fixed-point power-of-two division rounds negative products by adding
  `divisor - 1` before the arithmetic shift.
- Values cast back to signed bytes may require an explicit C cast to recover
  the target's final `sll`/`sra` pair.

### Structures and control flow

- Packed eight-byte structure assignments can generate
  `lwl`/`lwr`/`swl`/`swr`; replacing them with two aligned words changes the
  instruction stream.
- Local absolute jumps carry `R_MIPS_26 .text`, so branch order, fallthrough,
  and duplicated epilogues are part of the match.
- Input values retained across calls naturally occupy `$s0` and then
  `$s1`-`$s3` in declaration/use order.
- Three-way state initializers are especially sensitive to assignment order
  and whether the original source was a switch or nested conditionals.

### Source shapes that steer GCC, verified against the target

Each of these was isolated by taking a candidate that was already close and
changing exactly one thing. They are levers, not style: the alternative spelling
in each row is byte-different, not just different-looking.

**Keep a sign-extending load by widening the consumer.** A `s16` local read and
stored straight back into an `s16` global loads with `lhu` — the result is
truncated on the way out, so GCC drops the sign extension. Taking the value
through an `s32` first makes the extension load-bearing and GCC folds it back
into the load as `lh`. `func_800178BC` reads the two halves of a `swc2 $14`
write-back this way: `lhu` for the half that stays unsigned, `lh` for the one
that goes through an `s32`.

**Force a local onto the stack with an `"=m"` output rather than `volatile`.**
`volatile s16 x` does put the local in memory, but the read comes back as `lhu`
plus an `sll`/`sra` pair, and the local can still take a callee-saved register
elsewhere. Adding the local as an `"=m"` operand of the asm block that writes it
places it in the frame and reads it with a single `lh`. That is the difference
between 0xA4 and 0xAC in `func_8001B0CC`.

**Split a multiply and a divide across three assignments to defeat
reassociation.** `(x * 8 + 0x7FF) / 0x800` folds to `(x + 0xFF) / 0x100` —
identical arithmetic, one instruction shorter, no `sll`. Written as

```c
step = (s32)(x << 3);
step = (step + 0x7FF) / 0x800;
step = step + 1;
```

GCC keeps the shift, both bias arms and the `sra` exactly as the target has them.
Two statements is not enough; it has to be three, each assigning the same
variable. The `__asm__ volatile("" : "+r"(v))` barrier also blocks the fold, but
it pins the intermediate and reorders everything around it — 39 of 55
instructions in `func_80047788` — so it is the wrong tool here.

**Name a load to coalesce base-plus-index.** `p = (u8 *)(index + *(s32 *)(base))`
loads into one register and adds into another. Naming the load first,

```c
u8 *b = (u8 *)*(s32 *)(base);
p = (u8 *)(index + (s32)b);
```

coalesces the copy and produces the target's `addu $v1, $s0, $v1`. Note the
operand order matters and is not symmetric: `p = b; p += index;` also coalesces
but emits `addu $v1, $v1, $s0`, a different encoding.

**Write an unrotated loop with explicit `goto`.** GCC rotates a loop whose test
is at the top, duplicating the test and branching into the middle. Every
structured spelling of the inner search in `func_8003B5C8` does this and costs
three to eight instructions; only

```c
top:
    if (hit) { ...; goto done; }
    e++;
    if (!end) { idx++; goto top; }
done:;
```

reproduces the target's single top test with a back-edge to it. Compare with the
`goto` lever recorded under "GCC rotates a top-of-loop conditional exit", which
addresses block *placement*; this one addresses loop *rotation*, and the two are
independent.

**Hoist a loop-invariant probe by hand.** GCC did not lift `*(s32 *)D_801D9004`
out of the outer loop in `func_8003B5C8`; reading it into a local before the loop
is what reproduces the target's single load into `$t3`.

### Compiler-generated jump tables cannot currently be integrated

A `switch` that GCC compiles into a jump table cannot be accepted by the build
as it stands, however exact the C is. Check for this before starting a function,
because the C can be finished and still be unusable.

Scanning the generated assembly for `jtbl_` references puts **36 unmatched
game functions** in this position, so it is worth screening for rather than
discovering late. The smallest are `Ai_GetWinningCardRange` (`0x80070738`,
140 bytes) and `Ai_GetCardRange` (`0x800707C4`, 172 bytes), both of which look
like attractive small targets and neither of which can be landed. Reproduce
the list with:

```sh
grep -l 'jtbl_' tmp/splat/asm/generated/*.s
```

and cross-reference the `glabel` above each reference against the
`unmatched_asm` rows in `functions.csv`.

The symptom is a link failure rather than a mismatch:

```
ld: section .initialized_data LMA [00080ee0,0008b88f]
    overlaps section .rodata LMA [00080ed8,00080ef3]
ld: section .rodata VMA [800906d8,800906f3]
    overlaps section .text_padding VMA [800906d4,800906df]
```

Two facts combine to cause it:

- `linker/slus_01411.ld` defines no `.rodata` output section, so a compiler
  table becomes an orphan and is placed immediately after `.text`, on top of
  `.text_padding` and `.initialized_data`.
- The retail table is already in the image as data. For `Ai_GetWinningCardRange`
  it lives at `0x8001194C`, which falls inside `.initial_data`
  (`0x80010000` for `0x29D8`), a single blob emitted from `initial_data.o`.

So accepting one would require splitting that blob around the table's address
and placing the object's `.rodata` into the resulting hole, which changes the
data emission and the size assertions. That is a build-configuration decision,
not something to solve by trying more source shapes.

`Ai_GetWinningCardRange` (`0x80070738`) is the worked example. Its C reaches all
35 instructions exactly, and the emitted `.rodata` is `0x1C` bytes - exactly the
seven words of the retail table - so it is the right table at the wrong address.

Confirming this is a new case rather than an oversight: `symbols.txt` contains no
`jtbl_` symbols, no source under `src/game` references one, and every matching
source that contains a `switch` compiles to a comparison chain instead of a
table. Small or sparse switches are therefore fine; only a dense one that GCC
turns into a table hits this.

When reading a target, the tell is a `lw` from a `%hi`/`%lo` symbol pair
followed by `jr` on the loaded register.

#### How much this blocks

Worth knowing before deciding whether the build-configuration work is worth
doing: **36 of the 291 remaining resident functions contain a compiler jump
table, and they account for 62,408 of the 243,664 bytes still in assembly.**
That is 12.4% of the functions but **25.6% of the bytes**, because a function
dense enough for GCC to build a table is a large function. The remaining
resident work is not one queue but two, and the larger quarter of it by weight
is waiting on a linker script rather than on any decompilation.

Two independent counts agree exactly. Searching each unmatched function's
generated assembly for a `jtbl_` symbol, and searching it for the tell above —
a `jr` on a register other than `$ra` — select the same 36 addresses and the
same byte total. The smallest are `Ai_GetWinningCardRange` (`0x8C`) and
`Ai_GetCardRange` (`0xAC`); the weight is in the large ones.

The practical consequence for candidate selection: an ascending-size sweep of
the remaining functions will keep surfacing these, and each one can be finished
as exact C and still be unusable. Check for `jtbl_` in the target before
starting, as the section above says — the count is the reason that check pays
for itself rather than being a rare precaution.

The 54 unmatched overlay functions are tracked separately under #162 and are
not included in these figures.

#### maspsx drops the load-delay nop before a macro store

A second build-tooling blocker, in the same class as the jump table above: the C
can be finished, exact, and still not assemble to the target.

maspsx skips the load-delay `nop` when the instruction after a load is a *macro*
store of the just-loaded register — a store written against a bare symbol with
no base register. It assumes the assembler expands that macro into at least a
`lui` plus the store, which covers the delay by itself. That holds for a
`%hi`/`%lo` symbol. It does not hold at `-G8` for a symbol below the small-data
threshold, where gas emits a single `%gp_rel` store and nothing covers the delay.

The two cases differ only in the store's addressing form. Minimal repro against
aspsx 2.81 with `-G8`:

```
lbu $3,106($2)          lbu $3,106($2)
sb  $3,4($5)            sb  $3,D_8009B1B8
```
```
lbu $3,106($2)          lbu $3,106($2)
nop # DEBUG: Reuse ...  sb  $3,D_8009B1B8
sb  $3,4($5)            (no nop, no DEBUG line)
```

`_handle_nop_before_next_instruction` is never reached for the second form.
`.extern` sizes are parsed and discarded in the directive scan, so an `extern`
small global never enters `sdata_entries`/`sbss_entries`; the store is passed
through as a macro for gas to expand, and the load's nop decision goes with it.
Symbols the translation unit defines itself would land in `sbss_entries` and take
the `%gp_rel` path, where `_uses_gp` forces the nop — but nothing under `src/`
defines a global, so that path is never taken in this project.

##### The worked example

`func_80025028` (`0x80025028`, 0xA0) is exact C today. Under `gcc_2_8_1_g8_split`
it reproduces every instruction and relocation of the target and comes out 0x9C,
four bytes short, missing exactly one `nop` between `lbu $v1, 0x6A($v0)` and
`sb $v1, %gp_rel(D_8009B1B8)($gp)`. Inserting that one `nop` into maspsx's output
by hand and assembling with `-G8` matches all 40 instructions.

##### How much this blocks

Scanning the generated assembly for a load, a `nop`, and a `%gp_rel` store of the
loaded register puts **14 unmatched functions, 28,748 bytes**, behind this rule.
The scan only sees functions still in assembly, so this is a floor, not a
census — a matched function cannot contain the pattern, because it could not
have been matched.

```sh
# load / nop / %gp_rel store of the loaded register
grep -B2 '%gp_rel' tmp/splat/asm/generated/*.s
```

The smallest are `func_80025028` (0xA0) and `func_80012DB4` (0xA8); as with the
jump tables, the weight is in the large ones — `func_80019D18` alone is 5,044
bytes.

Unlike the jump-table blocker, this one is a bug rather than a configuration
decision, and the fix is local: give the load's nop check the same treatment for
a macro store that it already gets for a based store, or keep `.extern` sizes so
the small-data path can convert the store and let `_uses_gp` force the nop.

The second of those two has been tried and does not work on its own; see
[The recorded fix does not work as stated](#the-recorded-fix-does-not-work-as-stated)
below for what it costs and what the fix actually needs.

Screen for it the same way you screen for `jtbl_`: a load whose result is stored
straight to a `%gp_rel` symbol, with a `nop` between them in the target, cannot
be reproduced.

### GCC rotates a top-of-loop conditional exit

A third residual class, alongside the register-permutation and
schedule-permutation ones above. Here the loop is *structurally* different, not
reordered or misallocated, and no source spelling reaches it.

`func_8005A3D0` (`0x8005A3D0`) is the worked example. Its inner scan in the
target is a plain eight-instruction loop, match test at the top and counter test
at the bottom:

```
.L8005A414:
    lw    $v0, 0x4C($v1)
    nop
    beq   $v0, $a3, .L8005A434
    nop
    addiu $a2, $a2, 0x1
    slt   $v0, $a2, $t1
    bnez  $v0, .L8005A414
    addiu $v1, $v1, 0x50
```

GCC 2.8.1 copies the top test to the bottom and compensates the counter, which
costs seven instructions across the function — 45 against the target's 38:

```
    bne   $v0, $a3, <loop>
    addiu $a2, $a2, 0x1     <- duplicated increment
    addiu $a2, $a2, -0x1    <- compensation
```

Three spellings of the same loop produce byte-identical output, duplicated
increment and compensating decrement included:

```c
do { if (match) break; scan++; cursor += 0x50; } while (scan < limit);
for (;;) { if (match) break; scan++; if (scan >= limit) break; cursor += 0x50; }
while (!match) { scan++; if (scan >= limit) break; cursor += 0x50; }
```

The explicit `if (count != 0)` guard before the loop — which the target has too,
as `beqz $v1` — already tells GCC the loop runs at least once, so the rotation is
not being done to establish that.

Everything else about the function is reachable. The cohort supplies the access
idiom (`func_800593D0`: a `u8 *` model slot with a `u8` count at `+0xE17` and a
pointer to 0x50-byte entries at `+0xD14`), and the matched neighbour
`func_8005A53C` supplies the profile. Under `gcc_2_8_1_g0_no_sched1` the first
instruction is exact — `lbu $v1, 0xE17($a0)`, whose destination register is
precisely what every one of the six terminal rows missed — and the opening six
differ only in the loop counter's register, a knock-on of the extra live value
the rotation introduces.

Worth checking for before starting a function with a searching loop: if the
target's loop has its exit test at the top and only one copy of the increment,
GCC will not reproduce it, and the C can be finished and still be six or seven
instructions long.

### MASPSX misses a load-delay nop before a store to a small extern

The generated assembly runs under a single `.set noreorder` that MASPSX emits at
the top of every file, so MASPSX owns every hazard nop and GNU `as` will not add
one. There is one case it gets wrong, and it is silent at the source level.

Five lines reproduce it under `gcc_2_8_1_g8_split`:

```c
extern u8 g_small;          /* 1 byte, so -G8 makes it gp-relative */
extern u8 g_big[64];        /* 64 bytes, so it is not */
void f_small(u8 *p) { g_small  = p[0x6A]; }
void f_big(u8 *p)   { g_big[0] = p[0x6A]; }
```

```
f_small:  lbu $v0, 0x6A($a0)     f_big:  lbu $v1, 0x6A($a0)
          sb  $v0, 0x0($gp)              lui $v0, %hi(g_big)
          jr  $ra                        jr  $ra
          nop                            sb  $v1, 0x0($v0)
```

`f_big`'s store expands through `lui`, which fills the load delay by accident.
`f_small`'s assembles to one gp-relative instruction and the delay is left
unfilled, so the store writes the register's stale value.

The mechanism is in `_uses_gp`. It answers "is this next instruction
gp-relative, so a nop is needed" by looking the symbol up in `sbss_entries` and
`sdata_entries`, which hold only symbols *defined in the same translation unit*.
An `extern` is in neither, so `_uses_gp` returns False; `uses_at` returns True
because the store is still in bare-symbol form at that point; and
`nop_at_expansion` is False for ASPSX 2.81. No branch fires, no nop is emitted,
and the assembler then resolves the symbol gp-relative in a single instruction.
MASPSX cannot see what `as` is about to do.

`func_80025028` (`0x80025028`) is the worked example, and it is otherwise
finished. 39 of the target's 40 instructions are byte-exact, registers and
relocations included, from a body that uses only the cohort's own idioms —
`duel_card_selection.c` supplies `slot + D_8009B1D5 * 20`, the
`D_801A7AD8[D_800907D8[position]]` indexing, and the `0x6A` object field. The
one missing instruction is this nop, between `lbu $v1, 0x6A($v0)` and
`sb $v1, %gp_rel(D_8009B1B8)($gp)`. The build rejects it as
`resident text size mismatch`, which is at least a safe failure.

The exposure is bounded but the cost of hitting it is not: an accepted match can
never contain the bug, because the retail body has the nop and a body without it
cannot match. It shows up instead as a function that is exactly one instruction
short for no reason visible in the C, which is expensive to diagnose from the
source side. The tell is a load whose destination is used by the very next
instruction, where that instruction stores to a bare `extern` of eight bytes or
fewer under a G8 profile.

Like the jump-table case, this is a tooling decision rather than a source
problem. `tools/vendor/maspsx` is pinned, and the fix would be to treat a
bare-symbol load or store of a small undefined extern as gp-relative for the
purposes of hazard detection.

A second worked example reaches the same defect from the other side.
`func_80012DB4` (`0x80012DB4`) has

```
lw $v0, %gp_rel(D_8009B0C8)($gp)
nop
sb $v0, %gp_rel(D_8009B0C1)($gp)
```

where the candidate emits the `lw` and the `sb` adjacent. In `func_80025028` the
hazard was a `lbu` through a pointer feeding a store to a small extern; here it
is a `lw` from one small extern feeding a store to another. Both operands being
gp-relative changes nothing, because `_uses_gp` fails on the *store's* symbol
either way — both are `extern`, so neither is in `sbss_entries` or
`sdata_entries`. The load's own form is irrelevant to the decision.

That the two examples differ in load width, in where the loaded value comes
from, and in whether the source operand is gp-relative, while failing at exactly
the same point, is worth more than either on its own: the trigger is the store
alone.

`func_80012DB4` is also worth reading for its `volatile` requirement, which is
unrelated but was what its terminal history actually missed. Four of its six
rows fail at `+0x2c` with `fbff4014 != 03004010` — `bnez` back to the loop head
against `beqz` forward. It is a spin loop, and it only compiles that way when
both globals are `volatile`:

```c
while (D_8009B0C8 < D_8009B0C0) {
}
```

Without `volatile` GCC hoists both loads out and the loop degenerates, which is
what every one of those rows recorded. Two nearby reads are width-sensitive in
the same spirit: `D_8009B0C8` is read once and used for both the store and the
`& 0xFF` test, and `D_8009B0D8` is written as a word but read back as a byte, so
that read has to be `*(volatile u8 *)&D_8009B0D8`.

#### The recorded fix does not work as stated

Both notes above propose keeping `.extern` sizes so a small undefined extern
enters the small-data path and `_uses_gp` forces the nop. That was implemented
literally — parse `.extern <symbol>, <size>` in the directive scan and, for
`0 < size <= sdata_limit`, add the symbol to `sbss_entries` — and measured.

It does produce the nop. `func_8002E5AC` below goes from one instruction short
to byte-exact with no other change. But the tree no longer links:

```
relocation truncated to fit: R_MIPS_GPREL16 against `D_8009AF0C'
relocation truncated to fit: R_MIPS_GPREL16 against `D_8009AF20'
relocation truncated to fit: R_MIPS_GPREL16 against `runtime_gp'
```

The reason is that membership in `sdata_entries`/`sbss_entries` does not only
answer the hazard question. The same test drives the *rewrite*: in both the
load/store and the `la` paths MASPSX emits

```python
if gp_allowed and (symbol in self.sdata_entries or symbol in self.sbss_entries):
    res.append(f"{op}\t{r_dest},{gp_rel}")   # %gp_rel(symbol)($gp)
else:
    res.append(line)                          # leave the bare symbol for gas
```

so marking a symbol to get its nop also converts every access to it into an
explicit gp-relative instruction. A declared size at or below `-G` is not
evidence that the symbol is reachable from `$gp`: nothing under `src/` defines a
global, every address is fixed absolutely by `config/slus_01411/c_symbols.ld`,
and only symbols inside ±32 KB of `runtime_gp` can carry a GPREL16. The three
above are outside it, and one of them is `runtime_gp` itself.

So the fix has to separate the two decisions that currently share one test: the
hazard check may consult `.extern` sizes, the addressing rewrite may not. Giving
`_uses_gp` its own predicate — or the third branch of
`_handle_nop_before_next_instruction`, which already fires on
`uses_at(next) and nop_at_expansion` and is dead only because
`nop_at_expansion` is False above ASPSX 2.30 — reaches the nop without touching
the form gas is left to choose. That is a smaller change than it looked.

#### A third worked example, and it is finished C

`func_8002E5AC` (`0x8002E5AC`, 0x10C, 67 instructions) is the third smallest of
the fourteen and is complete. Under `gcc_2_8_1_g8_split` it builds **66
instructions against 67**; every instruction before the gap is byte-exact, and
everything after it is that one omission shifted by one slot. The gap is the
familiar one, both operands gp-relative as in `func_80012DB4`:

```
lhu $v0, %gp_rel(D_8009B27C)($gp)
nop
sh  $v0, %gp_rel(D_8009B28C)($gp)
```

With the nop supplied it matches all 67 instructions, registers and relocations
included, under `gcc_2_8_1_g8_split` — and also under
`gcc_2_8_1_g8_split_no_strength_reduce` and `gcc_2_8_1_cc_g8_as_g4_split`, which
is the usual sign that no threshold or strength-reduction lever is load-bearing
here.

```c
extern u8 *D_8009B290;
extern u16 D_8009B27C;
extern u16 D_8009B28C;
extern u16 D_8009B2A4;
extern u8 D_800EB0F8[];

extern s32 func_8002E3B4(void);
extern void func_8003B6AC(s32, s32);
extern u8 *TextBox_Create(s32, s32, s32, s32, s32, s32);
extern void DuelEffect_MarkObjectIfActive(void *);
extern void TextBox_Destroy(void *);

void func_8002E5AC(void)
{
    u8 *script;
    u8 *box;
    s32 value;
    u16 flags;
    u16 boxflags;

    if (func_8002E3B4() == 0) {
        script = D_8009B290;
        D_8009B290 = script + 2;
        value = script[0] | (script[1] << 8);
        D_8009B2A4 |= 0x4000;
        func_8003B6AC(0, 2);
        box = TextBox_Create(0, value & 0xFFF, 0x10, 0xB0, 0x120, 0x30);
        DuelEffect_MarkObjectIfActive(box);
        *(u16 *)(box + 0x34) |= 8;
        if ((value & 0x8000) != 0) {
            flags = D_8009B27C;
            boxflags = *(volatile u16 *)(box + 0x34);
            D_8009B27C = flags | 0x4000;
            *(u16 *)(box + 0x34) = boxflags & 0xFFF7;
        }
        D_8009B28C = D_8009B27C;
    } else {
        if ((D_8009B2A4 & 0x4000) == 0) {
            if ((D_8009B27C & 0x4000) == 0) {
                TextBox_Destroy(D_800EB0F8);
            }
            D_8009B28C = 0;
            D_8009B27C = 0;
        }
    }
}
```

Three levers, all measured against this body:

The two-argument call is real, and the shared constant is what shows it.
`addiu $a1, $zero, 0x2` is emitted once and used twice — as the increment in
`D_8009B290 = script + 2` and as the second argument to `func_8003B6AC(0, 2)`.
Reading the second argument off the pointer arithmetic like that is what fixes
it at 2; a one-argument call leaves `$a1` unexplained. The two ways of spelling
the increment do not matter: `D_8009B290 += 2` and
`D_8009B290 = script + 2` compile byte-identically.

The flag pair must be read before either is written. Written as the obvious
`D_8009B27C |= 0x4000; *(u16 *)(box + 0x34) &= 0xFFF7;` GCC does them in
sequence and pays a load-delay nop on each; hoisting both reads into locals lets
it interleave `lhu`/`lhu`/`ori`/`andi`/`sh`/`sh` as the target does.

Only the *reload* of the box field is `volatile`. Without it GCC forwards the
value it stored for `|= 8` a few instructions earlier and the second `lhu`
disappears. Qualifying the other two accesses as well costs more than it buys:
the `|= 8` store then cannot be moved into the `beqz` delay slot, which is where
the target keeps it, and the body lands 10 positions out instead of one.

The function is not committed, because a body that does not assemble to the
target cannot be `matching_c` and `make match` would reject it. It is recorded
here so that it can be dropped in as-is the day MASPSX emits the nop.

### Register pins

Issue #5 accepts `register` variables pinned to a hard register for functions
that are otherwise unmatchable. Two things are worth knowing before reaching for
one.

#### Fixing allocation is a smaller claim than coercing a sequence

Distinguish the two cases when recording a pinned match, because they are not
equally strong:

- the instruction sequence is already exact and only the register assignment
  differs, so pins correct **allocation**;
- the sequence itself is wrong and pins are used to force it, which is a much
  larger intervention.

`func_80026C0C` (`0x80026C0C`) is the first kind: all 24 instructions were in
the right order before any pin, and only the base index and record pointer were
swapped between `v1` and `a0`. State which case applies in the ledger row, since
a reader deciding whether to revisit a function cannot tell them apart from the
pin count alone.

#### Pins cannot place a split-address high temporary

A pin also does not reach the `%hi` temporary that `-msplit-addresses` creates
for a scalar load. That temporary is a compiler-internal pseudo, not a C
variable, so no `register` declaration names it.

`Ai_GetHandSize` (`0x80070710`) is the worked example. Pinning only the array
base to `$2` corrects both wrong roles of its 10-instruction body at once:

```c
register s8 *data asm("$2");

data = gDuel_aOpponentData;
return data[gDuel_bOpponentID * 9];
```

The base moves to `v0`, the index accumulator to `v1`, and the opponent-ID
value is already `a0`. Nine of the ten instructions are then byte-exact. The
one that is left is the address temporary of the ID load:

```
target      lui $a0, %hi(gDuel_bOpponentID) ; lb $a0, %lo(...)($a0)
candidate   lui $v1, %hi(gDuel_bOpponentID) ; lb $a0, %lo(...)($v1)
```

Retail coalesces the high temporary with the load destination; GCC 2.8.1 hands
it the next free register instead. Three ways of reaching for it fail:

- pinning the loaded value to `$4` as well changes nothing, because the
  temporary is a different pseudo from the pinned variable;
- pinning `&gDuel_bOpponentID` to `$4` and loading through it changes nothing,
  because GCC folds the pointer back into the symbol reference;
- pinning the accumulator to `$3` alone, or all three roles at once, is
  strictly worse: the first loses the split address form and the second emits
  `addu $v0, $v1, $v0`.

The practical rule matches the one below: if the register you want to control
holds a `%hi` address temporary rather than a value the source names, a pin is
the wrong tool.

#### Pins cannot place a call's return value

A pin works for a value the function **computes**. It does not work for a value
the function **receives from a call**: the return register is fixed at `v0`, and
GCC 2.8.1 will not make a pinned variable the direct destination of the call
result. It copies through a scratch instead, emitting

```
move a2,v0
move a0,a2
```

where the unpinned form emits a single `move`. The pin therefore *adds* an
instruction rather than relabelling one. Splitting the declaration from the
assignment does not avoid it:

```c
register PoolEntry *entry asm("a0");
entry = func_8002C5CC();          /* still copies through a scratch */
```

`func_8002C604` (`0x8002C604`) is the worked example. Its remaining difference
is that retail keeps the returned pointer in `a0` while GCC uses `a1`; pinning
raises the instruction count from 34 to 35 instead of fixing it, so that
difference is not reachable by pinning at all.

The practical rule: if the register you want to control holds a call result, a
pin is the wrong tool and will cost an instruction.

### Fixed-size builtin copies

Exact `func_800476B4` confirms that GCC 2.8.1 expands an 8-byte
`__builtin_memcpy` between alignment-one pointers into the retail
`lwl`/`lwr` and `swl`/`swr` pairs. This is a useful source-level lever when a
manually written pair of 32-bit copies produces the right data movement but
the wrong unaligned load/store schedule.

### GTE instructions

Projection helpers use scratchpad address `0x1F8003E0`, load GTE data
registers 0 and 1, execute RTPS, and read data register 14. GNU `as` does not
accept the historical `rtps` spelling in this pipeline, so matching C uses the
validated inline word:

```c
__asm__ volatile(".word 0x4A180001");
```

`func_80015D18` is the current matching template for this pattern.

## What a masked comparison cannot see

`tools/project/overlay_diff.py` compares one compiled function against the
retail bytes in a fraction of a second, which has made it the default way to
check a candidate. It excludes relocated fields from the comparison, reading
the object's own relocation table so that only the affected bits are ignored.
That is what makes it robust against unlinked objects, and it has an exact
consequence worth stating outright:

**A wrong symbol with the right register allocation is invisible to it.** The
`%hi`/`%lo` immediates are precisely the bits it does not compare, so a
candidate that reaches the wrong address in the right shape reports a
byte-exact match.

`CampaignMap_PickExit` did this in both overworld modules. Two globals were
declared at addresses `0x10000` too high; every instruction had the right
opcode, registers and offsets, and the tool reported `MATCH` on both. Only the
full module rebuild caught it, in three bytes, all of them `lui` halves.

So: **`overlay_diff` proves the shape, `make match-overlays` proves the link.**
A fast, trusted tool with an unstated blind spot is more dangerous than a slow
one, and this is the blind spot.

### The address mistake behind it

The underlying error is a MIPS reading trap rather than a typing slip. A `lui`
loads the upper half, but the following `%lo` is **sign-extended**, so a
negative `%lo` resolves `0x10000` *below* the apparent base:

```
lui  $v0, 0x800a
lhu  $v0, -19560($v0)     # 0x800A0000 - 0x4C68 = 0x8009B398
```

Read off the `lui` at face value this looks like `0x800AB398`. It is
`0x8009B398`. Whenever the offset printed with the load is negative, the
symbol lives in the segment below the one the `lui` names.

## The sound-packet sibling group at `0x8004Axxx`

Three unmatched functions in this module build the same request in the state
block rooted at `D_8009B458` and hand it to `func_80077450`:

| function | size | shape |
| --- | --- | --- |
| `func_8004A27C` | 124 B | scales two `u16` fields, stores `0xF` tag |
| `func_8004A6F8` | 108 B | copies three `u16`s from a second argument |
| `func_8004A764` | 92 B | stores a constant tag and zeroed halfwords |

All three write `D_80011434[index]` to `state + 0x4C0`, a tag to `0x4C4`,
further halfwords in the `0x4C8`-`0x4FC` window, and then call
`func_80077450(state + 0x4C0)`. The `0x4C0`-`0x4FF` window sits inside the
`pad04C0[0x40]` hole of `SDSecondaryState` in `src/game/sound.h`.

That hole is a `Packet`, the 0x40-byte struct already declared in
`src/game/sound_secondary_playback.c` and `src/game/func_8004ACE4.c`:

```c
typedef struct {
    s32 image;
    s32 type;
    s16 x;
    s16 y;
    u8 pad_0C[52];
} Packet;
```

which names the group's stores: `0x4C0` is `image`, `0x4C4` is `type`, and
`0x4C8`/`0x4CA` are `x`/`y` — the two fields `func_8004A27C` scales and
stores. The `0x4E4`, `0x4FA` and `0x4FC` fields written by the other two fall
inside `pad_0C` and are still unnamed.

The matched callers of `func_80077450` in this module stage a `Packet` as a
**local** and pass `&packet`. This group instead writes the one that lives in
the state block. Same type, two staging strategies, and that distinction is
the real difference between the matched and unmatched members of the module.

**Profile.** The compiler side is G0 for this group. None of the three targets
contains a `%gp_rel` operand, and the `%gp_rel`-implies-G8 rule recorded above
holds across 261 cases with no counterexample. This matters because a
G8-compiler profile can reach the *exact instruction count* on
`func_8004A6F8` (27 of 27) by emitting `lw $a0, 0($gp)` where the target uses
an absolute `lui`/`lw` pair. That count is an artifact of the wrong codegen
family, not progress; the G0 line is one instruction longer and is the one to
work from.

### `D_8009B458` behaves as `u8 * volatile`

`func_8004A43C` settles a question the three functions above left open. It reads
`D_8009B458` three times in one body without caching the pointer, and a plain
`SDSecondaryState *` declaration will not do that — GCC loads it once and reuses
the register. Declaring it in the translation unit as

```c
extern u8 * volatile D_8009B458;
```

reproduces all three reads, and is the same modelling `func_80049138` already
uses for `g_SDValue` under `SDVALUE_CUSTOM_EXTERN`. The two globals are the same
kind of thing and should be declared the same way.

Re-scored with the volatile declaration, `func_8004A6F8`, `func_8004A27C` and
`func_8004A764` each move by at most one instruction — they read the pointer once,
so it does not change their residual. It is correctness rather than a lever for
those three, but it is what any new function in the group should start from.

### A shared residual worth recognising

`func_8004A764` and `func_8004A6F8` both plateau on exactly the same
difference, and `func_8004A27C` shows the same shape in its retail assembly:

```
target:     base in $v1; stores are 1216($v1), 1220($v1), ...
            addiu $a0, $v1, 0x4C0     # argument derived BEFORE the call
candidate:  base bound straight into $a0; stores are 1216($a0), ...
            addiu $a0, $a0, 0x4C0     # argument adjusted in the DELAY SLOT
```

The registers are swapped: retail keeps the base in a temporary and derives
the argument from it, while GCC binds the base directly into the argument
register and fixes it up last.

Five independent source-level levers were measured against `func_8004A764`,
and every one produced the identical diff count or a worse one:

- the global used inline versus bound to a local
- the call argument given its own local, computed early
- the same local, computed immediately before the call
- splitting the table address from the load (`&D_80011434[i]`, then `*entry`)
- typing the base as a struct and passing `&state->field_04C0`

A sixth is worth stating separately, because it was the strongest available
hypothesis. Binding a `Packet *` at the packet address and storing through it
should collapse to exactly the retail shape: GCC folds the member stores back
to state-relative addressing (`1216($v1)`, `1220($v1)`) and derives the
argument with `addiu $a0, $v1, 0x4C0`. It does fold it that way — and still
allocates the base into the argument register. Reproducing retail's own
abstraction does not move the allocation either.

Two further negative results from `func_8002C604`, which shows the same
residual outside this module: hoisting a global load by binding it to a local
changed nothing, and pinning the pointer to the register retail uses made it
*worse* by two instructions. Both the mnemonic form `asm("a0")` and the
numeric form `asm("$4")` used elsewhere in this codebase were tried, with
identical results. A register pin does not repair an allocation mismatch; it
constrains the allocator and the cost reappears elsewhere.

Because two siblings plateau identically and a third has the same retail
shape, this is a property of the calling pattern — passing `base + constant`
while also storing through `base` — rather than a quirk of one function.
Renaming, resequencing and retyping the pointer expressions all canonicalise
to the same RTL, so further permutations of that kind are low-yield. Anyone
picking up this group should start from the G0 profile line and treat the
five levers above as already settled.

### A third instance, in a loop rather than a call

`func_80025028` reaches the exact instruction count with the correct shape,
the confirmed profile (`gcc_2_8_1_o1_g8`, G8 by four `%gp_rel` operands) and
the shared `DuelCardRecord` type, and then stops on the same residual.

Retail materialises both array bases before the loop and indexes off them:

```
lui/addiu $t0 = &D_800907D8
lui/addiu $a3 = &D_801A7AD8
```

The candidate materialises only one and routes the other through the
assembler temporary inside the loop (`lui $at` / `addu $at,$at,$v0` /
`lbu $v1,0($at)`). Four lever families were measured against it, all at the
correct count of 40:

| lever | result |
| --- | --- |
| hoist both bases into locals | 41 instructions, worse |
| hoist only the `$at`-routed base | 41 instructions, worse |
| inline the offset local | inert, identical score |
| reorder the leading zero store | inert |

Naming *either* base costs an instruction, because the local forces a
materialisation retail gets for free; not naming it costs the register. The
loop shape needs no experiment — retail recomputes the index in the branch
delay slot (`bnez $v0, L ; addu $v0,$a1,$a2`), which is what the obvious
source already emits.

So the residual now appears in three unrelated places: a packet pointer
passed to a call, an object pointer returned from an allocator, and a
loop-invariant array base. It is not a property of any one calling pattern.
The practical rule is that once shape, count, profile and types are settled
and the only difference is which register holds a long-lived address, further
source permutation is low-yield — record what was measured and move on.

## Value-level levers cannot move address CSE

Where retail re-materialises an address that GCC keeps live in a register,
the natural instinct is to reach for `volatile`, or to shorten the lifetime
of the loaded value. Neither works, and it is worth recording the measurements
so the same three attempts are not repeated.

`func_80012DB4` sits one instruction short for exactly this reason. It reads
`D_8009AFA4` and later writes it, and retail forms the address twice:

```
lui  $v1, %hi(D_8009AFA4)      # read
lbu  $v1, %lo(D_8009AFA4)($v1)
...
lui  $at, %hi(D_8009AFA4)      # write, address formed again
sb   $zero, %lo(D_8009AFA4)($at)
```

The candidate forms it once and reuses the register for the write, losing
that second `lui`. Three levers were measured against it:

| lever | result |
| --- | --- |
| `volatile` on the store target | one diff worse, no extra `lui` |
| `volatile` on the shared symbol | byte-identical |
| read into a local to shorten the value's live range | byte-identical |

All three act on the **value** — through volatile semantics, or through when
the loaded byte is consumed. None of them touches the address computation, so
none produces the re-materialisation. This is the same distinction already
recorded for volatile and CSE: constraining when a load happens says nothing
about whether the compiler keeps its base register live.

The pattern generalises across unrelated functions: a packet pointer passed to
a call, an object pointer returned from an allocator, a loop-invariant array
base, and now a re-formed store address. Once shape, count, profile and types
are settled and the only remaining difference is which register holds an
address, source permutation is low-yield. Record the measurements and move on.

## Mixed %gp_rel and absolute addressing is a declaration signal

When retail reaches some globals through `%gp_rel($gp)` and others with an
absolute `lui`/`%lo` pair *in the same function*, that split is evidence
about the original declarations rather than noise. A global reached
absolutely was not a small-data object, and modelling it as a plain scalar
costs an instruction at every access.

`func_80012DB4` reaches twelve globals gp-relatively and three absolutely:

```
lbu  $v1, %gp_rel(D_8009B0C0)($gp)     # one instruction
lui  $v1, %hi(D_8009AFA4)              # two instructions
lbu  $v1, %lo(D_8009AFA4)($v1)
```

Declared as `extern u8 D_8009AFA4;` under `-G8`, the compiler places the
byte in small data and emits the one-instruction form, so the candidate came
out three instructions short across the three affected accesses.

Declaring the same symbols as **unsized extern arrays** and subscripting them
restores the absolute form:

```c
extern u8 D_8009AFA4[];
...
if (D_8009AFA4[0] == 0) { ... }
```

An unsized array has unknown size, so it cannot be placed in the small-data
section and must be reached with an absolute pair. That recovered exactly the
three missing instructions.

So before writing the externs for a function, count the two addressing forms
in the target. The ratio tells you which globals to declare as size-unknown.

### The `-G` threshold is a real tuning axis

Small-data placement depends on the `-G` value the *assembler* receives, and
the profile set carries `-G1`, `-G2` and `-G4` variants as well as the
familiar `-G0` and `-G8`. On this function the best profile turned out to be
an assembler `-G4` one, sitting between the extremes.

Those intermediate profiles are easy to dismiss as padding in the profile
list. They are not: they select exactly which globals fall under the
small-data threshold, which is precisely the distinction the addressing split
above is made of.

## A fast probe harness must copy the real flags

Iterating on a candidate with a small local script - compile, run maspsx,
assemble, disassemble, diff against the generated assembly - is much faster
than a full `make match`. It is also easy to build one that is quietly wrong,
because it fails by flattering the result rather than by erroring.

Three faults found in one such harness while working `func_80012DB4`, all of
which made the candidate look closer than it was:

**The assembler needs `-G`.** `build_baseline.py` passes `-G{data_limit}`
from the profile. A harness that omits it inherits the binutils default of
`-G8`, so every profile gets assembled as though the assembler were G8. On a
`cc_g8_as_g0` profile that turned a true 53-instruction result into an
apparent 38, and made a profile that was never competitive look like the
leader. `data_limit` tracks the **assembler** side, which the cross profiles
confirm: `cc_g8_as_g0` has `data_limit` 0 and `cc_g0_as_g8` has 8.

This also explains where `%gp_rel` comes from. GCC does not emit it. GCC
emits a bare symbol reference plus a size directive:

```
lhu  $2, D_8009B098
.extern D_8009B098, 2
```

and the assembler performs the small-data conversion when it is given `-G8`.
Checking the pre-assembler output for `%gp_rel` therefore always reports
zero and proves nothing; the evidence is gp-relative addressing off `$28` in
the disassembled object.

**Counting instructions with `grep -c '^ '` counts labels.** Internal `.L`
labels are indented like instructions. On a function with three of them a
42-instruction target reads as 45, which inflated an apparent shortfall from
four instructions to seven and produced a wrong "whole statements are
missing" diagnosis. Match on a leading mnemonic instead, and print the
harness's own target count rather than hardcoding one - a hardcoded literal
agrees with the wrong number instead of exposing it.

**A matched function cannot be used as a regression test.** The obvious way
to validate a harness is to run it against something already matched and
expect a clean result. That cannot work here: once a function becomes
matching C, splat stops emitting generated assembly for it, so the comparison
runs against an empty target and reports success-shaped nonsense. Validate
the flags against `build_baseline.py` directly instead.

The common thread is that all three faults were silent and optimistic. A
harness that errors is harmless; one that quietly scores a wrong shape as
close costs cycles in the direction of false confidence.

## Choosing the right instrument to verify a candidate

Diff count is the cheap instrument and it is the one that misleads. Three
separate verification lessons from working `func_80025028`, each of which
cost a cycle.

**The link validates size; the diff does not.** A candidate at 39 of 40
instructions scored 16 diffs and looked close. Staged as matching C, the
build stops immediately:

```
mipsel-none-elf-ld: resident text size mismatch
```

`make match` checks the function's byte size at link time, so a wrong
instruction count fails loudly instead of scoring well. When the question is
"is the count right", the real build is the sharper instrument, and it is
worth reaching for before spending cycles tuning registers against a shape
that cannot fit.

That also settles a hypothesis worth recording as refuted: the missing
instruction was *not* an artifact of a local probe harness invoking maspsx
differently. The real pipeline produced the same short function.

**Staging is a two-part change.** Adding an entry to `matching_c.json` alone
fails with:

```
error: matching function 0x80025028 is not marked matching_c
```

The `status` column in `functions.csv` must move from `unmatched_asm` to
`matching_c` as well. Both edits are needed before `make match` will exercise
the candidate at all.

**A bucketed sweep can hide the profile the rules require.** Summarising a
profile sweep by instruction count and printing only the best-scoring profile
per bucket concealed that `gcc_2_8_1_o1_g8` reaches the exact count of 40; a
2.7.2 profile occupied the same bucket with a better diff score. That led to
a cycle spent on a 2.8.1 line one instruction short, plus a wrong conclusion
that the exact count was reachable only from the 2.7.2 cohort. Since the
project rule is 2.8.1 first, a sweep summary should surface the best 2.8.1
profile per bucket, not only the global best.

The general form: an aggregate that hides candidates is as dangerous as a
metric that ranks them wrongly, and both are cheaper to fix than the cycles
they cost.

## Instructions the compiler folds away

A candidate that is *short* by a few instructions is usually read as a missing
statement. It can instead be a statement the compiler folded, and a ceiling
division is the case most likely to disappear.

`func_80047788` computes the number of `0x800`-byte blocks needed to hold a
record count, and the retail code spends seven instructions on it:

```
sll   $t0, $v0, 3          # x * 8, materialised and kept live
addiu $v0, $t0, 2047       # + (2048 - 1)
bgez  $v0, .L
nop
addiu $v0, $t0, 4094       # negative-operand bias
sra   $t0, $v0, 11         # / 2048
addiu $t0, $t0, 1
```

Writing that arithmetic directly does not reproduce it. GCC 2.8.1 strength-
reduces `(x * 8 + 2047) / 2048` to `(x + 255) / 256`, emitting `sra ,8` and
costing three instructions. The blunter `(x * 8) / 2048` folds all the way to a
plain `/ 256` and costs three more. Both folds are arithmetically correct, so
nothing looks wrong; the candidate is simply six or seven instructions short
with no obvious hole.

The fix is to materialise the multiply into its own local first:

```c
total = *(u16 *)(g_SDValue + 2) * 8;
step  = (total + 2047) / 2048 + 1;
```

GCC 2.8.1 folds at expression level, so once the multiply is bound to a
variable the division sees a plain register and the reduction cannot fire.
That is exactly why the retail code keeps `x * 8` live in `$t0` across both
bias arms — the materialised temporary *is* the evidence that the original
source named it.

**Generalisation, and it cuts both ways.** A named local is usually treated as
a register-allocation hint, but it also decides what the folder is allowed to
see. Elsewhere in this file, naming a value wrongly *cached* something the
retail code recomputed; here, failing to name one let the compiler *fold*
something the retail code materialised. The question to ask of the target is
"does it materialise this subexpression?", and the answer determines the local
in both directions.

**A corollary for instruction counts.** While the fold was still firing, the
`no_sched1` profile reached 54 of 55 instructions — one short, and by far the
best count at the time — while still emitting the folded `sra ,8` in both bias
arms. The count came from unfilled delay slots, not from correct structure, and
following it would have meant tuning scheduling around a wrong divide. Count is
only meaningful once the shape of each arithmetic idiom has been checked; this
is the same padding trap recorded above for `no_sched2`.

## Confirmed and strongly supported layouts

### Transform and card data

| Base | Evidence-backed layout |
|---|---|
| `D_800F2848` | Signed 16-bit transform angles/parameters at `+0`, `+2`, and `+4`; object is larger than eight bytes |
| `D_801D4244` | 32-bit card/property table indexed by signed 16-bit ID minus one |
| `D_800908A0` | Array of signed 16-bit coordinate pairs |
| `D_801A7AD8` | `0x1C`-byte entries: pointer/value at `+0`, signed ID at `+0xC`, unsigned flags at `+0x16` |

Observed `D_801D4244` property fields include:

```text
value >> 18 & 0x0F
value >> 22 & 0x0F
value >> 26 & 0x1F
```

### Object and event state rooted through `D_8009B458`

`D_8009B458` and `D_8009B45C` are global pointers using absolute
`%hi`/`%lo` relocation pairs.

Strongly repeated layouts include:

- Event queue records are `0x30` bytes. Their type byte is at offset zero.
- A resource/object array at root `+0x180` uses `0x28`-byte entries.
  Repeated fields occur at `+0x183`, `+0x18D`, and `+0x19E` relative to the
  root.
- A transfer state at root `+0x4A4` has a signed 16-bit ID at `+0`, source
  pointer at `+4`, lengths at `+0x10/+0x14`, byte fields at `+0x18/+0x1B`,
  and accumulated transfer count at root `+0x818`.
- State cleanup around `func_80049010` clears sequence flags and counters near
  root `+0x1578` through `+0x1588`.

These offsets are supported by multiple callers and matching neighbors, but a
single final aggregate type has not yet been committed.

### Large `0xE20` records

Functions around `0x80058xxx-0x80059xxx` repeatedly use records with stride
`0xE20`.

Observed fields and subregions include:

- An alignment-one eight-byte block at `+0xDC8`.
- Per-slot transform records of `0x50` bytes beginning at `+0xD14`.
- Signed clamp/state fields at `+0xDA0`, `+0xDA4`, `+0xDA8`, and `+0xE11`.
- A relocation/pointer area near `+0x1E0`.
- A count byte at `+0xE1B`.

`func_80059000` and `func_8005C6A0` are matching anchors for portions of this
layout. The full record remains only partially typed.

### `0x70`-byte allocator objects

Analysis around `func_800400AC` supports:

- Object stride `0x70`.
- Linked-list heads at `D_800EFE38` and `D_800F2878`.
- Used flag `0x80`.
- Default type value 2.
- A type lookup through `D_8009AF74`.
- A pointer into the tail-data region.

The allocator itself is still deferred, so these fields are provisional.
Exact `Text_StartCampaignDuel` independently confirms that nearby stream handlers use
absolute G0 references for `D_8009B360` through `D_8009B374`.

### Two-slot object controller

Exact `func_8003D614` confirms a controller with:

- Object pointers at `+0` and `+4`.
- Type/index field at `+0x1A`.
- `0x64`-byte records rooted at `D_800EB0F8`; exact `func_8003F2B0`
  independently confirms the stride through a raw `u8 *` view and explicit
  multiplication by 100 rather than through the typed struct.
- Signed 16-bit motion fields.

### Transfer and animation anchors

Exact `File_InitTransferDescriptor` confirms the `0x48`-byte transfer
descriptor constructor used by `func_80013940`. Public arguments five through
eight arrive on the stack, and the vertical/configuration argument selects
direct, negative, or fixed-point initialization behavior.

Exact `func_80019BD0` confirms animation-object fields including:

- Flags at `+8`.
- Current and mode bytes at `+0x21/+0x22`.
- Callback pointer at `+0x24`.
- Threshold halfword at `+0x2E`.
- Object index at `+0x6A`.
- State field at `+0x6C`.

Exact `func_8001B7AC` confirms that `D_800EA030` uses `0x0C`-byte entries
selected by a signed byte.

### Coordinate and constructor anchors

Exact `func_8005A1F4` and `func_8005A2E0` confirm two unsigned-halfword
coordinate triplets in `D_800F56F0`, at offsets `0/4/8` and `0xC/0x10/0x14`.
Both functions copy an alignment-one eight-byte vector packet, accumulate
three signed square terms, and call `func_80086E50`.

Exact `func_800610E0` and `func_800611D0` reinforce the `0x70`-byte allocator
object model through two related parent/child constructors.

## Matching anchors from the deep wave

| Function | Reusable evidence |
|---:|---|
| `File_InitTransferDescriptor` | Transfer-descriptor constructor and stack argument order |
| `func_8001306C` | Nullable callback array, GP-relative callback, pacing counters, and 60-tick countdown |
| `func_8001EE44` | Signed card ID indexing and conditional 4-bit property extraction |
| `func_80021480` | Ten-child object iteration and bit `0x40` state updates |
| `func_80019BD0` | Animation object field widths and callback layout |
| `func_8001B7AC` | `0x0C`-byte global entry selection and child linkage |
| `func_80028310` | G8 state transition with child creation and cleanup |
| `func_8002ABB4` | `0x70`-byte object clone/initialization wrapper |
| `func_8002DF2C` | Three archive layouts selected by high byte; packed decimal index calculation |
| `func_8002E060` | Object creation wrapper with signed mode byte |
| `func_8002EB78` | G8 stream state with split absolute `0x4C`-byte table entries |
| `func_80030D5C` | G8 state machine mixing GP-relative state and absolute flag word |
| `func_800375A4` | Signed countdown state and object cleanup |
| `func_80037A58` | Signed duration, randomized coordinate snapshot, and restoration |
| `Text_StartCampaignDuel` | Four direct byte-stream reads with absolute G0 globals |
| `func_8003D614` | Two-slot controller and `0x64`-byte object records |
| `func_80043230` | G0 pointer-rooted queue/object state |
| `func_80044DC0` | Signed 16-bit argument, four-byte stack packet, and byte-order selection |
| `func_80049010` | Shared sequence-state cleanup |
| `func_800497E0` | Transfer ID validation, clamped read length, and accumulated byte count |
| `func_80049CF8` | `0x28`-byte object loop and split table-base relocation |
| `func_8004C77C` | `0x2C`-byte record initialization and variable-length decoding |
| `func_80058A7C` | Mixed signed comparisons and unsigned halfword bit extraction |
| `func_80059000` | `0xE20` record stride and unaligned eight-byte copy |
| `func_80059CE4` | G8-gated teardown while preserving an absolute state pointer |
| `func_8005A1F4` / `func_8005A2E0` | Coordinate triplets, packed vector copy, and signed square accumulation |
| `func_8005C6A0` | Relocation enumeration and translated handler count |
| `func_8005F828` | Nullable eight-byte record iteration and repeated throttle-byte loads |
| `func_800610E0` / `func_800611D0` | Related constructors using `0x70`-byte allocator objects |

## Deferred-function guidance

The following evidence narrowed future analysis at this snapshot. Deferred
functions remain terminal under the six-attempt policy unless genuinely new
evidence produces a separately recorded exact result.

- At this snapshot, `func_8003D334` had exact size and relocations but still
  differed in scratchpad-packet initialization and register scheduling. A
  later collaborator-derived C body reproduced all `0x138` bytes and
  relocations under `gcc_2_8_1_g8`.
- At this snapshot, `func_8003A990` was four bytes short and differed mainly
  in allocation plus one reload around signed division by `0x400`. A later
  post-terminal pass preserved the signed division expressions and store
  ordering, matching all `0x154` bytes under `gcc_2_8_1_g8_split`.
- At this snapshot, `func_8003AAE4` matched its initialization/call prefix but
  still differed in phase-register and color-replication ordering. The same
  collaborator body later matched all `0x164` bytes and relocations under
  `gcc_2_8_1_g0`.
- Future untouched handlers in the module surrounding exact
  `Text_StartCampaignDuel` should start from G0 direct byte-stream reads rather than
  wrapper helpers.
- Future untouched allocator/list users in the module surrounding the now-exact
  `func_800400AC` should begin with its confirmed `0x70`-byte slot layout and
  `gcc_2_8_1_g8_split` profile.
- Four-state callbacks in the `0x8003Bxxx` module use nested branch trees,
  while five- and six-state callbacks use explicit jump tables. Absolute
  destination tables in those callbacks require G0; G8 produces truncated
  `R_MIPS_GPREL16` relocations.

If a new exact neighbor, original type declaration, or compiler artifact later
changes one of these conclusions, record that evidence before revisiting any
terminal function.
