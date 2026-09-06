# Reading a Recorded Blocker

Every unmatched overlay function carries a note in its
`config/slus_01411/overlays/<module>_functions.csv` row. Those notes are worth
having — they save re-deriving what a function does — but the ones that
describe *why* a function will not match have a poor record. On 2026-09-05,
five were retested and all five were wrong.

This is not an argument for ignoring them. It is an argument for reading them
in two parts, because they are written in two parts and only one of them is
measured.

## What went wrong, five times

| Function | The note said | It actually was |
|---|---|---|
| `func_8018416C` | `no_sched2` is far worse, so sched2 is on and the epilogue is not reachable by flags | `no_sched2` was the only such profile and it is **non-split**, so it moved the scheduling flag and every `%hi`/`%lo` load at once |
| `func_80184254` | `-fno-schedule-insns2` fixes the epilogue but costs five instructions elsewhere, so sched2 is on | those five were the non-split address loads, same confound |
| `TextBox_GetGlyphAt` | blocked on base selection; GCC refuses the negative offset and splits one walker into two; neither `u8` nor `s16` typing helps | a symptom of loop rotation. `for (;;)` with explicit returns fixed the giv base and the offsets together |
| `func_8016A02C` | sched1 hoists the `+0x8` load above the `+0x21` store; no profile with sched1 disabled exists | the fields were reached by casts through a byte pointer instead of struct members. Stock profile matches |
| `CampaignMap_CreateLocationMarker` | the remainder is a swap of the two temporaries in the tail; `-fno-schedule-insns` fixes the saved-register allocation | the `move` was the **return value copy**. Stock profile matches; `no_sched1` is worse |

## The three shapes they take

**A confounded profile comparison.** Two of the five compared against a profile
that differed by more than the flag under test, and read the worse result as
evidence against the flag. Compare the flag lists in
`compiler_profiles.json`, not just the two outcomes. A profile whose name
mentions one flag may differ in several.

**A symptom recorded as the cause.** Two more described the difference
accurately — the giv base, the register in the tail — and then named that
difference as the obstacle. Both were consequences of something earlier in the
function, and neither could be attacked directly. When the difference is in
addressing or allocation, look for a control-flow or interface cause first:
loop shape, whether the function returns a value, whether a value the source
named is missing.

**A mechanism asserted rather than tested.** "because sched1 runs before
register allocation" is a plausible sentence that was never checked. It reads
as a finding and is an inference.

## Writing notes that hold up

Separate the two parts explicitly, because the first is durable and the second
is a guess that ages badly:

- **What differs.** Instruction counts, which positions, which registers, which
  profile. This is cheap to produce with `tools/project/overlay_diff.py` and
  stays true.
- **What was tried.** Source shapes and profiles, each with its result. Also
  durable, and it stops the next attempt repeating work.
- **What it might mean.** Mark it as inference. Never state a compiler
  mechanism as established unless a measurement isolates it.

Avoid absolutes about the toolchain — "not reachable by flags", "no profile
exists" — unless the flag list has actually been checked. Three of the five
were unblocked by a profile that could have been added at any time.

## Before you trust one

Re-measure it. A probe costs about a fifth of a second:

```sh
tools/environments/python/bin/python tools/project/overlay_diff.py \
    <module> <address> tmp/candidate.c --profile <name>
```

The notes were written when confirming them meant a full module rebuild, which
is why explanation substituted for measurement. That is no longer the trade,
and a recorded blocker is now cheaper to retest than to reason about.
