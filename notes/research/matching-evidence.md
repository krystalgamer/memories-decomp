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

#### MASPSX does not fill reorder-mode delay slots

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

On `func_80024824` that removed three instructions and three diffs, taking the
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
three signed square terms, and call `func_80086E50`.

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

`gcc_2_7_2_g0` produced 38 here against a target of 45, which is the usual
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
