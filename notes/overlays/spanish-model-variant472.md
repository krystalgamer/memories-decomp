# Spanish MODEL headers 472/622

This independently recovered single-sheet renderer is new matching C, not a
port of an accepted French or other regional implementation. MODEL 719's stages
7/8 are the only physical secondary loads with these headers in the Spanish
archive. Their compact record is **619**, accounting for both omitted model
ranges. The [instance ledger](spanish-model-variant472-instances.csv) records
sectors 171024/171034, both slot bases and independent image hashes.

Only the 936-byte helper at image `0x28B0` selects C. Both complete 20,480-byte
images reproduce the retail bytes with GCC 2.8.1 / MASPSX 2.81, using the named
`gcc_2_8_1_g0_split` profile. The second slot changes only the function symbol;
its absolute jumps are independently linked at `0x8017D8B0`.

| Offset range | Bytes | Owner | Entry reachability |
|---|---:|---|---|
| `0x4..0x1110` | 4364 | Generated assembly | Loader entry |
| `0x1110..0x1734` | 1572 | Generated assembly | Direct call |
| `0x1734..0x1D30` | 1532 | Generated assembly | Direct call |
| `0x1D30..0x28B0` | 2944 | Generated assembly | Direct call |
| `0x28B0..0x2C58` | 936 | Independent C | Direct call |

All five boundaries are independently checked closed control-flow spans, with
every instruction visited and one terminal return each. The four-byte header
and **9,128-byte unclassified suffix** at `0x2C58..0x5000` retain raw storage
owners; the suffix is not asserted to be non-code. The net addition is two C
instances / **1,872 instruction bytes**, with eight other function instances
still assembled. No existing overlay selections change.

## Recovered behavior and layout

Entry initializes one 152-byte sheet at context `0x2B24`. Four arrays of four
`SVECTOR` corners occupy offsets `0`, `0x20`, `0x40`, `0x60`; outer/inner RGB
start at `0x80/0x84`, followed by the signed scale at `0x88`. These measured
offsets permit reuse of the established local `ModelVariantSheet` declaration.
The private context view does not name unrelated storage.

The helper draws four textured Gouraud quads through the packet at `0x2C48`.
It adds signed `scale / 8` on odd frames and uses zero rotation and the world
matrix translation at `0x2E98..0x2EA0`. Entry's matrix pointer at `0x2E84`
establishes the enclosing `MATRIX`, rather than an invented translation array.
The usual local-matrix setup includes both `ReadRotMatrix` and a subsequent
`RotMatrix`, even though the latter may appear redundant.

Three corners use inner RGB; the fourth uses outer RGB. Projection writes four
screen-coordinate pairs at packet offsets `8/20/32/44`. Signed depth is scaled
by **7/10**; sorting requires nonnegative depth and projection flag and passes
the low sixteen depth bits. Texture attributes and UV fields are not rewritten
by this helper.

After drawing, a scale below 4096 in phase zero grows from the unsigned clock
and descriptor interval. It clamps to 4096 and sets phase one. Otherwise, once
the fade start has been reached, it shrinks toward zero; reaching zero advances
phase three to four. The fade branch is **not** conditional on phase three:
that condition applies only to the terminal phase transition.

The actual request is `638000`, whose dispatcher-normalized index is zero.
Entry uses image `0x2D54 + index * 52`; the selected growth interval is
`40..80` and fade interval `240..600`. Both unsigned divisors are nonzero.
Other descriptor words remain unnamed. This differs from accepted MODEL397's
two-sheet/path-dependent state machine and MODEL443's variable sheet set; no
accepted body was substituted for this helper.

## Evidence and ownership

The [attempt ledger](spanish-model-variant472-attempts.csv) records the first
instruction-exact reconstruction, entry-grounded matrix/descriptor refinement,
independent slot-one comparison and terminal production selections. No compiler
experiments, forced registers, artificial stack padding or instruction patches
were needed.

Thirty target-compiled layout constants cover the SDK records, sheet, partial
context, timing view and packet coordinates. Byte-level regressions check
record initialization, advancing pointers, all direct packet color stores,
signed division, unsigned timing arithmetic and rendering/phase gates.

The original context is captured in entry `s2` at image `0xC`. Initialization
later reuses that register, but the update branch bypasses those definitions.
A delay-slot-aware reaching-definition check proves that the sheet call at
`0xF28` passes the original context. The helper's stable context and packet
registers and its 256-byte frame are checked separately.

Full-image checks verify the defining input objects and final function extents
for both C helpers and all eight retained assembly functions, all four raw
owners, and ten static resident-call relocations per helper. Nine distinct
helper callees and all 36 module-level bindings resolve to actual Spanish
resident functions, not overlay-local or zero-size stand-ins.

Resident ownership checks also cover the four loader/dispatcher functions and
eight selected pointer-table words. Contexts `0x80136000/0x80176000`, including
the entry-observed minimum `0x2F48` bytes, do not overlap the corresponding
selected model, primary or secondary image loads. The C helper's partial view
ends at `0x2F44`; neither extent proves an allocation capacity or whole-game
lifetime isolation.

The focused regressions are in
`tools/project/tests/test_spanish_model_variant472.py`. Retail-dependent
checks require the legal Spanish inputs; input-owner checks additionally
require freshly built Spanish resident and MODEL472 images.
