# MODEL450 ribbon recovery: regional evidence, still unmatched

This is a bounded investigation of the last MODEL174 stage 9/10 helper,
not a C integration. The accepted baseline is
`f5a0b50e38439afb4d773723cc41f2e2afcf22ca`. The
[family ownership inventory](spanish-model-variant450.md) remains unchanged:
five assembly functions per Spanish image, including this helper, and
5,824 unclassified suffix bytes per image. No unknown code is excluded.

## Regional counterparts

Each selected image occupies ten sectors, at MODEL.MRG sectors 48224 or
48234, with load base `8013B000` or `8017B000`. Each last helper contains
2,100 instruction bytes. The comparisons below use corresponding slots,
not raw comparisons between differently relocated slots.

| Release | Function interval | Word differences from Spanish | Changed words that are JAL in both bodies |
| --- | --- | ---: | ---: |
| Spanish | `310C..3940` | 0 | 0 |
| Italian | `310C..3940` | 0 | 0 |
| German | `310C..3940` | 0 | 0 |
| French | `310C..3940` | 0 | 0 |
| English PAL | `310C..3940` | 24 | 24 |
| North American | `30A8..38DC` | 417 | 12 |
| Japanese | `30A8..38DC` | 417 | 12 |

Both slots give the same counts. All fourteen complete image hashes and
98 inventoried, closed function slices were independently checked.
The Spanish, Italian, German and French bodies are byte-identical per
slot. English PAL's differences occur only at calls, but this does not
by itself verify the callees or establish a matching C port. Positional
word differences in the NTSC bodies are not semantic-similarity scores.

For reproducibility, the Spanish slot-zero and slot-one body SHA-256s are
`78473553d8825b6126d6461fb93d2a0db7884163adf1f078cc346222f3c1d397`
and `78e2df1eb581cd8def2ec766b8a8b46436b3ef4151c9717ccd29f69418a62714`.
These hash exactly the function interval, not the complete image.

No direct J/JAL reference to the last helper was found in the seven
inventoried functions of any selected image. Those sequences contain no
indirect calls, and none of the fourteen complete images contains its
literal full-address pointer word. These are bounded local searches:
external tables, computed addresses and other loading paths remain
unresolved. **Absence of a local caller does not prove unreachability
and is not grounds to exclude this function.**

## Initializer-backed local layout

The actual entry prefix `4..88` establishes the context-relative ribbon
cursor at `39F8`, saved at stack `94`. The actual loop `9A8..9F4` advances
that cursor by 888 bytes for each of two groups. In every release and slot
it initializes seventeen four-byte color records at group `264`, setting
each RGB triple to `(128, 128, 128)` without touching its fourth byte.
It separately zeros the word at group `2A8`.

Executing those disjoint prefix/loop windows in all fourteen images
initialized 476 RGB records. Each supplied context changed exactly
110 bytes: 102 RGB bytes and two four-byte words. All other bytes in the
`42FC`-byte fixture were preserved. The middle of the initializer was
not executed; the proof assumes that skipped code and its callees
preserve the entry's saved cursor.

This corrects the earlier draw-only view of sixteen color records plus
eight unknown bytes. A local `CVECTOR colors[17]` followed by a separately
named unknown word describes the measured accesses using the canonical
four-byte SDK color type. It does not establish the original struct or
the meaning of the zeroed word.

| Group offset | Measured local view |
| --- | --- |
| `0` | Seventeen `SVECTOR` points |
| `88` | Seventeen `DVECTOR` projected points |
| `CC` | Seventeen 32-bit edge angles |
| `110` | Seventeen displaced `SVECTOR` points |
| `198` | Seventeen displaced `DVECTOR` projections |
| `1DC` | Seventeen 32-bit projected widths |
| `220` | 68 unknown bytes |
| `264` | Seventeen `CVECTOR` colors |
| `2A8` | Separately zeroed word; meaning unresolved |
| `2AC` | Seventeen 32-bit submission gates |
| `2F0` | Seventeen 32-bit depths |
| `334`, `356` | Seventeen signed-short X/Y offsets each |

The color loop does not initialize the submission-gate array. The two
groups end at context `40E8`; the shared `POLY_FT4` is at `4190` and is
40 bytes, not 36. Thirty-nine target-compiled constants validate the
current scratch views, including color-array size and the `2A8` boundary.
Unknown gaps remain explicit.

## Recovered behavior and its limits

Static reconstruction describes two seventeen-point strips. The geometry
uses base radius 192, two trigonometric perturbations, and signed width / 64
with truncation toward zero for displaced points. The perturbation angle
advances by 512 per point. The other phase advances by 1500 and carries
across strips; the base angle advances by 1024 between strips.

Projection repeats each adjacent point pair in `RotTransPers4`; endpoint
sixteen uses points fifteen and sixteen. A separate `RotTransPers` projects
the displaced point. The screen-edge direction is `ratan2(dy, dx) + 3072`.
Projected width is one when context width exceeds 512, otherwise the
displaced-minus-original screen X difference. Sixteen textured quads per
strip use these offsets and the first sixteen colors. Submission requires
both depth and the stored gate to be strictly positive, unlike the accepted
quad helper's nonnegative-depth test.

The final scalar update has stronger, independently executed evidence:

```text
if signed_phase < 5 and old_signed_width > 0:
    width = max(truncate_toward_zero(signed_scale / 8), 0)
```

Phase is at context `42F8`, width at `42D4`, and scale at `36C0`, accessed
through the owner view beginning at `3638` plus member `88`. The real tail
is `38B4..3910` in PAL and `384C..38A8` in NTSC. Each boundary was derived
from that image's phase load and branch target. An earlier end-relative
window was four bytes late for NTSC and included an epilogue instruction;
it is not used as tail evidence.

There are 588 executed cases per image, 8,232 total: seven signed phase
values, seven old widths, and twelve scales. They cover signed extremes,
the phase 4/5 transition, nonpositive old widths, and signed division near
zero and multiples of eight. Every image gives the same scalar results
and preserves every other byte of the supplied partial context. No
trigonometric or GPU hooks execute in these isolated tests.

The complete geometry, projection and renderer have not been executed by
these proofs. The partial view ends at `42FC`, 764 bytes beyond the other
loader window beginning at context `4000`. Nothing here proves allocation
size, phase isolation, noninterference, full SDK/GTE behavior or GPU output.

## Compiler investigation

Twenty-two materially distinct source/profile controls are retained under
`tmp/model450-ribbons391/`, including the rejected first layout. Only named
GCC 2.8.1 / MASPSX 2.81 profiles are used. No ribbon source is registered.

The current control `561309901822ac58` emits the required 2,100 bytes but
has a 336-byte frame instead of retail's 344 and 300 positional word
differences in each slot. Its literal endpoint accesses and reuse of the
geometry-radius scalar for projected width are not an exact match.
An explicit endpoint cursor reaches a 344-byte frame but emits 2,116 bytes
with 431 differences. Neither size agreement nor frame agreement is
sufficient.

Actual compiler RTL from the earlier paired-mode radius control shows
`REG_EQUAL` and a radius spill at stack `118`; ordinary single-definition
radius initialization is instead rematerialized. The current control
holds that radius in a saved register while spilling the quad pointer,
unlike retail. This remains a dataflow/allocation investigation, not a
reason to pin registers, add padding or patch instructions.

The local evidence receipts are
`tmp/model450-ribbon-regions392/verified.json`,
`tmp/model450-ribbon-regional-tail392/verified.json`,
`tmp/model450-ribbon-initialization393/verified.json`, and
`tmp/model450-ribbons391/summary394.json`. The last receipt recomputes
all 44 retained linked-text sizes, frame sizes and mismatch counts against
the Spanish retail slices, rather than relying on older result schemas.
Scratch candidates, executable bytes and probe artifacts remain untracked.
