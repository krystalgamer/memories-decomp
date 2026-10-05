# Spanish MODEL446 billboard strip

The game-owned helper at image `+0x12F8..+0x195C` is independently recovered
in `src/overlays/spanish_model_variant/variant446_strip.c`: 1,636 exact
instruction bytes at `0x8013C2F8` and `0x8017C2F8`. It uses the existing
GCC 2.8.1 / MASPSX 2.81 `gcc_2_8_1_g0_split` profile.

A fresh seven-region screen found no accepted normalized C shape among
5,957 configured entries, with no same-size C implementations. This is
retail-derived recovery, not a regional source port.

## Coverage and preservation

The existing [physical ledger](spanish-model-variant446-instances.csv)
covers both MODEL146 stage-9/10 images, sectors 40496/40506, headers 446/596.
Both complete 20KiB images match, with two C helpers and five retained
assembly functions each. The prior [Gouraud-line helper](spanish-model-variant446-lines.md),
its types and attempt history are preserved. No image/function census is
added or removed.

The raw header and tail retain their original owners. All 35 resident
binding addresses and their old aliases remain; six SDK names are added at
already inventoried addresses for the new compiler object's calls.
The first integration link exposed these missing aliases; adding them
resolved linkage without changing generated instructions or resident data.

## Original context and packet

Reaching-definition analysis proves that original entry `a0`, saved in `s2`
at `+0xC`, reaches the helper call at `+0xC8C`. Its delay slot passes `s2`
unchanged. The entry gate is phase > 0 inside the descriptor iteration loop;
the helper also checks phase > 0. This differs from the line helper's
phase <= 0 entry gate.

The POLY_GT4 occupies context `+0x19B0..+0x19E4`. Entry passes this actual
packet to `SetPolyGT4` at `+0x25C`, installs texture/CLUT/UV values, enables
semi-transparency at `+0x298`, and calls `SetShadeTex(..., 0)` at `+0x2A4`.
The packet register remains stable through those operations. The helper
changes only the four XY pairs and four RGB triplets, all inside 52 bytes;
it does not overwrite texture state or the primitive command.

## Geometry and timing

One `0xD0` strip starts at context `+0x1060`. Its upper, center and lower
point banks each contain five SVECTORs at offsets `0`, `0x28`, `0x50`.
Three five-word packed projection banks occupy `0x78..0xB4`;
five signed depths occupy `0xBC..0xD0`. The gap at `0xB4` is not given a
speculative meaning.

The helper generates the three points for each longitudinal position,
then uses the local camera/billboard transform sequence and `RotTransPers3`.
Native PSXLONG projection outputs are split into low-half X and signed
high-half Y, preserving the retail load signedness.

Four adjacent intervals produce two textured Gouraud quads each. The outer
edge is RGB `(0,32,192)` and the center edge `(192,192,192)`.
**Submission tests `depth[j] > 0` but passes `u16(depth[j+1])` as priority.**
There is no GTE-flag check. Neither rule is normalized to other renderers.

Descriptor command 612000 selects one iteration and width 32. Expansion
uses unsigned time interpolation over **184..192**; fade uses **340..400**.
Both divisors are nonzero in the actual descriptor. Radius starts at 1024
and progress at zero. Odd frames add `radius / 128` to the display width.
State updates occur only on the descriptor's final iteration. Expansion
clamps to 1024 and advances phase to 2; fade clamps radius to zero.

## Exact-match evidence

The [nine-row attempt ledger](spanish-model-variant446-strip-attempts.csv)
records five nonmatching candidates, exact slot-zero and slot-one candidates,
then both complete-image terminals. Existing local SDK `setVector` comma
expressions recover endpoint address common-subexpression handling. Native
packed projection outputs resolve eight high-half load differences; fade
comparison operand order resolves the last two instructions.

There are no artificial stack controls, forced registers, dummy work, inline
assembly or new compiler flags.

Regressions verify **41 target-compiled layout constants**, both private-header
inclusion orders, original-context invocation, packet initialization and
bounds, actual descriptor timing, all input/final code and data owners, and
all **sixteen calls to twelve resident callees plus two local jumps** in
each selected object. Independent relocation reproduces every instruction,
in addition to the complete-image hash and resident-owner checks.
