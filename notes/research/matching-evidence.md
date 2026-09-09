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

##### `volatile` also pins the *order* of a run of global stores

The section above is about re-issuing a load. There is a second, unrelated
effect that is worth more when a whole block of initialisation comes out
scrambled: GCC treats distinct extern globals as non-aliasing, so a run of
stores to different symbols is freely reorderable and the emitted order has no
relation to the source order. Declaring them `volatile` makes the order the
source's.

`Main_Init` (`0x80012B50`, 0x184) is the worked example. Its six recorded
attempts all fail at `+0x50`, the head of a block of twelve stores to
`D_8009B098`, `D_8009B09C`, `D_8009B0C0`, `D_8009B0C1`, `D_8009B0C3`,
`D_8009B0C4`, `D_8009B0C8`, `D_8009B0CC`, `D_8009B0D1`, `D_8009B0D8`,
`D_8009B230` and the pointer `D_8009B0B4`. Written plain, the block comes out
in an order that matches neither the source nor the target, and rewriting the
source order does not move it — the scheduler is choosing. Declared `volatile`,
emitted order equals source order and the block can be transcribed straight off
the target's disassembly.

The same declaration recovers a second thing here. The target zeroes
`D_8009B09C` and immediately re-reads it:

```
sw $zero, %gp_rel(D_8009B09C)($gp)
lw $a1,   %gp_rel(D_8009B09C)($gp)
```

Plain, GCC forwards the stored zero and the function is one instruction short;
`volatile` restores the load. But the read and its consumer are thirteen
instructions apart in the target — the `sw $a1, %gp_rel(D_8009B0C4)($gp)` is
the last store of the block — and `volatile` forbids moving either, so
`D_8009B0C4 = D_8009B09C;` as a single statement cannot produce it. It has to
be a local assigned right after the zeroing and consumed at the end.

Together these take `Main_Init` from a body that diverges at `+0x50` to 97 of
97 instructions with the whole block ordered correctly; what remains is a
two-instruction address materialisation scheduled at the other end of the block,
which is the ordinary schedule-tie-break residual.

This block is the same one `func_80012DB4` needs `volatile` on, so the
declaration is a property of these globals rather than a per-function trick.

The diagnostic is worth stating on its own: **a run of stores to distinct
globals emerging in an order that is neither the source's nor the target's is
not a source-order problem.** Reordering the statements will not fix it. Ask
whether the globals are volatile first.

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

### Compiler-generated jump tables need explicit `.rodata` ownership

Dense `switch` statements that GCC lowers to jump tables are now supported.
The table must be treated like any other compiler-owned section: carve its
retail byte range out of the extracted read-only-data blob and give the
matching C source a dotted `.rodata` subsegment at that exact address in
`config/slus_01411/split.yaml`.

`Ai_GetWinningCardRange` (`0x80070738`) and `Ai_GetCardRange`
(`0x800707C4`) are the resident worked examples. Their tables occupy
`0x8001194C` and `0x8001196C`; the tracked split places each source object's
`.rodata` there, with a pad between them, while the generated configuration
places their `.text` in executable order. Both functions and their compiler
tables therefore link from the same object without duplicating the retail
bytes.

The exact linker script is now generated by Splat at
`tmp/splat/slus_01411.ld` from the tracked split layout. The build supplements
it with `config/slus_01411/c_symbols.ld`,
`config/slus_01411/link_symbols.ld`, and Splat's automatic undefined symbol
scripts. Do not edit generated linker output or add one-off linker rules for a
function.

The practical workflow is:

1. Check generated target assembly for a `jtbl_` reference or a load followed
   by `jr` on a register other than `$ra`.
2. Confirm that the candidate emits the same table size and entries.
3. Split the retail blob at the table boundaries and assign a dotted
   `.rodata` entry to that source in `split.yaml`.
4. Run the clean full-executable match; text, table bytes, relocations, and
   placement all remain part of the acceptance gate.

The former failure mode was an orphan compiler `.rodata` section landing after
`.text`, overlapping text padding and initialized data. That diagnosed missing
section ownership, not an inherent inability to integrate jump tables. The
old snapshot counts for functions blocked by this limitation are historical
and must not be used as the current remaining-work inventory.

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

##### Correction: this is not a maspsx bug, and it does not block anything

Everything measured above is accurate; the diagnosis drawn from it is not.
maspsx was compared against ASPSX on this input and the two agree, so skipping
the `nop` before a bare-symbol store is faithful emulation rather than a defect.
Patching maspsx to insert it is therefore wrong, and the "14 functions,
28,748 bytes" figure is not a blocked-work inventory.

What the measurement actually shows is that **the input assembly is wrong, not
the assembler**. The assembler only resolves a small global gp-relative when the
translation unit *defines* it. Everything under `src/` declares its globals
`extern`, so the store stays a bare-symbol macro, ASPSX and maspsx alike leave
the delay unfilled, and the function comes out one instruction short. The retail
unit defined the global, which is why retail has the `nop`.

Two things follow, and both are reproducible on `func_80025028`:

- Declaring the global in the unit (`u8 D_8009B1B8;` instead of `extern`) makes
  GCC emit `.comm D_8009B1B8,1`, which lands in `sbss_entries`, so `_uses_gp`
  returns true and the `nop` appears with an unpatched maspsx.
- By default maspsx then emits that symbol as a *local* `.sbss` label with
  `.space`, which allocates storage and relocates against the section, so it
  will not link at its retail address. `--use-comm-section` makes it emit a real
  `.comm` instead: the relocation is `R_MIPS_GPREL16` against the symbol, the
  symbol is `*COM*`, and `c_symbols.ld` overrides it. The linked image has no
  `.sbss`, `nm` reports `8009b1b8 A D_8009B1B8`, and the executable hash is
  unchanged.

`gcc_2_8_1_g8_split_comm` is that profile. `func_80025028` matches 40 of 40
through it, so the pattern is not a blocker at all — it is a signal that the
function's unit owns one of the globals it touches.

#### MASPSX does not fill reorder-mode delay slots

> **Corrected: this is not a tooling blocker.** ASPSX pads a reorder-mode
> branch exactly as MASPSX does; the two were compared on this input and agree.
> The observations below are accurate and worth keeping as a description of the
> mechanism, but the conclusion drawn from them was wrong, and the functions
> listed as blocked by it are not blocked. See
> *Correction: the reorder-mode slot is a source-shape problem* at the end of
> this section for what actually reaches it, and `func_80012DB4` for a function
> that was attributed to this and then matched without any tooling change.

A third build-tooling blocker, alongside the jump tables and the load-delay nop,
and it is invisible from the C.

GCC splits its branches between two modes. Where its own delay-slot pass fills
a slot, it brackets the pair in `.set noreorder` / `.set nomacro`; MASPSX tracks
those directives and passes such pairs through exactly. Every other branch is
left in `.set reorder`, with the next instruction simply following it, for the
assembler to schedule. MASPSX does not schedule: in a reorder region it appends

```
nop  # DEBUG: branch/jump
```

after the branch and leaves the instruction behind it. The slot is empty for
good, because MASPSX also emits a file-wide `.set noreorder` and gas will not
revisit it. On one 95-instruction function GCC bracketed nine filled slots and
left twenty to the assembler; all twenty came out as nops.

That is only wrong where the retail assembler filled one, and it did.

##### The worked example

`func_80043BCC` (`0x80043BCC`, 0x17C). GCC emits, in a reorder region:

```
beq  $2,$0,$L5
la   $4,D_800EB0F8
jal  TextBox_Destroy
sb   $18,D_8009B428
```

The retail assembler fills both slots, splitting the `la` macro across the
branch to do it:

```
beqz  $v0, .L80043BF0
 lui   $a0, %hi(D_800EB0F8)
jal   TextBox_Destroy
 addiu $a0, $a0, %lo(D_800EB0F8)
```

MASPSX produces a `nop` in each slot and the body comes out two instructions
long. Nothing in the C reaches this: the source is already the shape that makes
GCC emit those four lines.

`-msplit-addresses` looks like an escape, because it turns the address into a
real `lui` that GCC will schedule into the slot itself. It is not, at least not
here: the same flag splits the three `D_8009B142/143/144 = 0xFF` stores, which
the target has as assembler macro expansions through `$at`
(`lui $at, %hi(...)` / `sb $s1, %lo(...)($at)`). Split, they come out through a
general register, and the third — now a single instruction — is pulled into the
following `j` delay slot where the target has a nop. The body needs macro form
for the stores and split form for the one address at the same time, so the two
profiles land two long and two short respectively.

##### How much this blocks

Screening the generated assembly for the specific shape MASPSX cannot make — a
delay-slot `lui $r, %hi(S)`, a branch or `jal`, then a delay-slot
`%lo(S)` through the same register, which is a macro expanded across a branch —
gives **7 unmatched functions, 13,188 bytes**, and **nothing matched**, the same
signature the other two blockers have. Zero matched is the point: a body that
cannot reproduce the pattern cannot have been accepted.

```python
# delay slots are the lines splat indents by one extra space
DELAY = re.compile(r'\*/\s{3}(\S+)\s+(.*)')
PLAIN = re.compile(r'\*/\s{2}(\S+)\s+(.*)')
```

| function | size |
|---|---:|
| `AiScript_CalcCardPower` | 0x18C |
| `func_80012E5C` | 0x210 |
| `func_80031084` | 0x2C8 |
| `func_8003A560` | 0x3C0 |
| `func_80043BCC` | 0x17C |
| `func_8001BD88` | 0x14B8 |
| `func_8004EB00` | 0x132C |

The count is a floor for the same reason the others are: it only sees functions
still in assembly. It is also narrower than the true exposure, because it
matches only the macro-split-across-a-branch case; a reorder-mode slot filled
with any ordinary single instruction is equally unreachable and much harder to
screen for.

Unlike the jump tables this is a bug rather than a configuration decision, and
unlike the load-delay nop it is not a question of what MASPSX can see — the
information is all present. Filling a reorder-mode delay slot is work MASPSX
currently declines to do.

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

##### Correction: the reorder-mode slot is a source-shape problem

The framing above treats the slot as something the assembler owes us. It is not:
ASPSX pads a reorder-mode branch just as MASPSX does. Where retail has a filled
slot, **GCC filled it**, bracketing the pair in `.set noreorder` itself. So the
question is never "why did the assembler not fill this", it is "why did GCC
hand the assembler a reorder-mode branch here when retail's GCC did not".

`func_80012DB4` is the worked example, and it was filed as a MASPSX defect
before being matched with no tooling change at all. Two source facts moved it:

- **Writing the `D_8009AFA3` selection as a ternary** rather than an
  `if`/`else` makes GCC fill the `beq` slot itself with `li $v0, 1` and emit
  `.set noreorder`. Same instructions either way; only the mode changes.
- **Dropping `volatile` from `D_8009AFA3`** removes a scheduling barrier that
  was stranding the `VSync` argument above the stores, so reorg can take
  `move $a0, $zero` into the call delay slot instead of padding it.

With both, plus `D_8009B0C1` defined rather than declared for the small-data
`nop`, the function matches 42 of 42.

The general rule: an unfilled slot in the build is evidence about the *shape*
of the C, usually a barrier in the wrong place or a branch GCC could not fill
because the only candidate computed the branch operand. Reach for the source
before reaching for the assembler.

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

##### Correction: no maspsx change is needed, and the link error is the clue

The failure recorded above is real but it was read as an obstacle to a maspsx
fix, when it is the evidence that no maspsx fix is wanted. Marking *every*
small `.extern` gp-relative is wrong precisely because a declared size is not
evidence of reachability from `$gp` — as the three truncated relocations show.

Marking the symbols a unit *defines* is a different claim, and a true one. Define
the global in the unit and build with `--use-comm-section`
(`gcc_2_8_1_g8_split_comm`): it enters `sbss_entries`, so the hazard check fires
and the nop appears, the rewrite converts only that symbol, and the emitted
`.comm` stays a COMMON that `c_symbols.ld` overrides — no storage, no section,
and the address still fixed absolutely. `func_80025028` matches 40 of 40 this
way against an unpatched, pinned maspsx.

The `runtime_gp` truncation cannot recur under this route, because a unit only
defines the globals it owns and those are by construction the ones the retail
unit reached through `$gp`. If a defined symbol ever does fall outside ±32 KB of
`runtime_gp`, the link fails loudly rather than silently mis-addressing, which is
the safe direction.

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

### Diagnosing when this lever applies, and when its signature lies

The route above is powerful and its surface signature is misleading, so it is
worth saying what actually indicates it. `func_8004E7B0` was filed as a maspsx
defect - a missing post-`mfhi` `nop` before a `-G8` store - and matched with no
tooling change once its nine globals were defined rather than declared. That is
the second function to reach this conclusion after `func_80025028`, and the
report should never have been written, because both this section and
`tools/maspsx_bugs/README.md` already recorded the finding and the fix.

The tempting generalisation is that a candidate declaring small globals
`extern`, especially one whose residual sits near a `%gp_rel` store or a `nop`,
is a candidate for this lever. **That is not the test, and two measurements say
so.**

| candidate | surface signature | result of defining the globals with `--use-comm-section` |
| --- | --- | --- |
| `func_8002EE94` | four plain small externs, residual around a `%gp_rel` store | inert: 13 differing, 349 of 359 aligned, imbalance unchanged |
| `func_80044838` | nine plain small externs, `%gp_rel` store / `j` / `nop` tails | inert: 283 differing, nine instructions short, on both profiles |

Both have the signature and neither moves at all. The lever fixes *addressing* -
whether the assembler can prove a store is one gp-relative instruction - so the
test is whether the candidate's gp-relative accesses actually differ from the
target's. If they already agree, the declaration is not the problem, whatever
sits next to them. `func_8002EE94`'s obstacle is store forwarding and
`func_80044838` is nine instructions short of a structural match; neither is an
addressing fault.

So: diagnose from the differing instructions, not from the declarations. A
count of `extern` lines is not evidence, and a `nop` near a gp-relative store is
where this lever *shows up* rather than where it is *indicated*.


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

## Reading loop shape, and what retail's registers do not tell you

Two independent results from `func_8003B5C8`, a leaf that searches a table
for each 16-bit value in a 20-byte buffer.

**Removing a rotation fixup.** A search loop written as `do { ... } while
(*p != 0)` with a `break` on the match produces a compensating pair:

```
addiu $a0, $a0, 4      # advance
...
addiu $a0, $a0, -4     # and undo it after the exit
```

Rewriting the exit test does not remove it — a `for (;;)` with an explicit
bottom `break` is byte-identical. What removes it is putting the advance in
the loop's third clause and testing one element **ahead**:

```c
for (;; e += 4, idx++) {
    if (*(u16 *)e == v) { out = idx; break; }
    if (*(s32 *)(e + 4) == 0) break;
}
```

The advance then becomes the back edge and nothing is rewound. That recovered
the exact instruction count as well as six diffs.

**Register count is not variable count.** Retail here keeps a pointer and a
second counter, initialised with `addiu $t0, $zero, 1` and stepped with
`addiu $t0, $t0, 2` — which reads exactly like a source-level `k = 1; ...
k += 2;` used as `buf[k]`. It is not. Writing that second index costs two
instructions; plain `buf[i + 1]` matches. Both extra registers are induction
variables GCC derived from one index loop.

The same trap appears in the other direction with array bases, so state it
generally: an extra register in the target usually means the compiler
strength-reduced an index, not that the programmer declared another variable.

**Loop fixes do not transfer between loops.** In this one function the outer
loop needed an integer index — retail compares with a signed `slt`, and
pointer comparisons are unsigned — while the inner loop needed a real
pointer walk. Applying the outer loop's fix to the inner one was the worst
result measured:

| inner-loop form | result |
| --- | --- |
| `do/while` with bottom test | baseline |
| `for(;;)` with explicit break | identical |
| fixed base + integer offset | 5 instructions over |
| `for(;; advance)` testing `e + 4` | exact count, best |

Read each loop's own evidence rather than reusing what worked elsewhere in
the same function.

## Statement order is a lever at call boundaries, not within a block

On a function containing calls, the highest-yield source change is usually
where a value is evaluated *relative to a call*. Two changes on
`func_8004A43C` account for most of its progress, and neither touched types,
names or profiles:

- Reloading the state pointer **after** `func_80049FB4` rather than holding it
  live across the call. Holding it live forced a second callee-saved register
  and grew the frame from `-24` to `-32`; retail reloads it. Worth 15 diffs.
- Computing the shift argument **before** the two packet stores, so it lands
  ahead of the call rather than sinking after them. Worth 9 diffs, and it
  restored the exact instruction count at the same time.

Both are visible by reading the target's ordering directly, and the frame size
gives the first away immediately: **if the candidate's frame is larger than
retail's, something is being held across a call that retail re-derives.**
That check costs one glance at the prologue.

The limit is worth stating, because the rule generalises badly. Moving
statements relative to each other *within* a straight-line block does not
work. On the same function, hoisting an array load into a local before a
neighbouring pointer load — purely intra-block — came out eight diffs worse
and one instruction short, because the scheduler simply reordered it back and
spent an instruction doing so.

So the mechanism is not "source order controls emission order". It is that
evaluation position relative to a call determines **what must stay live across
the call**, which drives register allocation and frame layout. Within a basic
block the scheduler owns the ordering and source position carries no
information.

### Corroboration on `func_80023144`: intra-block moves are *exactly* inert

The paragraph above rests on a count ("eight diffs worse"). `func_80023144`
supplies the stronger form of the same claim, measured at the residual, which
is worth having because a count alone cannot distinguish "no effect" from
"two effects that cancel".

That function's five remaining positions are two independent clusters, both of
them one global's address: a `sched2` swap of `%lo` against an unrelated
increment, and a three-slot rotation where retail leaves a branch delay slot
empty. Six source placements were tried across the two blocks - reading a
value before the update, sinking the update below its neighbour, spelling
`x += 4` as `x = x + 4`, and materialising the address before the guard,
after the guard's operand read, and comma-sequenced with it.

All six produce the *same differing set*, position for position. Not the same
count with a shuffled residual: identical. The only placements that changed
anything were the two that moved the statement **across a basic-block
boundary** - past the join of an `if` (+8) and out of the block entirely (+6) -
and both were worse.

So the rule is sharper than "intra-block source order is a weak lever". Within
a block it is not a lever at all, and in particular it cannot be combined with
another lever in the hope of partial credit, because it contributes nothing to
combine. For address materialisation specifically, the address is emitted at
block entry no matter where in the block it is written.

#### Correction from `func_80028B08`: the test is dependence, not position

The paragraph above originally ended with a CFG-shaped rule - that a reordering
which does not cross a basic-block boundary cannot change the output and need
not be compiled. That is wrong as stated, and following it costs real
positions.

On `func_80028B08`, moving one store up by a single statement, so that an
assignment to the object's `field_44` falls between the mask and the store of
its `field_4` rather than after both, is worth 6 positions. It crosses no block
boundary.

The difference is what the scheduler is permitted to do, not where the
statement sits. Everything measured on `func_80023144` was *independent*
instructions - an address materialisation and an unrelated increment - which
`sched2` may freely reorder, so source order carries no information and is
exactly inert. Stores and loads that the compiler cannot prove disjoint are a
different case: source order **is** dependence order there, and no later pass
has licence to undo it.

So the usable form is: **intra-block source order is inert for instructions the
scheduler may reorder, and load-bearing for memory operations it may not.**
Before skipping a reordering experiment, ask whether it changes the relative
order of two memory operations that might alias. If it does, compile it; if it
only permutes independent computation, the earlier result stands and it can be
skipped.

## Do not name an array base to reproduce a materialised base register

When retail keeps an array base in a register and the candidate reaches the
same array through the assembler temporary, the obvious response is to bind
the base to a local. It reliably makes things worse, and the intuition behind
it survives longer than it should because the target appears to endorse it.

`func_8004A43C` is the clearest case, because retail visibly materialises the
base:

```
lui   $a1, %hi(D_80011434)
addiu $a1, $a1, %lo(D_80011434)
sll   $v1, $v1, 2
addu  $v1, $v1, $a1
lw    $v1, 0($v1)
```

while the candidate emits the assembler-temp form:

```
lui  $at, %hi(D_80011434)
addu $at, $at, $v1
lw   $v1, %lo(D_80011434)($at)
```

Writing `s32 *tbl = D_80011434;` and indexing `tbl[...]` moved the candidate
from one instruction over the target to **two** over, and cost a diff. The
same lever on `func_80025028` also went one instruction over. Two functions,
opposite-looking evidence in the disassembly, identical measured outcome.

The reason is that the two forms are not the same operation. Retail's base
register is a scheduling artifact of code the compiler generated; a named C
pointer is a value with a live range, which GCC keeps alive rather than
folding back into the addressing mode. Reproducing the artifact by
introducing the value does not work.

So: a materialised base register in the target is **not** evidence that the
original source named it. Treat the addressing form as an output of register
allocation, not as something the source chooses.

## Changing what a value is works; asking for the same value twice does not

Several notes here record that register-level differences resist source
changes. `func_80048F14` shows both sides of that boundary in one function,
which makes it a useful discriminator.

**Reachable — change the value class.** Retail materialises two constants
where the candidate materialised one:

```
addiu $v0, $zero, -1        # for four halfword stores
sh    $v0, 5496($v1) ... sh $v0, 5502($v1)
ori   $v0, $zero, 65535     # 0xFFFF materialised again, separately
sh    $v0, 0($a0)
```

GCC merges these, because `-1` and `0xFFFF` are identical once truncated to a
halfword. Writing the fifth store as `*(u16 *)addr = 0xFFFFU` instead of
`*(s16 *)addr = 0xFFFF` gives the compiler a genuinely different value to
materialise, and the second `ori` appears.

**Not reachable — ask for a redundant copy.** The same function ends one
instruction short on a plain register move:

```
sw   $a1, 5476($v1)
addu $a0, $a1, $zero        # copy the pointer, then store through the copy
sh   $v0, 0($a0)
```

Binding the pointer to a local used for both the field store and the
dereference is score-neutral: GCC folds the local away and reuses the one
register. There is no version of the source that asks for the *same* value in
a *second* register, because that is a register-allocation decision rather
than anything the source expresses.

So the test before spending a lever is whether the source change alters what
the value **is**. A different type, signedness or constant gives the compiler
new information; a rename, an extra local or an alias for an identical value
gives it none.

**Instruction count outranks the diff number.** The `u16` change traded one
extra diff for one extra instruction, moving the candidate from two short to
one short. That is the right trade: a candidate short of the target can never
match, so count is the binding constraint and the diff total is advisory.

## The additive CSE lever, and the exact form it has to take

Almost every lever in this note is subtractive: pin a register, disable a
pass, reorder to prevent a hoist, add a `volatile`. `func_8005C1F4` finished
on an additive one - **write a redundant expression so that the compiler's own
CSE produces retail's shape** - and it is worth stating as a rule because nine
spellings that tried to *suppress* CSE had failed on the same residual first.

It needs stating precisely, because the obvious phrasing collides with the
rule above and the obvious phrasing is wrong. "Give the same value a second
local" does **not** work. The measurements, on the matched function with only
this one spelling varied:

| spelling of the second pointer | instructions | differing |
| --- | --- | --- |
| `out = src;` - alias the existing local | 94 | 78 |
| `base = D_8009B498; ... out = base + 0x40000;` - hoist the global read, repeat only the arithmetic | 94 | 78 |
| `out = D_8009B498; out = out + 0x40000;` - repeat the read, split the statement | 97 | 28 |
| **`out = D_8009B498 + 0x40000;` - repeat the whole expression** | **96** | **0** |

So the section above is right that an alias for an identical value gives the
compiler nothing, and hoisting the global into a local is the same thing in
another costume: both lose two instructions and 78 positions. What works is
repeating the **entire expression, global read included**, exactly as written
the first time. Splitting that expression across two statements is a third
behaviour again.

The mechanism is why the distinction is so sharp. Repeating the expression
re-enters both the load and the address arithmetic into CSE's table. CSE then
collapses the redundant *load* into a register copy - which is where retail's
`move` comes from - while the arithmetic that depends on it is rebuilt from
that copy rather than shared with the first pointer. An alias never creates
the second load, so there is nothing for CSE to collapse and nothing to force
the rebuild; the compiler simply keeps one register, which is the outcome the
rule above describes.

**The test to apply.** When a residual looks like "retail recomputes or copies
something my build shares", do not reach for a way to stop the sharing. Write
the source expression a second time, verbatim, and let CSE collapse it. And
when checking whether it worked, vary only that spelling - the three near
misses above differ from the match by a hoisted local or a statement break,
and each produces a different function.

A matched sibling is where this shape came from rather than the diff.
`func_8005BE3C` assigns four separate locals the identical
`D_8009B498 + 0x40000`, which is not something anyone would write by hand and
is unreachable from "how do I stop GCC doing this". **A matched neighbour is a
source of shapes, and shapes are what a diff cannot show.**

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
now-retired legacy-compiler profile occupied the same bucket with a better diff score. That led to
a cycle spent on a 2.8.1 line one instruction short, plus a wrong conclusion
that the exact count was reachable only from the legacy cohort. Since the
project builds only with 2.8.1, a sweep summary should surface the best 2.8.1
profile per bucket, not only the global best.

The general form: an aggregate that hides candidates is as dangerous as a
metric that ranks them wrongly, and both are cheaper to fix than the cycles
they cost.

## Keep a segment offset out of the field offset

When retail addresses a far-away structure, it often holds the large segment
offset in a register across a loop and uses only the small field offset as a
load displacement:

```
lui   $s5, 1
ori   $s5, $s5, 32768      # $s5 = 0x18000, live for the whole loop
addu  $v0, $s3, $s5
lhu   $v1, 15300($v0)      # field offset stays in the displacement
```

Writing the two offsets together in one subscript hands the folder a single
large constant to combine:

```c
D_8015C424[i + 0x18000 + 0x3B70]     /* folds to one 0x1BB70 literal */
```

GCC then materialises a separate `lui`/`ori` pair per combined constant — two
pairs where retail has one. Binding the segment base outside the loop
reproduces retail's form:

```c
base   = D_8015C424 + 0x18000;
id     = *(u16 *)(base + i * 2 + 0x3BC4);
rec[4] = base[i + 0x3B70];
```

On `Duel_PopulateCombinedDeckData` (`0x80024824`) that removed three
instructions and three diffs, taking the
candidate from two over the target to one short.

**When to apply it.** This is the opposite of what several other functions
want, where naming or sharing an address consistently costs instructions. The
distinguishing evidence is in the target, not in a general rule: if retail
keeps the value **live in a register across the loop**, bind it; if retail
recomputes the address at each use, do not. A small displacement on the load
plus a separate base register is the signature of the first case.

The mechanism is worth remembering on its own. Several constant offsets in one
expression invite constant folding, and the folded literal is more expensive to
materialise than the base-plus-displacement form it replaced.

## Read signedness off the opcodes before guessing at the source

MIPS encodes signedness in the mnemonic, so the target states outright which C
types produced it. Two fixes on `func_80017F04` came from that alone, and
neither was visible in the diff count.

**An unsigned divide is not a pointer difference.** The target computes an
index with:

```
subu  $v0, $s1, base
srl   $v0, $v0, 2
multu $v0, 0x24924925     # magic multiplier for /7
mfhi  $a3
```

`0x24924925` with the preceding `srl 2` is a division by 28, the stride of
`DuelCardRecord`, so this looks exactly like `rec - array`. Writing that
pointer difference produces no `multu` and no `mfhi` at all — GCC emits a
shift-and-add sequence instead. The reason is in the opcodes: `srl` is a
*logical* shift and `multu` an *unsigned* multiply, while a pointer difference
has signed type `ptrdiff_t` and would compile to `sra` and `mult`. The source
computed an unsigned byte offset and divided by the stride explicitly:

```c
(u32)((u8 *)rec - (u8 *)base) / 0x1C
```

That took the candidate from six instructions over the target to exact count.

**A shared header may need a cast at the use site.** The same function loads a
card id with `lh`, but `DuelCardRecord` declares `u16 card_id`, so the
candidate emitted `lhu`. Casting at the use site — `(s16)rec->card_id` —
restores the signed load. The header is not wrong; other callers may want the
unsigned field. Any function whose target uses `lh` on that field needs the
cast.

**Check opcode presence, not the diff total.** Both of these were settled by
counting `multu`/`mfhi` and `lh`/`lhu` in the candidate. The diff total was
unchanged across the `lh` fix and identical across a *wrong* lever tried
earlier, so it could not have distinguished either case. When the question is
"which idiom is the compiler choosing", grep the opcode.

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
| `gDuel_adwCardStats` | 32-bit card/property table indexed by signed 16-bit ID minus one |
| `D_800908A0` | Array of signed 16-bit coordinate pairs |
| `D_801A7AD8` | `0x1C`-byte entries: pointer/value at `+0`, signed ID at `+0xC`, unsigned flags at `+0x16` |

Observed `gDuel_adwCardStats` property fields include:

```text
value >> 18 & 0x0F
value >> 22 & 0x0F
value >> 26 & 0x1F
```

### Object and event state rooted through `D_8009B458`

`D_8009B458` and `g_SDValue` are global pointers using absolute
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
three signed square terms, and call `SquareRoot0`.

Exact `func_800610E0` and `func_800611D0` reinforce the `0x70`-byte allocator
object model through two related parent/child constructors.

## Matching anchors from the deep wave

| Function | Reusable evidence |
|---:|---|
| `File_InitTransferDescriptor` | Transfer-descriptor constructor and stack argument order |
| `func_8001306C` | Nullable callback array, GP-relative callback, pacing counters, and 60-tick countdown |
| `Duel_CalcGuardianStarBonus` | Signed card ID indexing and conditional guardian-star extraction before the matchup bonus |
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
| `SD_StartSequenceTracks` | `0x2C`-byte record initialization and variable-length decoding |
| `func_80058A7C` | Mixed signed comparisons and unsigned halfword bit extraction |
| `func_80059000` | `0xE20` record stride and unaligned eight-byte copy |
| `func_80059CE4` | G8-gated teardown while preserving an absolute state pointer |
| `func_8005A1F4` / `func_8005A2E0` | Coordinate triplets, packed vector copy, and signed square accumulation |
| `func_8005C6A0` | Relocation enumeration and translated handler count |
| `func_8005F828` | Nullable eight-byte record iteration and repeated throttle-byte loads |
| `func_800610E0` / `func_800611D0` | Related constructors using `0x70`-byte allocator objects |

## Deferred-function guidance

The following evidence narrowed future analysis at this snapshot. Deferred
histories are starting measurements rather than an exploration limit; continue
with materially distinct hypotheses and preserve later candidates under
`tmp/` until an exact result can be recorded.

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

## Binding a masked value blocks the single-bit-to-shift fold

When a single-bit test feeds arithmetic, GCC 2.8.1 recognises the bit pattern
and collapses the whole expression into one shift. Written directly,

    off = ((u32)(id & 0x1F) << 1) + ((u32)((id & 0x100) != 0) << 6);

compiles `((x >> 8) & 1) << 6` down to `(x >> 2) & 0x40`, emitting a single
`srl`. Retail instead keeps the three-instruction form `andi`, `sltu`, `sll`.

Binding the masked value to its own local before the comparison blocks the
fold and restores the retail sequence:

    m   = id & 0x100;
    b   = m != 0;
    off = ((u32)(id & 0x1F) << 1) + (b << 6);

This was measured on `SD_SEPlay` (0x80048658): the direct form produced `srl`
1 / `sltu` 0 against the target, and the bound form produced `sltu` 1 / `srl`
0, with no other opcode counts disturbed.

This refines the earlier rule that binding a local blocks folding only when
the bound value is computed. Both `id & 0x100` and `(id & 0x100) != 0` are
computed, but only binding the *mask* helps. Binding the boolean alone leaves
the mask and the comparison adjacent, which is exactly the pattern the
single-bit peephole matches. Bind the operand the peephole needs to see, not
the result you want to keep.

The same reading applies in reverse: an unexpected `srl` where the target has
`andi`/`sltu` is evidence of a folded single-bit test, not of a genuine shift
in the original source.

`func_80048768` is the stricter version of the same mechanism. There, binding
the boolean still let combine prove the single-bit source, while spelling the
value as `(x & 0x100) >> 8 << 6` preserved the three-instruction count but
emitted `andi` / `sra` / `sll`. The exact source made zero a register operand
and kept the masked value separate:

```c
register const u32 zero asm("$0");

flag = id & 0x100;
flag = zero < flag;
off += flag << 6;
```

That produces retail's `andi` / `sltu $zero` / `sll` without a branch. The
lesson is to check both count and opcode kind: a three-instruction spelling can
still encode the wrong operation, and binding the mask is not sufficient when
combine can still see the literal zero comparison.

## A canonicalising diff harness can invent differences as well as hide them

While comparing `SD_SEPlay` (0x80048658) and `func_80047DB0`, both candidates
appeared to materialise 0xFFFF with `addiu` where the target used `ori`. The
apparent lesson was that the compared variable had to be widened. That
conclusion was wrong, and the mechanism is worth recording.

The target side of a comparison is Splat's assembly text, which prints real
mnemonics. The candidate side is `objdump` output, and objdump prints the
`li` *pseudo-instruction* rather than the encoding gas selected. A
canonicalising rule that rewrote `li rd, imm` to `addiu rd, zero, imm`
therefore mislabelled every unsigned 16-bit constant: gas assembles
`li rd, 0xFFFF` to `ori`, because the `addiu` immediate would sign-extend to
-1. The candidate had been emitting the correct instruction all along.

The general rule when normalising two instruction streams for comparison:

- Only canonicalise between forms that are genuinely encoding-identical.
  `move`/`addu` and `nop`/`sll zero,zero,0` qualify. `li` does not, because it
  expands to `addiu`, `ori`, or `lui`+`ori` depending on the constant.
- Normalise both sides from the same representation where possible. Comparing
  assembler text against disassembler text mixes two different renderings of
  the same encoding.
- A canonicalisation that fires on one side only is a bug. `li` never appears
  in the Splat text, so the rule could only ever rewrite the candidate.

Earlier instrument failures in this project flattered the candidate by hiding
real differences. This one did the opposite, and cost two functions' worth of
type changes chasing a difference that did not exist. Both directions come
from the same cause: a metric that was not validated against a case with a
known answer.

## Rank compiler profiles by opcode histogram, not by positional diff count

A line-by-line diff against the target is a positional measure. One extra or
missing instruction early in a function shifts every later line, so the count
keeps rising even as the body converges. That makes it unfit for choosing
between profiles, because profiles differ precisely in where they insert and
schedule instructions.

Measured on `func_80047DB0`: ranked by positional diff, `gcc_2_8_1_g0_split`
looked clearly best at 53 against `gcc_2_8_1_g0` at 66. Ranked by the
difference between the two opcode histograms, the order reverses, 12 against
6. The positional count was selecting the wrong profile outright, and the
work done under it was spent on differences that profile had introduced.

The histogram measure is a multiset comparison: count each mnemonic in the
target and in the candidate, and sum the absolute differences. It ignores
ordering and register allocation, so it answers the question that actually
matters when picking a profile - whether the candidate is emitting the right
*work* - and defers scheduling and allocation, which are separate levers.

Practical use:

- Choose the profile by histogram delta. Only then read the positional diff,
  to see ordering.
- A histogram delta of zero with a nonzero positional diff means the remaining
  problem is scheduling or register allocation, not the source.
- Watch the two totals. When the candidate total settles at the target's real
  instruction count, alignment padding has stopped inflating the comparison.

## A known-constant local can become a variable shift amount

If a local holds a compile-time constant and is live where an unrelated
shift by that same constant occurs, GCC 2.8.1 will use the register as the
shift amount rather than materialise the constant twice, emitting `sllv`
where the target has `sll`.

In `func_80047DB0` a bitmask local is initialised to 1 before a dispatch
block, and a `<< 1` inside that block became `sllv v0,v0,s4` because `s4`
already held 1. Moving the initialisation after the block, to just before the
loop that actually consumes it, restored the constant `sll`.

An unexplained `sllv` against a target's `sll` is therefore a liveness
signal: some constant-valued local reaches that shift and should be
initialised later. This is the mirror of the usual liveness question - here
the fix is to *shorten* a live range rather than extend one.

## Initialise before the branch rather than in an else clause

Where a value has a default and one branch overrides it, the two spellings

    if (cond) { x = f(); ... } else { x = id; }

and

    x = id;
    if (cond) { x = f(); ... }

are not equivalent to the register allocator. The second form makes the
default live before the branch, which is what produces a parameter copy in
the entry block. In `func_80047DB0` switching to the second form removed two
instruction-count differences and brought the candidate to within one
instruction of the target, because retail initialises the match value from
the argument before testing it and overwrites it only in the taken branch.

Read the entry block for this: copies of an argument made before the first
conditional branch indicate a default assignment that precedes the branch in
the source.

## The histogram metric must exclude alignment padding, and report both lengths

The opcode-histogram comparison recorded above is the right way to rank
profiles, but it has a failure mode of its own that has to be closed before
the numbers can be trusted.

An aligned diff prints a placeholder on whichever side is short. If the
histogram is built by tokenising each column and counting the first word, the
placeholder is counted as though it were a mnemonic. The two totals then
agree even when the candidate is genuinely shorter, because the padding makes
up the difference exactly.

Measured on `func_8001944C`: the histogram reported 70 against 70, while the
diff header reported `target=70 candidate=68`. The candidate was two
instructions short and the metric said the lengths matched. Re-checking
`func_80047DB0` after the fix moved it from "one `addu` allocated
differently" to "one instruction missing", which is a different and more
tractable problem: a missing instruction means the source is not asking for
enough work, whereas a differently-allocated one means it is.

Two requirements follow:

- Exclude the placeholder token when counting. Only real mnemonics count.
- Always print both totals and flag them when they differ. A length mismatch
  outranks every per-opcode difference, because until the lengths agree the
  per-opcode counts are describing two different amounts of work.

The general point is the one already recorded for the `li` canonicalisation:
a comparison tool's own output format must not be allowed to enter the
measurement. Both bugs came from tokenising rendered text without first
removing what the renderer had added.
## Scheduling flags change instruction counts, not only instruction order

It is natural to treat `-fno-schedule-insns` and `-fno-schedule-insns2` as
ordering-only levers, to be tried once the candidate already has the right
number of instructions. That is wrong, and skipping them while a length
difference remains can leave the best profile unmeasured.

Measured on `Duel_GetBaseCardStat` (0x8002CBF4, 45 instructions). Candidate
lengths across the G0 GCC 2.8.1 profiles:

    gcc_2_8_1_g0_split_no_sched1     44
    gcc_2_8_1_g0_split               43
    gcc_2_8_1_g0_no_sched2_split     43
    gcc_2_8_1_o1_g0                  41
    gcc_2_8_1_g0                     40
    gcc_2_8_1_g0_no_split            40

`gcc_2_8_1_g0_split` and `gcc_2_8_1_g0_split_no_sched1` differ only by
`-fno-schedule-insns`, and they differ by one instruction in length. The
mechanism is that scheduling feeds back into addressing decisions and
delay-slot filling, both of which can add or remove an instruction rather
than merely move one.

So sweep the scheduling variants at the same time as the rest, and rank on
candidate length first. The practical rule that follows from this and from
the histogram note above:

1. Compare `target=` and `candidate=` lengths. A length difference outranks
   everything else.
2. Among profiles tied on length, rank by opcode histogram delta.
3. Only then look at the positional diff, which describes ordering.

The retired legacy compiler produced 38 here against a target of 45, which is the usual
signal that the cohort is wrong rather than that the source is wrong.

## A constant assigned before a single exit costs two instructions

Where a function clamps a value and returns it, these two spellings differ by
one instruction:

    if (total < 0)       return 0;
    if (total < 10000)   return total;
    total = 9999;
    return total;

    if (total < 0)       return 0;
    if (total >= 10000)  total = 9999;
    return total;

The first gives the compiler a `return` whose operand is a literal, so the
constant is materialised straight into the return register: one
`addiu v0,zero,9999`. The second assigns the *variable*, which lives in a
callee-saved register, and then falls through to a single `return` that
copies it out: `addiu s0,zero,9999` followed by `addu v0,s0,zero`.

That two-instruction pair is a readable signature. A constant materialised
into a non-return register and immediately copied to `v0` means the original
assigned a variable and fell through to one exit; it does not mean the
allocator made a poor choice. Conversely a bare `addiu v0,zero,K` before the
epilogue means the source returned the literal directly.

Measured on `Duel_GetBaseCardStat` (0x8002CBF4). Switching to the
single-exit form took the candidate from 44 instructions to the target's 45
and the opcode-histogram delta from 1 to 0, leaving only register allocation.

This is the same principle as the default-before-branch note above: what the
source names as a variable, rather than what it computes, is what decides
whether a value gets its own register and its own copy.

## Confirm addressing-form differences against the object

An aligned diff that renders `%lo(sym)` as a placeholder can make a plain
zero displacement and a `%lo` displacement look like different addressing
forms. While working the function above, the diff appeared to show
`lw v0,REL(v0)` against a target `lw v1,0(v0)`, which would have meant the
compiler was folding `%lo` into the load where retail materialised the base.

`objdump -dr` on the object showed the real instruction was `lw v0,0(v0)`
with no relocation at that offset - the same form as the target, differing
only in destination register. The apparent addressing difference did not
exist, and the actual residual was register allocation.

Before treating an addressing-form difference as real, read the object with
relocations shown. A normalised diff is for ranking, not for diagnosis.

## A multi-instruction constant materialised early is a source-level local

An earlier note records that binding a local blocks folding only when the
bound value is computed, because a bound literal is constant-propagated away.
That holds for the *value*, but not for where the value is built.

A constant that does not fit an immediate field needs `lui` plus `ori`.
Where that pair is emitted is decided by whether the constant is a named
local or an inline literal. Written inline, it is materialised at the point
of use. Bound to a local, it is materialised at the top of the function and
kept in a register until used.

Measured on `func_8005B4D8` (0x8005B4D8, 73 instructions). The target builds
`0xE1000200` and `0xE6000001` in its first five instructions, while their
uses are eleven and twenty instructions later. Writing them inline placed
both `lui`/`ori` pairs at the point of use; binding them to locals moved both
to the top and took the diff from 54 to 49, with no change in length or
opcode counts.

So an early `lui`/`ori` pair whose result is not consumed for many
instructions is evidence of a named constant in the original source, not of
aggressive hoisting by the scheduler. Small constants that fit an immediate
carry no such signal, since they are folded into the using instruction either
way.

## Read statement order off a no-scheduling profile

Scheduling profiles reorder instructions, which makes the target's order weak
evidence about the original source. Under `-fno-schedule-insns` the emitted
order tracks source order closely, so the target can be read as evidence of
the original statement order even when a scheduling profile is the one that
finally matches.

This is worth doing as a separate diagnostic pass. Compile the candidate
under the no-scheduling variant, line the two up, and read off the order of
the loads, the constant materialisations and the stores. Then apply what that
tells you and go back to ranking profiles normally.

Both findings above came from that pass on `func_8005B4D8`: the constant
hoisting was invisible under the scheduling profiles, which spread the
`lui`/`ori` pairs through the body, and only became legible once scheduling
was switched off on both sides.

## A switch and an if/else chain differ in branch polarity

A multi-way dispatch on one value can be written either way, and the two are
not interchangeable for matching purposes. The difference is visible without
any register analysis.

An `if`/`else if`/`else` chain tests each case and branches *away* on
failure, so each comparison emits a `bne` that skips over the case body,
with the bodies inline between the tests. A `switch` branches *into* the
matching case, so each comparison emits a `beq` to a block placed after all
the tests, and each block ends with a jump to the join point.

Measured on `func_80045334` (0x80045334, 70 instructions), dispatching on
`id & 0xF000` with two cases and a default. Written as an if/else chain the
candidate had `beq` 2 against 0, `bne` 0 against 2, `j` 3 against 2, `lui` 3
against 2 and `lw` 12 against 11. Rewriting it as a `switch` corrected all
five counts at once and took the opcode-histogram delta from 9 to 3.

So read the branch polarity before writing the dispatch. `beq` to a forward
block, with the blocks gathered after the comparisons and each ending in a
jump, means a `switch` even when there are only two cases and the values are
not contiguous. `bne` skipping over inline bodies means a chain.

The secondary counts move together with the polarity because the two shapes
also differ in how many join-point jumps they need, and in whether a value
loaded before the dispatch survives into each arm. That is why a single
structural change corrected the load counts as well.

## Re-sweep profiles after every structural source change

The best profile for a function is a property of the candidate source, not of
the function. Changing the shape of the source can change which profile is
closest, so a ranking taken before a structural edit is stale afterwards.

Measured on `func_80045334` (0x80045334, 70 instructions). An early sweep
ranked `gcc_2_8_1_g0` best, and it stayed the working profile across two
sessions at 71 instructions against a 70 instruction target. After the
dispatch was rewritten from an if/else chain to a `switch`, re-sweeping
showed `gcc_2_8_1_g0_no_sched1` producing exactly 70 with an
opcode-histogram delta of 2, while `gcc_2_8_1_g0` still produced 71. The
better profile had been available the whole time and was invisible because
the ranking predated the rewrite.

This compounds with the earlier finding that scheduling flags change
instruction counts. A structural edit changes what there is to schedule, so
the scheduling variants are exactly the ones whose ranking is least stable
across such an edit.

The rule: after any change to control flow, to the number of statements, or
to which values are named, re-run the sweep before drawing conclusions from
the diff. Ranking is cheap; a wrong profile silently caps how close the
candidate can get and makes the residual look like an unreachable
allocation problem when it is not.

A corollary for recorded results: a profile named in a saved note is only
valid for the source that was measured with it. When resuming a parked
function after editing it, re-establish the ranking first.

## Count the target's saved registers before blaming the allocator

The prologue states, exactly, how many values the original keeps live across
a call: one `sw` of a callee-saved register per value. Comparing that count
against the candidate's turns a vague allocation complaint into a countable
discrepancy.

Measured on `func_80047DB0` (0x80047DB0, 69 instructions). Retail sets up a
0x28 frame and saves `s0` through `s4`, five registers. The candidate set up
0x30 and saved `s0` through `s5`, six. The extra `sw`/`lw` pair was the whole
length difference, showing up in the histogram as `sw` 6 against 7 and `lw` 8
against 9.

Reading it that way also collapsed two apparently separate problems into one.
The target's entry has two register copies the candidate lacked, and the
candidate had one saved register too many. Both are the same fact: the target
keeps the incoming argument in a caller-saved register, which is why it
copies it at entry and why it never needs a sixth callee-saved register,
while the compiler parks that argument in the callee-saved bank even though
its live range ends before the first call.

So when a candidate is one or two instructions long and the extra
instructions are a `sw`/`lw` pair, do not look for a missing statement. Count
the saved registers on both sides, identify which value the compiler is
preserving that the original did not, and work on that value's live range.

A frame size that differs by exactly four bytes per extra saved register is
the confirming signal, since the saves and the frame move together.

## Two exit blocks returning the same value came from two spellings

When a function returns the same constant from several guards, the compiler
will tail-merge those exits into one block if the source spells them the same
way. So a target that has *two* separate exit blocks producing the same value
is evidence that the original did not spell them the same way.

Measured on `func_80045208` (0x80045208, 75 instructions), which returns 0
from three guards. Retail has two distinct failure paths: two of the guards
branch to a shared block that sets the return register to zero and falls into
the epilogue, while the third branches straight to the epilogue with the zero
already placed in its delay slot.

Writing all three guards as a plain `return 0;` let the compiler merge them
into a single block, and cost four opcode differences. Writing the two that
share as `goto fail;`, with a `fail:` label before a final `return 0;`, and
leaving the third as a direct `return 0;`, reproduced the asymmetry exactly
and took the opcode-histogram delta from 4 to 2 and the diff from 64 to 47.

The reasoning generalises. Count the distinct exit blocks in the target that
produce the same value. That count is a lower bound on the number of distinct
spellings in the original. One block means every guard used the same
statement; two means at least one guard reached the epilogue by a different
route, which in C is a `goto` to a shared label against a direct `return`.

Re-sweeping profiles after this change moved the best profile from
`gcc_2_8_1_g0` to `gcc_2_8_1_g0_no_sched2_split` and the diff from 47 to 33,
which is the rule from the preceding note paying off a second time.

## A standalone probe cannot measure small-data addressing

Functions that reference external data through `%gp_rel` cannot be measured
reliably by assembling one translation unit on its own, and the failure is
quiet: it shows up as a plausible-looking instruction-count shortfall rather
than as an error.

GCC does not emit the addressing itself. It emits `la` and `lb`-style macros
naming the symbol, and the assembler expands each one either to a single
gp-relative instruction or to a `lui` plus `%lo` pair. For a symbol the
assembler cannot see the definition of, that choice follows the assembler's
own `-G` flag rather than the section the symbol really lands in.

Measured on `func_800175A0` (0x800175A0, 76 instructions, a leaf):

    assembled with -G8    candidate 72
    assembled with -G0    candidate 82
    target                76, with five %gp_rel and nine lui

The target uses a *mix*, because some of the symbols it touches are in small
data and others are not. No single `-G` value can reproduce a mix, so both
measurements are wrong and the candidate sits between them. A six instruction
gap read as missing work in the source, and it was not.

Two practical consequences:

- Before treating a length difference as missing work, check whether the
  target mixes `%gp_rel` and `%hi`/`%lo` for *data*. If it does, the
  standalone count is not evidence either way, and the full build is the only
  authority.
- The existing rule that `%gp_rel` in the target implies a G8 profile is
  about the compiler side and still holds. This is a separate, assembler-side
  decision, and the two can disagree for one function.

The general shape of this is familiar from the earlier instrument failures
recorded above: a tool that silently substitutes its own default for
information it does not have, and reports the result with the same confidence
as a real measurement.

## Read where a loop reloads a global, not just how often

The existing rule about re-reading globals per use needs a loop-specific
form. Inside a loop the question is not only how many times the target loads
a global, but which uses each load serves, because one load can serve both
the loop body and the loop condition.

Measured on `func_8004B374` (0x8004B374, 74 instructions), which walks a
table whose base and length both live behind one global pointer.

Retail loads the pointer once before the loop and reloads it only at the
bottom. The pre-loop load serves the initial bounds test *and* the first
iteration's body; each bottom reload serves the loop condition *and* the next
iteration's body. Four `lui` in total.

Writing the body as

    do { p = GLOBAL + off; ... } while (i < *(s16 *)(GLOBAL + 0x510));

asks for the base again at the top of every iteration, costing an extra
`lui`/`lw` and two load-delay `nop`s. Writing it as

    p = GLOBAL;
    do { q = p + off; ... p = GLOBAL; } while (i < *(s16 *)(p + 0x510));

reproduces the target exactly. That took the candidate from 77 instructions
to the target's 74 and the diff from 73 to 26.

The reload placed at the *end* of the body is the tell. When a target reloads
a global just before the loop test rather than at the top of the body, the
original read it once per iteration for both purposes, and the loop condition
was written against the reloaded value rather than against the global.

## Under -G8 the declared size of a global picks its addressing form

Within one profile, a global's *declared type* decides whether its address is
built by the assembler into `$at` or by the compiler into an allocated
register. Getting the declaration wrong costs several instructions and shows
up as a shifted branch target, not as anything that points at the
declaration.

With `-G8` and `-msplit-addresses`:

- A scalar `extern` carrying `__attribute__((section(".data")))` stays a plain
  symbol reference. The assembler expands each access, so the address is
  rebuilt at every use and stores go through `$at`.
- An `extern` of incomplete array type has unknown size, so it cannot be small
  data. `-msplit-addresses` then splits its address at compile time into a
  compiler-allocated register, which the scheduler is free to hoist.

Both forms appear in the same function. In `func_8003B808` (0x8003B808) every
scalar global is rebuilt per access while `D_801AF000`, an array, gets its own
register:

    lui $v0, %hi(D_8009B0F4)      scalar, rebuilt each time
    lw  $v0, %lo(D_8009B0F4)($v0)
    lui $at, %hi(D_8009B0F4)      store goes through $at
    sw  $v0, %lo(D_8009B0F4)($at)

    lui   $v0, %hi(D_801AF000)    array, split into a real register
    addiu $v0, $v0, %lo(D_801AF000)

Declaring the scalars plainly under `-G0` instead lets GCC cache the address
in a register and reuse it, which builds five instructions short on
`func_8003B808` and six to eight on `func_8003BF00`. Both functions' canonical
campaigns recorded exactly that as a shifted dispatch branch target
(`+0x10: 64!=5c` and `+0xc: 68!=62`). The residuals were accurate; nothing in
them suggested the cause was a declaration.

The sibling functions `func_8003BA14` and `func_8003BD14` match under
`gcc_2_8_1_g0_no_split`, so profile inheritance from a neighbour is not safe
here either: the family splits on which members touch an array.

This is the compiler-side counterpart to the assembler-side `-G` problem
recorded above. There the mix of `%gp_rel` and `%hi`/`%lo` could not be
reproduced by any single `-G` value; here a single profile does reproduce a
mix, and the lever is the declaration rather than the flag.

## A two-instruction constant makes statement order observable

When two statements in the same basic block each need a constant, and one of
them costs a `lui`/`ori` pair while the other fits in a single `addiu`, the
expensive one is materialised first and source order decides the rest.

`func_8003B808` and `func_8003BF00` are switch statements whose arms all have
the same shape: assign a field, then mask a global. Most arms match with the
field assignment written first. Exactly one arm in each function does not:

    func_8003B808 case 3   field1C = 0x18000    lui + ori
    func_8003BF00 case 1   field1C = 0x43000    lui + ori

Those two need the mask statement written first. Every other arm uses a
constant that fits one instruction and is insensitive to the order.

The useful part is the diagnosis rather than the fix: an ordering difference
confined to one arm of an otherwise uniform switch is not arbitrary, and it is
worth checking the constant costs before treating it as noise or reaching for
a different profile.
## A residual at +0x0 is a frame size, so it names a missing local

The first instruction of a non-leaf function is `addiu $sp, $sp, -N`. A
recorded mismatch at offset `+0x0` therefore says nothing about the body: it
says the candidate declared different stack storage from the original, and the
difference is readable directly off the two constants.

`func_8005B64C` (0x8005B64C) deferred with:

    byte +0x0: e0!=e8; word +0x0: e0ffbd27!=e8ffbd27

`0xe8` is -0x18 and `0xe0` is -0x20, so the target's frame is eight bytes
larger. Eight bytes is one `RECT`, and the missing statement was a struct copy
by value into a local:

    RECT rect = *(RECT *)D_8009B058;
    LoadImage2(&rect, (u32 *)D_801DD000);

The give-away in the target is the `lwl`/`lwr` pair feeding a `swl`/`swr`
pair. That is how GCC copies a small aggregate whose alignment it cannot
prove, which happens when the source is reached through a `u8 []` symbol
rather than a typed one. Passing the global's address straight to `LoadImage2`
compiles and looks equivalent, but allocates no local and builds the smaller
frame.

Two things worth carrying:

- Read `+0x0` before reading anything else. It is the cheapest residual in the
  ledger to interpret, and it constrains the search to declarations rather
  than control flow.
- `lwl`/`lwr` into `swl`/`swr` over a fixed small size is a by-value aggregate
  copy, not clever pointer arithmetic. The size of the copy is the size of the
  local.

The same reasoning applies in reverse: a candidate whose frame is *larger*
than the target has a local the original did not need, usually because a value
was spilled that the original kept in a register or recomputed.

## A pointer cast writes the same bytes as a union but not the same schedule

Writing a wider view of two adjacent struct fields through a pointer cast and
writing it through a union member produce the identical store. They do not
produce the identical instruction order, because the cast constrains what the
scheduler is allowed to move across it.

`func_8002BD0C` (0x8002BD0C) stores one word over `field30` and `field32`,
which are `s16` elsewhere in the same function. Spelled as a cast:

    *(s32 *)&object->field30 = 0x26810;

the candidate held at six diffs out of 140, all inside that one arm. The
target completes the constant *after* an intervening global load:

    lui   $v1, 0x2                  begin 0x26810
    addiu $v0, $zero, 3
    sb    $v0, 0x46($s2)
    lui   $v0, %hi(D_8009B118)
    lw    $v0, %lo(D_8009B118)($v0)
    ori   $v1, $v1, 0x6810          finish 0x26810, filling the load delay
    sw    $v1, 0x30($s2)

The cast version emitted the same instructions with the load last, leaving the
load delay unfilled. Declaring the field as a union of a two-`s16` struct and
an `s32`, and writing `object->field30.w`, took it to zero:

    union {
        struct { s16 lo; s16 hi; } h;
        s32 w;
    } field30;

The reason is aliasing. A store through a cast pointer is not provably
confined to the object, so GCC will not schedule an unrelated load across it.
A union member is a typed access to a known field, and the load moves freely.

Practical form of this: when a residual is confined to one arm, the
instruction *set* already matches, and the difference is ordering around a
type-punned store, the punning construct is the suspect rather than the
statement order. Reordering the statements is the natural thing to try and it
does not help here; both orders were measured, and one was worse.

Note also that scheduling-variant profiles are not the answer to this class of
difference. `no_sched2`, `no_strength_reduce` and an as-G0 variant were all
measured against this function and left the count unchanged at six, because
the constraint came from the source, not the flags.

## Normalise objdump aliases before trusting an opcode histogram

The opcode histogram is the right instrument for ranking profiles, because it
ignores position and so does not cascade. It reports a false gap unless the two
sides are spelled the same way first.

Splat's generated listings print real mnemonics. `objdump` prints assembler
aliases for the same encodings, so a candidate that is byte-identical in those
positions still looks different:

    li   $v0, 1        is  addiu $v0, $zero, 1
    move $a2, $a3      is  addu  $a2, $a3, $zero
    nop                is  sll   $zero, $zero, 0

Measured on `func_80046294` (0x80046294, 151 instructions), the same candidate
and the same object:

    raw histogram distance        14
    alias-normalised distance      4

Ten of the fourteen were the alias spelling alone. Ranking four profiles on the
raw number also picked a different winner from ranking them on the normalised
one, so the error is not merely cosmetic.

The second half of this is the more useful half. For the same candidate:

    positional diff (DIFFS)      104
    normalised histogram           4

Both numbers are correct and they measure different things. The candidate has
151 of 151 instructions and four opcodes out of place; the other hundred
positions differ only in which register was chosen, and one early allocation
difference shifts everything after it. Reading 104 as "far away" is wrong, and
it is the reading that invites abandoning a nearly-correct structure to go
hunting for a different one.

Practical order: get the instruction *count* equal, then the normalised
histogram to single digits, and only then chase positions. A large positional
count on top of a small histogram distance is an allocation problem, not a
structural one.

## The target's saved-register set names the wrong declaration

`func_80046294` saves only `$ra`. An early candidate saved `$s0` as well, and
the reason was visible in what `$s0` held: `%hi(g_SDValue)`, cached once and
reused across the calls in the tail.

That followed from declaring the global as an array, `extern SDValue
*g_SDValue[]` (sound.h's `G_SDVALUE_AGGREGATE` arm), which is not small data,
so `-msplit-addresses` splits the
address into a register and common-subexpression elimination then keeps it
alive across calls in a callee-saved one. The target rebuilds `%hi` at each
use, which is the assembler macro form, which needs the scalar declaration
carrying `__attribute__((section(".data")))`.

The same repository declares this symbol both ways in different translation
units, so neither spelling is wrong in general. The saved-register set is what
distinguishes them, and it is cheap to read: count `sw $sN` in the target
prologue and compare.

## A temp-then-copy pair means the value was materialised before its store

Two stores into adjacent stack slots, immediately followed by two loads from
those slots and two stores eight bytes lower, is not clumsy scheduling. A
whole-structure assignment is one thing that produces it:

```
struct assignment          scalar stores
sw $4,48($sp)              sw $2,16($sp)
sw $2,52($sp)              sw $4,16($sp)
lw $5,48($sp)              sw $2,20($sp)
lw $6,52($sp)              sw $4,20($sp)
sw $5,16($sp)
sw $6,20($sp)
```

Measured on a pair of ten-line probes differing only in that one respect, the
struct form is **exactly four instructions longer** and the scalar form emits no
copy. So a target carrying those four extra instructions per element is telling
you the original wrote `a[i] = value;` with a structure-typed value, and a
reconstruction that assigns the fields individually will be four instructions
short per element with no diff that says why.

The reverse reading is just as useful: if a candidate is *long* by four per
element against a target that stores fields directly, the source has a struct
assignment the original did not.

**Correction, made while the entry was being written: this is sufficiency, not
necessity.** The probe shows a struct assignment *produces* the shape; it does
not show the shape *implies* one. Spilling produces the same six instructions -
a value computed into a frame slot and reloaded to store elsewhere is
indistinguishable from a structure copy at this level. In `func_80051350` the
destinations turn out to be indexed as `sp + i*4 + k` later on, so they are
two-element arrays rather than structures, and the function commits ten
callee-saved registers, which makes spilling the likelier cause.

So the reliable reading is the weaker one: **the two values were materialised
somewhere before reaching their destination.** Whether that is a structure
assignment, a spill, or a temporary the source names explicitly has to be
decided from the surrounding code. Counting the four extra instructions is
still worth doing - it says a plain field-by-field transcription will be short -
but it does not by itself name the construct.

## Byte arithmetic and typed indexing are not interchangeable

`base + i * RECORD_SIZE` on a `u8 *` and `&typed_array[i]` compute the same
address, and they do not reliably compile to the same instructions. Converting
raw `u8 *` record access to a typed pointer therefore has to be verified per
file, not assumed to be a pure readability change.

Measured while typing `D_801A7AD8`, a `0x1C`-byte duel card record reached
through raw casts in eleven files. One file converted cleanly and matched. Five
others, converted identically, did not:

- `func_8001825C.c` also passed `D_801A7AD8 + card[0x6A] * DUEL_CARD_RECORD_SIZE`
  to a callee. Once the array is typed, that expression scales **twice** - the
  multiply is still written and the pointer arithmetic scales again - which both
  changes the address and grows the text. Rewriting it as `&D_801A7AD8[i]` fixed
  the size and still produced different bytes, so the index form and the
  explicit multiply are not equivalent here either.
- `func_8001898C.c` failed the same way at a different address.

The trap is that the first symptom was a **link error about overlapping
sections**, not a hash mismatch: the text grew enough to collide with the next
section. A refactor that changes address arithmetic can fail before it ever
reaches the comparison that was supposed to catch it.

The rule: when retyping a raw record pointer, find **every** use of the base
symbol in the file, not only the accesses being converted. Any remaining
`base + i * SIZE` is now double-scaled, and any remaining assignment of the base
to a `u8 *` needs a cast. Convert one file, build, and only then continue.

### The safe subset: change the declaration, keep the arithmetic

The warning above is about changing how an address is *computed*. Changing only
how a symbol is *declared* is a different, much safer operation, and it is
enough to satisfy "define the type in one place".

Measured while unifying `D_800EB010`, three `0x4C`-byte records reached from six
files that declared the symbol six different ways: `u8 D_800EB010[16]` (a wrong
bound for a 228-byte object), `u8 D_800EB010[]`, `unsigned char D_800EB010[]`,
a file-local `struct Entry D_800EB010[3]`, a second file-local `struct Obj`
describing the *same* record through a different offset, and a file-local
`MenuRecord D_800EB010[]`. All six were replaced by one shared
`extern MenuRecord D_800EB010[];` and the executable still matched.

What was safe, all confirmed against the full-executable hash:

- **A complete array bound and an incomplete one are interchangeable.** `[3]`,
  `[16]` and `[]` all produce the same `lui`/`%lo` addressing. This does *not*
  contradict the scalar-versus-array finding recorded elsewhere: that one is
  about `extern u32 g;` versus `extern u32 g[];`, where the scalar form becomes
  a single gp-relative load under `-G8`. Array-to-array is free; array-to-scalar
  is not.
- **Changing the element type is free as long as every use is re-anchored.**
  Files that walked the table as bytes kept doing so; `e = D_800EB010;` simply
  became `e = (u8 *)D_800EB010;`, and `D_800EB010 + slot * 0x4C` became
  `(u8 *)D_800EB010 + slot * sizeof(MenuRecord)`. Casting back to `u8 *` before
  the arithmetic keeps the scale at one byte, which is exactly the double-scaling
  trap the previous section describes, avoided rather than risked.
- **A member-anchored cast keeps a divergent signedness without a union.**
  When one file reads a field at a different width or signedness from the
  rest, `*(s16 *)&record->card_id` reproduces the original `lh` while
  `record->card_id` on a `u16` member emits `lhu`. Measured on
  `Duel_CheckRitual`: the plain member read changed one byte at
  `0x8002C8AF` from `0x84` to `0x94`, which is exactly the `lh`/`lhu`
  opcode pair. The cast form still names the field, so it is far weaker
  than `*(s16 *)(base + 0x0C)` and it does not force every other consumer
  of the header onto a union member.

- **Replacing offset casts with named members is usually free, but not
  always, and the exception is aliasing.** `Model_UpdateViewMetrics` reaches
  a `GsRVIEW2` through `*(s32 *)(m + 0)` style casts on a `u8 *`. Retyping
  the pointer and reading `m->vpx` instead changed one byte at `0x80057F6E`,
  `0x69` to `0x6A`: one register number, with the executable size unchanged.
  The parameter type is not the cause -- keeping `u8 *` and casting to a
  local `GsRVIEW2 *` reproduces the same single-byte difference, so it is
  the member reads themselves. The function assigns `D_800F56F0 = *m`, a
  whole-struct store, and then reads the same fields; through a typed
  pointer those reads may alias the store and through a byte pointer the
  analysis differs, which moves register pressure. Where a function both
  stores a whole struct and reads its fields, expect the conversion to cost
  a build to check rather than being free by inspection.

- **When converting a record to a typed pointer, convert every access to it
  at once.** `func_800289BC` reaches a `0x40`-byte record through `u8 *e`
  and already casts four sub-rectangles to `RECT` before calling
  `LoadImage`. Rewriting only those four rectangles as `rect->w` while the
  neighbouring fields stayed as `*(u16 *)(e + 0x28)` moved a store: one byte
  at `0x80028A30`, the immediate `0x28` replaced by `0x33`, so the scheduler
  had reordered a store against a load it could no longer prove was on the
  same base. Converting *all* of the accesses, so every one goes through a
  single typed pointer, matched exactly. Half-converting a record is worse
  than not converting it, because two pointers into one object give the
  compiler less information than one, not more.
- **Watch the stride when the base becomes typed.** In the same file,
  leaving a stray `*(s16 *)(e + 0xC)` behind after `e` became a
  `DuelEffectResourceRecord *` silently scales that offset by `0x40`. It
  still compiles. Only the hash catches it.
- **Typing a pointer can also delete instructions, not just move them.**
  `func_8002FB78` reaches a `FileTransferDescriptor` entirely through offset
  casts, and every offset lands on a member the type in `ygo_types.h`
  already names. Converting it made the executable **eight bytes shorter**.
  The file holds its shape with a read-modify-write on the global
  `D_8009B0F4` around the stores through `p`; while `p` is a `u8 *` those
  stores might alias the global, and once it is typed they provably cannot,
  so the dance folds away. Left unconverted.

  `func_8003A01C` is the same descriptor, the same global, the same
  read-modify-write, and the same eight bytes -- measured 2026-09-09, also
  left unconverted. Its `mode` store is the one that moves. Retail issues
  `sw $2, 0x1C($16)` twice, once in each of the two switch arms that end the
  same way; once `p` is typed that store provably cannot alias `D_8009B0F4`,
  the two arms' tails merge, and it is issued once. 85 instructions become
  83. Every offset the function touches is already a named member, so this is
  not a gap in the type -- both functions are simply held in shape by the
  barrier.

  Both are callbacks `File_TryRequestAsyncTransfer` is handed. The whole
  family was measured on 2026-09-09 -- seven functions with the signature
  `(descriptor, mode)` -- and it splits five to two:

  | function | typed as `FileTransferDescriptor *` |
  | --- | --- |
  | `func_800289BC` | matches (once every access is converted) |
  | `func_80020BE4` | matches |
  | `func_8002F4C0` | one byte: the `0x100` store to +0x32 is scheduled later |
  | `func_800434F4` | one byte, same shape |
  | `func_8002FB78` | eight bytes shorter |
  | `func_8003A01C` | eight bytes shorter |
  | `func_80032184` | thirty-six bytes shorter |

  Nothing distinguishes the two that convert by inspection: they touch the
  same members, call the same helpers and do the same `D_8009B0F4`
  read-modify-write. `func_80020BE4` is the one with `do { } while (0);`
  blocks already pinning two of its store runs, which is a plausible reason
  and not a measured one. So convert one of these at a time and build; do not
  convert the family in a batch on the strength of a sibling.

### Screening rule for the three of these

The same cause runs through all of them, and it is the one the aliasing
section above states: a store through a cast byte pointer is not provably
confined, so GCC will not schedule across it. Giving the pointer a type
removes that barrier, and the generated code changes in whichever direction
the barrier was holding it:

| function | what changed |
| --- | --- |
| `Model_UpdateViewMetrics` | one register, `0x69` to `0x6A` |
| `func_800289BC` | one store reordered |
| `func_8002FB78` | two instructions deleted |
| `func_8003A01C` | two instructions deleted (same global, same descriptor) |
| `func_800580D4` | three instructions deleted (parameter only) |
| `func_80016784` | three loads floated across scratchpad stores |
| `func_8002A9C0` | two of fourteen reads floated; twelve free |

So before converting offset casts to members, look for the two shapes that
make the barrier load-bearing: a **whole-struct assignment** to or from the
same object, and a **read-modify-write on a global** interleaved with the
stores. Either one means the conversion costs a build to check rather than
being free by inspection. Neither means it will fail -- `func_800289BC` has
the first shape and matched once every access was converted -- only that it
must be measured.

- **The barrier can also be a scratchpad record the same function is
  filling, and then one field converts and the rest do not.**
  `func_80016784` builds two sprites in scratchpad through pointers of their
  own while reading its display object's fields between the stores. Typing
  the object floated all three reads of the colour word at `0x0C` across
  those stores; retail keeps every one of them where the source puts it. The
  parameter and the other ten offsets convert freely -- it is only the reads
  that sit *between* stores through the other pointer. The tell is a field
  read in the middle of a run of stores through a differently typed pointer.

  Try the member-anchored cast on such a read first, but do not count on it.
  `*(s32 *)&object->field_0C` restored the barrier in `func_80016784` and
  still named the field. It did **not** in `func_8002A9C0`, which fills four
  scratchpad `SVECTOR`s the same way: there `*(u16 *)&o->field_30.h.field_32`
  behaves exactly like the plain member read, because taking a member's
  address is still a struct reference. So the member-anchored cast fixes a
  signedness divergence reliably and a scheduling one only sometimes, and the
  fallback is a plain byte pointer held under its own name.

  Bisect rather than inspect. In `func_8002A9C0` twelve of the fourteen reads
  convert with no change and two do not, and the two are not the ones a
  reading of the function would pick: 0x32 and 0x3E fail while their
  immediate neighbours 0x30 and 0x3C are free. Converting a group at a time
  and rebuilding found them in five builds.

  Stores can diverge too, and there the tell is the value rather than the
  member. `func_80020F4C` stores -116 into `position.h.field_28`, and through
  the plain `u16` member that becomes `ori 0xff8c` where retail has
  `addiu -116`; `*(s16 *)&obj->position.h.field_28` restores it. Twelve lines
  down, a store of `0x198` to the *same member* goes through the plain member
  and matches. So the sign of the constant decides it, not the field, and no
  reading of the record would say which of the two sites to convert.

  When `make match` reports an offset, that beats bisecting. Subtract the
  function's base from the reported VRAM and disassemble the object:

      tools/toolchains/binutils-2.42/bin/mipsel-none-elf-objdump -d \
          tmp/splat/build/<path>.o

  The report's own "expected 0x24, got 0x34" is already the `addiu`/`ori`
  opcode byte in the case above. Bisecting is for when the build fails to
  link rather than mismatching, since then there is no offset to chase.

- **A typed local costs a callee-saved register unless every use goes through
  it.** Where the record's type cannot go on the parameter -- a callback
  whose table declares `void (*)(u8 *)` -- the record is taken through a
  local instead, and then the parameter and the local are two live names for
  one value. `func_8003A990` grew its frame by eight bytes and pushed `.text`
  past its segment that way; routing its two remaining `u8 *` calls through
  `(u8 *)r` as well left one name, one register, and it matched. This is not
  a general rescue: the nine short callbacks in
  `duel_effect_state_callbacks.c` still grow with every use routed through
  the local, and there the readable form is the inline
  `((DuelEffectChannel *)object)->state_51` instead. Measure both spellings;
  which one works is per function, and the failure mode is a link error --
  `section .initialized_data VMA ... overlaps section .text` -- not a hash
  mismatch.

- **The barrier can be a volatile pointer rather than a global, and then it
  is per-file rather than per-record.** `func_800580D4` writes one
  `GsCOORDUNIT` through a `u8 *` parameter while reading a second one through
  a volatile pointer at `D_800F2C40+0xD18`. Retail reloads that pointer for
  each of the three angles and eats a load-delay `nop` after every reload.
  Typing the parameter lets GCC prove the stores cannot alias the volatile
  load, so it hoists the first reload into an earlier delay slot and the
  function comes out three instructions -- twelve bytes -- short. The reads'
  own spelling makes no difference: byte-offset casts and `rec->rot.vx` both
  give the short version once the parameter is typed, and both give retail
  once it is not. The tell is a run of reloads with `nop`s rather than one
  hoisted load. Its *locals* convert freely, though -- the file's two private
  duplicate structs became one `GsCOORDUNIT` byte for byte -- so a file
  blocked at the parameter is still worth converting inside.
- **`sizeof(T)` may replace a literal stride** once the cast is in place.
- **But a proven-equal `sizeof` is not a licence to switch to typed indexing.**
  `model.h` asserts `sizeof(ModelSlot) == MODEL_SLOT_SIZE`, so
  `&D_800F2C40[index]` and `(u8 *)D_800F2C40 + index * MODEL_SLOT_SIZE`
  provably compute the same address. They still compile differently:
  replacing the second with the first in
  `Model_HasInsufficientBufferSpace` changed one byte at `0x8005A8C8` with
  the executable size unchanged. The rule is about the *form of the
  expression*, not about whether the arithmetic is right, and a static
  assertion proving the stride cannot rescue it.
- **Spelling a type through its alias is free.** `unsigned char` to `u8`,
  `short` to `s16`, `int` to `s32` are typedef identities in `src/types.h`, so
  reformatting a minified translation unit into the project's aliases cannot
  move a byte.
- **Correcting a wrong return type in a redundant prototype was free here.**
  `func_8002EB78` declared `void func_80039E9C(void)` while the definition
  returns `s8 *`. The call discards the result, and `$v0` was already live as
  scratch across it, so the corrected declaration produced identical text. This
  one is genuinely luck-dependent: a discarded return value in a different
  register-pressure context can change allocation, so it still needs a build.

The distinction worth carrying: retyping the *pointer you do arithmetic on* is
risky and must be verified per file; retyping the *declaration* while casting at
each existing use is close to free, and it retires the duplicate definitions
without touching a single address computation.

## Count one characteristic opcode before reading any diff

When a candidate is short by a lot, the positional diff is worthless: every
missing instruction shifts the rest, so the count measures the shift. A single
opcode count often names the cause outright.

`func_80045514`'s first draft was 386 instructions against 437 and reported 420
differing lines, which says nothing actionable. The opcode histogram said

    lui   target 52   draft 4

which is not a statement-level problem at all: it says the two pointer globals
were being addressed gp-relative under `-G8` where the target uses `%hi`/`%lo`.
One declaration change - `__attribute__((section(".data")))` on both externs -
closed 48 of the 51 missing instructions.

The general form: **pick an opcode that is characteristic of one decision and
compare its count.** `lui` for addressing mode, `jal` for whether calls got
merged, `addiu` for split addresses, and an instruction that can only appear
once per unmerged tail for cross-jumping. Each is a property of a declaration,
a flag, or a block structure rather than of any line, which is exactly what a
positional diff cannot show you.

The same entry is also a warning about the differing count as a ranking. Twice
on that one function it preferred the wrong reconstruction: it liked the
non-split profile that got the jump-table dispatch wrong, and it liked the form
that cross-jumped two calls the target keeps separate. Both times the multiset
and the aligned streams were right. **Prefer a metric that can distinguish
"missing" from "displaced" whenever the two candidates differ structurally.**

### A jump table's index subtract is decided by the lowest case value

If the smallest `case` is not zero, GCC subtracts it before indexing and sizes
the table from the range. `func_80045514`'s cases start at 17, so the build
emitted `addiu v1,v0,-17` and a 65-entry table where the target indexes the raw
byte against 82 entries and has no subtract.

Adding an explicit empty `case 0:` that breaks out of the switch moves `minval`
to zero and reproduces both the missing subtract and the table size. So a table
whose entry count exceeds the span of the interesting cases is evidence that the
original had a low case, not evidence of a compiler quirk.

### Tail label order in the source is block order in the output

When `goto` decides the edges, the compiler emits the labelled blocks in source
order. A function with several exits therefore has to have them transcribed in
the order the target's blocks appear.

`func_80045514` has three: clear neither, clear one byte, clear both. The target
stacks them so the clear-both block falls into the single-byte block, with the
other above them jumping forward to the epilogue. Writing that block last, after
the `return`, cost an extra `j` and a pointer reload because it then had to jump
backwards; writing it first, with an explicit `goto` from the end of the switch
to the clear-both label, reproduced the layout.

### Whether a pointer global is cached is decided per block, not per function

Both spellings can be correct in the same function, and the `lui`/`lw` count for
that symbol in a block is what decides it.

In `func_80045514`, one block keeps `g_SDValue` in a single register across five
field loads, which only happens if it is read into a local: as a global, each
store through the other pointer invalidates it and forces a reload. In the same
block `D_8009B460` is the opposite - the target loads it twice, once for the run
of stores and once more for the last one - so caching it for the whole block is
two instructions short, and caching it for the run while writing the final store
through the global reproduces both loads.

Count the loads of that symbol in the target's block and match the count; do not
pick a house style and apply it throughout.

## Read the jump count to place a shared tail

Two case arms ending in the same statements can be spelled with a shared label
and a `goto`, or by repeating the statements in each arm and letting
cross-jumping merge them. Both are correct C and they produce a different
block order, which the jump count exposes before anything else does.

In `func_80046294` the target has exactly two `j` instructions. A candidate
using a shared `decrement:` label had three, and the extra one was the second
arm jumping to a label that a third block had been laid out in front of:

    candidate    BODY_A -> j    BODY_B -> j    default    decrement
    target       BODY_A -> j    BODY_B         decrement  default

Writing the decrement into both arms instead of jumping to it lets GCC merge
the identical tails and place the merged copy immediately after the second arm,
so the second arm falls through and only the first jumps. That also moved the
default body after the merged tail, matching the target's layout.

Measured effect on this function: normalised histogram distance 4 to 2, and
the instruction count went from 149 to 151 against a target of 151 under
`gcc_2_8_1_g8_split`.

This is the same lever recorded above for assigning a value in each switch arm
rather than selecting an index, and it generalises: **the count of `j`
instructions is a statement about block placement, and block placement is a
statement about where the shared code was written.** It is worth reading before
register allocation, because it is a property of the source rather than the
allocator.

Not everything helped. Rewriting the loop's back edge as a `do { } while` in
the hope that GCC's loop-invariant pass would hoist the jump table base into a
register, as the target does, left the histogram distance unchanged at 2. The
hoist is still unexplained, and it is the remaining difference on this
function along with the choice of register holding the state pointer.

## Cross-jumping compares hard registers, so allocation is a structural lever

The rule above reads block placement from the source. `Duel_LoadPackageStage`
shows the other half: **whether two identical-looking tails merge at all is
decided after register allocation, on the hard registers.** Cross-jumping runs
late and compares instructions exactly, so two tails that differ only in which
register holds a pointer cannot merge.

There, three switch arms end in the same three instructions. The build gives
all three the same register and GCC folds them onto one label; retail holds the
pointer in `$v1` in one arm and `$v0` in the other two, so only two of them
merge. The instruction *order* already matched. The whole two-instruction
deficit came from one register choice.

This matters because it inverts the usual reading. A register difference is
normally a symptom to be fixed last, after the shape is right. When it gates a
merge it is the shape: it changes how many blocks exist. Before treating a
count deficit as missing code, check whether the target simply failed to merge
something the build merged, and read the registers at the merge point.

The practical test is cheap. Count one instruction that can only appear once
per unmerged tail - there, the `addiu` adding the buffer stride - in the target
and in the build. Two against one localises the whole difference immediately,
where a positional diff reports two hundred shifted lines.

The post-terminal resolution modeled that distinction directly. A block-local
image-tail pointer bound to `$v1` kept that tail separate from the `$v0` tails
for stages 0 and 10 and restored the missing `addiu` and `sw`. That one change
also moved the volatile flag value into `$v1`, so the exact source keeps
`flags` in `$v0`; spelling stage 10's first read-modify-write through that
named value then restores its original allocation and store schedule. This is
the useful order of work: identify the merge boundary and distinct lifetimes
first, then constrain only the two measured roles.

### Variable reuse steers registers only for expensive constants

A related lever, with a sharp boundary. Writing two values through **one
reassigned C variable** tends to give them one hard register, and that is
usable to free a register elsewhere: in the same function, making the
`0xFFDDFFFF` mask and a later `0x10000` the same variable moved `0x10000` into
`$a0`, exactly where the target has it. The exact source retains that shared
value and also names case 7's mask before the mode store, which restores the
target's independent constant-materialization order.

The same trick applied to `2` and `0x10` in the same function is completely
inert. Those are cheap immediates, so GCC propagates them back into their
stores and the shared variable never exists as a value. **Variable reuse
controls register sharing only for constants expensive enough to survive as a
value** - in practice the two-instruction `lui`/`ori` pairs. Do not spend
probes trying to steer single-instruction immediates this way.

## Correction: li cannot be normalised to a single opcode

The alias-normalisation rule recorded above is right in shape and wrong in one
entry. The pseudo-instruction li is not a spelling of addiu. The assembler
picks the encoding from the value: addiu for something that fits a signed
16-bit immediate, ori for an unsigned one such as 0x9000. Mapping li to addiu
unconditionally therefore invents a difference.

Caught on func_80057544 (0x80057544, 155 instructions), where the two
measurements disagreed:

    positional diff, comparing encodings    0
    normalised histogram                    2   addiu 32/33, ori 6/5

The candidate was byte-exact and the histogram was wrong. The target's
ori $v0, $zero, 0x9000 had been normalised into an addiu on the candidate side
only, because splat prints the real mnemonic and objdump prints li.

Two things follow.

The positional comparison works on instruction encodings and the histogram
works on printed names, so where they disagree the encoding comparison is the
authority. The histogram is the better instrument for ranking candidates,
because it does not cascade, but it is the weaker one for deciding whether a
candidate is finished.

More generally this is the same failure the alias note was written about,
committed one level deeper. A normalisation table is itself an assumption
about the data, and an entry that is right most of the time still fabricates
differences on the cases where it is wrong. Anything mapping a pseudo-op to a
real one needs checking against the encodings rather than trusting the name.

## Correction: classify opcodes from encodings, not printed names

The alias-normalisation rule recorded above needs replacing rather than
patching. Two corrections in a row on the same idea is the signal that the
idea itself was wrong.

The first correction removed li from the alias table, because li assembles to
addiu or ori depending on the value. That left li unmapped, which is worse:
the tool then counted it as an opcode in its own right and inflated every
distance involving one.

Measured on func_800577B0 (0x800577B0, 209 instructions), the same candidate
object, three versions of the same instrument:

    alias table with li mapped to addiu     distance 52
    alias table with li removed             distance 52, for a different reason
    classified from the encoding            distance 0

Distance zero was the truth. The candidate has the target's exact opcode
multiset and differs only in register choice and scheduling, which the
positional diff reports separately as 82.

The fix is to stop reading mnemonics. Every MIPS instruction carries its
opcode in bits 31..26, with the SPECIAL functions in bits 5..0, and both the
splat listing and objdump print the raw encoding next to the text. Classifying
from those bits needs no table of aliases and cannot disagree with the
assembler about what li meant this time.

The general lesson is not about MIPS. A normalisation table sits between the
measurement and the thing being measured, and it is exactly as trustworthy as
its least accurate entry. When a derived name and a raw encoding are both
available, deriving from the encoding removes a whole class of quiet errors
rather than fixing one of them.

## Switch arms are emitted in source order, so the layout names the order

GCC lays out switch case bodies in the order they appear in the source, and
the jump table records where each body ended up. Sorting the table's targets
by address therefore reads the source order back out directly, before any
compiling is attempted.

func_800577B0 (0x800577B0, 209 instructions, eleven entries) has a shared
case 5 and 9 body. Written in numeric order, between case 4 and case 6, the
candidate sat at 82 differing positions. The target's block addresses say
otherwise:

    case 0  1  2  3  4  6  7  8  5and9  10

Moving the shared body to sit after case 8, which is where its block lives,
took the count from 82 to 26 in one edit and changed nothing else.

Two things worth carrying:

- Read the case order off the table before writing any C. It costs one sort
  and removes a whole class of positional noise that otherwise looks like an
  allocation problem.
- A shared body has no natural place in numeric order, so it is exactly the
  arm most likely to be written somewhere surprising. Numeric order is a
  guess; the layout is evidence.

The remaining 26 were a scheduling detail with the same shape in two arms. A
volatile read-modify-write of a global was being hoisted ahead of plain struct
stores that precede it in source. Writing the second halfword store of the
field30 union third, immediately before the height assignment rather than
after the mask, gave the target's order. Cases 1 and 6 needed the identical
change, and each was worth 13 positions.

That last part generalises less well than the first, but the diagnostic does:
when the same small residual appears in two arms of one switch, it is one
source-shape mistake made twice, not two coincidences.

## Cross-jumping needs the arms textually identical, not merely equivalent

GCC merges the common tail of two switch arms only when the statement
sequences match. Two arms that do the same work in a different order do not
merge, and the cost is a whole duplicated tail rather than a few instructions.

Duel_LoadPackageStage (0x800171A8, 237 instructions, thirteen phases) has four
tail groups, and one of them was written with its statements in a different
order in each arm:

    phase 6    w, h, mask and, mask or, kind, size, ptr, ptr+0x800
    phase 12   w,    mask and, mask or, kind, size, h, ptr, ptr+0x800

Semantically identical; the height assignment simply sits in a different
place. That single difference kept the two arms from sharing their tail and
cost eighteen instructions, 255 against a target of 237. Moving phase 6's
height to where phase 12 has it took the candidate to 242 and the
encoding-based histogram distance from 18 to 7, with no other edit.

The diagnostic worth keeping is the shape of the excess rather than its size.
The extra opcodes were not scattered: they were one clean copy of a single
tail, and reading the histogram as a set of counts made that legible. When the
excess resembles one recognisable block, the question is which two arms failed
to merge, not which instruction is wrong.

The corollary is a warning about writing candidates. It is natural to write
each switch arm in whatever order reads best for that arm. Where arms share a
tail, that instinct is actively harmful: the shared portion has to be
byte-identical in the source, so the arms should be written to a common
template even where a different order would read more naturally.

## Two independent levers decide the `addu` operand order at an indexed load

A base plus an index plus a constant displacement compiles to one `addu` and a
displacement on the load, whichever way it is spelled. Which register lands in
`rs` is not free: `addu $v0, $s1, $v0` and `addu $v0, $v0, $s1` are different
words, so this shows up as a diff even when the count, the shape and the whole
register allocation already agree. Two separate things decide it.

**With an unscaled index, the lever is where the constant is grouped.**
Compiled at `gcc_2_8_1_g8_split`, `q` in `$a0` and `i` in `$a1`:

| spelling | emitted |
| --- | --- |
| `q[i + 0xA]` | `addu a0,a1,a0` |
| `*(q + i + 0xA)` | `addu a0,a0,a1` |
| `*(i + q + 0xA)` | `addu a0,a0,a1` |

Indexing binds the constant to the index, so GCC adds `i + 0xA` first and the
base arrives second. Writing the sum as `(q + i) + 0xA` puts the base first.
Reversing the two terms textually is inert; only the grouping matters.

**With a scaled index, no spelling of pointer arithmetic works.** The multiply
puts its result in `rs` and leaves the base in `rt`, whatever the source says:

| spelling | emitted |
| --- | --- |
| `*(u16 *)(q + i * 2 + 0xC)` | `addu a1,a1,a0` |
| `*(u16 *)(i * 2 + q + 0xC)` | `addu a1,a1,a0` |
| `((u16 *)(q + 0xC))[i]` | `addu a1,a1,a0` |
| `((Entry *)(p + 0x7C4))[i].field_00` | `addu v0,v0,a0` |
| `((Rec *)q)->field_0C[i]` | `addu a0,a0,a1` |
| `((ModelSlot *)p)->field_750[i].max` | `addu a0,a0,v0` |

Only a **member reference off a struct pointer** puts the base first. Casting
the base to the element type, or to the element type at an offset, does not:
the array has to be a member of a struct the base points at. So where a target
derives a second base register and uses it with small displacements, that base
is a struct in the original source, and typing it as one is what supplies the
member reference.

`func_80050F24` (`0x80050F24`, 82 instructions) is the worked example. Written
with `u8 *` bases and hex offsets it reaches the exact instruction count with
every register allocated as retail has it, and four `addu`s in the wrong order.
Regrouping the two unscaled sites closed those; the two scaled sites needed the
types. Naming `ModelSlot.field_DFE`, giving the `0xCF8`-`0xD13` window the type
`ModelSlotCF8Block`, and reaching `field_750[]`, `field_0A[]` and `field_0C[]`
through struct members closes all four.

The block's own shape is fixed by evidence rather than guessed: it starts at
`0xCF8` and cannot reach past `field_D14`, which bounds it at `0x1C` bytes; the
halfword access at `+0xC` bounds the leading byte array at two elements; and the
remaining `0x10` bytes are exactly eight halfwords. Both arrays are indexed by
the same `field_DFE`.

Note the contrast with "Do not name an array base to reproduce a materialised
base register": that result says introducing a *variable* for a base costs an
instruction. This one introduces no variable — the base already exists — and
only changes its declared type.

## GCC 2.8's loop optimiser only sees loops the front end marked

A global read inside a `for` or `while` loop has a loop-invariant address, and
under `-msplit-addresses` that address is a separate `lui` insn. GCC 2.8.1
hoists it into the preheader and keeps it in a callee-saved register for the
whole loop. So a target that **rematerialises** `lui %hi(sym)` at each read
inside a loop was not compiled from a structured loop.

The same body written with an explicit `goto` back edge is not hoisted at all.
GCC 2.8's `loop.c` works from the `NOTE_INSN_LOOP_BEG` / `NOTE_INSN_LOOP_END`
notes the front end emits for `for`, `while` and `do`; a loop assembled out of
labels and `goto` carries no notes, so the loop optimiser never runs on it.

Reduced to a probe, with `f1` a call so that a hoisted address needs a
callee-saved register:

```c
for (i = 0; i < 0xB; i++, slot++) {      /* lui $18,%hi(gID) before the loop */
    ...                                   /* both reads use $18 */
    f1(*slot, (i << 1) | (gID != i));
}

i = 0;                                    /* lui $2,%hi(gID) at each read */
top:
    ...
    f1(*slot, (i << 1) | (gID != i));
    i++; slot++;
    if (i < 0xB) goto top;
```

Read it as a diagnostic before it is a lever. Two `lui` of the same symbol
inside one loop is not a scheduling accident and not something source order can
produce; it says the loop had no loop notes. The cost of getting it wrong is
larger than the two instructions it looks like, because the hoisted address
occupies a callee-saved register and so changes the frame size and the whole
saved-register set.

`func_80180390` in the main menu overlay is the worked example. As a `for` loop
it builds a `-0x30` frame saving `s0`-`s5`; as a `goto` loop it builds the
target's `-0x28` frame saving `s0`-`s4`, and both reads of `gMain_bMenuID`
rematerialise `%hi` exactly as retail does. Note also what the count did: the
`for` version reaches exactly 495 of 495 instructions and the `goto` version is
four over, because the two extra prologue stores cancel the two saved `lui`.
Count parity with the wrong saved-register set is an artifact, so check the
frame before reading anything into it — the companion to "Count the target's
saved registers before blaming the allocator".

`func_80048768` shows the same cause through two apparently unrelated
symptoms. Its far-pan window test is loop-invariant, so a structured loop
hoists the test and ends the original `arg1` live range before the body. With
no value to preserve across `SpuGetVoiceEnvelope`, the prologue also loses the
retail copy into `$s6`. A label and `goto` back edge keeps the test inside all
four iterations, extends `arg1` across the call, and restores the saved copy.
When an invariant moves and a parameter copy disappears together, treat them
as one loop-optimiser decision before debugging register allocation
independently.

## A pointer local reused across blocks is allocated by a different pass

GCC 2.8.1 allocates registers in two passes. `local_alloc` handles pseudos that
live and die inside one basic block and gets first pick, which on this target
means `$v0`, `$v1`, then the argument registers. `global_alloc` runs afterwards
for everything that crosses a block boundary, and takes what is left.

That makes one C-level decision visible in the disassembly: whether a global
was read into **one** local reused through the function, or into a **separate
local in each block**. A single local crosses blocks, so it is allocated
globally and lands wherever there is room; per-block locals are allocated
locally and land in `$v0`/`$v1`. Both spellings emit the same loads and the
same stores — only the register numbers differ, so the diff is total while the
instruction count and the opcode histogram both already agree.

Read it in that direction. A target that re-reads a global into `$v0` or `$v1`
in block after block was written with a fresh local each time, and the fix is
to declare one.

`func_80046DE8` (`0x80046DE8`, 92 instructions) is the worked example. It reads
`g_SDValue` seven times. Written with a single `SDValue *state`, it reaches the
exact 92 instructions with the exact opcode histogram and **78 of 92 positions
differing**, because the pointer sits in `$a2` everywhere instead of `$v0`.
Splitting it into one local per block took that to 11 positions in a single
change, and nothing else in the source moved.

Three smaller consequences of the same rule closed the rest:

- **The value locals split too.** One `s32 value` used for two independent
  `!= -1` tests also crosses blocks; giving each test its own local — or
  writing the comparison inline — put the loaded halfword back in `$v1`. That
  was the last two positions.
- **The tail store groups are readable.** Where the target re-reads a global
  between two stores it could have shared, the source had a new local there;
  where it stores five fields through one register, the source cached it. The
  grouping in the disassembly maps one-to-one onto the locals in the source.
- **A read hoisted above a store group was a local declared early.** The target
  loads `g_SDValue` into `$v1` before four stores that go through a different
  register, then uses it after them. Writing that as a local assigned before the
  group, rather than as the last statement's own read, is what lets the
  scheduler place the load there and removes a load-delay `nop`.

This is a different mechanism from "Do not name an array base to reproduce a
materialised base register", which is about the *cost* of naming a base. Here
naming costs nothing and changes only which allocator sees the pseudo.

## Four spellings of one global, and the addressing each produces

A `u16` global read repeatedly in one function can be declared four ways, and
under `-G8 -msplit-addresses` each produces different code for the *same*
source. The choice is not cosmetic: it decides both which addressing form is
used and whether the address survives as one register across the body.

| declaration | access |
| --- | --- |
| `extern u16 g;` | `%gp_rel(g)($gp)`, one instruction |
| `extern u16 g[];` | one `%hi` materialised once, reused by every access |
| `extern volatile u16 g[];` | same — `volatile` stops the *load* being reused, not the address |
| `extern volatile u16 g __attribute__((section(".data")));` | a fresh `lui %hi` / `%lo` pair at **every** access |

Only the last reproduces a target that spells `lui $v0, %hi(g)` followed by
`lhu $v0, %lo(g)($v0)` over and over with the same register. Read it in that
direction: repeated `%hi` materialisation of a *small* global says the source
declared it a `volatile` scalar in an explicit section, and no rearrangement of
the C will produce it from the array form, because it is CSE unifying the
address and CSE does not care that the load is volatile.

The array form is still what a base whose address is genuinely taken wants, and
the two can appear in one function. `func_80013360` (`0x80013360`, 85
instructions) has both: `gInput_wPad1Held` and `gInput_wPad1Pressed` are
volatile section scalars, re-materialised at each of their six reads, while
`D_800E9D28` is an array whose base is computed once. `D_8009B098` is a plain
scalar and comes out `%gp_rel`, which fixes the profile at `-G8` on both the
compiler and the assembler.

Two other things that function needed, both already recorded rules paying off:

- the loop is a `goto` loop, so GCC's loop pass never sees it and does not hoist
  anything out of the body — see "GCC 2.8's loop optimiser only sees loops the
  front end marked";
- `step = 2; if (held & CROSS) step = 4;` rather than an `if`/`else`, which is
  "Initialise before the branch rather than in an else clause".

What did not come free is the base register. `D_800E9D28`'s base has to live in
`$s0` across the loop; declared as a plain local it is allocated globally and
lands next to the `%hi` temporary instead of coalescing with it, and the seven
prologue instructions come out permuted. One `register s16 *p asm("$16")` pin
settles it. That is a pin correcting an allocation on an otherwise exact
sequence, the weaker of the two kinds of pin, and it is the only one the
function needs.

## Correction: the `0x4C0` sound-packet window is a Psy-Q `SpuVoiceAttr`

The section "The sound-packet sibling group at `0x8004Axxx`" above reads the
`0x4C0`-`0x4FF` window as the 16-byte-plus-padding `Packet` from
`sound_secondary_playback.c`, names `0x4C0`/`0x4C4`/`0x4C8`/`0x4CA` as
`image`/`type`/`x`/`y`, and says `0x4E4`, `0x4FA` and `0x4FC` "are still
unnamed". That is superseded. `func_80077450` is `SpuSetVoiceAttr`, and the
window is one `SpuVoiceAttr`, whose size is exactly the `pad04C0[0x40]` hole.
Every offset the three functions touch decodes:

| offset | `SpuVoiceAttr` field |
| --- | --- |
| `+0x4C0` | `voice` |
| `+0x4C4` | `mask` |
| `+0x4C8` / `+0x4CA` | `volume.left` / `volume.right` |
| `+0x4CC` / `+0x4CE` | `volmode.left` / `volmode.right` |
| `+0x4E4` | `a_mode` |
| `+0x4FA` / `+0x4FC` | `adsr1` / `adsr2` |

The masks decode too, and confirm the reading independently. `func_8004A27C`
writes `0xF`, which is `SPU_VOICE_VOLL | VOLR | VOLMODEL | VOLMODER`, and the
four fields it writes are exactly the volume and volume-mode pairs.
`func_8004A764` and `func_8004A6F8` write `0x60100`, which is
`SPU_VOICE_ADSR_AMODE | ADSR_ADSR1 | ADSR_ADSR2`, and the three fields they
write are exactly `a_mode`, `adsr1` and `adsr2`. `func_8004A764`'s `a_mode`
constant `5` is `SPU_VOICE_EXPIncN`.

Two consequences for the group's recorded plateau, both measured:

**The profile line is wrong as well.** "The compiler side is G0 for this
group" no longer holds: `gcc_2_8_1_g8_split` with a `section(".data")` alias on
`D_8009B458` is what reproduces the mixed addressing, because the shared
declaration in `sound.h` is small enough for `-G8` to make it gp-relative while
the target uses `lui`/`lw`.

**The base must be a `u8 *` with hex offsets, not a struct pointer at the
packet address.** The section above records binding a `Packet *` at `state +
0x4C0` as the strongest available hypothesis and reports that it folds the
member stores back to state-relative addressing but still allocates the base
into the argument register. Storing through the *state* pointer at `0x4C0`
offsets instead does not: the base stays in `$v1` and the argument is derived
as `addiu $a0, $v1, 0x4C0`, which is retail's shape. All three functions then
reach their exact instruction count with opcode distance 0, at 6, 8 and 9
differing positions, and what is left in each is prologue scheduling. The
stored candidates are in `notes/candidates/`.

### Emission order is not source order for a run of stores to one struct

The same three functions disagree about which store order to write, and the
disagreement is not guessable from the disassembly. `func_8004A764` emits
`voice`, `mask`, `adsr1`, `adsr2`, `a_mode` and wants that order in the source,
with `a_mode` last so it lands in the `jal` delay slot; writing it third, where
it reads most naturally beside `mask`, costs two positions. `func_8004A6F8`
emits `mask` before `voice` but wants `voice` first in the source: `mask` first
costs three. So a run of stores through one pointer has to be measured in both
directions rather than transcribed from the emitted order.

## Operand association decides which term a constant folds into

A constant OR-ed into a chain of shifted and masked fields is not associative
as far as the emitted code is concerned, because GCC folds it into whichever
partial term it is adjacent to, and that changes how many instructions the
neighbouring terms need.

`func_8005B36C` builds a GP0 `0xE2` texture-window word from four 5-bit fields.
Written with the constant third in the chain the body is 88 instructions;
moving it to the front or to second position gives 89 -- the target's count --
and drops the differing positions from 84 to 69. Nothing else changed. The
constant folds into the masked-and-shifted term next to it, and where two of
those terms are already being folded from `sra`/`andi`/`sll` into a single
`sll`/`andi` pair, which term absorbs the constant decides whether that fold
still happens.

Worth trying early on any `|` chain that mixes a large constant with shifted
fields, because it costs one recompile per position and the alternatives are
all register-level.

## A three-way equality test on one value is a `switch`

GCC 2.8.1 lowers even a two-case `switch` to a comparison tree, so the tell is
the branch sense and the block layout rather than a jump table. A chain written
as `if (v == A) ... else if (v == B) ... else ...` emits `bne` to skip each arm
and lays the arms out in order. A `switch` on the same three values emits `beq`
to each arm and a `j` to the default, with the arms placed *after* that jump.

`func_80045334` tests `arg0 & 0xF000` against `0x8000` and `0x9000` with a
default. As an `if`/`else if` chain it builds 67 instructions with `bne`; as a
`switch` it builds the target's exact 70 with `beq`, `beq`, `j`. The three
instructions are the difference between the two layouts, not between two
spellings of the same one.

This is the same family of tell as "A flat comparison chain is if/else, and the
last arm is the else" in `notes/overlays/matching-patterns.md`, read from the
other direction: that one says a *flat* chain of `beq`s was not a `switch`;
this one says `beq` to each arm plus a `j` to the fall-through default was.

## `no_sched2` reads the natural store order, and `volatile` is the knob

The existing "Read statement order off a no-scheduling profile" note covers
`-fno-schedule-insns`, the pre-reload pass, and uses it to recover the original
*statement* order. The post-reload pass is a separate question and needs a
separate profile. `gcc_2_8_1_g0_no_sched2` is the one that shows which order
the back end hands to the scheduler, and it also fills no delay slots, so its
listing is usually one instruction longer than the target. Read the order, not
the length.

This matters because that natural order can be completely invariant to the
source. On `func_80045334` the three stack stores that build a request emit as
`kind`, `second`, `first` under `no_sched2`, and the target needs `first`,
`second`, `kind`. About 1800 variants crossing assignment order, declaration
order, `volatile` subsets, member against pointer against cast-address stores,
named locals against inline expressions, register pins and every profile all
produced the same natural order. When a residual is two swapped independent
stores, check this first: if the order does not move under `no_sched2`,
rewriting the statements is not going to move it either, and the sweep should
go somewhere else instead of enumerating orderings.

The lever that does control it is `volatile` on the destination fields, which
forces the emitted store order to be exactly the source order. It is a real
tool for pinning an order, with one cost that decides whether it can finish a
match: a `volatile` store may not sink into a `jal` delay slot. So it works
when the stores are all interior, and it cannot produce a target whose last
store sits in the delay slot -- there the argument setup takes the slot
instead. Marking a subset `volatile` pins the relative order of just that
subset and leaves the rest free to sink.

## A `u8` local loses the mask that a `(u8)` cast keeps

`(x & 7) & 0xFF` is redundant and GCC 2.8.1 knows it: `nonzero_bits` proves the
upper bits of `x & 7` are already zero, so the second mask folds away. That is
the right answer for the value and the wrong answer for the match whenever the
target keeps the `andi`.

Which spelling is used decides whether the fold happens at all, and the two
spellings are not interchangeable even though they compute the same value:

- `u8 mode = flags & 7;` and every variation on it -- a separate `s32 idx`
  assigned first, `flags % 8`, `(flags << 29) >> 29`, `u32` instead of `s32`,
  the variable held in a one-member struct -- all emit `move`, not `andi`. The
  narrow local is promoted to `SImode` with its unsignedness tracked, so no
  extension insn is ever created and there is nothing left for the fold to
  remove.
- `(u8)idx` written at each use site emits `andi ..., 0xFF`, because the cast
  creates a real conversion in the expression and CSE then shares one `andi`
  across every use of it.

On `func_8005B0B4` this single choice was worth 7 of 15 differing positions,
and none of them looked like a masking problem. The `andi` is what keeps the
masked value in a *different* pseudo from the unmasked one, so with the `u8`
local GCC coalesced the two and the whole downstream register assignment
followed the wrong one: the loop index, the branch destination and the operand
of the `+ 3` all came out of the wrong register. With the casts the pseudos
stay apart and all seven positions fall out together.

So when the target has an `andi ..., 0xFF` on a value the compiler could prove
is already narrow, do not read it as the compiler being redundant. Read it as
evidence that the source wrote a cast in an expression rather than declaring a
narrow variable, and reach for it early -- the symptom shows up as register
choice several instructions away, not as a missing mask.

## An ignored non-void return still changes the caller's allocation

A callee's return type is observable in the caller even when the value is
thrown away. `func_8005B0B4` ends with `*out = c; return out;`, and with
`func_8005A98C` declared `HsvT *` the closing three-byte struct copy comes out
based on `$v0` where the target bases it on the parameter's own `$s6`.
Declaring the same callee `void` -- the value is unused either way, and the
emitted call is identical -- moves the copy back onto `$s6` and finishes the
match. Nothing else in 107 instructions changes.

The mechanism is worth knowing because the symptom appears nowhere near the
call. GCC's pre-reload scheduler hoists the return-value copy `$v0 = out`
above the block move, and a later pass then substitutes `$v0` for `out` as the
copy's base register. Whether the scheduler does that depends on what else is
competing for `$v0` at the tail, and an ignored non-void return is one of the
things that competes. `-fno-schedule-insns` confirms the chain: with the
pre-reload scheduler off, the base is the parameter's register under either
declaration.

So when a residual is a base or scratch register at the *tail* of a function
and the instruction multiset is already exact, check the return types of
everything the function calls before spending time on the tail itself. There
is precedent for the void spelling in the tree: `src/game/func_8005B054.c` and
`src/game/func_8005B0B4.c` both declare `func_8005ABA0` as returning `void`
while `src/game/color_transform.c` defines it returning `Color *`, so a caller
whose prototype disagrees with the definition is an existing shape here rather
than a new liberty.

Two negatives worth recording, because both look like the obvious fix and
neither works. Pinning a variable to the wanted base register does not help:
the substitution happens to the register inside the MEM after allocation, so
`register Color *p __asm__("$22")` is simply overwritten. And capturing the
returned pointer into a second variable before the copy does not help either,
at any of five placements, because the two are provably equal and GCC folds
the second away -- laundering it through `u32` does not stop the fold.

## Split addressing and coalescing are one choice, and a function can want both

`-msplit-addresses` decides how a symbol's address reaches a register, and it
decides it for the whole translation unit. Two shapes come out of it and they
are not independent of the register allocator:

- **Coalesced**: `lui $s0, %hi(X)` then `addiu $s0, $s0, %lo(X)`. The `HIGH`
  temporary and the destination are the same register.
- **Uncoalesced**: `lui $v0, %hi(X)` then `addiu $s0, $v0, %lo(X)`. Same two
  instructions, one more register live.

Under every `_split` profile GCC 2.8.1 emits the uncoalesced form; under the
non-split profiles the address comes from the `la` macro, which is the
coalesced shape by construction. That is easy to misread as a scheduling
effect -- it is not. It holds under `gcc_2_8_1_g0_split_no_sched1` and
`gcc_2_8_1_g0_no_sched2_split` as well, so turning either scheduler off does
not recover it.

The reason it matters beyond two register names is that the uncoalesced form
leaves a value in a *call-clobbered* temporary, and the next address in the
same block usually wants the same temporary. That is an anti-dependence, and it
forces the scheduler to emit the two address pairs in a fixed order. On
`func_8002FD10` this turned a two-register difference into an eight-position
prologue permutation: `&D_800EAE98` had to be materialised before the callback
address purely because both `HIGH`s landed in `$v0`. So when a residual is a
permutation of a prologue window with an identical opcode multiset, look for an
uncoalesced `HIGH` before enumerating statement orders -- the order is the
symptom, the register is the cause.

The same function shows why the profile cannot simply be flipped. It needs the
non-split shape for `D_800EAE98`, whose address is only ever taken, and the
split shape for `D_800E9D70`, whose `%hi` is kept in a saved register and
reused as the base of `sh $v1, %lo(D_800E9D70)($s3)`. Compiled non-split that
store becomes an ordinary `0($s0)` and the second `%lo` relocation disappears.
No profile in `compiler_profiles.json` currently produces both, which is worth
knowing before spending a rotation deciding between them.

## The `.data` attribute and the array spelling are two different levers

"Four spellings of one global, and the addressing each produces" covers the
`-G8` threshold. `func_8002FD10` adds a second axis that the existing note does
not separate, because two of its globals need opposite answers on it.

`D_8009B2A4` (2 bytes) is gp-relative in the target and wants the plain
`extern u16 D_8009B2A4;`. `gGraphics_sViewportX` and `gGraphics_sViewportY`
are also 2 bytes and are **not** gp-relative, so they need
`__attribute__((section(".data")))` -- that part is the known lever. What is
new is that the attribute alone is not enough: the target stores to them with
the assembler macro through `$at`,

```
lui $at, %hi(gGraphics_sViewportY)
sh  $zero, %lo(gGraphics_sViewportY)($at)
```

and that shape only appears when the global is declared as a **scalar**.
Declared `extern u16 gGraphics_sViewportY[];` and written `[0] = 0`, GCC
materialises the address into a general register first and emits
`lui $v0` / `sh $zero, 0($v0)` instead. Both are two instructions, so the size
and the opcode multiset are unchanged and only the register names move -- which
is exactly the kind of residual that gets attributed to allocation and swept in
the wrong direction. On this function the scalar spelling was worth 27 of the
67 positions that remained after the profile was settled.

Read it as a rule: `section(".data")` chooses *whether* the access is
gp-relative, and scalar-against-array chooses *whether the address is
materialised at all*. A direct store to a named scalar can stay a `(mem
(symbol_ref))` and reach the assembler as a macro; a subscript cannot.

## A global read once before a loop and addressed inside it costs a copy

Under `-msplit-addresses`, this pattern emits one instruction more than the
retail code does, and no source shape found so far removes it:

```c
extern u32 T[]; extern u16 OUT[];
void p1(void) {
    u32 first = T[0]; u32 *e; s32 i, n;
    for (i = 0; i < 10; i++) {
        e = T; n = 1;
        if (first) { while (*e) { e++; n++; } }
        OUT[i] = n;
    }
}
```

GCC emits `lui $2, %hi(T)` then `move $8, $2` then `lw $7, %lo(T)($8)`, and
inside the loop `addiu $3, $8, %lo(T)`. Retail's equivalent has a single
`lui` into the register that both the setup load and the in-loop address use.

The RTL dumps name the cause exactly. The setup read produces
`(set (reg 91) (high (symbol_ref)))` with a `REG_EQUIV` note. The loop pass
then hoists the loop's own `high` as a *second* pseudo, `cse2` rewrites that
to `(set (reg 96) (reg 91))`, and the allocator gives the two different hard
registers even though `reg 91` carries `REG_DEAD` at the copy and its only
remaining use is that copy. Neither coalescing nor the `REG_EQUIV`
rematerialisation that would delete the copy fires.

Crossed on `func_8003B5C8` without removing it: five placements of the setup
read, the base hoisted into a local (which moves the cost rather than removing
it -- the loop then emits `move` instead of the low-part add and the setup
grows by one), the symbol declared as a scalar with its address taken, the
read spelled `*(u32 *)T` and as `*e` after assigning `e`, fourteen
permutations of the setup statements, three outer-loop forms, the store as a
direct array index instead of through a pointer, and all eighteen viable
profiles. Reading a *different* element (`T[1]`) does remove the copy, because
the two addresses no longer CSE, but it costs a second `lui` and the count is
unchanged.

Two things follow. When a function reads a global once outside a loop and
takes the same global's address inside it, expect to be exactly one
instruction long, and check that before concluding the loop shape is wrong.
And when the residual is that copy, the lever wanted is something that changes
the allocator's coalescing, not the loop -- the loop shape around it can
already be exact.

### Resolved: `const` makes the guard read loop-invariant and the copy vanishes

The framing above is what blocks: it treats the setup read as fixed and hunts
for a way to coalesce. The lever is on the other side. Write the read *inside*
the loop and let the loop pass hoist it, and there is only ever one `high`
pseudo, so no copy is created to coalesce.

That hoist normally does not happen. `invariant_p` in `loop.c` rejects a `MEM`
whenever `true_dependence` cannot separate it from anything the body stores,
and the body here stores through a pointer, which nothing disambiguates
against a `symbol_ref`. But `invariant_p` checks `RTX_UNCHANGING_P` *before*
it reaches that scan, and returns 1 outright:

```c
case MEM:
  if (MEM_VOLATILE_P (x)) return 0;
  if (RTX_UNCHANGING_P (x)) return 1;      /* taken -- no dependence scan */
  if (unknown_address_altered) return 0;
  for (i = loop_store_mems_idx - 1; i >= 0; i--)
    if (true_dependence (loop_store_mems[i], VOIDmode, x, rtx_varies_p))
      return 0;
```

Declaring the table `const` sets `RTX_UNCHANGING_P` on loads from it, so the
guard read hoists into the preheader regardless of the stores. The preheader
then holds the hoisted `high` and the guard load together, the in-loop address
uses that same pseudo, and the setup read that produced the second pseudo no
longer exists:

```c
extern u16 OUT[];
extern const u32 T[];                 /* const is the lever */
...
do {
    e = T; n = 1;
    if (T[0] != 0) { ... }            /* read inside the loop, hoisted */
    ...
} while (p < end);
```

`func_8003B5C8` matches exactly this way, 57 of 57 instructions on
`gcc_2_8_1_g0_split` and `gcc_2_8_1_g8_split`. Everything listed as crossed
above stays crossed; none of it moved the count, because all of it kept the
read in the setup.

The negative that pins the mechanism: the same shape with the read inside the
loop but the table left non-`const` builds 57 instructions too, and is still
wrong -- the load stays in the body as `lw $a0, %lo(T)($t2)` and the preheader
holds only the `lui`. The instruction count coincides; the placement does not.

Generalise it as: when a loop-invariant load will not hoist and the body
stores through a pointer, the question is not the loop shape but whether the
loaded object can be declared `const`.


## A block copy through `lwl`/`lwr` means the source is a byte array

When a target moves a fixed-size block with a run of `lwl`/`lwr` and
`swl`/`swr` pairs instead of aligned `lw`/`sw`, the copy is a struct
assignment whose member is a `u8` array: GCC cannot assume word alignment for
byte-typed storage, so it falls back to the unaligned pair form. Writing the
same bytes as a `u32` array, or copying them in a loop, gives aligned moves
and cannot reproduce the block. `struct { u8 b[20]; }` assigned whole does,
one pair per word plus the tail.

Derived independently on `func_8003B5C8` in #1723 and #1725.

## Screen for split addressing before starting, alongside jump tables and the nops

The queue screen in `notes/research/matching-evidence.md` lists three classes to
skip before spending a rotation: a `jtbl_` reference, the MASPSX load-delay nop,
and the MASPSX reorder-mode delay slot. There is a fourth, and it cost two
rotations to find twice.

**A target that needs the split-address form is currently unmatchable**, for the
reason written up under "Split addressing and coalescing are one choice". Every
`_split` profile emits the `HIGH` into a fresh temporary and never coalesces it
with the destination, so wherever the target has `lui $sN, %hi(X)` followed by
`addiu $sN, $sN, %lo(X)` with the same register, or reuses a held `%hi` as a
load or store base, the build is one copy off and the anti-dependence that copy
creates then permutes the surrounding schedule. On `func_8002FD10` that was 8
positions, on `func_8003B5C8` one extra instruction it could not shed.

The screen is cheap. Over a function's splat asm, flag it when either appears:

- `%lo(SYM)(\$rN)` where `rN` is neither `$gp` nor `$at` -- a held `%hi` reused
  as a base;
- `addiu $rA, $rB, %lo(SYM)` with `rA != rB` and `rB` not `$gp`/`$at` -- an
  uncoalesced `HIGH` that the target *did* coalesce elsewhere, or the split
  address form generally.

What does **not** disqualify a function is `lui $sN, %hi(X)` and
`addiu $sN, $sN, %lo(X)` on the same register separated by other instructions.
That is the coalesced form, it is what `_split` produces once the two halves
belong to one pseudo, and `func_8002DC38` needed exactly it -- there the split
profile was the difference between 79/45 and 78/7. So the flag is on the
*register mismatch*, not on the separation.

Running all four screens over the resident queue leaves 84 of 140 unmatched
functions, and the four smallest survivors are `func_8004D75C`,
`func_8004D58C`, `func_8004DC38` and `func_8004E7B0` -- all leaves, none with a
prior external attempt.

## A redundant-looking store at the end of a function can force a copy earlier

On `func_8001944C` the last statement before the closing call is
`*(u16 *)buf = 0`, where `buf` holds the same address the function has been
using since its first call. It looks like it could be spelled through the
array name instead, and it computes the same thing either way. It cannot.

The function loads a block through `buf`, sets a bit on every halfword of it
in a loop, clears some entries, and writes it back. The target walks that loop
with its own pointer, copied from the callee-saved register that holds `buf`
(`addu $a1, $s0, $zero`). That copy only exists because `buf` is still live
after the loop. Spell the final clear through the array name, or through the
second base local, and `buf` dies at the loop; GCC then walks the callee-saved
register directly, the copy disappears, and the build is one instruction
short. Everything else in the function is unchanged.

Two things follow. When a build is exactly one instruction short and the
missing one is a register-to-register copy at the top of a loop, look at what
keeps the copied value alive *after* the loop rather than at the loop itself.
And when transcribing, do not "simplify" a late store that repeats an address
already in a local: which spelling is used is observable, and the version that
looks redundant is the one that reproduces.

## The store of a load-produced value is scheduled last in its block

Found on `func_80045208` and its twin `func_80045334`, whose entries are in
`notes/candidates/`. Both reconstruct to the exact instruction count with the
exact register assignment and stop on the same two positions: the order of
three word stores into a stack request block, and therefore which of them the
delay-slot filler steals for the following call.

The rule is reproducible in a probe of ten lines:

```c
void probe(s32 *table, s32 type) {
    u8 req[0x30];
    s32 first = table[0];
    s32 *second = table + 2;
    f();
    req[0] = 0x24;
    *(s32 *)(req + 4) = first;
    *(s32 *)(req + 8) = type;
    *(s32 *)(req + 0xC) = (s32)second;
    g(req);
}
```

GCC emits the three stores as `+8`, `+0xC`, `+4`, with the load-fed one last,
for **every** permutation of the three source statements and for both
definition orders. Source order does not reach it. Change `first` so it comes
from an `addiu` rather than a `lw` - `first = (s32)(table + 1)` - and the order
becomes `+4`, `+0xC`, `+8`, and `first`/`second` also swap their hard registers
from `$s1`/`$s0` to `$s0`/`$s1` without any pin. So how a value is produced,
not where it is written, decides both.

Two practical consequences.

**Do not spend permutations on store order when one of the stored values comes
from a load.** Check the probe's shape first: if the odd store is the load-fed
one and it is at the wrong end of the block, no ordering of the source
statements will move it.

**Disabling sched2 does restore source order for the stores, and is still the
wrong answer.** `gcc_2_8_1_g0_no_sched2` emits the three stores exactly as
written, but the rest of a function of this size needs sched2: the same
candidate goes from 2 differing to 73. Read the `no_sched2` build as a
diagnostic that tells you what the scheduler did, not as a profile to ship.

## The store of a load-produced value is scheduled last in its block

Found while taking `func_80045208` from opcode distance 3 to 0; the entry is in
`notes/candidates/`. It and its twin `func_80045334` both reconstruct to the
exact instruction count with the exact register assignment and stop on the same
two positions: the order of three word stores into a stack request block, and
therefore which of them the delay-slot filler steals for the following call.
`func_80045334`'s entry already records that source statement order is not the
input this order is computed from, over about 1800 variants. This is what it is
computed from instead.

A ten-line probe isolates it:

```c
void probe(s32 *table, s32 type) {
    u8 req[0x30];
    s32 first = table[0];
    s32 *second = table + 2;
    f();
    req[0] = 0x24;
    *(s32 *)(req + 4) = first;
    *(s32 *)(req + 8) = type;
    *(s32 *)(req + 0xC) = (s32)second;
    g(req);
}
```

GCC emits the three stores as `+8`, `+0xC`, `+4`, with the load-fed one last,
for **every** permutation of the three source statements and for both
definition orders. Change `first` so it comes from an `addiu` rather than a
`lw` - `first = (s32)(table + 1)` - and the order becomes `+4`, `+0xC`, `+8`,
and `first`/`second` also swap their hard registers from `$s1`/`$s0` to
`$s0`/`$s1` with no pin. So how a value is produced, not where it is written,
decides both the store order and that pair of registers.

Two practical consequences.

**Do not spend permutations on store order when one of the stored values comes
from a load.** Check the probe's shape first: if the odd store is the load-fed
one and it is at the wrong end of the block, no ordering of the source
statements will move it, and neither will a pin.

**Disabling sched2 does restore source order for the stores, and is still the
wrong answer.** `gcc_2_8_1_g0_no_sched2` emits them exactly as written, but the
rest of a function of this size needs sched2: the same candidate goes from 2
differing to 73. Read the `no_sched2` build as a diagnostic that tells you what
the scheduler did, not as a profile to ship.

## Two `%hi` materialisations: sched1 picks the order, local-alloc picks the registers

`func_8001944C` sat at 15 differing positions for six canonical attempts, and
the recorded reason - "the two address materialisations after the loop are
emitted in the opposite order" - was only half of it. With relocations
resolved rather than masked the residual was six positions, and they split
into two independent questions about the same four instructions:

```
lui   $3, %hi(D_800E9D70)      target:  lui   $3, %hi(D_8015C424)
addiu $4, $3, %lo(D_800E9D70)           addiu $5, $3, %lo(D_8015C424)
lui   $6, %hi(D_8015C424)               lui   $6, %hi(D_800E9D70)
addiu $5, $6, %lo(D_8015C424)           addiu $4, $6, %lo(D_800E9D70)
```

The two pointers, `$4` and `$5`, were already right. Only the pair of `%hi`
temporaries was swapped, and each one is reused once more as the base of its
object's zero-offset store, so a swap there costs four positions in the
setup and two in the stores.

**They are decided by different passes, and you have to move both.**
`-fno-schedule-insns` flips which pair is emitted first and leaves the hard
registers alone; nothing about the source order of the two objects' first
references moves either. So the emission order is the first scheduling pass,
and the `$3`/`$6` assignment is local-alloc ranking the two quantities.

**What moved the registers was where the zero-offset store was written.**
The target's `*(u16 *)D_8015C424 = 0` sits between the rectangle's width and
height stores, and the earlier candidate wrote it there - the note even
records that placement as "worth 7 positions on its own". Writing it instead
*before* the whole rectangle block swapped the two `%hi` registers into the
target's assignment, because it is the first reference to that symbol in the
block that decides which quantity local-alloc ranks first. sched2 then sinks
the store back between the width and the height, so the emitted code is
unchanged. The store's final position told us nothing about where it belonged
in the source.

**What moved the order was a pin on a pointer that reuses a dead register.**
With the registers correct, pinning `base`, the second pointer to the buffer,
to `$5` - the register the bit-setting loop's walking pointer has just
finished with - made the first scheduling pass emit that object's pair first.
Function matched at 70 of 70 under `gcc_2_8_1_g8_split`.

Three things worth carrying forward. **Masked comparison hides swapped
relocation symbols**: `lui $3, %hi(A)` and `lui $3, %hi(B)` differ only in
the relocated immediate, so any harness that masks those fields to avoid
counting unresolved `%hi`/`%lo` will score this class of miss as a match and
send you looking in the wrong place. Resolve the relocations instead.
**A statement's position in the output is not evidence about its position in
the source** when sched2 is free to move it, and an independent store to a
provably distinct symbol always is. **Write it where it makes the allocator
behave, not where it lands.**

## Correction: one scheduling flag is not a control when two passes agree

The entry above on `func_80045208`/`func_80045334` records that
`-fno-schedule-insns2` restores source order for the three request stores and
is "a diagnostic that tells you what the scheduler did". Re-measuring on
`func_80045334` shows that reading is unsafe.

`-fno-schedule-insns2` alone leaves the order unchanged. `-fno-schedule-insns`
alone leaves it unchanged. Only **both** off restores source order. The two
passes independently arrive at the same arrangement, so switching off either
one on its own produces no visible change and invites the conclusion that the
pass is not responsible.

This is the same instrument failure as masking relocated immediates, in a
different dress: a control that cannot distinguish "this pass did not do it"
from "the other pass would have done it anyway". **When two passes can both
produce an effect, disabling one proves nothing; disable them together first
to establish that the effect is scheduling at all, then re-enable one at a
time.** The `func_8001944C` result in this file is the shape to copy - there
`-fno-schedule-insns` moved the emission order and left the hard registers
alone, which separated two questions precisely because the passes disagreed.

The mechanism itself also sharpens. Measuring all six permutations against the
emitted order gives: **the store of the load-fed value is removed from its
position and appended after the others, and the remaining stores keep their
source order.** Not merely "scheduled last". That matters because it makes the
rule falsifiable against a target: if the target has the load-fed store
anywhere other than last, no permutation, pin, aliasing shape, or profile will
reach it, and the disagreement is in the value model rather than in the
ordering. That is the state `func_80045334` is now in.

## 2026-09-07: `func_80060E70` bounded candidate cycle

Refreshed master to `25b2e728` and checked open PRs before selecting this stored
candidate; only #2061 was open and did not own this function. At the end of
cycle 1, work remained local on `copilot-fixer/func-80060e70-cycle1`; no
publication or manifest changes had been made.

The stored baseline reproduced 102/102 instructions, encoding-based opcode
distance 0, and four differing words, both relocation-masked and fully resolved:
`+0x68`, `+0x6C`, `+0x74`, `+0x78`. Its positional opcode count is 3, correcting
the previous heading's 2. Resolving the object with tracked symbol addresses
confirms that no additional relocation-only mismatch was hidden.

All four variants used the same named
`gcc_2_8_1_g8_split_no_strength_reduce` / MASPSX 2.81 profile:

| Variant | Instructions | Opcode distance | Resolved differing positions |
| --- | --- | --- | --- |
| Baseline | 102/102 | 0 | 4 |
| Output pointer pinned to `$30`, target-order setup | 102/102 | 0 | 13 |
| Also pin inner stats pointer to `$8` | 99/102 | 3 | 89 |
| Remove output-pointer pin; retain stats pin | 101/102 | 1 | 65 |
| Also pin explicit byte offset to `$2` | 102/102 | 0 | 3 |

Counts include missing trailing instructions. Each source, SHA-256, object,
compiler/MASPSX assembly, relocation listing, resolved binary, full diff and
measurement JSON is preserved under
`tmp/copilot-fixer/attempts/func-80060e70-cycle1/`; `baseline-note.md` preserves
the original entry. The scratch harness uses the existing `compile_c` pipeline
and `overlay_diff` comparison primitives, then links only the candidate with
retail-address symbol definitions to resolve its relocations. This comparison
is diagnostic, not the canonical full-executable gate.

**Pinning the long-lived output pointer is unsafe here.** The first variant
gets the preheader right, but the loop hoist then assigns the stats base to
the same `$fp`, overwriting the output base before `sw ...,4($fp)`. The second
stops that hoist but loses `$fp`'s save/restore and folds the index subtraction
into `lw ...,-4(...)`. Neither is a usable candidate.

**Leave the long-lived output pointer to the allocator and constrain only the
inner arithmetic.** The third variant restores the target's saved `$fp` and
the entire preheader, but still folds the subtract into the load displacement.
An explicit `$2` byte-offset lifetime restores the subtract/shift sequence and
the zero-displacement load. The fourth variant is the newly stored best source.
All twelve relocation entries now occur at the target positions and resolve
without additional differences.

Only three adjacent words remain, beginning in the `beqz` delay slot:

```
target   move v1,a1 / addiu v0,v1,-1 / sll v0,v0,2
build    addiu v0,a1,-1 / sll v0,v0,2 / move v1,a1
```

The next discriminator is whether an explicit short-lived ID lifetime in the
target's `$3` forces the copy before the offset computation, without changing
allocation elsewhere. This was not tried: the cycle stopped at four material
variants. Keep the promising resident candidate active for that bounded
follow-up. At the end of cycle 1 it remained `unmatched_asm`; no canonical
acceptance was claimed.

### Cycle 2: exact source and canonical acceptance

The improved stored source reproduced its three-word residual before further
work. The first and only material variant replaced `s32 id` with
`register s32 id asm("$3")`. This makes the ID copy precede the subtract/shift
and occupy the retail `beqz` delay slot: **102/102 instructions, opcode distance
0, and zero differences after all relocations are resolved** under the same
`gcc_2_8_1_g8_split_no_strength_reduce` profile. No other source or flag changes
were needed. The short-lived ID, stats pointer and byte-offset pins are ordinary
register declarations; there is no statement-level inline assembly.

Recorded the post-terminal resolution with `record_external_attempt.py`, then
used `integrate_verified_match.py --evidence-source post-terminal
--allow-register-pins` to integrate `src/game/func_80060E70.c`.
The only integration adjustment is the relative include of `src/types.h`.
`func_80039A14` and `TextBox_Create` were checked against the current inventory
and need no callee renames. The promoted candidate entry was removed as required
by the candidate-store check; its prior forms remain preserved in the cycle
scratch directories.

**The first canonical build exposed a local compiler failure.** The unmodified
sequence `MAKEFLAGS=-j2 make clean` followed by `MAKEFLAGS=-j2 make match`
initially exited 2 before linking. The native source-built GCC 2.8.1 crashed
while compiling the already configured `src/game/mdec_sync.c` with its existing
`gcc_2_8_1_g8` profile:

```
mips-sony-psx-gcc: Internal compiler error: program cc1 got fatal signal 11
```

The same source/profile reproduced the failure through `compile_c` in isolation.
Both existing named no-strength-reduction alternatives also crashed before
producing assembly. Neither `mdec_sync` manifest entry was removed, no source
was switched to assembly fallback, and no flags, profiles or retail inputs
were changed.

The missing prebuilt compiler used by both CI workflows was installed with
`MAKEFLAGS=-j2 make compiler-281-prebuilt`, using the already cached archive
whose SHA-256 matches `tools/bootstrap/old_gcc_prebuilt.json`. The native
compiler was preserved under ignored `tmp/` for rollback. The unchanged MDEC
source then compiled with its original profile. This was a local tool
distribution change, not a source or build-policy workaround.

**The canonical clean gate then passed with the integrated candidate.**
`MAKEFLAGS=-j2 make clean` followed by `MAKEFLAGS=-j2 make match` reproduced the
complete untouched retail executable with SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
Every preexisting matching C entry remained enabled, including both functions
in `mdec_sync.c`.

Cycle 2 evidence is under
`tmp/copilot-fixer/attempts/func-80060e70-cycle2/`: `baseline.json`,
`v1-pinned-id.json`, the corresponding exact sources, objects, relocation
listings and resolved diffs, `record-attempt.log`, `integrate.log`, `clean.log`,
`canonical-match.log`, and `blocker-reproduction.log`. The successful recovery
and acceptance are recorded in `install-ci-compiler.log`,
`ci-compiler-mdec.log`, `ci-canonical-clean.log`, and `ci-canonical-match.log`.
Further candidate variants are unnecessary.

### Preserved upstream preheader investigation (#2078)

The following findings were added to the older, unpinned stored candidate
while its promotion was under review. They concern that source spelling, not
the final pin-based match above; its deletion must not discard these negatives.

Re-measured with relocations resolved: four differing positions, all in the
loop preheader, and the whole body is exact. The preheader holds the same five
instructions in both builds, in reversed group order:

```
built    addiu $s1,$s3,2   move $s5,$s2   move $s0,$s4   lui/addiu $s8
target   lui/addiu $s8     move $s0,$s4   move $s5,$s2   addiu $s1,$s3,2
```

Two separate things are going on and they were previously conflated.

**The order of the three initialisations is source order.** Writing them as
`w = s; y = 0; q = e + 1;` reproduces the target's `move $s0` / `move $s5` /
`addiu $s1` exactly. That alone takes the residual from four positions to a
three-instruction shift.

**The address hoist is placed by loop-invariant motion, which appends.** It
inserts before `loop_start`, so it lands after anything the source already put
in the preheader, and no source order can put it first. It is also invariant to
both schedulers: `-fno-schedule-insns`, `-fno-schedule-insns2`, and both
together leave the preheader untouched, so this is not a scheduling question
for that candidate.

**What separates the two groups is biv versus giv.** Compiling the same source
with strength reduction enabled moves exactly one initialisation - `w`, which
becomes the derived `$s4 + 10` - to *after* the hoist, and leaves `y` and `q`
before it. So plain induction variables keep their source position in the
preheader, and derived ones are initialised after the invariants in this
experiment. The target has all three after the hoist, suggesting that all three
may have been derived in the original translation unit. That is a hypothesis
about the original source, not a conclusion proved by instruction order alone.

Rewriting the loop index-based so all three become derived does reproduce the
`[hoist][inits]` order - confirmed in the preheader of that build - but costs
far more than it saves, because the same rewrite makes GCC fold the `+10` into
the base (`addiu $s0,$s4,10` with `sh $v0,0($s0)`, against the target's
`move $s0,$s4` with `sh $v0,10($s0)`) and re-derives the `e` accesses. Reported
scores were 62 to 63 across the split profiles, against 4 for the
pointer-increment spelling. Pinning the four derived variables to the target's
registers made the score 101.

Also crossed without improving on four: an explicit `s32 *t = D_801D5608` in
place of the array spelling, with and without a pin to `$30`, and with
`gDuel_adwCardStats` made `volatile` or pushed to `.data` to stop it being
hoisted instead. Reported scores were between 53 and 67. The array spelling
is required for a second reason worth recording: the target materialises
`D_801D5608[0]` inline as `lui` plus `sw ...,%lo(sym)(reg)` and reaches `[1]`
through the hoisted pointer at `+4`. A pointer variable makes both go through
the pointer and loses the inline form.

## Small-data addressing: one mechanism, five levers

Requested on #2087 after the fifth separate note about the same thing. The
existing entries stay where they are; this one states the mechanism once and
says which lever answers which question, so a residual can be read straight
into a spelling instead of into a rediscovery.

**The mechanism.** A global reference passes two gates before it reaches the
linker.

1. *Is the symbol small data?* GCC answers this at compile time from the `-G`
   value and the symbol's **declared size**. A symbol it believes is small is
   emitted `%gp_rel(sym)($gp)`, one instruction, no address to allocate.
2. *If not, who builds the address?* With `-msplit-addresses` GCC builds it
   itself into an allocated register (`lui`/`addiu`, or `HIGH`/`LO_SUM` fused
   into the memory operand). Without it, or when the reference is a plain
   `(mem (symbol_ref))` that never becomes an address expression, the operand
   reaches the assembler as a macro and the assembler builds it through `$at`.

Every lever below moves one of those two gates. Read the target first: a
`%gp_rel` says gate 1 pulled it in; a `lui $at` / `%lo(sym)($at)` pair says
gate 1 pushed it out and gate 2 left it to the assembler; a `lui $rN` /
`addiu $rN, $rN, %lo` pair in a real register says gate 2 built it.

| Lever | Moves | Evidence |
| --- | --- | --- |
| the profile's compile-time `-G` | gate 1, for every symbol at once | #2077 |
| a declared size on an array | gate 1, in | #2056 |
| an incomplete array type | gate 1, out | #2078, #2083 |
| scalar against subscript | gate 2 | `func_8002FD10`, #2087 |
| compile-time `-G` against assemble-time `-G` | the two gates independently | #2087 |

**1. The profile's `-G` is the blunt instrument, and it is per function, not
per cohort.** `func_8003B054` (#2077) sits among `-G8` neighbours, but its
2-byte `D_8009B0D8` is reached through a `%hi`/`%lo` pair; at `-G8` that
collapses to a single gp-relative `lhu` and the function is short. Inheriting a
neighbour's profile is the right first move and the wrong last one.

**2. A declared size pulls an array in.** `func_800222F4` (#2056) needed
`u8 D_8009B16C[4]`, not `u8 D_8009B16C[]`, to reach its gp-relative form. An
unsized array has unknown size, so it can never be small data.

**3. The incomplete array type pushes a symbol out, and
`section(".data")` is not a substitute for it.** `Main_RunTrade` (#2078) has a
4-byte slot that `-G8` makes eligible; `extern u8 D_800E9EF0[]` with casts at
the use sites produces the split form, while adding `section(".data")` to the
pointer declaration leaves the macro store exactly where it was. #2083 is the
same finding from the other direction: an array declaration changed the
addressing where the attribute had not.

**4. Scalar against subscript decides whether an address exists at all**, and
this is the lever that hides, because both forms are two instructions and the
opcode multiset is unchanged — only register names move, which reads as an
allocation residual and gets swept the wrong way. A direct store to a named
scalar stays a `(mem (symbol_ref))` and reaches the assembler as a macro; a
subscript is an address expression and cannot. On `func_8002FD10` the scalar
spelling was worth 27 of 67 remaining positions. On `func_80023144` the same
choice was worth two instructions and the `$at` form for five separate panel
globals at once: declared `extern u8 X[]` they are not small data either, but
cse then materialises **one** base register and shares it across every access,
which is neither of the target's two forms.

**5. The two `-G` values answer different questions, so a function can want
both forms at once.** `gcc_2_8_1_cc_g8_as_g0_split` compiles `-G8
-msplit-addresses` and assembles `-G0`. `Model_LoadMonsterMerge` (#2087) needs
exactly that: its 4-byte `D_8009B0F4` keeps the assembler-macro store because
at link time it is not in small data, while `D_800F2C40` and the two path
symbols get explicit `lui`/`addiu` pairs. The controls matter for reading this
one correctly — plain `-G0 -msplit-addresses` reaches the same instruction
count but materialises the scalar into an allocated register instead of `$at`,
and every non-split profile is eleven instructions longer.

**Screening order that follows from this.** Take the profile from a matched
neighbour, then look at each global's access form in the target and pick the
declaration that produces it, one symbol at a time — the levers are per symbol
and a single function routinely needs three different answers. If no single
`-G` reproduces the mix, reach for a `cc_gN_as_gM` profile before rewriting any
C. And do not spend a rotation on a target whose `%hi` is held in a callee-saved
register and reused as a base: that is the separate, still-unmatchable class
recorded under "Screen for split addressing before starting".

## 2026-09-07: `func_80039794` — load macros select the useful invariant

The existing stored candidate reproduced 103/104 instructions and opcode
distance 1 with the pinned CI compiler and
`gcc_2_8_1_g8_split_no_strength_reduce`. Its historical positional-opcode count
of 7 did not reproduce as a direct encoding-class positional comparison: that
count is 84, and 95 complete instruction words differ with relocations resolved.
The shifted instruction stream makes the latter counts poor measures of the
small underlying residual; all comparisons in this cycle also retained full
resolved diffs.

Four cumulative source variants reached the exact 104-instruction function.
No compiler flags, profiles, shared headers or volatile qualifiers changed.

| Source discriminator | Instructions | Opcode distance | Resolved differing words |
| --- | --- | --- | --- |
| Stored baseline | 103/104 | 1 | 95 |
| Absolute scalar loop-limit access | 103/104 | 1 | 59 |
| Also use absolute scalar pad access | 104/104 | 0 | 8 |
| Store the reset value before initializing the counter | 104/104 | 0 | 7 |
| Explicit reset/table/cursor preheader order | 104/104 | 0 | 0 |

**Hide the loop-limit high-half inside the load macro, not a pinned register.**
The candidate declared `D_8009B0C1` as an array and read element zero. GCC
therefore exposed its high-half as a loop invariant and allocated it to `$s5`;
the table address at `D_801D9000` was instead materialized within the loop.
Declaring the same byte as a scalar with `section(".data")` avoids a small-data
access while preserving the absolute load macro. Its address is no longer a
separate loop invariant, so the table base becomes the useful `$s5` hoist.
Both declarations read exactly the same byte; neither defines or moves data.

**The pad-read `nop` is another consequence of that distinction.**
`gInput_wPad1Pressed[0]` exposed a high-half that GCC moved into the preceding
branch delay slot, where retail has a `nop`. An absolute scalar `u16` declaration
with the same section attribute keeps the address inside the halfword-load
macro. That preserves the delay slot and restores instruction 104, with the
target's `lui`/`lhu` register pair. This is the same original halfword read,
not a change to the controller mask or input semantics.

**Ordering two assignments chooses the already-held minus-one register.**
Writing `cnt = -1` before the global reset let CSE use `$s0` for the halfword
store. Writing the global reset first makes it use the held `$s4`; scheduling
still puts the counter initialization into the earlier branch delay slot.
Only that store register changes.

**Explicit preheader values work after removing the competing invariant.**
The older candidate's explicit table/minus-one attempts had lost while the
loop-limit address still competed for hoisting. On the corrected base,
initializing `reset_value = -1`, then `table = D_801D9000`, then the second
cursor reproduces the seven remaining prologue positions. The two cursors,
0x64-byte stride, out-of-line reset arm, calls and loop bodies remain intact.

The terminal post-policy result was recorded with `record_external_attempt.py`
and promoted through `integrate_verified_match.py`. All address-based callees
were checked against the current inventory. The promoted source is
`src/game/func_80039794.c`; its only promotion adjustment is the relative
`src/types.h` include. The stored candidate entry was removed.

`MAKEFLAGS=-j2 make clean` followed by `MAKEFLAGS=-j2 make match` passed without
excluding any matching entries. The complete rebuilt executable has SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
Sources, source hashes, profile selections, compiler/MASPSX assembly, relocation
records, resolved binaries/diffs and canonical logs are retained under
`tmp/copilot-fixer/attempts/func-80039794-cycle1/`; the original note is preserved
there as `baseline-note.md`.
## `func_8005B36C`: parameter-copy order is set by pseudo-register numbering

The stored candidate sat at two differing positions for two campaigns. Both
were the function's entry parameter copies, in the wrong order:

```
build    move t3,a1 / move t0,a0
target   move t0,a0 / move t3,a1
```

Everything else, all 89 remaining instructions, matched exactly.

**The cause is not scheduling.** `-fno-schedule-insns` leaves the order
unchanged, which is the control that matters here: it rules out `sched1`
without needing to reason about the block's critical path. It is also not the
profile. The order is identical under `gcc_2_8_1_g0`, `g8`, `g8_split`,
`g8_split_comm`, `g8_no_split`, `cc_g8_as_g0`, `cc_g0_as_g8`,
`g8_split_no_strength_reduce`, `g0_keep_large_ori` and
`cc_g8_as_g4_split`, and every legacy-compiler and `-O1` profile is far worse.

**The cause is that the source copied the parameter into a local.** The
candidate opened with `s = src;` and then used `s` throughout. Copy
propagation removes the copy, but the pseudo that survives into reload is the
one created for `s`, which has a *higher* number than the pseudo for the
`ot` parameter. Reload emits the entry copies in pseudo-number order, so the
`a1` copy was emitted first. Deleting the local and incrementing the parameter
directly - `src++` rather than `s++` - leaves the low-numbered `src` pseudo
alive, and the entry copies come out in parameter order. That single change
took the function from two differing positions to **91 of 91 instructions and
zero differences**, integrated on `gcc_2_8_1_g8`.

**The lever generalises to a diagnostic.** When the only residual is the order
of the entry `move` instructions that copy argument registers, and the order is
insensitive to both scheduling flags and profile, look for a parameter that the
source copies into a local before use. The parameter whose copy is emitted too
early is the one that is *not* shadowed by a local. Removing the shadow is
free: it does not change the instruction count, only the emission order.

The converse is a real risk. Applying the same removal to the second and third
locals of this function - `from` in the copy loop, `index` for the OT
subscript - costs 47 and 7 positions respectively, because those locals do
carry a distinct live range. Only the local that merely shadows a parameter for
its whole lifetime is safe to remove.

## `func_80045C98`: naming an anonymous temporary, and reusing one variable

The stored candidate sat at seven differing positions, all of them register
names on an otherwise exact instruction stream. It reached zero in two steps,
and both are reusable.

**Step one: give an anonymous temporary a name so it can be pinned.** Retail
computes each `mult` product straight into `$v0` (`mult v1,v0` / `mflo v0` /
`andi v0,v0,0xffff`); the candidate returned it through `$a2` and then moved it.
The product lived inside a conditional expression,
`x = m ? (u16)(m * (SD->field_1580 + 1)) >> 8 : 0`, so there was nothing to
constrain. Rewriting the conditional as a statement `if`/`else` is **neutral by
itself** - not one instruction changes - but it creates a scope in which the
product can be declared. Naming it there and pinning it,
`register u16 p1 asm("$2") = (u16)(m * (SD->field_1580 + 1));`, removed four
positions. `$3` costs 8 more and `$6` 59 more, so the register is the content of
the lever, not the naming.

**Step two: one variable for both stages, not two.** The remaining three were a
single value, `(u16)x`, held in `$v1` by the candidate and `$a0` by retail;
`$v1` had just been freed by `m` and GCC reused it. Pinning `x` to `$4` costs 67
because GCC 2.8.1 reserves a `register asm` variable for the whole function
regardless of the scope it is declared in - closing its scope before the
intervening call does not help, which is worth knowing. What works is removing
the second variable: the two fade stages chain, the output of the first being
the input of the second, so one `u16 v` carrying both, with `y = v` afterwards
for the call, allocates the way retail does. **169 of 169, zero differences.**

**The general shape.** Two variables joined only by `b = f(a)` are a *choice*,
and the choice is visible in the allocation. When consecutive stages of a
computation are spelt as separate locals and the residual is that one of them
sits in a freshly-vacated register where retail takes a fresh one, try one
variable for the chain. Reverting either step alone confirms both are
load-bearing: without the pins the single-variable form is four off, and with
the pins but ternaries instead of statement `if`s it is 59 off.

**This function is also the worked example of triaging by residual type.** Its
seven positions were all register names at identical positions - an *allocation*
disagreement, which register constraints reach. That is why it was chosen ahead
of `func_80023144`, `func_80012E5C` and `func_8002FD10`, whose residuals are
instructions in different *positions*, which they do not.

## `func_8001BAF0`: pointer casts, operand order, and reusing an existing local

Sixty-one differing positions, all register names at the right positions, closed
to zero. Two of the levers are new and one earlier finding in this repository
was too broad.

**Casting a pointer to `s32` escapes `plus` canonicalisation.** An earlier pass
on this function measured that flipping a C addition does not change the
emitted `addu` operand order, and recorded it as a general rule. That is right
for *pointer* arithmetic - `deck + sidx * 6` and `sidx * 6 + deck` are
identical, because GCC canonicalises the `plus` before the allocator sees it -
but wrong in general. Writing
`(DeckCardRecord *)(sidx * 6 + (s32)deck)`, so that the addition is between two
integers, **does** flip the emitted order. Six positions here came back that
way. The direction is per-site: of the four additions changed, one wants the
base first and three want it last, so they cannot be treated as a group.

**Pinning a destination is materialised as early as its declaration allows.**
Naming the inner re-read of `gDuel_aActiveCards` and pinning it to `$t1` fixed
its register but put the `lui`/`addiu` five positions too early, before the
`v * 12` chain that retail computes first. Wrapping the multiply in an enclosing
scope, so it is a finished statement before the pinned declaration is reached,
reorders them. This is the placement half of a cost whose allocation half was
recorded on `func_80057AF4`: everything feeding a pinned value collapses into
it, *and* the value is materialised as early as it can be.

**The last two positions were a store reusing an existing local.**
`sel[v - 0xB] = -1;` computed its address into `$a1`, reusing the pinned hand
value that dies there, where retail uses `$v0`. Five spellings of the store, a
`continue`-shaped loop, a pointer walk, a separate loop variable and a pinned
address temporary were all neutral or worse. What works is assigning the address
to `p`, **the `u8 *` local that already exists in the function and holds
`D_8009B1C8` earlier**, and storing through it. Using the function's other
pointer local `q`, an `s8 *`, costs nine, so it is that specific variable.

That last change exposed a second, smaller thing worth stating: with `p` typed
`u8 *`, `*p = -1` compiles to `li a2,255`, and retail has `li a2,-1`. The store
has to be written `*(s8 *)p = -1` to keep the value signed. **A one-instruction
immediate difference of 255 against -1 is a sign-of-the-pointee problem, not an
allocation problem.**

**150 of 150, zero differences**, on `gcc_2_8_1_g8_split`. All six pins are
load-bearing, costing 24, 9, 18, 14, 4 and 34 positions when removed one at a
time.

## `func_80057AF4`: the variable-count diagnostic, in both directions

Sixty-two differing positions, every one a register name at the right position,
closed to zero. The instrument that made it tractable is the classification
itself: a residual whose differing positions all carry the *same mnemonic* on
both sides is an allocation disagreement and is reachable by constraining
registers; one whose positions carry *different* mnemonics is a placement
disagreement and is not. `tmp/harness/triage.py` applies that test to every
stored candidate, and it is what selected this function.

**Pin one side of a swapped pair, not both.** The slot index and the slot
pointer were `$s1`/`$s2` the other way round. Pinning only the pointer swaps the
pair and takes 62 to 27; pinning the index as well costs 175. Constrain one
side and let the allocator place the other.

**A pin makes GCC compute into the pinned register.** With the slot pointer
pinned, GCC built the whole `index * 0xE20` chain directly into `$s1`, where
retail computes it in a temporary and only then forms the pointer. The same
happened one level up with the table base. Naming each - `off` pinned to `$2`,
a block-scoped `tbl` pinned to `$3` - restores the temporaries: 27 to 22 to 18.
The unpinned naming is neutral in both cases, so the register is the lever and
the name only creates somewhere to put it. **This is the standing cost of
pinning a destination: everything that feeds it tends to collapse into it.**

**Count the hard registers retail uses for a value, and give the source that
many variables.** Two applications, in opposite directions:

- Retail holds the map entry in `$a0` in one walk and `$v1` in another. One
  function-scope variable is one pseudo and therefore one hard register, so it
  cannot do that. Declaring `entry` separately inside each `switch` case takes
  18 to 6 (with the bit-index pin, which is worth another 4 on its own).
- The last four positions were retail computing the map address *in place*,
  `addu a0,s1,a0`, reusing the register that already held the running offset,
  where the candidate allocated a fresh `$v0`. Spelling the subscript as an
  accumulating offset in a single pinned variable -
  `o = i * 2; o += m->current * 116; entry = *(u16 *)((u8 *)m + o + 712);` with
  `o` pinned to `$4` - reproduces the in-place accumulation exactly. **203 of
  203, zero differences.**

The unpinned form of that last step is 4 and the pointer-typed form 2, so all
three parts matter: one variable, the right register, and integer rather than
pointer accumulation. Taking `&m->map[m->current][i]` as a pinned pointer
instead costs 146, because the row stride then folds differently.

All five pins are load-bearing; removing them one at a time costs 53, 9, 4, 12
and 4 positions.

**Together with `func_80045C98` this is a matched pair of opposite cases.**
There, two locals spelt as separate stages of one chain had to become one
variable because retail allocated them to one register. Here, one local used in
two independent walks had to become two, and an anonymous temporary had to be
folded into the variable that already held the running value. The rule is the
same in both directions and it is about counting registers, not about
preferring more or fewer locals.
## `func_8005106C`: three levers, and a new one for addition operand order

Twenty-six differing positions to zero, on `gcc_2_8_1_g8_split`.

**One variable per site, again.** The animation slot index and the value read
from it are each read twice, once before the display test and once inside it.
Retail allocates the index to `$a0` at the first site and `$v0` at the second,
and the value to `$v0` then `$a3`. A single function-scope variable is one
pseudo and one hard register, so it cannot do that; declaring a second pair
inside the `if (show)` block takes 26 to **8**. Splitting only the index and
leaving the value shared is 21, so both have to move.

**A stored value that retail re-reads means the load is volatile.** The counter
at `+0xE08` is written and then read back as an argument in the same block.
GCC forwards the stored value and masks it (`andi v1,v1,0xffff`); retail issues
a second `lhu`. Reading it through `*(volatile u16 *)` reproduces that and takes
8 to **2**, fixing six positions - the extra `andi`, the reload, and the four
instructions whose order changes around them.

**Explicit shift instead of multiply flips the addition's operand order.** The
last two were `addu v0,v0,s0` where retail has `addu v0,s0,v0` - the same
commutative add with the base second instead of first. This is the same class
as the operand-order work on `func_8001BAF0`, but the lever that worked there,
casting the pointer to `s32`, is **neutral here**: eight spellings were measured
including `(s32)m + 0xD08 + cur * 4`, `cur * 4 + (s32)m + 0xD08`,
`(cur * 4 + 0xD08) + (s32)m`, an `((s32 *)((u8 *)m + 0xD08))[cur]` subscript and
a `(s32 **)` pointer form, and all eight leave both positions.

What works is writing the scale as a shift:

```c
value = *(s32 *)((s32)m + (cur << 2) + 0xD08);
```

`cur * 4` and `cur << 2` are the same value and the same `sll` instruction, but
they reach the `plus` through different tree shapes, and only the shift form
leaves the base as the first operand. **185 of 185, zero differences.**

So the operand-order lever now has two independent forms, and they are not
interchangeable: on `func_8001BAF0` the `s32` cast was necessary and sufficient,
here it does nothing and the shift spelling is what matters. When an `addu` has
its operands the wrong way round, try both, and try them per site - the
direction is not uniform within a function.

## 2026-09-07: `func_80018608` - absolute loads close the startup guard

The stored duel-startup candidate now matches all 225 instructions under
`gcc_2_8_1_g8_split`. Its baseline was 224/225 with opcode distance 1.
The previous note's eleven positions described the local phase-4 guard window
at `0x80018778` through `0x800187AC`, not the full positional comparison:
resolving relocations and counting the shifted suffix gives 137 differing
words. The missing instruction was the branch-delay `nop` at `0x8001879C`.

### Preserved candidate structure and earlier results

The first call requests combined deck data and initializes the three display
halfwords. Phase 2 animates them down to their endpoints. Phase 3 opens the
side's selection message and waits for its counter. Phase 4 waits for file
transfer completion, starts BGM, populates the decks and computes the average
attack/defence of both forty-card decks. Phase 5 initializes the hand and
returns control to the outer state. This state/loop structure is unchanged.

The earlier candidate established several useful source-shape constraints:

- Keep `w = D_800F2848` before the outer flag test. This retains the high-half
  and full address in their function-wide saved registers. Offset zero must
  still use `D_800F2848[0]`, while the other offsets use `w`: that preserves
  the direct low relocation and prevents the later halfword load from moving
  before the offset-zero store.
- Keep phase-2 halfword updates in place, followed by the signed comparison.
  Computing replacement values in separate locals changed load/add ordering
  in the recorded trials, regardless of source-statement order.
- Share `atk`, `def`, `i` and `rec` between deck loops, but keep the two stat
  temporaries and rank-record bases separate (`stat`/`stat2`, `r1`/`r2`).
  Sharing those latter values changed the global pseudo allocation and
  permuted the target's loop registers.
- Use an explicit rank-record base so offsets `0xE`/`0x10` remain on the
  stores. Conversely, the selection record needs the local member view at
  offset 8: a raw byte expression folded that offset into the relocation,
  while an extra pointer local changed allocation and load ordering.
- The small scalars use GP-relative addressing, while the phase-4 guard and
  BGM inputs need absolute accesses. The original array declarations provided
  that distinction, but they are not the only valid representation: the
  matching absolute scalar declarations below supersede that restriction.

Earlier unsuccessful guard trials included both operand orders of `&` and
`|`, loading the guards into locals, `break`/`return`/inverted-if shapes,
signed/unsigned/sized-array declarations, and the named G8-split, comm,
no-strength-reduction and compiler-G0/assembler-G8 profiles. None solved the
mask allocation and exposed address scheduling together.

The BGM-address result explained the empty delay slot. When the first
not-taken instruction wrote a temporary register, it could fill the preceding
branch slot. A literal argument writing `$a0` restored the empty slot in the
historical diagnostic, but changed the value and was not a valid candidate.
Other negatives included `u16`/`u32` value locals, dereference spelling, K&R
and narrowed callee declarations, sized arrays that changed small-data
selection, and a pointer local pinned to `$4` that folded away.

### Three source variants that close the residual

| Variant | Instructions | Opcode distance | Resolved differing words |
| --- | --- | --- | --- |
| Stored baseline | 224/225 | 1 | 137 |
| Short-lived guard mask in `$2` | 224/225 | 1 | 131 |
| Also make the BGM halfword an absolute scalar | 225/225 | 0 | 2 |
| Also make the second guard word an absolute scalar | 225/225 | 0 | 0 |

The single `register u32 mask asm("$2")` declaration corrects six register
words. Its lifetime is confined to the phase-4 guard; no statement-level
assembly is used.

Declaring `D_8009B36A` as a scalar `u16` with `section(".data")` keeps its
address inside the halfword-load macro and restores the target argument
register. The compiler retains the empty branch slot without an explicit
`nop`. Finally, the same absolute scalar treatment for the `u32`
`D_8009B134` keeps its `lui`/`lw` together until assembly and closes the last
two scheduling positions. Widths, qualifiers, addresses and loaded values are
unchanged; these are extern declarations and do not allocate or move data.

The terminal result was recorded and promoted with the existing tools.
Only the five include paths were normalized for `src/game/func_80018608.c`
(since coalesced into `src/game/duel_phase_entry.c`),
then the integrated source was remeasured. The clean full-executable gate
passed with every existing matching entry enabled and retail SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
The original candidate note, three exact source variants, resolved diffs and
canonical acceptance are retained under
`tmp/copilot-fixer/attempts/func-80018608-cycle1/`.

## `func_8001825C`: selective absolute loads and separate replay inputs

The stored source reproduced under `gcc_2_8_1_g8_split` at 234/235 instructions,
opcode distance 1, and **100 fully resolved positional word differences**.
The historical 27 below is not reproduced by that full metric: the missing
instruction also shifts the suffix and changes branch/jump destinations.
The precise retail locations are word 138 (`0x80018484`) for the empty BGM
branch slot and words 189–190 (`0x80018550`–`0x80018554`) for the replay offset.
The older snippets' address labels below are preserved as historical
transcription, not as authoritative locations.

Three bounded source variants resolve the remaining code:

1. Declare `D_8009B0F4` and `D_8009B134` as `u32` scalars and `D_8009B36A`
   as a `u16` scalar, each with `__attribute__((section(".data")))`, and replace
   their `[0]` uses with scalar accesses. They remain extern declarations at
   the same addresses, with the same widths and qualifiers; no storage is
   defined or moved. GCC retains their absolute load macros while the rest
   of the function still uses split addresses. This reproduces the guard's
   non-interleaved loads and register assignment without a mask pin, and
   restores the BGM argument-register high half and empty branch delay slot.
   Result: 235/235 instructions, opcode distance 0, 15 differences, all in
   replay addressing. The new discriminator came from the independently
   resolved guard/BGM mechanism in `func_80018608`, not another profile sweep.
2. Hold only the replay offset `0x48000` in its observed `$5`. Its `lui`/`ori`
   pair and scheduling become exact, leaving 13 words: the counter/base
   register inversion and the final computed-address destination.
3. Hold the immutable replay table base in its observed `$3`; do not reuse
   that local for the computed slot address. Leave that final address
   allocator-managed. All 235 words now match with every relocation resolved.
   Both pins describe short-lived replay inputs; the counter, loaded index,
   final address, and all other locals remain unpinned. No compiler-owned
   data sections are emitted.

The source was recorded and promoted with the existing post-terminal tools.
Canonical `MAKEFLAGS=-j2 make clean` followed by `MAKEFLAGS=-j2 make match`
passed with every existing C entry enabled. The complete executable is
byte-identical, SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
Full source variants,
compiler/MASPSX output, object sections and relocations, linked disassembly,
measurements and ownership checks are under
`tmp/copilot-fixer/attempts/func-8001825c-cycle1/`.

### Preserved original candidate investigation

The following findings and negative results predate that resolution. In
particular, the apparent profile conflict is historical, not an outstanding
requirement to change compiler flags.

`gcc_2_8_1_g8_split`, 234 of 235 instructions, opcode distance 1, 27 differing
positions in three clusters. Written from scratch; there was no previous
candidate.

The duel refresh and replay tick. On its first call it re-lays the two sides'
card rows (records 5 to 14 and 20 to 29): for every occupied record it saves the
sprite y and the sticky flag bits, calls `func_80024D34` with the slot index and
the object's byte at +2, puts both back, and re-applies the object flags. It
then ticks the display, spawns a pending object for each side whose rank record
has a non-zero byte at +0x19, and finally either re-shows the ten field zones of
the current side (when the hand state byte is 0x28) or starts the BGM. Later
calls fall into the second phase, which waits for the file transfer to go idle
and the byte at `D_800E9ECE` to clear before arming a three-step replay counter,
and then into the third, which walks that counter down through `D_8009B208`,
pulls the recorded card out of the `D_8015C424` table, copies its three
coordinates into a fresh object and plays the sound.

**Levers, in the order they paid.**

- *One induction pointer per record, not three.* Writing the body against `rec`
  and letting each field be its own displacement gives three registers: the biv
  plus a giv for +0x16 and a giv for +0x12. The target has one giv, based at
  +0x12, with 0x4, 0x0 and -0xE hung off it. Assigning `p = rec + 0x12` inside
  the loop body and spelling every field relative to `p` collapses them.

- *`combine_givs` takes the LAST giv recorded as the base, and `record_giv`
  prepends,* so the base is the last rec-relative access in the body. With the
  flag write last the base is +0x16 and the displacements come out 0, -18, -4;
  swapping the two stores so the +0x12 write is last moves the base to +0x12 and
  the displacements to 4, -0xE, 0, which is the target. The two stores are to
  the same base and the scheduler puts them back in the target's order either
  way, so the swap costs nothing else. Worth 14 positions.

- *Index both arrays of the pending-object loop rather than walking a pointer.*
  `D_8009B1F0[i]` is already a giv; making the rank record `D_800E9FF0 + i*0x20`
  a giv as well puts the two preheader initialisations in the target's order,
  because givs are emitted in reverse of the order they were created. A `p +=
  0x20` pointer instead is a biv, which emits first and swaps `$s0`/`$s1` for
  the rest of the loop.

- *A struct cast keeps a member displacement off the giv.* `D_800E9FF0 + i *
  0x20 + 0x19` folds the 0x19 into the giv's base and loads at `0($s0)`; casting
  to a struct whose member sits at 0x19 keeps the giv at `D_800E9FF0 + i*0x20`
  and the displacement on the load, which is what the target does.

- *Separate variables where the target uses separate registers.* The replay
  block's table base, card pointer and object pointer have to be locals of their
  own. Reusing the loops' `rec` for the table base makes it a global pseudo and
  it lands in `$s3` instead of `$v1`; reusing the loop's object variable for the
  card pointer swaps `$s0` and `$s1` across the whole block.

- *`D_8009B208` is small data.* The target reaches it as
  `addiu $v1, $gp, %gp_rel(D_8009B208)`, so it needs a declared size of eight
  bytes or fewer; `[]` keeps it out of `.sdata` and turns the access into a
  `%hi`/`%lo` pair.

- *The `D_8015C424` table offset splits as `+ 0x48000` then `0x36B4`,* the same
  shape `duel_trap_resolution.c` and `duel_setup_card_record.c` already use.
  Writing the whole 0x4B6B4 in one expression folds it into the relocation and
  drops the separate register add.

**What is left** is 27 positions in three clusters, and all three are the same
kind of disagreement: a large constant or a `%hi` is allocated a *lower*
register in the target than in the build, and the schedule follows the
allocation.

```
    0x800184B0   target                          build
                 lui   $v0, 0x200                lui   $a0, 0x200
                 ori   $v0, $v0, 0x30            ori   $a0, $a0, 0x30
                 lui   $v1, %hi(D_8009B0F4)      lui   $v0, %hi(D_8009B0F4)
                 lw    $v1, %lo(...)($v1)        lui   $v1, %hi(D_8009B134)
                 lui   $a0, %hi(D_8009B134)      lw    $v0, %lo(...)($v0)
                 lw    $a0, %lo(...)($a0)        lw    $v1, %lo(...)($v1)

    0x8001848C   nop                             lui   $v0, %hi(D_8009B36A)
                 lui   $a0, %hi(D_8009B36A)      lhu   $a0, %lo(...)($v0)
                 lhu   $a0, %lo(...)($a0)

    0x800184F4   lui   $a1, 0x0004               lui   $v0, %hi(D_8015C424)
                 ori   $a1, $a1, 0x8000          addiu $a1, $v0, %lo(...)
                 lui   $v1, %hi(D_8015C424)      lui   $a0, 0x4
```

**The profile evidence for this function is genuinely split, and that is the
finding worth recording.** Two of these three clusters are the exact output of
the *non-split* profile, and the rest of the function is the exact output of the
split one. Reduced to two probes:

```c
extern u16 G[]; extern u32 A[], B[];
void p2(void) { if (((A[0] & 0x2000030) | B[0]) != 0) return; f(G[0]); g(); }
```

  - `gcc_2_8_1_g8` (no `-mgas -msplit-addresses`) emits `lui $v0, 0x200`,
    then `lui $v1`/`lw $v1`, then `lui $a0`/`lw $a0` -- the target's register
    assignment and the target's non-interleaved load order, because without
    split addresses the loads are single macro instructions and there are no
    `%hi` pseudos competing for `$v0` and `$v1`.
  - It also emits `lui $a0, %hi(G)` / `lhu $a0, %lo(G)($a0)`, one register,
    because the assembler expands a load macro through its own destination.
    That is the target's shape at 0x8001848C, and it is what leaves the branch
    delay slot before it empty: `fill_eager_delay_slots` declines the first
    instruction of the not-taken thread when it writes an argument register.
  - `gcc_2_8_1_g8_split` emits `lui $a0, 0x200` and `lui $v0, %hi(G)` /
    `lhu $a0, %lo(G)($v0)` for the same source, which is the build.

```c
extern u8 X[]; void q(void) { u8 *p = X; u8 *r = p + 0x12; ... }
```

  - `gcc_2_8_1_g8_split` emits `lui $2,%hi(X)` / `addiu $16,$2,%lo(X)` -- two
    registers, which is what the target does at 0x8001828C, 0x80018374,
    0x800183E0 and 0x80018464.
  - `gcc_2_8_1_g8` emits `la $16,X`, one register, and the whole function is
    then 60 aligned positions off instead of 27.

So the address materialisations require `-msplit-addresses` and the two
allocation clusters require its absence. Nothing in the profile table produces
both. Either the retail unit was built with a combination that is not modelled
yet, or the split build can be pushed to the target's allocation by some source
shape I did not find.

Crossed without moving the two allocation clusters: both operand orders of the
`|` and of the `&`, the loaded words read into locals before the test, the guard
as `break` versus `return` versus wrapping the body in `if (... == 0)`, the BGM
value read into a `u16` or `u32` local, `*D_8009B36A`, a K&R declaration and a
`u16` parameter for `SD_BGMPlay`, `D_8009B36A` declared `[2]` (small data, wrong
addressing), pinning a `u16 *` local to `$4` (GCC folds it away), `do`/`while`/
`for(;;)` for the replay counter loop, and the profiles `gcc_2_8_1_g8`,
`gcc_2_8_1_g8_no_split`, `gcc_2_8_1_g8_split_comm`,
`gcc_2_8_1_g8_split_no_strength_reduce`, `gcc_2_8_1_cc_g8_as_g0`,
`gcc_2_8_1_cc_g0_as_g8`, `gcc_2_8_1_cc_g8_as_g0_split`,
`gcc_2_8_1_cc_g0_as_g8_split`. Also crossed at the compiler, on the guard probe
under split addresses: `-fno-schedule-insns`, `-fno-schedule-insns2`,
`-fno-delayed-branch`, `-fno-expensive-optimizations`, `-fno-caller-saves`,
`-fno-cse-follow-jumps`, `-fno-thread-jumps`, `-fno-peephole`, `-fno-force-mem`,
`-fno-strength-reduce`, `-fno-function-cse`, `-fno-rerun-cse-after-loop`,
`-fno-regmove`.
## `func_8003A560`: a pin that had to be added, then removed

Sixteen differing positions to zero, all of them in the loop that walks the
five-entry resident page table. The chain is worth recording because two of its
steps contradict each other and both are correct.

**An unchanged count after a pin does not mean the pin did nothing.** Pinning
the page pointer `slot` to `$s2` left the count at 16, and a first pass here
recorded it as neutral and moved on. It is not neutral: with the pin the
`0x18400` block offset lands in `$s1` exactly as retail has it and the whole
`$s1`/`$s2` exchange disappears, replaced by `tbl` moving one register. The diff
has to be read, not just its length. Pinning `tbl` as well then gave 15.

**In-place accumulation and a temporary are both needed, in the same function.**
Retail computes the `0x18000` image offset into `$s0` and adds in place, so the
image pointer wants a pin; it loads the backing-store base into a temporary and
adds into `$s2`, so the base wants a *different* pin to stop GCC accumulating
into the destination. Those are opposite shapes twenty instructions apart, which
is why neither can be applied as a rule - each has to be read off the target per
value. Together they were worth five positions.

**Then the `tbl` pin had to come out.** After the explicit pointer walk, the two
new pins and the reordered preheader, an unpin sweep showed `tbl` had inverted:
keeping it cost two positions, removing it gave **3**. This is the second
function in a row where a pin that was worth having when added became the thing
blocking the end of the campaign, and only a re-sweep after the structural work
found it. **Re-run the unpin sweep after every structural change, not once at
the end.**

**The last three positions were the order of two `for` increment expressions.**
Retail hoists the `lui` half of the `-0x18C10` page stride into the preheader
and completes it in the loop, spending the counter decrement afterwards; the
candidate did the counter first. Writing `for (; i >= 0; slot -= 0x18C10, i--)`
rather than `i--, slot -= 0x18C10` reproduces it exactly. **240 of 240, zero
differences.**

The comma operator's order in a `for` increment is not something the language
forces on the code generator, but GCC preserves it, and it is worth trying when
a loop-end residual is two instructions trading places.

## Allocno priority: reference count, not live range, picks the register

A candidate that reproduces the exact instruction multiset and every opcode,
offset, constant and branch, and still differs in a set of positions that are
all register names, is usually one allocation inversion rather than many
independent choices. GCC 2.8.1 ranks allocnos by

    floor_log2(n_refs) * n_refs / live_length

and assigns hard registers in that order, so one pair swapping at the top
cascades through everything allocated after it.

The diagnosis is cheap: count references and live range for the two pseudos
that swapped and compute both priorities. If they are close, that is the cause,
and the fix belongs on whichever one is mis-ranked rather than on each symptom.

Note the `floor_log2` cliff. Between 15 and 16 references the multiplier jumps
from 3 to 4, so a single reference either way can move a pseudo past several
others. That is why some small source rewrites flip a whole allocation and
others do nothing at all, and why the useful lever is often the *number of
times a variable is mentioned* rather than anything about its type or scope.

`func_8006C120` (0x8006C120) is the worked example. It sat at 119/119
instructions and opcode distance 0 with eight differing prologue positions;
what decides them is how many times the recursion depth is read, and spelling
the depth test to match retail's count reproduces the prologue exactly.

This is the counterpart to the existing advice to count the target's saved
registers before blaming the allocator. That tells you *whether* allocation is
involved; this tells you which pseudo to move and in which direction.

## Read the lui spacing to tell which addressing form retail used

The section above records what the two `-G8` declaration forms produce. The
converse is also readable: retail's instruction *spacing* says which one the
original used, so the declaration can be chosen up front instead of by sweep.

The assembler expands a plain symbol reference as one unit, so its `lui` is
always immediately before the access and always in the same register. Split
addressing makes the `%hi` a compiler-allocated pseudo, which the scheduler can
move and the allocator can put anywhere. So:

- `lui` adjacent to its use, same register: assembler macro form, which under
  `-G8` means a scalar carrying `section(".data")`.
- anything between them, or different registers: split addressing, which means
  an incomplete or oversized array.

It is a per-symbol reading, and one function can want both. `func_800179F4`
(0x800179F4) wants the macro form for `gDuel_bTerrain`, `gDuel_bOpponentID` and
`D_8009B369`, and split addressing for `D_800E9FF0`, `D_800EA0E8`,
`D_800F284A` and `gDuel_awPlayerDeck`. Getting `gDuel_bTerrain` wrong alone was
worth five words: as an array its `%hi` became a separate pseudo and the
scheduler emitted it ahead of the callback address instead of after it.

## Combine folds `&sym + k` unless the address has more than one use

An access like `(&sym)[-1]` normally compiles to two instructions, because
combine folds the displacement into the relocation and emits `lui %hi(sym-1)` /
`lb %lo(sym-1)(reg)`. When retail instead shows

    lui   $v0, %hi(sym)
    addiu $v0, $v0, %lo(sym)
    lb    $v0, -0x1($v0)

the address was in a register combine could not fold into, and the reason is
combine's single-use requirement: it only substitutes the address-forming insn
into the load when that insn's result has exactly one use.

Spellings that do **not** defeat the fold, all measured on `func_800179F4`: a
local pointer assigned in the same block, a cast through `u8 *`, an explicit
pointer decrement, and a `register` pin on the holder -- the fold happens
before allocation, so pinning cannot prevent it. A `volatile` pointee does
block it, but then the load comes back as `lbu` plus a `sll` for the sign test,
one instruction long.

What works is giving the holder a live range long enough that it is not a
single-use pseudo at combine time; assigning it at the top of the function was
enough there. Worth trying whenever a candidate is exactly one instruction
short at a negative-displacement global access.

## Constant materialisations have no position, and that bounds two near misses

The two closest unmatched functions in the tree bottom out on the same
mechanism, which is worth naming as a class rather than rediscovering per
function.

`func_800283F4` sits at 2 differing words: `li a0,3` and `li s0,-1024` trade
the branch delay slot for the call delay slot. `func_80012E5C` sits at 5, and
its whole residue is that retail's loop body begins with `move a0,zero` /
`move a1,a0` while the candidate's begins with the index computation. Both are
*argument setup made of constants*, and in both cases every source-level
placement has been measured and is byte-identical to the baseline.

The mechanism is the one established on `func_80028B08`: **a value the compiler
rematerialises has no position in the output to move.** GCC constant-propagates
these values to their uses, so there is no assignment instruction anywhere for
a source reordering to relocate, and `reorg` sees whatever order the
propagation left. That explains three separate observations that were recorded
as unrelated negatives:

- binding the constant to a local, on either function, is constant-propagated
  away and changes nothing;
- pinning the local to the wanted register does not help either, because the
  pin constrains allocation and the problem is emission;
- every ordering of the surrounding statements is *exactly* inert, not merely
  unhelpful, which is what the intra-block ordering rule predicts for
  independent computation with no memory operation between the two.

**The practical consequence is a stopping rule.** When the residual reduces to
the relative order of two constant materialisations, and neither participates
in a memory dependence, the position is not expressible in C. Record it and
move on rather than enumerating placements: the search space looks large -
before the store, after it, before the call, after the call, in the enclosing
block - and is actually empty.

Reaching such a residual would need a *dependence* introduced between the two
constants, and C offers no way to make one constant depend on another that
survives constant folding. That is a stronger claim than "we tried the
orderings", and it is why both entries are now bounded rather than open.

## Pin the neighbour, not the variable whose register you can see

`func_8002EE94` was stuck two words from a match with retail carrying
`lui $v0,%hi` in a branch delay slot and completing `addiu $s0,$v0,%lo` sixty
instructions later, where the candidate used `$s0` for both. The candidate
pinned `box` to `$16` because that is the register retail plainly holds it in,
and the pin fixed thirty-odd other positions, so it looked settled.

The pin was the obstacle. Told the destination is `$16`, GCC materialises the
address straight into `$16`; retail's compiler had no such instruction and
reused the register the branch condition had just freed. **Unpinning `box` and
pinning its neighbour `obj` to `$17` instead** gives both: `box` still lands in
`$16`, and the `%hi` goes to `$v0`. That was the whole remaining residual, and
the function matches.

The general form is worth stating because the wrong move is the intuitive one.
A pin is evidence about *where a value ends up*, not about *how it gets there*,
and pinning the variable you can see in the target forecloses the second. When
a pinned variable's address or initial value materialises directly into its pin
and retail routes it through a temporary, try removing that pin and constraining
an adjacent variable so the allocator is pushed to the same assignment from the
other side.

Unpinning alone is not the answer either: with both unpinned the two swap, and
`box` takes `$17` while `obj` takes `$16`, for 33 differing words. It is the
*exchange* of which end is pinned that matters.

Two further results from the same function, both about store forwarding:

- The coordinate stores and their read-backs must be in **array-subscript
  form** with an unrelated global's store moved **below** them. Either alone is
  nearly worthless - the store move on its own costs 166 - and together they
  remove the forwarding that replaces retail's `lh` with a `move`. Combinations
  of two individually poor shapes are worth trying when each is cheap.
- The remaining `or $s0,$t0,$a2` against `or $s0,$s0,$a2` needs the loaded byte
  pinned to `$8`. A plain local does not stop GCC coalescing a load's
  destination with the destination of the operation consuming it.

### `try.py`'s `RELOC-ONLY` is not a byte-exact body

On this function `try.py` reported `RELOC-ONLY`, which reads as "the body is
exact and only relocations differ", and `make match` then failed at the first
differing word. `try.py` masks relocated words entirely, so a *register*
difference inside a `lui`/`addiu` pair is invisible to it - which its own
docstring says, and which `lines.py` was written to fix. `lines.py` reported 2
and was correct. Where a residual could be a register in a relocated
instruction, the positional count is the authority.

## A "scheduling" window around a load is often allocation

When a candidate differs only in the order of a few instructions and one of
them is a load, the instinct is to permute source statements. That is often
wrong: GCC fills the load delay slot with whatever is ready, and readiness
depends on which register the load targets, so the symptom is ordering while
the cause is allocation.

On `func_800179F4` retail's tail is

    lui   $v0, %hi(func_800164FC)
    lw    $v1, %gp_rel(D_8009B21C)
    addiu $v0, $v0, %lo(func_800164FC)
    sw    $v1, 0x50($s2)

and the candidate had the `lw` first with the next symbol's `%hi` in the delay
slot. Six statement orders were crossed, including hoisting the callback into a
local and reading the global into a local, and every one measured the same five
differing words. The actual difference was that retail holds the loaded pointer
in `$v1` where the build used `$v0`; pinning it closed the window at once.

So in a differing window that contains a load, compare the load's destination
register before permuting anything.

The inverse reading is also useful. On `func_8002E128` (0x8002E128), whose
residual looks like the same class, every pin is *worse* than no pin: naming
the product and table base and pinning them to retail's registers measures 16
against 13, either pin alone 14, a pinned constant 19. Pins making things worse
is the signature of a genuine scheduling tie, and says to stop looking at
allocation.

## When a pinned value costs an instruction, pin what consumes it

`func_8005C1F4` (0x8005C1F4) finished on a rule that inverts the usual pin
advice. Its last residual was four positions naming one value, `slot + 1`,
which retail keeps in `$a1` and the build put in `$a0`. Pinning that value
directly was measured across fifteen registers and **every one produced a
97th instruction**: a pinned local only takes its register for free when
something can write it there directly, and a value computed by `addiu` from a
pseudo needs a `move` to reach a reserved register.

The value that *consumes* it has no such problem. Pinning the modulo's
**result**, whose store can take its operand straight out of the reserved
register, took the residual from 7 to 4 and also placed the masked temporary
correctly without naming it at all. Finishing the job then needed the operator
written out as its own statements, because GCC's `% 4` expansion exposes only
one of its three values to naming; written out, all three are nameable and
three pins place them.

Two negative results from the same function are worth as much:

- *Availability is not the lever.* It was tempting to conclude that `slot + 1`
  shared `$a0` with a later load only because their live ranges did not
  overlap, and that forcing an overlap would separate them. Retail refutes it -
  `$a0` is free across that block in retail too, and retail still chooses
  `$a1`. Manufacturing the overlap costs an instruction and 44 positions.
- *A lower count can be a worse diff.* Spelling the modulo as an explicit
  division remainder measures 6 differing against the accepted form's 7, and is
  wrong: it recomputes the addend where retail copies it, so its opcode
  multiset carries an extra `addiu` and is short a `move`. Every earlier trap in
  this campaign was a count that stayed still while the difference set moved;
  this is a count that improves while the difference becomes worse in kind.
  Only the opcode multiset distinguishes them, and it does so in one command.

The structural half of this match came from the matched sibling `func_8005BE3C`
rather than from the diff: it declares a fresh pointer local per group of
accesses, all assigned the same `D_8009B498 + 0x40000`. That is what makes CSE
collapse the repeated global read into a register copy while still rebuilding
the address arithmetic, which nine earlier spellings had failed to reach by
trying to *suppress* CSE rather than to give it a second expression to
collapse. Writing the copy destination off the global with its own `+ 0x40000`,
instead of off the already-biased source pointer, was worth 54 positions on its
own.

## The allocno reference count is a lever you can spend one reference at a time

The section above establishes that GCC 2.8.1 ranks allocnos by
`floor_log2(n_refs) * n_refs / live_length`, and that the useful handle is
often the number of times a variable is *mentioned*. `func_8002FD10`
(0x8002FD10) is a clean second worked example, and it is worth recording
because the winning change looks like a stylistic preference rather than a
lever.

The function sat at 112/112 with opcode distance 0 and eight differing
positions, all in the prologue and all one permutation of one window. Retail
forms `&D_800EAE98` as `lui $s0` / `addiu $s0,$s0` - the `HIGH` temporary
coalesced into the destination - and emits it *after* the callback address.
The build emitted `lui $v0` / `addiu $s0,$v0` *before* it. Two facts, and the
register one causes the order one: with the high half stranded in `$v0` there
is an anti-dependence against the callback address, which also wants `$v0`, so
the scheduler has to run the pair first.

What closed it was writing the record-clearing loop as

```c
for (i = 0; i < 3; i++) {
    slot[i].unk00 = 0;
    slot[i].unk04 = 0;
}
```

instead of the walking-pointer form with `slot++`. Both spellings produce 112
instructions and the same multiset. The difference is that `slot++` is another
reference to `slot`, and removing it re-ranks the allocnos so the `HIGH`
pseudo no longer takes `$v0` first.

Two things make this worth generalising:

- **The change is invisible at the instruction level.** Neither spelling adds
  or removes an instruction, so nothing about the diff suggests the loop body
  is where to look. Only the reference count does.
- **Neighbouring spellings do nothing.** Writing the fourth record's marker
  through the array instead of the pointer, which also removes one reference,
  measures the same 8; so does adding a reference by spelling the increment
  `slot = slot + 1`. It is not "fewer references is better" - it is one
  specific count, and the cliff has to be crossed in the right place.

So when a residual is entirely register names and the multiset is identical,
enumerate the spellings that change a local's reference count by one in each
direction before concluding the allocator is out of reach. On this function the
`%hi`-coalescing lever from `func_800179F4`, a `section(".data")` attribute in
three forms, register pins on three locals, five statement placements, all
twenty-four declaration orders and every profile were all measured first and
all left it at 8.

## The order of two independent loads is set by their uses, not their positions

When a residual is the order in which two independent values are computed, the
instinct is to permute the statements that compute them. That is almost always
inert, and `func_80031874` (0x80031874) shows why, and what to move instead.

Its entry had six statement permutations of three opening loads, a `volatile`
load on either side, a struct view, a pin on the raw value and inlining each
read - all measured, all inert - and concluded that "GCC places these three by
its own scheduling and the source has no say in it". The conclusion was drawn
from the wrong half of the problem.

The scheduler ranks a value by the length of the dependence chain hanging off
it, and that chain is made of the value's **uses**. Three of those uses were
moved, and nothing else in the source changed:

| move | residual |
| --- | --- |
| starting point | 43 |
| the two texture stores below the `y` subtraction | 29 |
| the two sprite halfword stores below the `idx` read | 26 |
| the record-index read below the texture stores | **16** |

Fifty-two positions of the original sixty-eight came from moving statements
that *consume* values, and none from moving the statements that produce them.

Three qualifications, each measured, because the lever is easy to over-apply:

- **It is one-directional.** Every gain came from moving a use *later*.
  Both attempts to move one earlier are worse - hoisting the subtraction that
  consumes the viewport value costs nine instructions and 272 positions in one
  placement and 17 in another. Delaying a consumer delays the value; demanding
  it sooner forces the whole chain forward against whatever already holds those
  slots.
- **Placement is specific, not "later is better".** The index read pays below
  the texture stores and is worthless below the sprite stores; the texture
  stores pay below the `y` subtraction and cost 32 and 45 if pushed past the
  next two computations.
- **It does not reach a value whose chain is already the shortest.** The one
  load this function still gets wrong is the one with a single in-block
  consumer, where the fix would require *lengthening* its chain rather than
  shortening a competitor's, and no source change does that without adding
  instructions.

The companion negative is worth stating with it: a *constant* is hoisted to the
top of its block wherever the source writes it, so this lever never applies to
one. That now holds on three functions - the scratchpad pointer here, the slide
constant in `func_800283F4`, and the loop-invariant address in
`func_80012E5C`.

## A callee's base pointer can reveal a false whole-function lifetime

`func_80056828` (341 instructions) had a hand-transcribed candidate that saved
`s0` through `s7`, while retail saves only `s0` through `s6`. The excess came
from case 8: the fourth argument to `func_8004DC38` had been reconstructed with
the player index as its first argument, which kept the player live across the
largest loop. Retail passes the per-player record pointer. Correcting that call
removed the extra saved register and reproduced the target's 56-byte frame.

The same case exposed three semantic details that instruction count alone had
hidden. The callback loaded from `D_80010000[3]` or `[4]` starts four bytes into
the loaded object, the timestamp is reduced modulo 1000, and the current player
is written to the GP-relative byte `D_8009AFA0`. Pinning the record, callback
argument, loop cursor and halfword argument to `s2`, `s1`, `s3` and `s5`
respectively then reproduced retail's register roles throughout the case.

The durable candidate now compiles to 341/341 instructions with
`gcc_2_8_1_g8_no_split`, at opcode distance 24 and 291 differing linked words.
It replaces the 75-instruction automated sketch. The remaining work is centered
on the state dispatch, case 0's eager default pointer, case 3's loop shape, and
common-tail scheduling rather than missing behavior.

## An identical-arm conditional is a scheduling input, but only inside a block

`func_80045208` sat at two differing positions for several campaigns. The
residue was a straight swap of two stores into the request block:

```
target   sw $s0, 0x14($sp)   sw $s1, 0x1C($sp)   jal ... / sw $s2, 0x18($sp)
build    sw $s2, 0x18($sp)   sw $s1, 0x1C($sp)   jal ... / sw $s0, 0x14($sp)
```

`$s0` is fed by a load, `$s2` by `li`. Everything before the block matched,
including the producers, so only the emission order was wrong.

**The load-fed store is emitted last, and source order cannot move it.** Prior
notes inferred this from samples; all six permutations of the three stores were
then measured, and the result takes only two values. Whichever of the two
non-load-fed stores is written first keeps its place, and the load-fed store is
last in every one of the six. So source order permutes the other two and
nothing else - the rule is exhaustive here, not a tendency.

**What defeats it is an identical-arm conditional on the loaded value:**

```c
first = table ? (s32)*table : (s32)*table;
```

Both arms perform the same load, so this is not a null guard and not a
different value; it is the same value reached two ways.

**But at function scope it is folded away and completely inert.** Four
spellings - `table[0]`, `*table`, with and without the cast, and with the
pointer retyped to match the sibling - all left the count at 2 with no change
anywhere in the function. The lever only becomes live when the values it feeds
are scoped to an inner block:

```c
{
    register s32 first asm("$16");
    register u8 *const second asm("$17") = (u8 *)table + 8;

    first = table ? (s32)*table : (s32)*table;
    ...
}
```

With that scoping the conditional reorders the stores to retail's exactly, and
the residue becomes a pure allocation shift - three values each one register
off - which one pin corrects. 2 -> 0, seventy-five of seventy-five, opcode
multiset identical.

**How the transfer was found, and the general point.** The already-matching
sibling `func_80045334` carried this spelling, and it was recorded as possibly
decorative. Deleting it from the *matched* source costs exactly three positions,
and they are the same store-order positions - which identified the technique as
real before any attempt to transfer it. That check is cheap and worth making
whenever a matched source contains something unexplained: delete it and
remeasure.

The general point is a sharper form of a rule already in this file. An inert
result is not evidence that a lever is dead; it can mean a precondition is
unmet. Here the precondition was **lexical scope**, which is not something the
earlier statement of that rule anticipated - it had only ever been about other
transformations clearing the way. Scope belongs on the list of things to vary
before concluding that a technique does not transfer.

## Qualify a technique against a matched source before transferring it

When a matched source contains a construct nobody can explain, the cheap move
is not to reason about it and not to transfer it hopefully. It is to **delete it
from the matching source and rebuild**.

A matched function is a byte-exact baseline, so removing one construct isolates
that construct's contribution with no confounds: any change in the residual is
caused by the deletion and nothing else. The answer comes back as a price in
positions.

`func_80045334`'s note flagged its identical-arm conditional load as *possibly
decorative*. Deleting it costs **exactly three positions, and they are the same
three store-order slots** that the entry being transferred to was stuck on.
That converted "possibly decorative" into "worth exactly three positions, on
this exact residue" before any effort went into the transfer - and it is what
justified spending the effort, because the price and the residue shape both
matched the target.

Three things make this worth doing routinely:

- **It is one rebuild.** Cheaper than any transfer attempt.
- **It reports the positions, not just a number.** That tells you whether the
  entry you want to transfer to has the same residue shape. If the construct
  buys positions of a kind your target does not have, the transfer is already
  answered.
- **A null result is just as useful.** If deleting it changes nothing, the
  construct is genuinely decorative and can be removed from the matched source
  as well as ignored in the transfer.

The precondition is that something already matches. That is now common in this
project rather than rare, so any unexplained construct in `src/game/` is an
experiment waiting to be run. Note that a matched source is *tracked*, so the
ablation copy belongs under `tmp/` with its include paths adjusted.

## What the "scope" axis actually is, and what it is not

Inner-block scoping has now decided the outcome on several functions, and it is
tempting to read that as a single dominant lever. The measurements do not
support one lever; they support one *mechanism* with several unrelated
consequences.

An inner block is the only way C89 offers to introduce a **new declaration
part-way through a statement stream**. Everything scope has bought follows from
that, and what it buys depends entirely on what a new declaration changes:

| function | what the inner block provided | worth |
| --- | --- | --- |
| `func_80045C98` | somewhere to name an anonymous `mult` product so it could be pinned | 4 positions |
| `func_80057AF4` | a separately declared `entry` per `switch` case, so one value could occupy two hard registers | 18 to 6 |
| `func_80045208` | a scope in which an identical-arm conditional stops being folded | 2 to 0 |

And two measured places where it does nothing:

| function | why not |
| --- | --- |
| `func_800283F4` | the competing instruction is a plain constant assignment, and the hoist of a constant to its block top is indifferent to the block it is written in - three shapes, all byte-identical |
| `func_80023144` | the value is an address constant, so `fold` collapses the conditional whatever scope it sits in - three spellings, all inert |

There is also a direct negative already recorded on `func_80045C98`: **GCC 2.8.1
reserves a `register asm` variable for the whole function regardless of the
scope it is declared in**, and closing its scope early does not shorten it. So
scope is emphatically *not* a lifetime or allocation control.

The useful statement is therefore narrower than "scope is the dominant axis",
and more actionable: when a lever is inert, ask whether it needs a declaration
you have not been able to write, because a declaration is the one thing an
inner block adds. If the lever does not turn on a declaration - a constant's
hoist, a foldable expression, a pinned variable's lifetime - scope will not
reach it, and the two negatives above are what that looks like.
## Check which section a relocation record belongs to

A measurement harness that reads `objdump -r` must track the
`RELOCATION RECORDS FOR [section]` header, because `objdump` prints one block
per section and the offsets restart in each. Accepting any record that falls in
the function's offset range mixes other sections' relocations into `.text`.

This is not hypothetical and it is not rare. A jump-table function keeps its
table in `.rodata` as `R_MIPS_32` entries pointing back into `.text`. Those
offsets are small and dense, so they collide with `.text` offsets constantly,
and each collision silently replaces the real relocation at that word.

**The damage runs both ways, which is why it survives casual checking.** When
the wrong relocation is applied to a build word that genuinely differs, the
word can come out equal and the difference disappears. When it is applied to a
word that genuinely matches, the word is reported as differing. Four entries
were affected here:

| entry | before | after | direction |
| --- | --- | --- | --- |
| `func_80046294` | 8 | 7 | a false difference |
| `func_80015EF4` | 339 | 340 | a concealed real difference |
| `func_80044838` | 285 | 288 | three concealed real differences |
| `func_80029934` | 230 | 231 | the note was right, the tool was wrong |

The last row is the useful one. That entry's note recorded 231 from an earlier
hand count and the tool disagreed; the natural reading was that the note was
stale. It was not.

**The general practice:** when a tool and a durable note disagree, do not assume
the note is stale. Reproduce the number a second way before overwriting it. And
when a harness result changes after a tool fix, re-measure the *unaffected*
entries too - here the three closest candidates were confirmed unchanged at 2,
5 and 10, which is what makes the corrected numbers trustworthy rather than
merely different.

## Key an opcode histogram on encodings, never on printed mnemonics

`move rd, rs` is a pseudo-instruction for `addu rd, rs, $zero`, and `li rt, n`
for `addiu rt, $zero, n`. They are not similar instructions - they are **the
same encoding**, and a disassembler prints the pseudo whenever the relevant
operand is `$zero` and the real mnemonic otherwise. A histogram built from
printed names therefore counts one instruction under two labels depending on its
operands.

The failure mode is specific and nasty. When a single instruction differs -
retail's `move $a0,$zero` against a build's `addu $v0,$s1,$s3` - the `addu`
count moves by one **and** the `move` count moves by one, in opposite
directions, so a mnemonic-keyed distance reports 2 where the true distance is 0.

Three entries were checked against both keys and all three disagreed, in both
directions:

| entry | encoding | mnemonic |
| --- | --- | --- |
| `func_80012E5C` | 0 | 2 |
| `func_80029EC4` | 5 | 7 |
| `func_80056828` | 24 | 22 |

**In all three the durable note was right and the tool was wrong.** That is the
same result as the relocation bug's fourth row, and it is now a pattern rather
than an anecdote: when a tool contradicts a recorded figure, the tool is a
live suspect. Reproduce the number a second way before overwriting the note.

I did not follow that here, and the cost is worth recording. `func_80012E5C`'s
entry said distance 0; a mnemonic-keyed survey said 2; I overwrote the entry,
wrote a confident mechanism for the "missing" instruction, and shipped it. The
mechanism was right - a rotation duplicates the loop's leading instruction into
the branch delay slot - but the conclusion drawn from it was exactly backwards.
Because `move` and `addu` are one encoding, that duplication changes *no*
encoding count, which is why the true distance is 0.

**Why this one is expensive rather than cosmetic.** Distance 0 is not a neutral
number. It asserts that every instruction retail has, the build has, and that
only ordering remains - which rules out a whole class of causes and points the
next attempt at scheduling and placement. Reporting 2 instead sends someone
looking for an instruction that does not need to exist.

The fix is to derive the key from the word: opcode field, plus the function
field for SPECIAL, the sub-op for REGIMM, and the cofun for coprocessor
instructions. Relocations only patch immediate fields, so an unlinked build word
still carries the right opcode. Keep an all-zero word distinct as `nop` rather
than merging it into `sll`, so a nop standing where retail has a real shift
still shows up.

This is the same family as the `.word`-versus-`c2` artifact: **the
disassembler's naming choices leak into any measurement built on its output.**

## Argument setup is emitted at the call, and that is not addressable from C

GCC materialises a call's arguments immediately before the call. Nothing in C
moves that: there is no position between argument evaluation and the call, and
binding the value to a local - pinned or not - is constant-propagated away.

This has now blocked two functions in different disguises, which is what makes
it worth stating as a wall rather than as two separate failures.

- **`func_800283F4`, two delay-slot words.** `li $a0,3` and a constant
  assignment compete for a branch slot and a call slot. Retail needs the
  argument at the *front* of the block and the constant at the back; GCC sinks
  argument setup to the call and hoists the constant to the block top, so retail
  needs the reverse of both placements. Every position C offers for a side
  effect relative to a call was measured - before, after, inside the argument
  expression, and in the function designator - and the hoist defeats all four.
- **`func_80012E5C`, a loop rotation.** Two independent chains tie on scheduling
  priority, so `INSN_LUID` breaks it. The losing chain is the call's argument
  setup, which has the highest LUID in the block *because* it is emitted at the
  call. Retail leads the loop body with it, which means its RTL had it earlier
  than a call site can place it.

The tell is the same in both: a value that is only an argument, needed earlier
than the call, with the source offering no way to say so. Four explicit-argument
forms were measured on the second function - including locals pinned to `$a0`
and `$a1` assigned at the top of the loop body - and GCC constant-propagates
them and re-materialises at the call, which is the same result the first
function records for its channel index.

Making the local `volatile` does defeat the propagation, and costs more than it
buys in both cases, because the volatile write is then materialised as its own
instruction in the wrong place.

**What this is not.** It is not a scheduling-pass setting.
`-fno-schedule-insns` was measured against both functions and a third with the
same signature, and is worse on all three. The position is decided when the RTL
is generated, not when it is scheduled.

## Marginal independence is not joint independence: always run the joint arm

Dropping each pin in turn and finding them all inert does **not** show they are
jointly inert. Two entries audited the same way come out opposite:

| entry | pins inert *alone* | dropped **together** |
| --- | --- | --- |
| `func_80012E5C` | `i`, `off`, `slot` - all 5, identical set | still 5, identical set - genuine decoration, removed |
| `func_80046294` | `i`, `j`, `k`, `dst1`, `dst2` - all 7, identical set | **13** - all five are real |
| `func_80023144` | `rank` in both blocks - both 5, identical set | still 5 - decoration, removed |

The `func_80046294` row is the one that matters. Five pins each measure the
baseline with the *identical differing set* when dropped alone, which is exactly
what decoration looks like under a single-drop test, and together they are worth
six positions. The reason is mechanical rather than mysterious: each pin holds a
register that GCC would otherwise take for a value whose pin remains, so no
single removal changes anything and the interaction only appears once several
are gone.

**This generalises past pins.** It is the same defect as the sweeps that have
misled this campaign, stated more precisely. "Breadth is not control" says a
family of experiments can vary many things and still hold the deciding axis
fixed. This says the opposite failure: a family can vary the right axis, one
value at a time, and still miss an effect that only exists jointly. A
thirteen-register sweep moving one variable at a time, seven placements holding
two statements fixed, and five profiles holding the split axis fixed all fail
one of these two ways.

**The practice**, once a set of candidates each measure inert alone:

1. Drop them **together** and measure. This is one extra build.
2. Compare the differing **set**, not its size - two candidates agreeing on a
   count can be structurally far apart.
3. If the joint arm moves, the marginal results were not wrong, they were
   uninformative: record that the pins are jointly load-bearing so the next
   reader does not delete them one at a time on the strength of a single drop.

### Single drops are not enough either: the effect can be non-monotone

The joint arm above answers "are these pins jointly inert". It does not answer
"is there a better configuration", and on `func_80046294` there is one that no
single-drop search reaches:

| dropped | differing |
| --- | --- |
| nothing | 7 |
| `q` | 10 |
| `src_base2` | 20 |
| **both** | **5** |

Each pin is worse alone and the pair is better than either. A greedy search -
drop the best single pin, recurse - rejects both at the first step and stops at
the baseline. `func_80023144` shows the milder version of the same shape:
dropping both its pins scores 11 while dropping only the second scores 13, so
even there the greedy path stops short.

So for a small pin set, enumerate the subsets. Eight pins is 256 builds, a few
minutes, and it is the difference between 7 and 5 on this entry.

**The sweep has now been run across the close entries, and it terminates.** Two
found gains and three did not, which is the useful shape - a search that always
finds something is usually measuring noise:

| entry | pins | result |
| --- | --- | --- |
| `func_80023144` | 3 | 5 to **3** (one pin was costing two positions) |
| `func_80046294` | 8 | 7 to **5** (a non-monotone pair) |
| `func_800283F4` | 1 | no subset beats 2 |
| `func_80031874` | 4 | no subset beats 10 across all 16 |
| `func_80018FEC` | 1 | no subset beats 114 |
| `func_80012E5C` | 1 | no subset beats 5 |

So pin configuration is exhausted as a lever on every close candidate, and what
remains on each is the mechanism its entry already names.

`tools/project/candidate_pin_audit.py` runs both arms over an entry's
stored source, reporting the differing count and whether the differing
*set* is unchanged for each pin, then the joint arm over every pin that
looked inert alone.

## Re-run the closed levers after the base moves: `func_80023144` reached zero

`func_80023144` sat at five differing positions with three sections of closed
directions. One of them read, of the block-2 address cluster:

> Assigning the pointer after the `y` test **does** produce retail's empty slot,
> but costs eight elsewhere and only recovers to 8 with `side` pinned to `$5`.

That measurement was correct. It was taken against a base carrying two `rank`
pins that later proved jointly inert, and a `dst` pin that later proved to be
**costing two positions**. Removing those three moved the base from 5 to 3, and
re-running the same one-line change on the new base gives **zero** - 210 of 210,
opcode multiset identical, full-executable SHA-256 reproduced.

So a lever recorded as "costs eight" was worth an exact match three pin-removals
later, and nothing about the lever changed.

**The practical rule is narrower than "levers are base-dependent", which this
file already says.** It is: *when the base moves, the closed list is no longer
closed*, and the cheapest thing to re-run is the direction that was recorded as
**nearly working**. A direction rejected at +8 with a note explaining that it
produced the right delay slot is not the same as one rejected at +200; the first
says the mechanism is right and something else is in the way, and that something
else is exactly what a base change removes.

The sequence that got here is worth keeping because no single step found it:

1. the joint arm showed two `rank` pins were decoration and removed them;
2. re-measuring the remaining pins on that base showed a third was costing two
   positions, taking 5 to 3;
3. re-running a direction closed at +8 against the new base gave 0.

Each step was cheap. The first two were bookkeeping - removing pins that did
nothing - and neither looked like progress toward a match at the time.

## Compare `nop` counts first: a difference of one is a delay-slot decision

Before any positional analysis, count the `nop`s on each side. Filling a delay
slot *removes* a `nop`, so a difference of exactly one says a single delay-slot
decision went the other way, and the whole positional residue downstream of it
is displacement rather than disagreement.

A scan of every candidate makes the diagnostic concrete:

| delta | entries |
| --- | --- |
| `+1` (retail has one more) | `func_80023D08`, `func_80056828` |
| `-1` (the build has one more) | `func_800279BC`, `func_80029EC4`, `func_800528AC` |
| `0` | the six closest candidates, including `func_80018FEC` |

`func_80018FEC` is the instructive row. It reads `0` and is *not* a
counter-example: it has **two** opposite decisions, one each way, which cancel in
the count. So a zero delta means "no net delay-slot difference", not "no
delay-slot problem" - the count is a cheap pointer, not a proof.

`func_80023D08` matched through this. Its `+1` said one slot, positional
analysis put it at a load delay, and it turned out retail stores a field and
reads it back where the build carried the value in a register and sign-extended.

## Three changes that only pay together: `func_80023D08` 56 to 0

None of these three was sufficient and none was obviously related to the others:

1. **Assign `shift` after the call, not before.** The entry described what retail
   does correctly and attached it to the wrong statement order. 56 to 44.
2. **Put any store between a field write and reading that field back.** GCC
   otherwise carries the value and sign-extends with `sll`/`sra`; retail reloads
   with `lh`, which costs a load-delay `nop` - the missing one. 44 to 28.
3. **Pin the quotient to `$2`.** Retail keeps the glide quotient in `$v0` and the
   message constant in `$v1`; the build had the pair swapped. 28 to **0**.

The third is worth dwelling on. It is a single-register pin that closed
twenty-eight positions at once, because both remaining clusters were the *same*
swap appearing in the vertical and horizontal arms. A residue that looks like
several clusters can be one allocation decision seen more than once, and the
count of clusters is not the count of causes.

The order also mattered in a way worth recording: the pin was tried only after
the other two, and against the base they produced. Tried first it would have had
a different cost, which is the same base-dependence that `func_80023144` turned
on.

## A register pin relocates a live range; it does not split one

Register pins have been a workhorse here - per-arm pins took `func_80046294` from
18 to 8, and a single pin on the quotient closed the last 28 positions of
`func_80023D08`. That success encouraged a wrong model of what a pin *is*:
"force this value into register X". It is not that, and the difference decides
whether a pin can be used to request a redundant copy.

A direct probe, compiled with `gcc_2_8_1_g8_split`:

```c
extern int *g;
int sink(int *);

int probe_two_pins(void)            /* source and destination both pinned */
{
    register int *dst asm("$21");
    register int *src asm("$2");
    src = g;
    dst = src;
    return sink(dst);
}
```

GCC 2.8.1 emits **`lw $4,g`** and nothing else. Neither `$21` nor `$2` appears.
Both pins were discarded - not resolved in favour of one of the two named
registers, but ignored entirely, because the coalescer had already merged `src`,
`dst` and the argument into a single live range before allocation considered the
pins at all.

Change one thing, so that the source is still needed *after* the destination is
formed, and the same pins are honoured:

```c
    src = g;
    dst = src + 1;
    return sink(src) + sink(dst);   /* both live at once */
```

Now `$21` is allocated as asked and GCC emits three `move`s, including one it
introduced on its own to carry `src` across the first call.

**Pins do not create copies. Overlapping lifetimes create copies, and a pin then
chooses which register holds one of them.** A pin applies to a live range that
already exists; it cannot bring a second live range into being.

Two consequences worth carrying forward:

- **A redundant `move` cannot be requested by naming a value twice.** To
  reproduce one, the two values must be genuinely live at the same time for a
  reason the compiler can see. `func_800528AC` needs exactly this, and confirms
  the probe: pinning the source, the destination, or both produces byte-identical
  output, because GCC materialises the address straight into the pinned
  destination and never touches the other register.
- **An "inert" pin is often a pin that was never applied.** The joint-arm audit
  recorded pins that cost nothing alone but six positions together, and read that
  as an interaction between pins. The mechanism is simpler: alone, each pin lands
  on a live range that coalescing had already dissolved, so it does nothing;
  together they create the overlap that makes both binding. Before concluding a
  pin is inert, check that the register actually appears in the output.

## No GTE command instruction can currently be emitted from C

The three functions #2390 reopened into the candidate queue — `func_80033DB0`,
`func_80034830` and `func_80067220` — all need GTE *command* words. None of
them is buildable as C today, and the reason is a pipeline gap rather than
anything about their source shape. Measured end to end so the next attempt
does not rediscover it one build at a time.

`src/psyq/inline_c.h` is an authentic header written for DMPSX, so its command
macros do not contain COP2 encodings. `gte_rtps()` expands to `.word
0x0000007f` and `gte_nccs()` to `.word 0x0000107f`; those are SN assembler
markers. A probe carried them through the real `gcc_2_8_1_g0` pipeline:

| Stage | Result |
|---|---|
| GCC 2.8.1 `-S` | `nop;nop;.word 0x0000007f` |
| MASPSX 2.81 | unchanged |
| GNU as 2.42 | `0000007f  .word 0x7f` in the object |

Retail has `4a180001`. Nothing between the header and the object rewrites the
marker, so the wrong word is assembled silently — there is no error to read.
MASPSX has no option for it either; its whole flag set was checked.

**The transfers are fine and only the commands are affected.** `lwc2`, `swc2`,
`mtc2`, `mfc2`, `cfc2` and `ctc2` are real mnemonics, which is why
`gte_stopz` works in `display_object_projection.c` — the only GTE use in
accepted C, and a transfer. Reading that file as proof that "GTE works from C"
is the trap here.

**GNU as does not know the command mnemonics.** `rtps`, `rtpt`, `ncds`,
`nccs`, `ncct`, `nclip`, `avsz3` and `avsz4` are all `unrecognized opcode`
under `-march=r3000`.

**It does accept `cop2` with an immediate, and that is how the assembly
fallback works.** Splat ships `tmp/splat/include/gte_macros.inc`, whose
`cop2op` macro composes the encoding from its field arguments. Assembling all
eight families that occur in this executable reproduces the retail words
exactly:

| Command | Encoding |
|---|---|
| `rtps` | `4a180001` |
| `rtpt` | `4a280030` |
| `ncds` | `4ae80413` |
| `nccs` | `4b08041b` |
| `ncct` | `4b18043f` |
| `nclip` | `4b400006` |
| `avsz3` | `4b58002d` |
| `avsz4` | `4b68002e` |

So the gap is narrow: the encodings are reachable from GNU as, and only the
DMPSX marker words are untranslated. Closing it means rewriting the known
markers on the way to the assembler — the optional per-profile
`assembly_filter` hook already exists for exactly this kind of bridging, and
is currently unused by every profile. Do not "fix" it by editing the imported
SDK header, which is correct for the assembler it was written for.

Until then, treat a target containing any of the eight words above as blocked
at the toolchain, not at the source. That is a different conclusion from the
usual "the residual is N instructions": there is no candidate to refine,
because the command cannot be spelled at all.
## objdump hides identical runs, and a text column can drift off its bytes

Three earlier entries here record measurement bugs in the comparison itself -
section-scoped relocations, mnemonic versus encoding histograms, and discarded
REL addends. This one is different in kind and easier to miss, because it does
not touch any figure. It corrupts only the *reading*.

The diff harness renders both byte streams through `objdump -D` and prints the
disassembly beside each word. GNU objdump, by default, collapses runs of
identical bytes and prints `...` in their place. For `func_800528AC` that
removed **twelve** words from the listing while leaving all 288 in the byte
stream, so from the first collapsed run onward every line paired a word with the
text of an instruction twelve slots later.

Nothing in the output looks wrong. The addresses are still consecutive, the
mnemonics are still plausible MIPS, and the scores are entirely unaffected -
positions and multisets are computed from the words, never from the text. Only a
reader is misled, and only about *where* things are.

The damage was real. The entry for `func_800528AC` gained a description of the
loop bottom at `+0x3FC` that is actually at `+0x42C`, a claim that the function
has "two epilogues, one per return path" when it has one, and three `continue`
branches said to jump into the middle of the epilogue - which should have been
the tell, since that would be nonsense.

**Two lessons, and the second is the durable one.**

`-z` (`--disassemble-zeroes`) turns the collapsing off, and the harness now
passes it. But the fix that matters is the assertion beside it: the renderer
compares its line count against the word count and refuses to return a listing
that cannot be aligned. A silent shortfall becomes a hard error at the point of
production rather than a wrong sentence in a note weeks later.

The general form is worth stating, because it applies to every tool that pairs
two representations of the same data: **when one stream is derived from another
for human consumption, assert that they are the same length.** A derived view
that quietly drops elements is indistinguishable from a correct one right up
until a conclusion is drawn from it.

The cheap cross-check, when no assertion is available, is to resolve a branch
target by hand. `bnez v0,0x800528ec` must land on an instruction that plausibly
begins a loop body; when three separate branches appear to jump into the middle
of a register-restore sequence, the listing is wrong rather than the code.

## An incoming-register view can break post-copy equivalence

`func_8004D134` reached an exact body before its three entry instructions
matched. A volatile signed-halfword read masked back to `0xFFFF` first kept
case `0x20D` distinct from case `0x15` without changing the target `lhu`, and
moving `n = count` before the mode guard put the count copy in the guard's
delay slot. After that, every instruction following the prologue was exact.

The remaining order was:

```text
target:    move t7,a0 ; sw s0,0(sp) ; move s0,a1
candidate: sw s0,0(sp); move s0,a1  ; move t7,a0
```

Binding the saved selector to `$s0` recovered that order, but changed register
pressure enough to park the long-lived first-field adjustment in `$s2`.
Keeping that adjustment in the now-free `$a1`, staging its setup through
`$v0`, and naming the adjusted halfword value restored the exact frame and
body. One word remained because GCC still knew that `$a1` and `$s0` held the
same pointer and folded the first selector read back to `$a1`.

The exact source gives the incoming ABI register and its saved copy separate
compiler identities:

```c
register u16 *selector asm("$16");
register u16 *selector_source asm("$5");

selector = selector_source;
```

`selector_source` is intentionally an incoming-register view rather than an
ordinary initialized local. It emits the required `move s0,a1`, but GCC cannot
reuse the parameter identity for the later `lhu`, so that load stays based on
`$s0`. This is a narrow, compiler-specific boundary: use it only after source
shape has made the body exact, record it with `--allow-register-pins`, and
verify the complete linked bytes and relocations.

## Combine deletes a copy whose source is a single-use pseudo dying at it

`func_800528AC` was one `addu` short for several sessions. The missing
instruction was retail's `move $s5,$v0`, the initialiser of the loop's general
induction variable, and the search for it went through eight source spellings,
thirty compiler profiles, `-fforce-addr`, declaration order and statement order
without moving. All of that was aimed at the wrong pass.

An RTL dump settles what is happening in one command. The loop optimiser **does**
emit the copy:

```
Insn 32: giv reg 95 src reg 80 ... replaceable mult 24 add (reg/v:SI 96)
(insn 525 (set (reg:SI 258) (reg/v:SI 96)))
```

By `.combine` it has become
`(set (reg:SI 258) (lo_sum:SI (reg:SI 97) (symbol_ref "D_800F2B50")))`. The base
pointer is defined once and dies at its only use, which is exactly the two-insn
pattern the combiner collapses: it substitutes the definition into the use, and
the copy is gone before allocation runs.

**So the instruction was never missing from expansion. It was being folded away
afterwards.** The lever is therefore not how the address is spelled but whether
the pseudo holding it has a **second live use in the same basic block**.

### What counts as a live use

Three kinds do not, and each was measured:

- **A use inside the loop** is satisfied by rematerialising the address, which is
  two cheap instructions against holding a register across the calls, so the
  original pseudo still has one use.
- **A use after the loop** is rematerialised for the same reason.
- **A dead use** is deleted by flow before the combiner runs. Adding
  `sv = (s32)table;` to twelve variants changed the output of none of them.

The un-reduced giv computation is a live-looking reference that also does not
count: strength reduction rewrites the uses to the reduced register, the original
becomes dead, and flow removes it. Counting *references* in a dump therefore
misleads; only uses that survive to the combiner matter.

### What worked, and why it was free

Hoisting the loop-invariant part of an expression the function already computes:

```c
base1 = (u8 *)table + 1;
    ...
    D_8009AF9C = (s32)(base1 + off);
```

`base1`'s initialiser is a second live use of the base pseudo in the same block,
so the definition can no longer be substituted away and the copy survives. It
costs nothing, because GCC rematerialises the in-loop reference exactly as retail
does - the hoisted expression pays for itself. The candidate went from 287 of 288
instructions at opcode distance 1 to **288 of 288 at distance 0**, with retail's
instruction sequence exactly.

### The general rule

When a build is short one register-to-register move, check whether the compiler
emitted it and something later removed it, before concluding it was never
generated. `-dL`, `-dc`, `-dl` and `-dg` dump the RTL after loop, combine, local
allocation and global allocation; comparing the **contents** of the relevant insn
across those dumps names the responsible pass directly. Comparing only whether an
insn number still exists is not the same question and gave the wrong answer here
first time round.

### Why the unpinned distance-zero source stops at allocation

The `base1` source has retail's complete 288-instruction sequence, but 63 words
still differ as a rotation of four callee-saved registers. Five saved registers
already have identical occurrence counts. Retail keeps the flags word in
`$s2`, `field_0A` in `$s3`, the slot pointer in `$s4`, and the element pointer
in `$s5`; GCC assigns those values to `$s5`, `$s2`, `$s3`, and `$s4`.

The `.lreg` and `.greg` dumps bound the remaining choice. Global allocation
orders these pseudos by `floor_log2(refs) * refs / live_length`:

| pseudo | refs / live length | priority | build register |
| --- | --- | --- | --- |
| slot pointer | 42 / 98 | 2.14 | `$s3` |
| element pointer | 45 / 147 | 1.53 | `$s4` |
| flags word | 21 / 74 | 1.14 | `$s5` |

Retail's order requires the flags word to outrank the slot pointer. That means
shortening the flags live range below 39, but its read must precede the `side`
calculation and its write follows the first inner loop. The other route is
lengthening the slot pointer beyond 185, which would require reusing it for an
earlier slot lookup that retail demonstrably materialises a second time.
Making the flags value block-local also fails in the opposite direction: it no
longer crosses a call and falls into caller-saved `$v1`. The ordinary-C source
is therefore structurally complete but bounded at global allocation.

## A pin-dependent exact match for `func_800528AC`

The integrated source uses a different boundary for the table-base copy. It
writes the 32-bit address into the low member of a pinned 64-bit union and
reads that member back as the table pointer. The partial-width boundary stops
`combine` from forwarding the address expression through the GIV assignment,
so retail's `addu $s5,$v0,$zero` remains.

That source is intentionally pin-dependent: extensive measured hard-register
bindings preserve the flags, pointer, scratch, hard-zero, and statement-
expression roles needed for the retail allocation and schedule. It also gives
tracked symbol `D_800F2C40` a second C declaration:

```c
extern ModelSlot D_800F2C40_alias[] asm("D_800F2C40");
```

The second declaration prevents CSE between two lookup materialisations. Its
assembler name is the real tracked symbol, not an expression or fabricated
linker name. `--allow-symbol-aliases` accepts only that narrower form.

The pins are not a substitute for the recovered behavior. The function still
implements the ten-entry tint queue, three-channel interpolation, temporary
part-position override, draw, restoration, and clock update established by
the unpinned reconstruction. The exact source applies register constraints
only after that lifecycle and the complete instruction shape were understood.

## Levers kept from three candidates migrated into the build

`func_80048658`, `func_800482B0` and `func_8005E808` moved from
`notes/candidates/` into `src/candidates/` (#2493). Their per-function state
now lives in the source comment and the inventory row; these are the parts
that transfer to other functions.

**Write a shared call tail out at each site instead of reaching it by
`goto`.** Both early paths of `func_800482B0` end in the same seven-argument
call. A single labelled block reached by `goto` puts that block at the *end*
of the function; writing the call out in each loop body lets cross-jumping
merge the common suffix and lands the merged block where retail has it. Worth
48 positions on its own, 114 down to 66, and it is what puts the two spill
slots at retail's 0x22 and 0x28. The same shape pays in `func_8005E808`,
where retail has two copies of the compare-and-store tail and a single copy
after the `switch` is 14 positions worse.

**A stack-passed argument stays in its home slot, and where you copy it out
matters.** GCC leaves a stack argument whose nominal and passed modes agree in
memory: every use re-reads it, and byte and word loads each cost a delay-slot
`nop` too. `md = mode;` gives it a pseudo. Putting that assignment *before*
the guard rather than after it moves the load into the entry block where the
scheduler covers its delay. A `u8` parameter needs no such copy - the
narrowing conversion already forces a pseudo.

**Switch case bodies are emitted in source order, so write them in retail's
layout order.** `func_8005E808`'s blocks run 0x80/0x81, then 1, then 4.
Writing them in numeric order costs 93 positions on its own, 223 down to 130,
because every block lands at the wrong address and the dispatch tree inverts
with it. Read the target's block order off the labels before writing the
switch.

**A cast that looks free can cost a register.** Retail's `func_800482B0`
recomputes `(u16)id` inside its second loop. Writing the cast makes it
loop-invariant, GCC hoists it into the preheader, and that tenth long-lived
value against nine callee-saved registers evicts `mode` and takes the whole
function from distance 4 back to 18. Dropping the cast is one instruction
short locally and 14 opcodes better overall. Weigh a hoist against the
register file, not against the instruction it saves.

**Signedness of the compared value picks `sltu` over `slt`, and can carry a
`multu` with it.** `func_8005E808`'s radius has to be `u32`: an `s32` gives
`slt` and, in case 4, also drops the unsigned `multu` that the `/ 4096000`
needs.

**Two counters holding the same value are still two variables.** Both scan
loops in `func_800482B0` step an `s32` envelope index and a `u8` slot index;
retail masks the second at every use, so folding them into one variable
cannot reproduce it.

**`-fno-schedule-insns` is a diagnostic, never a profile to ship.** For
`func_80048658` it produces retail's entry-copy order and then re-allocates
the whole body: 66 instructions against 68 and 51 differing positions. Use it
to identify which pass owns a difference - here, that the two exchanged entry
copies are a first-pass scheduling decision that source order cannot reach,
because `rank_for_schedule` only falls back to original insn order when
priority and dependence class tie. Same conclusion the `func_80045208` entry
reaches about `-fno-schedule-insns2`.

## SD_SEPlay: preserving the entry schedule without undefined reads

`SD_SEPlay` (`0x80048658`) now matches all 272 text bytes under the unchanged
`gcc_2_8_1_g0` profile. The previous two-word entry-copy residual is resolved
by two identical initialization blocks and an intermediate `u16` stop value.
The blocks test the initialized input's `0x8000` stop bit. Flattening them
changes four words at offsets `+0x08`, `+0x0C`, `+0x10`, and `+0x14`.
No statement-level assembly or scheduling flag override is used.

The search first produced two exact machine-code candidates whose redundant
conditions read uninitialized locals. Those sources were rejected for
integration. Replacing the condition with the input stop-bit test preserves
all bytes and removes that read; both paths assign the same values before
any subsequent use. Testing plain `arg0` instead costs two instructions,
so not every equivalent spelling has the same code-generation effect.

The accepted source uses the shared `SDValue` type and existing callee
prototype from `sound_output_state.h`. The one retained byte-based state
lookup is measured: spelling it from `&a->field_044C` changes one word at
`+0x68`, despite the same address and size. The wider local declaration of
the unmatched `func_800482B0` call is unchanged from the existing candidate.
Caller-side `SD_SEPlay` declarations remain profile-specific; this change
does not unify them.

The complete `make match` build, using this C object at `0x80048658`,
reproduces the retail executable byte for byte with SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.

## When a local struct can be replaced by the Psy-Q type it copies

Issue #16 asks for the SDK's runtime structures instead of redefined ones, and
layout equality is not sufficient. The test is **whether anything writes two
adjacent members as one word.**

- `fade_draw_overlay.c`'s `FadeBox` is `GsBOXF` field for field, and retail
  writes the `0x04` and `0x08` words whole -- x together with y, w together
  with h. Those are two separate members each, so the struct can become
  `GsBOXF` and the cast moves to the two whole-word stores.
- `func_80040588.c`'s `SpritePrim` is `GsSPRITE` field for field, but its
  position and size words each span two `GsSPRITE` halves. There is no store
  to cast, so the local struct has to keep its union-shaped members and the
  swap is a codegen change, not a rename.

Same symptom, opposite conclusion: check where the whole-word store lands
before assuming a layout-identical struct is convertible.

## The display object's `+0x4` word is a libgs attribute, and libgs.h names the bits

`GsALON` (`1<<30`), `GsAONE` (`1<<28`), `GsATWO` (`2<<28`), `GsROTOFF`
(`1<<27`), `GsPERS` (`1<<26`), `GsDOFF` (`1<<31`). So the composites the tree
spelled as literals are `0x50000000` = `GsALON | GsAONE` (additive),
`0x60000000` = `GsALON | GsATWO` (subtractive), `& 0x8FFFFFFF` =
`& ~(GsALON | GsATWO | GsAONE)` (semi-transparency off) and `& 0xF7FFFFFF` =
`& ~GsROTOFF` (rotation on). Swapping the literal for the macro is
codegen-neutral across all five overlays and the resident build.

Two cautions. `libgs.h` does not parse on its own: it needs `libgte.h` and
`libgpu.h` ahead of it, in that order, and a file that already included one of
them further down will fail if the new `libgs.h` include goes above it. And
not every 32-bit write to a `+4` field is an attribute: `0x1000000` (bit 24)
and `0x2000000` (bit 25) have no name in `libgs.h`, and
`file_set_position_table.c`'s `*(s32 *)D_800E9DF0 = 0x8000000` is not a
display object at all.

## A narrower parameter type is not free at the call site, but a wider one is

`func_80040410` was defined as `(DisplayObjectConfig *object, u8 value)` while
its seven consumers each declared it themselves, five of them spelling the
second parameter `s32` or `int`. Giving them all one prototype meant choosing
a spelling, and the two directions are not symmetric.

Narrowing to `u8`, which is what the definition said, costs an instruction at
some call sites but not others. Measured one file at a time:

- `func_80029108.c` passes a local whose value is either the constant 2 or an
  `lbu` of a `u8` field. GCC 2.8.1 can see the range is already 0..255 and
  emits nothing extra; the build stays byte-exact.
- `func_8003B378.c` passes `n`, whose range the compiler cannot prove. The
  build breaks at VRAM `0x8003B484`, where the expected `addu` (`0x21`)
  becomes `andi $a1, $a1, 0xFF`. The truncation the prototype now demands is
  emitted at the call, not inside the callee.

Widening in the other direction is free. Changing the *definition* to
`(DisplayObjectConfig *object, s32 value)` keeps the full executable
byte-exact, because the body's only use of the parameter is
`object->field_69 = value`, an `sb` that truncates regardless of the declared
width.

So when a definition's narrow parameter disagrees with its callers' wider
declarations, widen the definition rather than narrowing the declarations. The
generalisation to watch for: any prototype change that makes an argument
narrower than `int` is a call-site codegen change wherever the compiler cannot
prove the range, and it will be invisible in the files where it can.

## Completing a small array's type is a -G8 addressing change, not a comment

`tmp/quality/scan_conflicting_decls.py` groups array-against-array differences
as the safe class to unify, and its docstring warns only about array against
scalar. That boundary is in the wrong place: an *incomplete* array and a
*complete* small one are also different addressing decisions.

`D_8015C410` was declared `extern s8 D_8015C410[]` in `func_8003A560.c` and
`extern s8 D_8015C410[5]` in `menu_record_reset.c`. Adopting the bounded
spelling in the first file does not merely add information. Five bytes is
under the `-G8` threshold, so once the type is complete GCC treats the symbol
as small data and emits a gp-relative reference, and the link fails outright:

    (.text+0x260): relocation truncated to fit:
        R_MIPS_GPREL16 against `D_8015C410'

The symbol is not in the small-data region the `$gp` window covers, so there
is no offset that works. The incomplete spelling is load-bearing: it is what
keeps GCC from making that choice.

The size is what decides it, not the presence of a bound. In the same pass
`D_800EAF08` went from `u8 []` to `u8 [DUEL_EFFECT_OCCUPANCY_COUNT]`, which is
240 bytes, comfortably above the threshold, and the executable stayed
byte-exact. So:

- bound added, array larger than `-G8`: free, and worth doing for the reader.
- bound added, array 8 bytes or smaller: an addressing change, and usually a
  link error rather than a silent mismatch, which at least fails loudly.

A redundant declaration that is merely *duplicated* rather than differently
spelled is free to delete either way. `func_80040588.c` carried
`extern DisplayObject D_800EFE48[]` while already including `display_object.h`,
which declares the same symbol with its named capacity; the two are compatible
types, and dropping the local line changed nothing.
## The redundant-redeclaration class has one real member and three traps

A natural survey for #2501 is: find a C file that declares a global its own
included header already declares, and delete the local copy. Run against
`src/game` it produces very few hits, and which few depends on details of the
survey that are easy to get wrong. Writing the whole result down is worth more
than the one deletion it yields.

**The one real member.** `func_800528AC.c` declared `extern ModelSlot
D_800F2C40[]` while already including `model.h`, which declares the same symbol
as `ModelSlot[MODEL_SLOT_COUNT]`. Compatible types, the array is far above the
`-G8` threshold, and deleting the local line keeps the executable byte-exact.
That file is otherwise full of load-bearing spellings -- a `register const u32
hard_zero asm("$0")`, and a second name for this very symbol via `extern
ModelSlot D_800F2C40_alias[] asm("D_800F2C40")` -- so the redundant line looked
as deliberate as its neighbours and had to be measured rather than assumed.

**Three traps, each a different kind.**

`func_80024E58.c` / `gDuel_bTerrain` is not a declaration at all. The survey
believes `duel_terrain_boost.h` declares this symbol; it declares only
`gDuel_aTerrainBoost` and `Duel_GetTerrainBoost`. What a regex finds is the
header's own comment, which lists five example spellings while explaining why
they differ. Strip comments before scanning and it disappears. The real
declaration is load-bearing besides: that file compiles at `-G8` but assembles
at `-G4`, and the header records that relaxing its `[8]` to `[]` costs four
bytes of text.

`save_data_checksum.c` / `gSaveData_dwMaskStateLow` and
`gSaveData_dwMaskStateHigh` are genuinely declared twice, on purpose.
`save_data.h` wraps its pair in `#ifndef SAVE_DATA_DECLARE_MASK_STATE_LOCALLY`
and the consumer `#define`s that macro immediately before including the header,
so it can keep its own declarations further down the file, where the comment
says the source position preserves GCC 2.8.1's allocation in
`SaveData_NextMaskWord`.

**What the survey has to get right to see all four.** It must strip comments,
or `gDuel_bTerrain` appears and `D_800F2C40` is all that is left to find by
luck. It must treat `T[]` and `T[N]` as the same declaration, or `D_800F2C40`
never appears at all. And it cannot see a guard macro that switches a header
declaration off, so deliberate duplication and accidental duplication look
identical to it and the `save_data` pair must be read by hand. A first pass of
mine got two of those three wrong and concluded the class was empty.

## A caller may pass an argument the matched callee does not take

`func_80049C40` is matched, exactly, with `gcc_2_8_1_g0`, and its definition in
`sound_secondary_playback.c` is:

    void func_80049C40(void)

The body reads `D_8009B458` and no parameter. Yet all three of its callers
declare it as taking one, and pass one:

    extern void func_80049C40(s16 a0);   func_80049010.c
    extern void func_80049C40(s32);      sound_output.c
    extern void func_80049C40(s16);      sound_runtime.c

    func_80049C40(g_SDValue->field_157E);

That reads like three files getting the same prototype wrong, and it is the
opposite. Retail's call sites compute `g_SDValue->field_157E` and put it in
`$a0` before the call; the callee ignores it. The caller's declaration is the
only thing keeping that computation alive.

Measured, by making `sound_runtime.c` agree with the definition -- declaration
to `void (void)` and the call to `func_80049C40()`, which is exactly what a
#2495 sweep would do:

    error: rebuilt executable has size 0x1d07f0, expected 0x1d0800

Sixteen bytes, four instructions, from one call site. Dropping the argument
does not just remove the argument move: the whole `g_SDValue->field_157E` load
chain becomes dead and GCC deletes it too.

So "the definition takes `void`" is not a reason to correct a caller that
passes something. The two questions are separate: what the callee reads, and
what the retail call site sets up. This is the mirror of the
`func_8004036C`/`func_8004CB0C` case, where a caller passes *no* argument to a
function that takes one; the same rule covers both, which is that a call site's
argument list is retail's, not the callee's.

Before unifying any prototype under #2495, check whether the callers agree with
each other rather than whether they agree with the definition. Here all three
agree that there is one argument, and only disagree about its width -- `s16`
against `s32` -- which is the part that is actually open.

### Measured: the same holds for func_80049CB0 and func_800498F8, three ways

The `func_80049C40` entry above guessed that its two neighbours in the same
sound files were load-bearing for the same reason. They are, and the three
measurements fail differently, which is worth having on record because only
one of the three looks like the failure you would expect.

`func_80049CB0`, dropping `g_SDValue->field_157E` at `sound_output.c`:

    error: rebuilt executable has size 0x1d07f4, expected 0x1d0800

Twelve bytes. Same shape as `func_80049C40`: the argument's load chain dies
with it.

`func_800498F8`, dropping `value` at `sound_output.c`:

    ld: section .initialized_data VMA [800906e0,8009b08f]
        overlaps section .text VMA [800129d8,800906e3]

Text got *longer*, not shorter, and ran into the next section. Removing an
argument is not reliably a removal: it changes what the register allocator
does with the surrounding code, and here it cost four bytes rather than
saving any.

`func_800498F8`, dropping the CONSTANT `0` at `func_80049010.c`:

    error: mismatch at file offset 0x398b4, VRAM 0x800490b4:
        expected 0x21, got 0x00

Same size, one instruction changed: the `addu` that materialised `$a0` became
a `nop`. This is the important one. The argument here costs nothing to
compute, so there is no dead load chain to lose, and the edit still breaks the
match. What the declaration preserves is the *call sequence*, not the expense
of the value -- retail sets `$a0` before this call and the C has to as well.

So the rule does not depend on the argument being interesting. Four measured
call sites across three functions now, and the failure was a shrink, a growth
and an in-place substitution respectively; a sweep that only watched the size
would have caught two of the three.

## Thirty-nine matched functions are called with the wrong number of arguments

Prompted by `func_80049C40` above, I checked the whole tree rather than the
one function: for every matched function with a definition in `src`, does any
consumer declare it with a different argument count? There are thirty-nine
such pairs. That is not a backlog of bugs. It is the size of the class that a
#2495 prototype sweep would silently destroy, so it is worth having the list
before anyone runs one.

Both directions occur, for different reasons.

**A caller declares FEWER arguments than the definition takes** (25 pairs).
The call site sets up only the arguments it names, and the callee reads the
rest from whatever the registers happen to hold. `func_8004036C` in
`free_duel/sparkle_runtime.c` is the documented example: declared `void
(void)`, defined `void (void *)`, called with nothing. These cannot be
"corrected" by writing the missing argument, because there is no expression
in the caller that produces it.

    Duel_LoadPackageStage  def 2 (duel_load_package_stage.c)  <-  decl 0 in func_8001798C.c
    Duel_LoadPackageStage  def 2 (duel_load_package_stage.c)  <-  decl 0 in func_800179F4.c
    func_80013154          def 1 (main_services.c)  <-  decl 0 in main_init.c
    func_80017F04          def 3 (duel_card_display_state.c)  <-  decl 1 in func_80018004.c
    func_80019B2C          def 1 (func_80019B2C.c)  <-  decl 0 in func_80019BA0.c
    func_80020BE4          def 2 (func_80020BE4.c)  <-  decl 0 in func_80020F4C.c
    func_80022EEC          def 1 (func_80022EEC.c)  <-  decl 0 in display_parent_links.c
    func_8002348C          def 1 (duel_field_display_objects.c)  <-  decl 0 in func_80023D08.c
    func_80023D08          def 2 (func_80023D08.c)  <-  decl 1 in duel_cursor_status.c
    func_800289BC          def 2 (func_800289BC.c)  <-  decl 0 in duel_effect_resource_setup.c
    func_8002A9C0          def 2 (func_8002A9C0.c)  <-  decl 0 in func_8002ABB4.c
    func_8002C604          def 1 (func_8002C604.c)  <-  decl 0 in func_8002C68C.c
    func_8002FB78          def 2 (func_8002FB78.c)  <-  decl 0 in func_8002FD10.c
    func_80032184          def 2 (func_80032184.c)  <-  decl 0 in duel_reward_setup.c
    func_8003C328          def 2 (func_8003C328.c)  <-  decl 0 in func_8003C498.c
    func_8004036C          def 1 (func_8004036C.c)  <-  decl 0 in sparkle_runtime.c
    func_80041D60          def 3 (func_80041D60.c)  <-  decl 1 in func_80040814.c
    func_80041D60          def 3 (func_80041D60.c)  <-  decl 1 in func_80029108.c
    func_80041D60          def 3 (func_80041D60.c)  <-  decl 1 in screen_runtime.c
    func_80043230          def 4 (display_object_interpolation.c)  <-  decl 3 in mem_card_dialog_runtime.c
    func_80043328          def 2 (func_80043328.c)  <-  decl 0 in func_80043960.c
    func_800434F4          def 2 (func_800434F4.c)  <-  decl 0 in func_80043960.c
    func_8005B64C          def 2 (func_8005B64C.c)  <-  decl 0 in file_request_main_menu_package.c
    func_8005CEF0          def 1 (func_8005CEF0.c)  <-  decl 0 in model_packet_handlers.c
    func_80060B38          def 2 (func_80060B38.c)  <-  decl 0 in func_80061008.c

**A caller declares MORE arguments than the definition takes** (14 pairs).
The retail call site computes and passes a value the callee ignores, and the
declaration is what keeps that computation alive. `func_80049C40` is the
measured example: making one of its three callers agree with the definition
removes four instructions, because the argument's whole load chain becomes
dead. Note that `func_800498F8` and `func_80049CB0` sit beside it in the same
two sound files with exactly the same shape, so the pattern is a property of
that call sequence rather than a one-off.

    SD_StartSequenceTracks def 0 (sound_sequence_timing.c)  <-  decl 1 in sound_secondary_playback.c
    func_80018004          def 1 (func_80018004.c)  <-  decl 3 in func_8001BAF0.c
    func_80018004          def 1 (func_80018004.c)  <-  decl 3 in duel_phase_entry.c
    func_80024088          def 1 (duel_cursor_status.c)  <-  decl 2 in func_8001D5B4.c
    func_80041C8C          def 1 (func_80041C8C.c)  <-  decl 4 in func_80041D60.c
    func_80049120          def 0 (sound_sequence_state.c)  <-  decl 1 in duel_effect_state_callbacks.c
    func_800498F8          def 0 (sound_secondary_reset.c)  <-  decl 1 in func_80049010.c
    func_800498F8          def 0 (sound_secondary_reset.c)  <-  decl 1 in sound_output.c
    func_80049C40          def 0 (sound_secondary_playback.c)  <-  decl 1 in func_80049010.c
    func_80049C40          def 0 (sound_secondary_playback.c)  <-  decl 1 in sound_output.c
    func_80049C40          def 0 (sound_secondary_playback.c)  <-  decl 1 in sound_runtime.c
    func_80049CB0          def 0 (sound_secondary_playback.c)  <-  decl 1 in func_80049010.c
    func_80049CB0          def 0 (sound_secondary_playback.c)  <-  decl 1 in sound_output.c
    func_8004B374          def 2 (func_8004B374.c)  <-  decl 3 in sound_sequence_events.c

Four of the thirty-nine have been measured, all in the "more" direction and
all in the two sound files above: func_80049C40, func_80049CB0 and
func_800498F8 at two separate call sites. The remaining thirty-five are
unverified in either direction. An entry here means the declaration and the
definition disagree, not that the disagreement has been shown to be
load-bearing, and not that it is safe to correct either. Treat the list as the
set that needs measuring before it is touched.

The measured four are also the reason to be careful about how a sweep checks
itself: those four failed as a size shrink, a section overlap from text
growing, and an in-place instruction substitution at unchanged size. Only the
first two show up in a size comparison.

The practical rule, repeated from the `func_80049C40` entry because this is
where someone will look for it: when unifying a prototype, check whether the
callers agree with each other, not whether they agree with the definition.


## A caller's return type disagrees with the definition forty-nine times

The companion to the arity inventory. Same method, applied to the other half of
the signature: for every matched function defined in `src`, does any consumer
declare a different return type? Forty-nine pairs do.

They are not one phenomenon, and the difference decides whether correcting one
is free or a regression.

**The result is discarded** (most of them). `func_80029164` is declared `void`
by five consumers against a `FileTransferDescriptor *` definition;
`func_80040510` by five against `DisplayObjectConfigView *`. Nobody reads `$v0`,
so the declaration is misinformation rather than a lever. This is the class the
`sound_voice_selection.h` review already called out: the build cannot tell you
about a return type nobody uses, so only reading the definition finds these.

**A `void *` definition with typed consumer declarations.** `func_800591FC`,
`func_80059208` and `func_80059520` are each defined `void *` and declared
`unsigned short *` or `u8 *` by their callers. Nothing complains because
`void *` converts silently to any of them, so every spelling "works" and none
is checked. No diagnostic exists for this one at all.

**A narrower return that is load-bearing.** `func_80049F50` is defined `s32` in
`sound_secondary_playback.c` and declared `s16` in `sound_runtime.c`, where the
result is compared:

    if (... func_80049F50() != 1)

Widening that declaration to the definition's `s32` -- which is what
"correcting" it means -- gives

    error: rebuilt executable has size 0x1d07f8, expected 0x1d0800

Eight bytes. The `s16` forces the value to be narrowed before the comparison,
and the `sll`/`sra` pair that does it is retail's.

That last one is worth putting beside `func_800181EC`, where the identical
`s16`-against-`int` disagreement is free. The difference is not the types, it
is what the caller does with the value: `func_800181EC`'s three callers all
store the result into a 16-bit field, so the `sh` truncates regardless and the
narrowing costs nothing. `func_80049F50`'s caller compares it, so the narrowing
has to be materialised.

So the rule for return types has the same shape as the one for arguments. A
declaration that disagrees with its definition is not automatically wrong, and
whether it can be corrected depends on the consumer, not on the definition.
Checked while compiling this list: none of the four functions defined `void`
but declared with a value type -- `func_8003A440`, `func_8003A920`,
`func_80040424`, `func_8004A27C` -- has a caller that actually reads the
result. There is no case in the tree of a caller consuming a return its callee
never produces.

## Three things a declaration survey does not see, and one it invents

Three separate notes above warn that a regex survey of declarations under-
reports. This one names the specific causes, because each of them changed a
conclusion rather than just a count.

The survey in question is the one that drives #2874 and #2495 work: which
globals are declared `extern` in a C file, defined in no C file, and absent
from every header?

Three ways it misses a declaration that is really there:

  - A declaration inside a header comment is counted as code.
    `duel_terrain_boost.h` lists five example spellings of `gDuel_bTerrain`
    while explaining why they differ, and a scan reads all five.

  - A declaration inside `#ifdef` is invisible as a declaration and invisible
    as a guard. `mem_card.h` wraps a pair in
    `#ifndef GMEMCARD_NIORESULT_IS_VOLATILE`, so "declared twice" and
    "declared twice deliberately" look identical, and inserting into that
    region silently hides the new declaration from the consumers that do not
    define the macro.

  - A declaration whose type and name are on different lines is not seen at
    all. This is the one that changes conclusions rather than counts.
    `D_80090800` was reported as an unmatched global with four unanimous
    consumers. It is neither unmatched nor unanimous: `duel_field_layout.c`
    defines it across two lines, and two more consumers declare it
    two-dimensionally, one of those also wrapped.

And one way it reports a definition that is not there. Deciding "is this
symbol defined in C?" with a regex for `<type> <name>` invites the type
position to match a keyword. A scan that read

    return D_8009B3EF;

as a definition of `D_8009B3EF` reported two homeless symbols as having
owning translation units, which would have routed both into the wrong
header. Anchoring on statement boundaries is not enough; the check has to
know it is at file scope, and has to exclude `return`, `case` and `goto`
from the type position.

And a second way, found later and worth separating from the first because it
manufactures conflicts rather than owners. One `extern` statement may carry
several declarators:

    extern u8 D_8009B368[9], D_8009B362[9], gCampaignSceneIndex;

A regex that captures "everything between `extern` and the symbol" as the type
reads the trailing part of that line as a spelling, and reports
`gCampaignSceneIndex` as declared both `u8` and
`u8 D_8009B368[9], D_8009B362[9],`. Two symbols were listed as disputed on
that basis and neither is: every declarer agrees. The scan has to split on
commas inside the statement before it decides anything about a type.

And a third way, which is neither a miss nor an invention but a silent merge
of two spellings that differ. A trailing attribute sits AFTER the declarator:

    extern s16 gDuel_wSelectedCardID __attribute__((section(".data")));
    extern s16 gDuel_wSelectedCardID;

A scan that treats "everything between `extern` and the name" as the type sees
`s16` for both and reports the symbol as unanimous. gDuel_wSelectedCardID has
three plain declarers and three `.data` ones and came back clean.

That is the worst of the three failures here, because the attribute is exactly
what the note below on addressing groups turns on. A scan blind to it keeps
offering symbols that are two addressing groups wearing one spelling, and the
rule looks wrong rather than the tool. Capture the text after the declarator
as part of the spelling, not just the text before the name.

The three failure directions matter differently. A miss leaves work undone and
is discovered by someone else later. An invention sends you to reconcile a
disagreement that does not exist, and the reconciliation is a source change
that has to be justified to a reviewer. A silent merge is worse than both: it
proposes a change that looks safe, and the thing it hid is the thing that
would have stopped you.

The `D_80090800` case is worth following to the end, because the wrong survey
led to the wrong plan. On the four-consumer reading it looked like a symbol
needing the two-arm `#ifdef` treatment `D_800907D8` has, one arm per shape.
The complete reading shows the four flat consumers all cast to `u8 *` at the
point of use, so a single declaration in the record's real two-dimensional
shape serves every consumer and no guard is needed at all.

Deliberately absent here: a total. Successive parsers of this tree have
produced very different population counts, and each jump came from fixing a
bug in the previous parser rather than from new evidence, so no count has yet
earned a place in this file. Treat a survey as a way to generate candidates,
and verify by hand every symbol you are about to touch.

## A signed read can be spelled three ways; only the unsigned one differs

Twelve globals in the tree are declared with two spellings that differ only in
signedness. `gCardGrid_bCursorColumn` and `gCardGrid_bCursorRow` are the first
pair measured, and the answer is not the one the shape of the problem suggests.

Two sources use them. `func_8002A788.c` declares them `s8` and reads them
straight into an `s32`. `func_8002BFCC.c` declares them `u8` and writes
`(s8)gCardGrid_bCursorColumn` at each use. Editing only `func_8002BFCC.c` and
leaving the other alone -- it carries hand-written
`.reloc .-4, R_MIPS_GPREL16` directives naming these symbols, so touching it
would confound the result -- gives four cases:

    extern u8  + (s8) cast     matches   (what master had)
    extern s8  + (s8) cast     matches, object byte-identical
    extern s8  + no cast       matches, object byte-identical
    extern u8  + no cast       FAILS, executable 16 bytes short

The fourth case is the control, and it matters: without it, three passes in a
row would equally well be explained by the edit never reaching the build.

So the declaration's signedness is not itself load-bearing here. What the code
requires is that the read be *signed*, and three different spellings express
that, all compiling to the same instructions:

      lbu  v1,0(gp)
      sll  a1,v1,0x18
      sra  a0,a1,0x18

GCC 2.8.1 loads unsigned and sign-extends by shifting even when told `s8`,
because the shifted value is a shared subexpression -- the following
`sra v0,a1,0x1f` reuses `a1` to get the sign. That is why removing the cast
costs nothing, and why only the genuinely unsigned read generates different
code.

The useful consequence: a conflict of this shape is resolvable rather than
load-bearing, and it resolves toward the spelling that states the requirement
once. Both sources now say `s8` and neither casts.

Do not generalize this to the other eleven without measuring them. The rule
recorded above for return width was that the consumer decides, not the
definition, and this pair has a consumer that was already casting. A pair
whose declarers both read the symbol directly is a different shape and may
well answer differently.
## Three more signedness conflicts, and the failure signature when one is real

Following the `gCardGrid_*` measurement above, three more of the twelve
signedness conflicts resolve. Each is a different shape, and the shape is what
predicts the answer.

    D_8009B079   func_8005F91C.c u8, model_transfer_state.c s8
                 Both declarers only ever WRITE it, and only constants:
                 `= 1` and `= 0`. Nothing reads its sign, so no load is
                 generated that could differ. Both the mixed spelling the
                 tree had and a unified u8 match.

    D_8009B33C   duel_effect_play_sound_command.c u16, duel_effect_state_callbacks.c s16
                 The s16 declarer does `D_8009B33C--` and then tests
                 `D_8009B33C > 0`, which is a genuinely signed comparison:
                 decrementing past zero gives -1 under s16 and 65535 under
                 u16. The u16 declarer only assigns. Resolves to s16.

    D_8009B35A   func_80039794.c s16, text_box_build_step.c u16
                 The s16 declarer reads it as an index; the u16 declarer only
                 assigns to it. Resolves to s16.

The pattern across all four measured pairs is the one already recorded for
return width: THE CONSUMER DECIDES. A declarer that only stores to the symbol
does not constrain its signedness and can be changed freely; a declarer that
loads and then compares or sign-extends is the one holding the requirement.
Resolve toward the reader's spelling.

The control is worth recording for its signature. Changing the READER of
D_8009B33C from s16 to u16 fails like this:

    mismatch at file offset 0x28363, VRAM 0x80037b63:
    expected 0x87, got 0x97; size 0x1d0800/0x1d0800

One byte, and the size does not move. 0x87 and 0x97 are `lh` and `lhu`: the
signed and unsigned halfword loads. This is the third instance of the
size-unchanged failure class noted earlier in this file, and it is the reason
a sweep that only compares executable size is not enough to clear a
declaration change.

## Element width follows the same rule as sign, but the control is louder

`D_801845C0` in the main-menu value-setup screen was declared `u8 []` by two
sources and `u16 []` by two others. That looked like a harder question than
the twelve signedness pairs, and it was recorded as one in `value_setup.h`,
because element width changes index scaling as well as the load: `x[6]` is
byte 6 under `u8` and byte 12 under `u16`. Two spellings that disagree about
width are, on the face of it, addressing different bytes.

Reading the uses dissolves it. Only two of the four sources ever index the
symbol, and both of those spell it `u16`:

    update_value_setup.c   [0] [1] [6] [7], compared and stepped
    value_setup_visuals.c  [1] and [7], read

The other two never index it at all. Both take its address as bytes:

    finish_value_setup.c   u8 *state = D_801845C0;
    start_value_setup.c    state = D_801845C0;

(All four of those sources are now `value_setup.c`, coalesced under #39. The
file names above are the ones the question was reasoned about under, and are
left as they were; the conclusion is unaffected, since one unit carries the
single `u16 []` spelling this section arrived at.)

So the `u8 []` spelling was never a claim about the element width. It was an
addressing device, the same class as `char D_8009B104[1]` in
`file_transfer.h`, and it did not conflict with the `u16` view -- it just
declined to describe it.

Unifying on `u16 []` and writing `(u8 *)D_801845C0` at the two address-taking
sites builds byte-identical, resident image and overlay both.

The control matters more here than in the signedness cases. Putting an
*indexing* declarer on the wrong width -- `update_value_setup.c` back to
`u8 []` -- fails with

    error: main_menu: rebuilt module does not match its input

which is what you would expect when `[6]` and `[7]` start addressing bytes 6
and 7 instead of 12 and 14. Sign gets you a different load instruction; width
gets you a different address. So the rule is the one already recorded --
the consumer decides, and a declarer that does not consume the elements does
not constrain them -- but the cost of getting it wrong is larger, and a
declarer that only takes an address must be recognised as abstaining rather
than voting.

## The oversized array and the .data attribute are one device, not two

Several notes here and in the headers record that a symbol sometimes has to be
kept out of -G8 small data so the assembler reaches it with `lui %hi` / `%lo`
instead of gp-relative. Two spellings do that, and they have been treated as
separate tricks:

    extern s8 X[9];                                    /* read as X[0] */
    extern s8 X __attribute__((section(".data")));     /* read as X   */

They are the same device. `gDuel_bOpponentID` is declared all three ways
across the tree -- plain in four sources, `[9]` in three, `.data` in two --
and swapping one for the other is free. In `func_80019CC8.c`:

    extern s8 gDuel_bOpponentID[9];   ... gDuel_bOpponentID[0] >= 0   matches
    extern s8 gDuel_bOpponentID .data ... gDuel_bOpponentID   >= 0    matches
    extern s8 gDuel_bOpponentID;      ... gDuel_bOpponentID   >= 0    FAILS

The failing case is the control and it is the familiar one: 0x1d07fc against
0x1d0800, four bytes short, the %hi/%lo pair collapsing into a single
gp-relative load.

Why the array form works at all: nine bytes is over the -G8 threshold, so the
object is not small-data eligible, and `[0]` then reads the byte at the base.
The bound is not a size claim -- it is the smallest number that clears eight.
`gSD_bOutputType` is spelled `s8 [16]` in one source and `u8 [9]` in another
for exactly this reason, and `options_init.c` says so in as many words: "an
oversized array extern to force absolute (lui+lbu) addressing instead of
gp-relative".

What this is worth: a symbol that looks like it has four incompatible
spellings usually has two addressing groups with two spellings each. Sort the
declarers into small-data and absolute before concluding anything about the
type. gDialog_bChoice, for instance, has eleven declarations in four
spellings, which reduces to plain-vs-absolute plus one genuine sign question
in dialog_read_choice_input.c -- a much smaller problem than the count
suggests.

## Whether a divergent declarer blocks a header move is one question

The note above sorts declarers into addressing groups. This is the operational
consequence, because getting it wrong is cheap in one direction and expensive
in the other, and both have happened here.

When a symbol has a plain group and an absolute group -- `.data` attribute or
oversized array -- the question is NOT "does a divergent declarer exist". It
is:

    does any file carrying the divergent spelling include the target header?

If none does, the two declarations never meet. A plain declaration can go in
the header for the small-data group, the divergent files keep their own, and
no guarded arm is needed. fade.h does this for D_8009B141, and mem_card.h for
D_8009B3D4, whose `.data` declarer func_8002D458.c does not include it.

If any does, the header needs a guarded pair and every file in that group has
to select its arm. That is a different size of change, and it drags in every
consumer rather than the ones being tidied.

Both mistakes have been made in this campaign:

  Too cautious   D_8009B3D4 was excluded from mem_card.h because
                 func_8002D458.c named it with a .data attribute. That file
                 does not include mem_card.h, so there was nothing to
                 collide with and the exclusion cost a round.

  Too eager      gDuel_wSelectedCardID looked like a three-file move, since
                 its three plain declarers were free. Its three `.data`
                 declarers all include duel_card.h, so a plain declaration
                 there would have broken them. Caught by checking, not by
                 building.

The check is one grep per divergent declarer and it settles the question, so
run it before deciding rather than after the build fails.
