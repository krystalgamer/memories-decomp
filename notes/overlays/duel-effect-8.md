# Complete Spanish duel effect 8

`func_80147B18` at `80147B18..801481A8` is a complete 1,680-byte C
translation unit using `gcc_2_8_1_g0_split`. The accepted dispatcher selects
it for effect 8. Recovery used the Spanish bank's instructions and established
local declarations, not reference-project types or compiler flags.

The complete independent 90,112-byte bank and the production Spanish overlay
image reproduce SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
All 65 previously accepted Spanish manifest entries remain unchanged, and their
linked code extents still match. This batch alone brings the bank to 66/85
matching C functions and 28,828 C bytes; 19 boundaries remain explicit assembly.
It does not claim exhaustive runtime-overlay completion.

## Recovered storage and lifecycle

The target-GCC layout probe verifies a `0x820`-byte work view:

| Offset | Field |
| --- | --- |
| `000` | Configuration pointer |
| `004` | 32 ray vectors |
| `104` / `304` | 64 particle positions / velocities |
| `504` | Three rings of 32 vectors |
| `804` | Origin vector |
| `80C` / `80E` | Unsigned width / height |
| `810` / `814` | Unsigned ring scale / frame counter |
| `818` | Cross-effect counter |
| `81A` | Byte-aligned color |

Configuration stride is 38 bytes. Six records at `8015A514` occupy exactly
228 bytes, ending at the already accepted gather configuration `8015A5F8`.
The initial scale at `80146014` is 16 bytes. Both symbols retain real generated
data-object storage, exact ELF object sizes and original bytes; neither is an
absolute data alias. All 16 distinct overlay callees resolve to real code.

All six retail records request 32 rays, range 64, ray width 4 and 32 particles.
Only variants four and five enable rings. The declared ray/particle arrays
therefore cover the observed counts, and the random range is nonzero.
The particle arrays retain their independently observed `0x200`-byte spans
rather than shrinking to the current configuration count.

Positive phases below six initialize the selected configuration. Larger
positive phases request the cross-effect path, whose counter completes after
180. Negative-phase rendering preserves the shrinking-width and growing-height
clamps, joint-boundary fade, optional two-scale ring drawing, frame-step
override, matrix stack and color-completion flag.

The zero vertical-acceleration update deliberately remains: GCC 2.8.1 emits
the otherwise unused halfword read at `80147E48`. The color remains at the
unaligned `81A` offset, preserving its unaligned copy sequence.

## Caller-supported shared contracts

Two previously unconstrained callee parameters are refined in both declaration
and definition:

- `func_8014EE0C` takes a signed-halfword height. The caller multiplies the
  unsigned configuration halfword by the ring index and sign-extends the
  resulting low halfword with `sll` / `sra`.
- `func_80156E58` takes an unsigned-halfword ray width. The caller passes the
  field with `lhu`, not `lh`.

Both helper bodies only use these values in halfword coordinate writes, so the
refinements preserve their exact instruction bytes and contiguous source-group
extents. No cast-through-function-pointer or conflicting local declaration is
used to conceal a caller/callee mismatch. The resident matrix getter uses
canonical `Model_GetLightSourceMatrix` at Spanish `8005C328`, not a new local
address-based declaration.

## Experiments and verification

The first candidate was 1,676 bytes with 346 differing words, including the
unsigned-height truncation that displaced the remaining instructions. Correcting
the two shared halfword contracts and assigning the packet pointer after the
scale copy produced 1,680 bytes with zero differing words. Both measured
candidate snapshots and compiler/source fingerprints remain local under `tmp/`.

The instruction comparison was only the candidate gate. Promotion followed an
independent full-bank link. Production verification also checks every current
C object's and final ELF function's address, size, executable section and
function type, the new data owners, target-GCC field offsets and actual retail
configuration bounds.

Spanish, French, English PAL, Japanese and North American overlay builds all
pass after the shared contract changes. Spanish duplicate-copy verification,
repository metadata checks and 104 duel-focused tests pass as well.
