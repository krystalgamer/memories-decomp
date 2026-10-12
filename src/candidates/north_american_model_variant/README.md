# North American MODEL variant — partial candidates (review-only archive)

Snapshots of the best partial C sources for the North American (`SLUS-01411`)
MODEL variant bodies that are still `unmatched_asm` after the exact helpers
in the open #7244 work. The `.c.txt` / `.h.txt` suffixes are intentional: like
`french-review/`, this archive is not part of `config/slus_01411/candidates.json`,
is not compiled by the candidate builder, and claims no ownership, boundary or
production match. Target bytes, disassemblies, objects and permuter output are
deliberately not included.

All of them were measured against the retail module image under the
`gcc_2_7_2_cdk_g0` profile. "Structural rows" is an opcode-aligned diff
(difflib over the instruction stream with registers and immediates masked);
"register rows" is the same alignment with registers kept. They are a
diagnostic, not acceptance evidence: only an exact object plus the full
`make match-overlays` link counts.

| Body | Source | Module / address | Result | Status |
|---|---|---|---|---|
| B10, header 415 entry | `b10-entry/entry.c.txt`, `view.h.txt` | `model_variant_401_stage9_slot0` `0x8013B004`, 0xDAC | exact extent, 15 structural rows, 103 register rows; 102 of 875 words differ | Parked, nonexact. |
| B09, header 433 entry | `b09-entry/entry.c.txt`, `view.h.txt` | `model_variant_174_stage9_slot0` `0x8013B004`, 0xDD8 | 28 bytes short (0xDBC), 96 structural rows | First draft, nonexact. |
| B05, header 425 entry | `b05-entry/entry.c.txt` (smallest structure), `entry_exact_length.c.txt` (exact extent), `view.h.txt` | `model_variant_259_pos0_slot0` `0x8013B004`, 0x1174 | 8 bytes short with 13 structural rows / exact extent with 28 | Parked, nonexact. |
| B15, header 433 helper `+0x1E7C` | `b15-helper/helper.c.txt` | `model_variant_174_stage9_slot0` `0x8013CE7C`, 0x8D8 | exact extent, 3 structural rows; 49 words differ, 38 of them immediates (spill offsets) | Parked, nonexact. |
| B17, header 401 helper `+0xFE4` | `b17-helper/helper.c.txt` | `model_variant_410_stage9_slot0` `0x8013BFE4`, 0x780 | exact extent, 25 structural rows, 80 register rows | Parked, nonexact. |
| B20, header 415 helper `+0xDB0` | `b20-helper/helper.c.txt` | `model_variant_401_stage9_slot0` `0x8013BDB0`, 0x59C | 4 bytes short, 2 structural rows | Parked, nonexact. |

Each is a stand-alone translation unit written for the repository's
`model_variant` sources. The entries' `view.h.txt` files are private measured
layout views, not accepted shared types; the includes of the `.c.txt` files
point at the archived `.h.txt` names and at source-tree headers that are not
resolvable from this directory. The B05 view additionally expects the colour
rows `cb[]` / `ca[]` that the B05 draft added to
`variant425_rays.h` / `variant425_spiral.h` (replacing the `unknown78` /
`unknown50` filler); those header edits are described here and were not
applied.

## What the measurements say (do not repeat)

* **Counter variables decide register roles.** Which variable is each loop's
  counter, and which loops share one, moved the allocation more than any
  statement order: B05 went from 18 instructions long / 186 differing words
  to 2 long / 13 structural rows by renaming loop counters alone. The retail
  grouping can be read from the `slti ...,sN,K` loop tests (loops that share
  a register in disjoint regions usually share a variable).
* **Priority arithmetic.** gcc 2.7.2 global allocation orders pseudos by
  `floor(log2(refs)) * refs / live_length`. B05's remaining fault is a 17-point
  tie: the strength-reduced `(row+1)*2048` pseudo loses `$s8` to the shared
  `outer` counter by that margin when the spoke loop reuses `outer`. One extra
  reference on the giv, or about two fewer on the counter, flips it; none of
  the renames tried found a spelling that does.
* **A named stack struct explains an odd low slot.** B10's `76($sp)` halfword
  is the CLUT half of the `stack` struct also used by the matched French
  entry 422 (60 unused bytes then `u16 texture_cluts[2]`), not a spill.
* **Statement relocation across `rcos`/`rsin`/`ratan2`** (pure calls) changes
  scheduling and is invisible to adjacent-swap permuters; an exhaustive
  single-statement relocation search moved B10 from 35 to 24 structural rows.
  `bar = work->bars` after the first `func_80059A50`, and `angle -= 4096`
  after the strand's `scale`, were found this way.
* B20's one missing instruction is a `nop` in a load-delay slot that this
  build fills with `j = 0` (the sched1 "last free set sinks" rule); six
  placements of `j`/`rec` initialisation and the declared types of the
  counters do not move it.
* B17 keeps `srl 12; sll 16; sra 16` apart in retail where combine merges them
  into `sll 4; sra 16` here; spelling the narrowing differently (wider local,
  cast at use, second name) all compile to the merged form.
* Thousands of random declaration shuffles, simulated-annealing steps and
  wide relocation passes produced no movement once the roles were right.

One trap that is easy to reintroduce: an automated counter rename that does not
look at enclosing loops can give a nested loop the same counter as its parent.
That compiles, scores better, and changes behaviour. The sources archived here
were checked for it; B15's candidate deliberately reuses `j` inside its
`j == 8` tail, which is a different case.
