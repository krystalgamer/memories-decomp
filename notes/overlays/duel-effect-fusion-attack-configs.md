# Fusion and Attack Slash configuration owners

These four views name data used by the already accepted live-traced Fusion
and Attack Slash lifecycles. They do not infer new effect identities or
attribute exclusive artwork to either handler. The evidence for the handler
names remains in [the dispatcher note](duel-effect-dispatch.md).

| C view | Native label | NA address | JP address | PAL address | Bytes |
|---|---|---|---|---|---:|
| `gDuelEffect_FusionConfigs` | `D_8015B420` | `8015B48C` | `801694AC` | `8015B420` | 48 |
| `gDuelEffect_FusionInitialScale` | `D_80146208` | `80146250` | `80154260` | `80146208` | 16 |
| `gDuelEffect_AttackSlashConfigs` | `D_8015AF18` | `8015AFE4` | `80168FA4` | `8015AF18` | 350 |
| `gDuelEffect_AttackSlashInitialScale` | `D_801461B8` | `80146200` | `80154210` | `801461B8` | 16 |

Addresses are hexadecimal. The existing five bank maps retain the native
labels and actual storage providers. French/Spanish explicit extents are
`0x30`, `0x10`, `0x15E` and `0x10`; the configuration bounds and layouts
remain the independently recovered local declarations in `effect_1.h`
and `effect_2.h`, not reference-project types.

## Fusion's two complete records

The effect-1 provider selects `&table[phase]` only for nonnegative phases
below two. Each 24-byte `DuelEffect1Config` supplies beam color, initial
rotation step, rotation acceleration, beam rotation step, particle speed,
sprite size, two trapezoid widths, height and the two stage thresholds.
The name covers both existing records without assigning phase-specific
outcomes. Higher phases retain the timed crossed-line fallback.

The entry copies the separate complete SDK `VECTOR` before either its
initialization or update path. Initial scale describes that copy's role,
not an identity-value claim, immutable object or new default initialization.
All four words, including padding, remain in their original owner.

The full provider retains its 12 rotation records and 64 position/velocity
pairs, frame-step override, separate saved frame step and tick, three colors,
growth/rotation arithmetic, matrix ordering, stage transition and particles.
The quarter-color copies are unchanged, including the strip draw that
receives the original beam color rather than its reduced local copy.
Screen-color rendering still publishes request progress; completion still
requires all three colors to be black.

## Attack Slash's seven complete records

The effect-2 provider rejects phases at least seven before selection.
For an accepted nonnegative phase it selects record six when the signed
number argument is zero, otherwise `&table[phase]`. That independent
zero-number override is preserved; the seventh record is neither an
out-of-bounds approximation nor separately allocated data.

Each 50-byte `DuelEffect2Config` retains its three effect-color bytes, the
two red/green/blue number-color alternatives, sprite size, three ring
radii, four heights, five widths, particle speed/count, numeric variant,
two strip widths, strip height and uncertain final halfword. The
unaligned color sequence, layout padding and last field are not repacked,
renamed or normalized. Positive and negative numbers retain their original
color selection; zero clears the number color. This is presentation of
the supplied signed number, not application of battle damage.

The full provider retains all three 32-vertex rings, 64-element position,
velocity and rotation arrays, texture-page/CLUT selection, exact UVs,
indexed quad stages, half-color ring pass, optional variant-one ring/64-strip
path, capped growth and three-color completion test. The projected number
still receives `-abs(number)`. Progress remains tied to the existing
nonzero-stage color path; no waiting or gameplay mutation is added.
The separate entry-copy scale keeps its complete SDK `VECTOR` and padding.

## Conservation boundary

`fusion_attack_semantic_views.h` includes the two unchanged private
declaration headers and binds these four typed views with `__asm__` to
their unchanged native labels. Each complete provider changes only those
data identifiers and one include after its original private declarations.
Removing that include and reversing the substitutions reconstructs both
accepted sources byte-for-byte. No function definition, argument, statement
order, structure, native declaration, generated-data definition or storage
extent changes.

This component starts from accepted sound-grouping master
`f577a2b1155d017b091cbdd483f4266992771f5e`. Pending live-handler definition,
card-flash and error-fallback edits are not imported. All five function
inventories, dispatcher, guards, maps/profiles and the complete accepted
semantic-registry prefix remain unchanged; four records append at actual
NA data addresses. The fingerprinted bolt source/utility header, original
attempt ledger and regression are unchanged.

Native compiler/MASPSX assembly, symbols and disassembly/relocations are
compared for both complete C providers under the unchanged named
`gcc_2_8_1_g0_split` profile. Full-image acceptance remains mandatory;
native conservation is not a local Japanese, PAL or cloud-test claim.
