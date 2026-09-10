# Overlay near-miss candidates

Candidate sources for unmatched overlay functions live in `tmp/`, which is not
tracked, so they are lost when a session ends. Every run that wants to continue
a near miss has had to rebuild its candidate from the prose in the function's
inventory row first. That has happened at least six times, three of them on
`func_80168CDC` (now `NameEntry_SpawnGlyphSprite`) alone, and each rebuild
costs several minutes and can drift
from the state the row describes.

This file is the durable copy. A candidate belongs here once it is close enough
that the next run would otherwise rebuild it: roughly, when it reaches the
target's instruction count, or when its recorded state is the base for further
measurement.

Rules:

- Store the exact source that produces the recorded state, not a tidied version.
  These are measurements, and reformatting them can change the output.
- Use the include an integrated overlay source uses, `#include "../../types.h"`,
  which is what every file under `src/overlays/<module>/` has. A stored
  candidate is meant to be promotable as-is, so an include that only resolves
  from a scratch directory makes it fail to build the moment someone integrates
  it. A sweep harness that compiles from somewhere else should rewrite the
  include as it writes its temporary file rather than change what is stored.
- Record the profile and the measured result in the heading, and re-verify with
  `overlay_diff.py` before trusting a stored candidate. If it no longer
  reproduces, say so in the function's inventory row rather than silently
  editing it here.
- Delete an entry once the function matches and its source is promoted into
  `src/overlays/`.
- The inventory row remains the place for findings, negatives and levers. This
  file holds only code.
- If a row claims a near-miss state, the source that produces it belongs here.
  A claimed state with no stored source is not a result, because nobody can
  reproduce it. That has happened three times so far and each one cost a full
  reconstruction to recover: `func_80169734`, which the row had at 309 of 309
  with two differing positions, `CampaignMap_UpdateLocationTransition` at 217
  of 217 with seventy, and `func_8016A37C` at 365 of 365 with five. The first
  two were rebuilt and then matched; the third was subsequently matched in
  #2158, and #3859 made it a candidate again because that match pinned a
  register.
  `FreeDuel_Init` was recovered the same way and has now matched, so a lost
  reconstruction is worth rebuilding from the row's own prose every time.

The following were historical recovery targets before the configured-overlay
campaign reached matching C. `func_80181728` is no longer an unmatched work
queue; the other two are candidates again:

- `func_8016A37C` (`Password_UpdateShopScreen`): formerly claimed at 365 of
  365 with five differing positions, then matched in #2158. That match pinned
  `pool` to `$2`, so #3859 made it a build-integrated candidate,
  [`src/candidates/password/func_8016A37C.c`](../../../src/candidates/password/func_8016A37C.c).
- `func_80181728`: formerly claimed within four instructions, attributed
  to four callee-saved hoists; now matching C.
- `MainMenu_UpdateTradeScreen` (`0x801821DC`): formerly without a stored
  claimed state, then matched with a Trade-specific semantic name. That match
  pinned four variables to hard registers (`dirty0`, `dirty1`, `source2`,
  `destination2`), so #3859 made it a build-integrated candidate,
  [`src/candidates/main_menu/func_801821DC.c`](../../../src/candidates/main_menu/func_801821DC.c).

Consult the current module inventories before attempting to recover any
candidate from historical prose.

Each stored candidate is one file in this directory, named for the
function's address. `README.md` is a generated index of them and is
not tracked; run `make candidate-index` to produce it.

## Build-integrated overlay candidates

A stored candidate here is only text: nothing compiles it, so declaration
drift or a changed toolchain goes unnoticed until someone rebuilds it by hand.
The four functions #3859 moved out of matching C are therefore not stored here
but build-integrated, the way resident candidates are:

- the source is `src/candidates/<module>/func_ADDR.c`, with the same relative
  includes an overlay source uses, and local overlay headers reached as
  `../../overlays/<module>/`;
- the target is the module's own generated assembly for the function, in
  `src/candidates_target/<module>/func_ADDR.S`;
- the entry in `config/slus_01411/candidates.json` carries `"module"`, and
  its row is checked against `<module>_functions.csv`;
- `make build-overlays` (and so `make match-overlays`) compiles each one
  without linking it, compares its fingerprint, and requires its undefined
  symbols to resolve in that module's ELF; `make check-metadata` checks the
  sources, targets and canonical contracts, which for an overlay candidate
  include its own module's headers.

A build-integrated candidate cannot also keep a note in this directory.
