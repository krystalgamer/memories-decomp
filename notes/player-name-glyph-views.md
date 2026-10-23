# Player-name glyph output views

## Three output roles, distinct from saved SJIS sources

| Original North American identity | Canonical view | Observed role |
|---|---|---|
| `D_801B122B` | `gText_aFirstPlayerNameGlyphs` | First loaded player's converted name in two-player setup |
| `D_801B1238` | `gText_aSecondPlayerNameGlyphs` | Second loaded player's converted name in two-player setup |
| `D_801B125A` | `gText_aCurrentPlayerNameGlyphs` | Existing loaded/edited current-player name output |

All declarations remain mutable incomplete `u8[]` views. They describe
encoded output bytes, not a `u16[]`, a fixed-width character array or a
complete allocation. Each label retains its original independent
provider and relocation identity.

`SaveData_UpdateDuelLoad` converts the two loaded SJIS names only after
its existing deck validation succeeds. Its first destination receives
the conversion of `gSaveData_aFirstPlayerNameSjis`; the second receives
the conversion of that source plus `TWO_PLAYER_SAVE_SLOT_STRIDE`.
Both calls retain `SAVE_DATA_PLAYER_NAME_CHAR_COUNT` (`6`), call order,
source expressions and failure paths. The SJIS source and trade
workspace views remain separate; see
[save-pair-workspaces.md](save-pair-workspaces.md).

`SaveData_ApplyRuntimeState` converts the loaded state's
`player_name_sjis` into the current-name output. `NameEntry_Init`
converts `gSaveData_aPlayerNameSjis` there, and
`NameEntry_UpdateDialog` converts the editing buffer there and scans
the result to `TEXT_STRING_TERMINATOR`. European
`Text_PushPlayerNameStream` publishes the same output address as the
next text stream. "Current" names this loaded/edited role; it does not
guarantee fresh contents, initialization or ownership by whichever
player is active in another subsystem.

## Converter and extent limits

`Text_SjisToGlyphCodes` remains unchanged. Its existing backward
halfword scan, signed count, lookup traversal, single-byte versus
extended two-byte encoding, zero-on-lookup-miss behavior and final
`TEXT_STRING_TERMINATOR` store are not repaired or normalized.
In particular, no all-zero-input, capacity, alignment or malformed
encoding behavior changes.

The six-character input count is not an array declaration bound.
The first two North American outputs differ by thirteen bytes, as do
their Japanese providers, but PAL uses `0x801BF800` and `0x801BF814`,
twenty bytes apart. Neither spacing proves an allocation extent or
one uniform regional layout. The two labels are not folded into a
shared record or indexed array.

## Regional and compiler boundaries

| Role | North America | Japan | PAL |
|---|---|---|---|
| First paired name | `D_801B122B` | `D_801C1B41` | `D_801B122B` at `0x801BF800` |
| Second paired name | `D_801B1238` | `D_801C1B4E` | `D_801B1238` at `0x801BF814` |
| Current name | Original `D_801B125A` provider | `D_801B125A` at `0x801C1B70` | `D_801B125A` at `0x801BF848` |

The Japanese pair mappings are actual compiler-expanded assembler
identities from the existing wrapper, not guessed bindings derived
from North American-looking suffixes. Current-name aliases in the
resident and Password images retain their original image-local
providers. The original automatic North American providers remain
untouched. The North American Password canonical import uses
`PROVIDE(gText_aCurrentPlayerNameGlyphs = D_801B125A)`: actual glyph
imports still require the original automatic provider, while unrelated
standalone import links do not evaluate an unused alias expression.
No production address is pinned or invented, and the existing
positive/missing-target import controls remain unchanged.

The paired-output header follows `unmatched.h` only in the paired
consumer. The current-output header follows the original `save_data.h`
owner in its four roots. In the text-command source it remains inside
the existing European guard, preserving the complete North American
and Japanese absence. Old owner headers and all regional wrappers
remain unchanged.

Actual CPP and named-profile objects cover 45 source/profile contexts
and 414 registrations, including the separately named PAL Password A
and B manifests and their shared import files. Twenty-one imports are
active: four each for the paired outputs and thirteen for the current
output. Canonical declarations retain twenty-six additional unused
current-name views. Four other unused current-name declarations stay
legacy-only, and all eighty-four declaration/reference absences
remain absent.

No new storage, copy, converter, decoder, stream helper, bounds check,
testcase or harness is introduced. All existing tests and physical
ownership controls remain unchanged.
