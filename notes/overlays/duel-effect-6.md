# Spanish duel effect 6

`func_80154B30` owns all **3,012 bytes** at `80154B30..801556F4`.
The first independently recovered candidate matched every instruction word
with `gcc_2_8_1_g0_split`; a separate complete-bank link then reproduced all
90,112 bytes, SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Candidate snapshots and the link receipt remain under `tmp/effect-6-probe/`
and `tmp/effect-6-full/`. The initial private linker lacked the two new data
bindings; adding their observed addresses completed the proof without a
source/compiler change.

## Lifecycle and layout

The six configurations select initialization; higher nonnegative phases
start the distinct 180-frame crossed-line path. Negative phases update
sixteen four-element trails, stagger their activation every other tick,
transition individual trails to particle/ring fading, and update a separate
column and background color. Variant five deliberately follows different
completion logic. The saved model frame step and per-call tick remain
separate, as do the hold, trail-age and crossed-line counters.

Random differences retain signed modulo/division, including the division by
the configured duration and trail index plus one. No clamp or replacement
random generator is introduced. The observed horizontal/vertical factors
are 70/106; this commit claims Spanish only, not unverified regional reuse.

The effect-local work view is `0x268C` bytes:

| Field | Offset |
| --- | --- |
| Configuration pointer | `0000` |
| Sixteen four-vector trails | `0004` |
| Sixteen trail velocities | `0204` |
| Three 32-vector rings | `0284` |
| Sixteen 32-vector particle groups | `0584` |
| Corresponding velocities | `1584` |
| Column vector / step / size | `2584` / `258C` / `258E` |
| Sixteen sizes / scales | `2590` / `25B0` |
| Saved-step frame / per-call tick | `25F0` / `25F4` |
| Active count / hold / sixteen states | `25F8` / `25FA` / `25FC` |
| Column mode / sixteen ages | `261C` / `261E` |
| Variant / crossed-line frame | `263E` / `2640` |
| Sixteen colors / background / column color | `2642` / `2682` / `2686` |

Colors deliberately have the observed unaligned placement. Existing SDK
vectors/matrices/colors and canonical resident declarations are reused.
The still-unmatched `func_801566D4` remains an actual assembly callee; its
observed caller contract is local to this effect header.

The configuration stride is 30 bytes. Its first six bytes are two RGB
triplets, not two four-byte colors; the count starts at offset six. The
six-record phase bound establishes 180 bytes at `D_8015B30C`. The initial
scale vector at `D_801461F8` is 16 bytes. Both retain generated data storage,
not new C definitions or absolute overlay aliases.

Production Spanish overlay matching, all seven retail bank copies, metadata
policy and 76 targeted tests pass. Target-GCC probes verify every listed
offset and both structure sizes. Object/final-ELF checks establish complete
C ownership, both exact-size non-executable input data owners and their
retail bytes, and all 17 distinct overlay callees. The six actual records
have trail counts 16/6/8/12/16/16, particle counts 8/12/16/24/32/32 and
duration 14 throughout, fitting the recovered arrays without a zero divisor.

Against accepted `9b25c6617`, all 59 prior manifest entries remain intact:
the independent branch reaches **60/85 C functions / 20,356 bytes**, with
**25 assembly boundaries**, and **184/209 configured-overlay C instances /
76,208 bytes**. The separate pending three-effect batch is not included.
Neither count closes the exhaustive runtime campaign.
