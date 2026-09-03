# Remaining Decompilation Campaign

## First evidence batch

The campaign described in `notes/remaining-decompilation-plan.md` began with
same-address collaborator evidence and the newly exposed zero-attempt
function.

### Accepted pure-C matches

| Address | Result |
|---|---|
| `0x800240B0` | Unchiga's cursor callback source matches under `gcc_2_8_1_g8_split`. Three isolated absolute C stores preserve the retail `$at` address materialization. |
| `0x80070AC0` | `AiScript_JumpEqual` matches after introducing a local AI-state pointer and updating the existing offset variable in place; the GCC register roles then match without asm. |

### Terminal refinements

| Address | Result |
|---|---|
| `0x8005C4F0` | `File_Exists` pure C remains two register-allocation words away under GCC 2.8.1. GCC 2.7.2 changes a branch displacement. Six refinements exhausted; exact inline source retained. |
| `0x8003CE74` | Pure PRNG source reaches one commutative operand-order word from retail. GCC canonicalizes both source operand orders. Six refinements exhausted; exact inline source retained. |

### Preserved partial histories

| Address | Current residual |
|---|---|
| `0x8002A788` | Exact 0x238-byte size and relocations; four dead delay-slot/scheduler words differ. Three refinement attempts remain for genuinely new scheduling evidence. |
| `0x80020BE4` | Exact 0x168-byte size; 21 words differ from store scheduling and the collaborator's removed register pin. Three canonical attempts remain. |
| `0x80050584` | Exact 0x24C-byte size; two documented register-role blocks account for 24 differing words. Four canonical attempts remain. |

### Newly exposed callback

`func_8002DDFC` received six ordinary GCC 2.8.1 G0 variants using local
assembly, GMS pseudocode, and explicit target-order labels. The closest source
is 300/304 bytes but its event-1 branch target is four instructions early.
The function is terminally deferred and remains exact assembly.

## Preserved-candidate profile sweep

`tools/project/run_remaining_profile_pass.py` tests exactly one untried
default/split/no-split profile for a preserved pure-C candidate. It is
single-worker and writes review artifacts under
`tmp/agents/remaining-profile-pass/`.

`tools/project/import_remaining_profile_results.py` imports a reviewed pass
atomically into `attempts.csv`; exact results are deliberately rejected so
they must be integrated manually.

Results:

| Pass | Tested | Exact | Nonmatch | Tool errors |
|---|---:|---:|---:|---:|
| Split | 72 | 0 | 62 | 10 |
| No-split | 71 | 0 | 71 | 0 |
| Default after explicit profiles | 9 | 0 | 9 | 0 |
| **Total** | **152** | **0** | **142** | **10** |

The tool errors are preserved evidence, mostly G8 small-data relocation
overflows or unresolved m2c pseudo-symbols. They show that those candidates
need declaration/source repair rather than another profile.

After the sweep:

- 218 unmatched game functions have terminal six-attempt deferrals;
- 145 unmatched game functions remain nonterminal;
- their canonical attempt counts are 26 at one, 44 at two, 69 at three, five
  at four, and one at five.

The sweep demonstrates that compiler split/no-split selection alone is not the
remaining blocker. Further progress must use better source structure,
declaration repair, GMS semantics, or collaborator residual notes.

## Inline-assembly completion pass

The preserved pure-C refinement candidates were tested sequentially under
untried compiler profiles:

| Pass | Tested | Exact | Nonexact/error |
|---|---:|---:|---:|
| GCC 2.8.1 split | 71 | 19 | 52 |
| GCC 2.8.1 no-split | 57 | 0 | 57 |
| GCC 2.8.1 default | 14 | 0 | 14 |
| GCC 2.7.2 fallback | 58 | 0 | 58 |
| GCC 2.8.1 no second scheduler pass | 49 | 0 | 49 |
| GCC 2.8.1 O1 final variant | 47 | 1 | 46 |

Together with the earlier `AiScript_JumpEqual` source-shape fix, this campaign
replaced 21 matching inline-assembly sources with exact pure C.

The 59 matching sources that still contain GCC assembly now each have a
terminal six-attempt `inline_refinement` history. They remain exact current
sources; no further pure-C retry is allowed without genuinely new compiler or
source evidence.

The profile pass also showed why declaration repair must be separated from
profile variation: recurrent errors include invalid G8 small-data placement,
incomplete m2c pointer types, unresolved `.rodata`/pseudo-register symbols, and
missing global declarations.

## Canonical profile completion

Preserved pure-C unmatched candidates received the same bounded fallback
profiles:

| Pass | Tested | Exact |
|---|---:|---:|
| GCC 2.7.2 fallback | 72 | 0 |
| GCC 2.8.1 no second scheduler pass | 71 | 0 |
| GCC 2.8.1 O1 terminal variant | 66 | 0 |

Sixty-six preserved-source histories reached attempt six and were deferred.

Ten additional candidates whose prior G8 attempts failed only through
out-of-range small-data relocations were repaired by forcing G0:

| G0 repair pass | Tested | Exact |
|---|---:|---:|
| Default | 10 | 0 |
| Split | 10 | 0 |
| No-split | 10 | 0 |
| GCC 2.7.2 | 10 | 0 |
| No second scheduler pass | 5 | 0 |

All ten GPREL-repair histories are now terminal. The source repair was
successful—the candidates compile and link—but their C structure still does
not reproduce retail code.

`func_80058938` was manually reconstructed from GMS after m2c failed on two
unaligned 32-bit copies. Pure-C `memcpy`, packed-pointer assignment, and
packed-parameter forms all trigger a GCC `cc1` signal-11 internal compiler
error under both 2.8.1 and 2.7.2. Its six-attempt history is terminal with the
record layout and compiler blocker documented.

Current canonical state:

- all unmatched game functions have terminal canonical histories;
- 59 retained matching inline-assembly sources have terminal refinement
  histories;
- the manual batches recovered exact C for `0x8002A9C0`, `0x800336F0`, and
  `0x8006041C`, while terminally documenting the remaining large functions.

The decompilation completion gate is therefore satisfied. Post-campaign naming
adds thirteen conservative mechanical names; ambiguous matches remain
address-based.

## Final campaign outcome

The remaining campaign accepted:

- four new canonical matching functions:
  - `Duel_UpdateCardPickCursor` (`0x800240B0`);
  - `0x8002A9C0`;
  - `0x800336F0`;
  - `Model_GetPrimitiveHandler` (`0x8006041C`);
- 21 exact pure-C replacements for matching inline-assembly sources.

Final game-function state:

| State | Count |
|---|---:|
| Matching C | 773 |
| Terminal unmatched assembly | 360 |
| Intentional handwritten assembly | 63 |

The 59 matching sources that retain GCC assembly are terminally documented
after six pure-C refinement attempts. No canonical unmatched function or
inline-refinement history remains active.

Post-campaign work then followed the required gate order:

1. Semantic registry expanded to 189 accepted names.
2. Source grouping expanded to 681 translation units, with 135 functions in
   43 grouped units.
3. `notes/global-usage.csv` recorded 3,356 function/global relationships for
   784 globals across 923 game functions.
4. Shared layouts were added for:
   - AI active cards, interpreter state, and duelists;
   - primary and secondary sound-driver state;
   - DuelEffect channels and entries;
   - model slots and handler registry;
   - duel-card records;
   - fade/transition state.

Each structure note records exact-safe migrations, remaining assembly users,
and raw code-generation exceptions.

## Continuous campaign restart

The completion target was reopened after terminal histories were explicitly
reclassified as research indexes rather than project completion. The first
continuous wave recovered exact C from previously rejected relocation-only
results:

| Address | Semantic name | Resolution |
|---|---|---|
| `0x8003D0F4` | `SaveData_ApplyRuntimeState` | Collaborator C already emitted the retail instructions. Three verified absolute store literals preserve `$at` address materialization; the full executable matches. |
| `0x8003D46C` | `DuelEffect_CreateChannel` | Shared `DuelEffectChannel` C already emitted the retail instructions. One verified absolute byte store preserves `$at` materialization; the full executable matches. |
| `0x8004545C` | `SD_ArmBusyCallback` | Replaced register-pinned matching C with two ordinary C absolute-address stores, eliminating one GCC asm-bearing source while retaining the exact executable. |

The immutable six-attempt histories remain unchanged. Each accepted result is
recorded once as `post_terminal_resolution` evidence, which is allowed only
after a deferred canonical or inline-refinement history and only for an exact
pure-C result.

State after this wave:

| State | Count |
|---|---:|
| Matching C | 775 |
| Terminal unmatched assembly | 358 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 193 |

`GMS` in historical notes refers to **GenericMadScientist**, whose `gms.c` IDA
decompilation is used as supporting evidence rather than ground truth.

### Continuous wave 2

The second continuous wave promoted three more terminal functions:

| Address | Semantic name | Resolution |
|---|---|---|
| `0x8002892C` | `DuelEffect_UpdateState` | The preserved pure-C candidate was already 128/128 byte-identical. Two symbolic `D_8009B24A` GPREL16 relocations correspond to raw `$gp+0x342` target accesses omitted by the relocation oracle; the complete executable confirms the match. |
| `0x800356A0` | `Util_CopyWords` | Reproducing separate `case 1` and `case 2` bodies yields the retail switch layout. The self-contained locally typed source matches 168/168 bytes with no relocations. |
| `0x80035748` | `Util_FillMemory` | The same duplicated-case source distinction yields a strict 160/160-byte match for the repeated-byte fill counterpart. |

The utility pair is contiguous, uses `gcc_2_8_1_g8`, and now shares
`src/game/util_memory.c`.

Two new evidence-backed scheduler experiments did not match and remain under
`tmp/candidates/continuous-wave-2/`:

- `0x8003C628`: an oversized signed output-mode declaration fixed the absolute
  relocation form but scheduled its `lui` one word too early.
- `0x8004A6F8`: splitting `0x60100` into a high-half value plus a later OR
  retained exact size but did not move the high-half materialization ahead of
  table addressing.

State after wave 2:

| State | Count |
|---|---:|
| Matching C | 778 |
| Terminal unmatched assembly | 355 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 195 |

### Continuous wave 3

`SD_ProcessSequenceTracks` (`0x8004C8C8`) matched as pure C after correcting
one control-flow scope in the closest preserved candidate. Retail skips the
entire command-processing and accumulation block while a track timer is below
`0x100`; the previous source skipped only the wrap-counter update. Removing
the candidate's register pins and expressing that scope directly produced an
exact 408-byte function with exact relocations.

State after this one-function checkpoint:

| State | Count |
|---|---:|
| Matching C | 779 |
| Terminal unmatched assembly | 354 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 196 |

### Continuous wave 4

`DuelEffect_UpdateObjectLayout` (`0x80036DBC`) matched after correcting a
double-scaled halfword-table index. The candidate declared `D_80090E58` as
`u16[]` but also multiplied `p[0x57]` by two before indexing, producing
`sll ...,2`; direct halfword indexing produces the retail `sll ...,1`.
The corrected source matches 452/452 bytes with exact relocations.

State after this one-function checkpoint:

| State | Count |
|---|---:|
| Matching C | 780 |
| Terminal unmatched assembly | 353 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 197 |

### Continuous wave 5

`SD_ResetSequenceTracks` (`0x8004CA60`) matched after replacing raw parallel
pointer arithmetic with a typed array of 16 `0x2C`-byte sequence-track
records. The typed `tracks[i].flag` and `tracks[i].value` expressions preserve
the retail `root + offset` operand order, resolving the two commutative `addu`
words in the previous candidate. The function matches 92/92 bytes with exact
relocations.

It is contiguous with `SD_ProcessSequenceTracks` and uses the same
`gcc_2_8_1_g0` profile, but compiling both in one translation unit changes the
resident text size. They remain separate exact objects.

State after this one-function checkpoint:

| State | Count |
|---|---:|
| Matching C | 781 |
| Terminal unmatched assembly | 352 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 198 |

### Continuous wave 6

`DuelEffect_PlaySoundCommand` (`0x80038798`) matched after recovering the
callee argument contract visible in both GenericMadScientist's pseudocode and
the target registers. Keeping the script value as a 32-bit local, explicitly
masking it to 16 bits, and passing it to both sound wrappers preserves the
retail `andi $a0,$v0,0xffff` followed by `andi $v0,$a0,0x8000` sequence
without register pins. The result matches 104/104 bytes and exact relocations.

State after this one-function checkpoint:

| State | Count |
|---|---:|
| Matching C | 782 |
| Terminal unmatched assembly | 351 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 199 |

### Continuous wave 7

`Text_EncodeDecimalNoPadding` (`0x800358A0`) matched after expressing the
backward scan's decrement in its original source order. The scan decrements
unconditionally after loading each byte, branches back with that decrement in
the delay slot for digit bytes, and restores the index only before clearing a
non-digit marker. This produces exact 92/92-byte code and relocations without
register pins.

The function follows `Text_EncodeDecimalDigits` contiguously but remains a
separate object because the base encoder uses `gcc_2_8_1_g0_split` and this
wrapper uses `gcc_2_8_1_g0`.

State after this one-function checkpoint:

| State | Count |
|---|---:|
| Matching C | 783 |
| Terminal unmatched assembly | 350 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 200 |

### Continuous wave 8

`File_ActivateTransfer` (`0x800143DC`) matched after correcting the width of
`D_8009B112`. Target instructions use `lhu`/`sh`, proving a 16-bit flag; the
previous local candidate declared it as `u8` and differed only at those two
accesses. The corrected source matches 220/220 bytes with exact relocations.

The function copies the queued `FileTransfer` at `D_800E9E18` to the active
descriptor at `D_800E9E60`, rotates an eight-word shared buffer half, and
propagates the active-state flags.

State after this one-function checkpoint:

| State | Count |
|---|---:|
| Matching C | 784 |
| Terminal unmatched assembly | 349 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 201 |

### Continuous wave 9

`Model_HasInsufficientBufferSpace` (`0x8005A8C4`) matched after correcting the
storage class of `D_800FE240`. Declaring it as a direct `s32` scalar forced to
`.data` preserves the target's self-referential `$a0` address/load sequence
while avoiding the G8 small-data relocation overflow. The function matches
200/200 bytes with exact relocations.

The predicate accounts for the current model-data heap pointer, bank base,
loaded-bank count, and the selected model slot's `0xE00` size field; it returns
true when less than `0x401` bytes would remain.

State after this one-function checkpoint:

| State | Count |
|---|---:|
| Matching C | 785 |
| Terminal unmatched assembly | 348 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 202 |

### Continuous wave 10

`Main_RunCampaign` (`0x8002CE64`) matched after correcting two absolute-address
forms. `D_8009B27A` is declared as an absolute `.data` scalar so GCC loads it
directly into `$a0`; the write-only `D_8009B254` clear uses the documented
absolute C literal exception to preserve retail `$at` materialization. Linked
text is byte-identical, and the complete executable remains the acceptance
authority for the omitted literal-store relocation.

State after this one-function checkpoint:

| State | Count |
|---|---:|
| Matching C | 786 |
| Terminal unmatched assembly | 347 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 202 |

### Continuous wave 11

`func_8002EDB0` matched after separating the masked call argument from the raw
command byte saved across either callback. Forming the `0x7F` mask first keeps
the loaded byte in `$v1`; copying it afterward places the required move to
`$s0` in the conditional-branch delay slot. The function matches 112/112 bytes
with exact relocations.

State after this one-function checkpoint:

| State | Count |
|---|---:|
| Matching C | 787 |
| Terminal unmatched assembly | 346 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 202 |

### Continuous wave 12

`func_8003F7D4` matched by combining split-address call setup with the
documented absolute C literal exception for the isolated write-only
`D_8009B0D1` clear. The split profile interleaves the destination and source
address pairs exactly; the literal preserves the retail `$at` clear before the
four-argument call. The function matches 60/60 bytes, with the complete
executable serving as the authority for the omitted literal-store relocation.

State after this one-function checkpoint:

| State | Count |
|---|---:|
| Matching C | 788 |
| Terminal unmatched assembly | 345 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 207 |

### Continuous wave 13

`func_800428EC` matched after declaring only the four-entry halfword lookup
table volatile. The object remains nonvolatile, preserving the retail object
byte store before the indexed table load while keeping the original argument
normalization and result store schedule. The function matches 44/44 bytes
with exact relocations and no allocated non-text sections.

State after this one-function checkpoint:

| State | Count |
|---|---:|
| Matching C | 789 |
| Terminal unmatched assembly | 344 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 207 |

### Continuous wave 14

`func_8004CABC` matched after expressing its object scan as a direct `for`
loop over the loaded halfword count. That form preserves the retail zero-count
exit, counter increment, object-pointer stride, loop branch, and return paths
without asm or register bindings. The function matches 80/80 bytes with exact
relocations and no allocated non-text sections.

State after this one-function checkpoint:

| State | Count |
|---|---:|
| Matching C | 790 |
| Terminal unmatched assembly | 343 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 207 |

### Continuous wave 15

`AiScript_PushComboEmpty` matched after retesting the collaborator's direct
count/filter `for` loop under `gcc_2_8_1_g8_split`. The split-address pipeline
preserves the retail state-base register role, output index, loop bounds, and
delay-slot scheduling. The function matches 96/96 bytes with exact
relocations and no allocated non-text sections.

State after this one-function checkpoint:

| State | Count |
|---|---:|
| Matching C | 791 |
| Terminal unmatched assembly | 342 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 210 |

### Continuous wave 16

`func_8003C328` matched after importing the collaborator's typed three-mode
object initializer under `gcc_2_8_1_g0_no_split`. The structure layout and
switch preserve the retail case dispatch, shared state stores, and mode-two
callback sequence without asm or register bindings. The function matches
368/368 bytes with exact relocations and no allocated non-text sections.

State after this one-function checkpoint:

| State | Count |
|---|---:|
| Matching C | 792 |
| Terminal unmatched assembly | 341 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 213 |

### Continuous wave 17

`func_8003C120` matched after importing the collaborator's typed four-mode
object initializer under `gcc_2_8_1_g0_no_split`. The structure layout and
switch preserve the retail case dispatch, nested shared tails, and mode-two
callback sequence without asm or register bindings. The function matches
404/404 bytes with exact relocations and no allocated non-text sections.

State after this one-function checkpoint:

| State | Count |
|---|---:|
| Matching C | 793 |
| Terminal unmatched assembly | 340 |
| Intentional handwritten assembly | 63 |
| Matching sources retaining GCC asm | 58 |
| Accepted semantic mappings | 213 |
