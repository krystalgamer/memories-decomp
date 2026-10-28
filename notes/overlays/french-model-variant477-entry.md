# French MODEL477 loader entries

The header-477/627 family in French physical MODEL117 stages 9/10,
MODEL262 stages 7/8 and MODEL631 stages 7/8 now has matching C for all
three functions in each image. All six complete 20,480-byte images reproduce
retail byte-for-byte. This completes a family in #6460.

The entry occupies `8013B004..8013C484`, or the corresponding `8017` range:
5,248 instruction bytes, 1,312 words and a 200-byte frame. Same-slot entry
bytes agree across the three physical models. One implementation and a slot
wrapper use the named `gcc_2_8_1_g0_split` profile.

All native words were read before reconstruction. Local game/SDK headers and
independently measured accesses supply the declarations. No reference-project
types, headers or compiler claims are imported.

## Preserved ownership and reachability

| Slot-0 address | Bytes (hex) | Existing C owner |
|---|---:|---|
| `8013C484` | `4D4` | Spanish `variant477_panels.c` |
| `8013C958` | `468` | Spanish `variant477_quads.c` |

Both helper implementations, slot wrappers, profiles and inventory rows are
unchanged. Production ELFs verify all three original-sized C owners in all
six images. The panel helper requires `elapsed >= config->draw_start`;
the quad helper independently requires `phase >= 2`. Both are directly reached.

## Measured constructor and runtime view

The minimum observed context extent is `20BC`, not an allocation-capacity
claim. Configuration records have a `38`-byte stride at suffix `8013CDC0`
plus `FC`. Two byte-indexed part lookups and first-part matrix refresh remain.

| Context offset | Measured storage |
|---|---|
| `0000` | Two `8C`-byte one-quad panels, including matrix and capture/direction fields |
| `0118` | Two `2FC`-byte ten-quad groups |
| `0710` | Sixteen `A8`-byte nine-point trails |
| `1190` | Three `1A0`-byte four-by-six near/far webs |
| `1670` | One `A0`-byte two-color-pair record |
| `1710` | Two `98`-byte sheets |
| `1840` | Six `90`-byte eight-point rings |
| `1BA0` | Four `90`-byte eight-point rings |
| `1DE0` | One `90`-byte five-point fan |
| `1E70` | Triangle, quad, textured packets and line |
| `2010` | Matrix, target, direction and projection state |
| `2064` | Timing, configuration, part pointers and controller state |

The panel layout is not a shortened ten-quad pair record: its one-point arrays
start at `0/8/10/18`, position at `20`, matrix at `4C`, captured vector at `6C`
and direction at `7C`. The following groups use the independently measured
standard ten-quad constructor. Unobserved holes and packet storage stay untouched.

The target starts at `(0,-300,slot0 ? -450 : 450)`. Matrix refresh uses
`elapsed < config->matrix_end`. Unlike MODEL470, the two-panel capture loop is
unconditional and there is no corresponding motion-interval calculation.
Each panel captures when size is nonnegative and initialized is zero.
Native slot arms and the counted one-quad loop remain separately emitted.
Capture uses the part's matrix and either the direct target or 192-unit circular
X/Y offsets with a 1300-unit bearing step. Direction is captured minus anchor.

Unsigned projected-X and signed projected-Y treatment, unsigned timing,
native geometry shifts and separate frame-step calls remain. Camera offset
is `(-512,-256,0)`; ambient decay, fade scaling, phase changes and return
values preserve the observed native sequence.

## Angular ownership recovery

The final exact experiment separates the actual small-ring point bearing
from the shared fan/web/runtime bearing, and shares the real angular variable
between web latitude and later ring phase. The first distinction recovers all
fan/runtime words; the second removes the latitude/column overlap and restores
the native web allocation. The same source ownership independently matches
MODEL470, without copying its distinct component or runtime declarations.

The ledger records twelve measured source/header or named-profile experiments
and six complete production-image terminals. Failed scopes, latitude ownership
and scheduler controls remain evidence, not integrated alternatives. The final
reset is a literal 4096 inside the last ring initializer, consumed afterward.
There is no inline assembly, forced register, volatile trick, source-local
extern, fabricated access or index-derived reset arithmetic.

## Complete production image identities

| Physical model / stage / slot | SHA-256 |
|---|---|
| 117 / 9 / 0 | `5f36ffe31b555f3366c9ab66eb17bf1a5d70fc0f3ab2067ff96d38d76e852820` |
| 117 / 10 / 1 | `78aa61245708a91842ce770a207668a080fe8c9f8d7a6623e65eada59841251a` |
| 262 / 7 / 0 | `59db1311833f53dad3598fe90ce5c339212ef1bb4f1d089ca233cc0676246bef` |
| 262 / 8 / 1 | `830a989adf4d3d047e886ee5a873864c0c860873d705eb0231e84ab408a68013` |
| 631 / 7 / 0 | `b106338ebd3f40f022c83639a9901357fde585d91166804aa92dd182f33260bf` |
| 631 / 8 / 1 | `af154f46a36d17d7dcbb231c6d37c4ce99d8d091c4c6fa542c50fb4ee825b8db` |
