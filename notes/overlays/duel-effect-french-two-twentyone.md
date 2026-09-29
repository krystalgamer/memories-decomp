# French duel effects 2 and 21

Two complete routines independently reproduce the French retail bank using
the existing `gcc_2_8_1_g0_split` profile, GCC 2.8.1 and MASPSX 2.81:

| Routine | Extent | Bytes | Shared source |
|---|---|---:|---|
| Effect 21 | `8014E3EC..8014EA7C` | 1680 | `src/overlays/duel_effects/effect_21.c` |
| Effect 2 | `80153200..80153ADC` | 2268 | `src/overlays/duel_effects/effect_2.c` |

The four source/header files are unchanged from Spanish #6569 head
`7247c3840a4a337cb2d7d41cc33f642d242b93bd`. That PR was still awaiting CI
input repair when this independent French proof was performed. Its status
is not an acceptance shortcut: the entire French bank was compiled and
linked independently, both complete object extents were checked, and all
new code/data owners were checked against French retail bytes.

This branch starts at accepted master `fbb7e6b6e`; it does not inherit an
unmerged PR's history or Spanish registrations. All 63 accepted French
entries remain unchanged. Adding 3,948 bytes gives **65/85 bank C routines /
26,892 bytes**, 20 assembly boundaries, and **189 configured French C
instances / 82,744 bytes**. The separate pending French effects 0/6 are not
included in these totals.

## Whole-image and ownership proof

The French archive remains SHA-256
`e00de6fac1660bcf142a20a2c0c965a020e75382d22cf62c26e0bfc153cdfe7d`.
All seven terrain copies are at sectors `7193 + terrain * 240`, each 44
sectors. The complete 90,112-byte bank retains SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.

The first unchanged-source trial matches the complete bank. Each new object
has exactly one defined function, occupying its entire `.text` section.
All 65 C owners retain exact addresses, sizes and executable definitions;
the 27 distinct overlay routines named by the two bodies resolve to real
inventoried functions. The number renderer remains a real assembly function,
not an absolute alias or a claimed C match.

| Data symbol | Bytes | Actual owner |
|---|---:|---|
| `D_80146158` | 16 | Generated header data, effect 21 initial VECTOR |
| `D_801461B8` | 16 | Generated header data, effect 2 initial VECTOR |
| `D_8015AB68` | 160 | Generated data, ten interleaved pairs of SVECTORs |
| `D_8015AF18` | 350 | Generated data, seven 50-byte configurations |

All four symbols have their exact extents and retail bytes in both the
generated input object and final ELF. The zero-number configuration at
`0x8015B044` is record six of `D_8015AF18`, not an independent alias or an
out-of-bounds sixth-record approximation.

`D_80010000` is the canonical resident payload pointer at `0x80010000`,
independently checked in the French resident linker map.
`Model_GetLightSourceMatrix` is the existing 12-byte resident C function at
`0x8005C328`, checked against the authoritative French inventory. Existing
frame-step, matrix-stack, random and request bindings remain unchanged.

## Preserved layout and lifecycle

Effect 2 has a `0x920`-byte work view and a 50-byte configuration. Its three
rings precede 64-element position, velocity and rotation arrays. The seventh
configuration handles zero requests; positive and negative requests retain
separate color selection. Saved frame step, staged drawing, particle/ring
updates, optional 64-strip rendering, crossed lines and all three completion
colors are preserved.

Effect 21 ignores the work argument and uses two `0x80`-byte slots at
`D_80010000 + 0x2800` and `+0x2880`; it allocates no new global or payload.
Each slot has ten vectors at `0`, selectors at `0x50`, count at `0x64`,
unaligned color at `0x66`, enabled at `0x6A`, and offsets at `0x6C`.
The original writes only selector eight after eight random swaps. Its
increment-and-compare consumes the updated halfword directly, and its
signed side selection, completion tests and resident status writes remain
unchanged.

The target compiler independently verifies 32 layout constants, including
all three structure sizes and the recovered field offsets. The seven retail
effect-2 particle counts are `32, 64, 64, 32, 64, 64, 64`; each fits the
64-element arrays. Both effect-21 slots fit exactly within `0x2800..0x2900`.

No compiler flags, assembly, register constraints, storage aliases for
overlay data, or SDK/handwritten exclusions are added. Boot, MODEL/SU,
overworld-tail and other bank ownership remain open; these configured-image
totals are not a French campaign completion claim.
