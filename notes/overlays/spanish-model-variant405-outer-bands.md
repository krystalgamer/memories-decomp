# Spanish MODEL405 progress-gated outer bands

Independently recover the 1,112-byte helper at `+0x1B64..+0x1FBC` in all
eight [MODEL405 images](spanish-model-variant405.md). A fresh pre-recovery
check of 5,845 accepted regional C registrations found no same-size entry.
This is new decompilation with `gcc_2_8_1_g0_split` (GCC 2.8.1 / MASPSX 2.81),
not a regional port.

The first retail-derived candidate had the exact size but thirteen
spill-slot and initialization-order differences. Declaring the two
record pointers before the OT pointer, and initializing the inner index
before the packet pointer, reproduced every instruction. A separate
slot1 compilation also matched; both bodies were compared with all
eight physical inputs. No forced registers, frame padding or new flags
were used.

## Original context and record ownership

Unlike the uncalled `+0x1774` helper, this helper has an actual entry call
at `+0xB10`. The argument is prepared by `move a0,s2` at `+0xAF8`, not in
the call delay slot. A reaching-definition walk proves that entry `+0xC`
(original incoming `a0`) is the only `s2` definition reaching that setup,
and `+0xAF8` is the only `a0` definition reaching the call. Initialization
temporarily reuses `s2`, but that path bypasses this call.

Entry invokes the outer bands **before** the projection/growth helper
at `+0xB18`. The progress gate therefore observes the value available
before that later update; do not reinterpret it as a same-call post-update
test.

| Private view | Offset / extent |
|---|---|
| Three 116-byte primary records | `+0x0000..+0x015C`; progress at record `+0x64` |
| Three 288-byte outer bands | `+0x0864..+0x0BC4` |
| Sixteen 52-byte GT4 packets | `+0x2E08..+0x3148` |
| Signed SVECTOR origin | `+0x31DC` |
| Signed step / descriptor pointer | `+0x3224 / +0x3230` |
| Shared phase | `+0x3254` |

Entry establishes both record bases, three-element bounds and strides.
Outer bands reuse the already verified two-seventeen-point geometry,
color, size and completion layout from `variant405_bands.h`. Their initial
radii are 512 and 1024, with zero size and completion counters.
The descriptor's signed count is two for MODEL419 and three for the
other physical models.

The `0x3258` state is a partial helper view, not allocation capacity.
With the resident's stored contexts `0x80136000/0x80176000`, it remains
before both corresponding load banks. Sixteen adjacent-point strips stay
inside each seventeen-point array; all packet writes stay in the sixteen
initialized GT4 packets.

## Rendering and shared-phase updates

Render only when `0 < size < 4096`. Translation sign-extends the SVECTOR
origin, with zero rotation and uniform scale. Above size 3072, each RGB
channel is multiplied by `4096 - size`, divided by 1024 with signed
truncation and narrowed to a byte. Inner color covers the first two
corners; outer color covers the last two. Sorting requires nonnegative
depth and flag and narrows priority to sixteen bits.

Growth requires the corresponding primary progress to be at least 1024
and outer size below 4096. Add `step * 160`; on reaching 4096, clamp and
increment the completion counter. If this is the last descriptor-selected
band and shared phase is 2, advance phase to 4. Growth is outside the draw
guard, so a zero-size band may become visible on a subsequent call.
Unlike the other band helper, a completed size 4096 band is not drawn.

## Preserved coverage and regression evidence

The eight existing images now contain sixteen C instances
(16,960 instruction bytes) and 24 retained assembly instances. No new
function boundary, image or descriptor declaration is introduced.
The accepted inner-band C, wrapper and header are unchanged; the entire
overlay manifest and resident bindings are unchanged. Four-byte headers
and 12,356-byte suffixes remain raw owners.

Four new regressions verify 31 target-compiled layout constants, both
original-context reaching definitions, initialization and growth
instructions, packet bounds, selected C ownership, ten external calls
and one local-jump relocation per outer object, and terminal fingerprints.
The family regressions additionally check every complete image and all
input/final C, assembly and raw owners, resident bindings and stored
context pointers.

The attempt ledger retains three scratch comparisons and eight
complete-image terminal records. Production dependency fingerprints hash
outer C, outer header, then the unchanged shared inner-band header.
