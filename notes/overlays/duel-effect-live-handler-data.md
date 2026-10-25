# Remaining live-traced handler data owners

These nine typed views name the existing configuration, image-input and
initial-scale owners used by the accepted Destroy Flames, Terrain Change,
Guardian Star Bonus and Board Destruction providers. The handler identities
remain the already accepted observations in
[the dispatcher note](duel-effect-dispatch.md), not new runtime evidence,
phase-to-card mappings or exclusive artwork identities.

| C view suffix after `gDuelEffect_` | Native label | NA address | JP address | PAL address | Bytes |
|---|---|---|---|---|---:|
| `DestroyFlamesConfigs` | `D_8015A648` | `8015A714` | `801686D4` | `8015A648` | 16 |
| `DestroyFlamesTextureImage` | `D_8015A62C` | `8015A6F8` | `801686B8` | `8015A62C` | 28 |
| `TerrainChangeConfigs` | `D_8015AB14` | `8015ABE0` | `80168BA0` | `8015AB14` | 84 |
| `TerrainChangeInitialScale` | `D_80146138` | `80146180` | `80154138` | `80146138` | 16 |
| `TerrainChangeTextureImages` | `D_8015A8C8` | `8015A994` | `80168954` | `8015A8C8` | 588 |
| `GuardianStarBonusConfig` | `D_8015A5F8` | `8015A6E4` | `80168684` | `8015A5F8` | 20 |
| `GuardianStarBonusInitialScale` | `D_80146024` | `8014607C` | `80154024` | `80146024` | 16 |
| `BoardDestructionConfigs` | `D_8015A60C` | `8015B7A8` | `80168698` | `8015A60C` | 32 |
| `BoardDestructionInitialScale` | `D_80146034` | `801462A0` | `80154034` | `80146034` | 16 |

Addresses are hexadecimal. All 816 bytes retain their real generated-data
owners, original types/bounds and existing five bank maps. The original
private headers remain unchanged; their local declarations and instruction
uses supply the layouts, not reference-project headers or compiler claims.
No new storage, packed aggregate, absolute alias or const qualification is
introduced.

## Destroy Flames

Nonnegative phases below two select the two complete eight-byte
`TileEffectDescriptor` records: size, delay, frame limit and sparkle-texture
selector. The separate complete `GsIMAGE` is passed to the texture-page/CLUT
packer with mode one. Its name describes that input role, not pixel
identity, a replacement for the packed output pairs, or the sparkle atlas.

The full provider keeps all five-by-seven active/sparkle/radius/jitter/color
arrays, random call order, per-column cursor/countdown behavior and the
strict frame-limit comparison. Sparkle selector four retains its distinct
UV branch. All tile/sparkle submission modes, color timing, and PAL-only
row-step/origin/UV constants remain exact. Higher phases still take the
timed crossed-line fallback rather than indexing the two records.

## Terrain Change

Nonnegative phases below six select six complete 14-byte
`DuelEffect10Config` records. Each has a complete `CVECTOR` followed by
height step, maximum height, particle speed, mode and an uninterpreted
halfword. No terrain identity or meaning for the last field is inferred.

The separate 21-record `GsIMAGE` table remains a distinct 588-byte owner.
The accepted [effect-ten recovery](duel-effect-ten.md) established its
complete extent and its equality to the primary image table in the French
image, not permission to deduplicate the storage or assume every regional
table has identical bytes. This provider reads only record 12's mode and
coordinates when its config mode is not one, then replaces only packed
output pair 12's texture-page word. CLUT/output ownership and every other
record remain untouched.

The complete provider keeps its two four-vector rings, plane, 64-particle
and 64-column arrays, halfword speeds/ages and four column layers. The
combined readiness word remains exactly `0x00010001`; publishing request
progress does not start dissolution. The original `phase < -1` test and
mode-two color reset still start that stage independently. Spawn/age/color
recycling, frame-step/tick, casts and completion remain exact.

## Guardian Star Bonus

All nonnegative phases below ten select the same single 20-byte
`GatherEffectDescriptor`; phase chooses the existing sprite UV arithmetic,
not a different config record. The fields retain spread, size, frames,
three ring radii, curve size, two widths and height. No star-index mapping
is inferred from the observed phases.

The full provider keeps its 32 and 48 inward-moving particle groups, 16
rotation records, three 32-vertex rings and two four-vector contours.
Signed divisions, narrowing, convergence thresholds, per-group fades,
scale cap, hard-coded texture words and regional curve height remain
unchanged. Completion still tests particle color 31 and the shared color,
not a newly introduced all-particle predicate.

## Board Destruction

Nonnegative phases below two select two complete 16-byte
`DuelEffect17Config` records, with adjacent three-byte primary/burst colors
and halfwords for radius, spin, height, fan spin and mode. The existing
mode chooses the collector selector and whether the origin is included in
the center; this does not make the data exclusive to Dark Hole.

The full [effect-seventeen lifecycle](duel-effect-seventeen.md) retains the
actual `u32[21]` zero-terminated collector scratch, casts, 20 card work
slots, 64 particles and 48 eight-vertex paths. All object motion/rotation/
visibility stores, last-card/completed-card indexing, signed random
doubling, fan ordering and the original red-for-blue color target remain.
The accepted jagged-column call/stat header and native bolt provider stay
unchanged; no null-offset guard, optional-pointer interpretation or
phase-one runtime result is invented. The pending focused human probe and
its existing replay are not modified.

## Scale copies and conservation

The three initial-scale views name complete existing SDK `VECTOR` copies
made at entry, including padding. They describe that role without
asserting universal identity values, immutable storage or a shared packed
context; NA storage order demonstrably differs from the native PAL order.

This independent component starts at accepted master
`691d49b851e9639b52395a029676879b20e3eb5f`, including all six canonical
live-handler definitions, MODEL, Swords/AI-selector and sound groupings.
Only the nine data identifiers and one include per complete provider
change. Removing the include and reversing those substitutions reconstructs
each accepted source byte-for-byte. Four complete providers cover 12,728
instruction bytes; no function name, parameter, original private header,
record, statement order or regional conditional changes.

The dispatcher, six live semantic bindings, complete owner guard, all five
function inventories, maps/profiles/layouts and data definitions remain
unchanged. The complete accepted registry is an exact prefix before nine
records at actual NA data addresses. Pending fallback and Fusion/Attack
Slash data edits are not imported. The bolt source/utility header retain
fingerprint `92265855122745bd0783dea64ff14758454b424223167da70446e6d09851524e`;
the original attempt history and regression remain byte-exact.

Native compiler/MASPSX assembly, symbols and disassembly/relocations are
compared for all four complete providers under the unchanged named
`gcc_2_8_1_g0_split` profile. Full-image acceptance remains mandatory; these
native comparisons are not local Japanese, PAL, MODEL or cloud-test claims.
