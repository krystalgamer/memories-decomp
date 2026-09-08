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
  #2158.
  `FreeDuel_Init` was recovered the same way and has now matched, so a lost
  reconstruction is worth rebuilding from the row's own prose every time.

The following were historical recovery targets before the configured-overlay
campaign reached matching C. They are no longer an unmatched work queue:

- `func_8016A37C`: formerly claimed at 365 of 365 with five differing
  positions; now matching C.
- `func_80181728`: formerly claimed within four instructions, attributed
  to four callee-saved hoists; now matching C.
- `MainMenu_UpdateTradeScreen` (`0x801821DC`): formerly without a stored
  claimed state; now matching C with a Trade-specific semantic name.

Consult the current module inventories before attempting to recover any
candidate from historical prose.

Each stored candidate is one file in this directory, named for the
function's address. `README.md` is a generated index of them and is
not tracked; run `make candidate-index` to produce it.
