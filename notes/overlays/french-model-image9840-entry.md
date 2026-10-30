# French MODEL9840/9850 entry

The two game-owned entries are matching C through
`src/overlays/french_model_variant/image9840_entry.c` and its slot-1 wrapper.
Each entry is `0x1D50` bytes at image offset `0x4`, with the original 360-byte
frame. Both use the named `gcc_2_8_1_g0_split` profile: GCC 2.8.1 and MASPSX 2.81.
No reference declarations, compiler settings, inline assembly, source-local
extern declarations, register pins or artificial storage were used.

The images retain their distinct header words, 648 and 715. Their complete
20,480-byte contents, including the raw suffix beginning at `0x1D54`, reproduce
the corresponding slices of the French `MODEL.MRG`:

| Image | Complete-image SHA-256 |
| --- | --- |
| 9840 / `0x8013B000` | `415a9f88482a98de2d9151c3f70fc715b37ee1e7076bc54ecc57cd4231cb0e25` |
| 9850 / `0x8017B000` | `f721ba6d40e306c9cc34d6dc3789f97b21c208bd809d6d1fd2fb65f1585d096b` |

## Recovered ownership

The entry clears `0xF68` bytes. A command row occupies `0x10` bytes and is
selected directly, not through a modulo-100 command index. The row supplies
delay, radius, length, two end radii and two RGB triples. The measured
`0x4C`-byte part contains activation/colour/UV bytes, mode, elapsed/duration,
coordinate pointer, rotation, scale, position and growth.

| State offset | Recovered content |
| --- | --- |
| `0x20` | Shared `POLY_G3` |
| `0x3C` | Shared `POLY_GT4`, not `POLY_FT4` |
| `0x70`, `0x90` | Four-point square and five-point inner ring |
| `0xB8`, `0x140`, `0x1C8` | Three 17-point geometry arrays |
| `0x250` | Five event flags, followed by three active-count bytes |
| `0x258`, `0x2F0`, `0x5E8` | Primary, secondary and burst part arrays |

The measured adjacent bases and cleared span reserve 2, 10 and 32 part slots;
command zero activates counts 2, 1 and 16. This establishes the accessed state
layout, not the capacity of an external allocation. Primary parts start at
elapsed zero; the secondary shell and successive triangle bursts start from
elapsed 45. Result precedence and initialization falling through into the
delay/update path are preserved.

French resident bindings were recovered independently. In particular,
`rsin` is `0x80086628`, `rcos` is `0x800866F8`, the canonical matrix
`D_800FE168` binds to `0x800FF4F0`, and the SDK ordering-table getter
`func_80058F10` binds to `0x8005C018`. The provider contains precisely the
symbols required by the two independently compiled owners.

## Exact source recovery

The [attempt ledger](french-model-image9840-entry-attempts.csv) records all
30 source/compiler experiments, including the rejected before-array pointer
candidate. Native-size bodies and equal instruction counts were only
diagnostics, never acceptance.

The exact result depends on actual value ownership: secondary half-widths
belong to the geometry phase after matrix setup; raw width quotients remain
distinct from their signed half-width consumers; headings are formed before
subtracting view yaw; and the signed vertical factor is doubled before its
radius product. Sine and cosine scaled values have independent identities.
Their radius input is captured after the trig call as the promoted 32-bit
multiplier input, not a halfword temporary.

The five-flag initialization uses the ordinary ascending indexed loop also
present in the locally accepted neighbouring effects, rather than a
descending source pointer. GCC reverses that loop into the native descending
machine cursor. The C therefore never forms a before-array pointer. Byte
activation is produced before the signed loop index, and result 3 is stored
before the finished-event flag.

Forty-three target-compiler layout assertions and an independent fresh
terminal compilation accompanied complete-image comparisons for both slots.
The production overlay build additionally verifies the real manifests,
relocated wrapper, provider and unchanged raw regions.
