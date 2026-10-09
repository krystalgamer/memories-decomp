# French raw-layout candidates — review-only archive

These are snapshots of the retained best C source attempts from the French
un-inventoried-layout campaign. The `.c.txt` and `.h.txt` suffixes are
intentional: this archive is not part of `config/slus_01411/candidates.json`,
is not compiled by the candidate builder, and does not claim function
boundaries, ownership, or production matches.

All source hypotheses used the named `gcc_2_8_1_g0_split` profile with
MASPSX 2.81. Candidate sources retain their original behavior and declarations
except that local includes point to the archived `.txt` snapshots and source
tree headers. They are review snapshots, not ready-to-promote production
translation units. Original payloads, disassemblies, build products, and
compiler artifacts are deliberately omitted.

| Family | Retained source | Private result | Status |
|---|---|---|---|
| MODEL611/215, headers 194/324 | `model611-215/entry.c.txt`, `entry_slot1.c.txt` | Four complete private 20 KiB images matched; each C owner was sized at 5,372 bytes | **Private exact only.** Raw-layout ownership is unresolved; no inventory promotion or production gate. |
| 8,412-byte cohort | `cohort8412/entry.c.txt` | Best 8,360 bytes, original 368-byte frame, 52 bytes short | Parked, nonexact. |
| 6,196-byte cohort | `cohort6196/entry.c.txt` | Best retained source reaches 6,196 bytes, original 408-byte frame, 1,319 differing words | Parked, nonexact; equal extent is not a match. |
| 8,188-byte cohort | `cohort8188/entry.c.txt` | Best 8,180 bytes, original 408-byte frame, 8 bytes short | Parked, nonexact. |
| MODEL537 helper | `gap134612/helper.c.txt` | 1,776 bytes versus 1,772 target, original 312-byte frame, 405 differing words | Parked, nonexact. |
| MODEL528 swarm helper | `model528-helper/swarm.c.txt` | 1,484-byte extent; 12 instruction words differ | Parked at its six-probe cap. |
| MODEL82 helper | `model82-helper/helper.c.txt` | Best 1,492 bytes versus 1,624 target, original 328-byte frame, 389 differing words | Parked, nonexact. |

`model528-helper/swarm.h.txt`, `model82-helper/view.h.txt`, the three cohort
`view.h.txt` files, and `gap134612/view.h.txt` are the corresponding measured
private layout views. `model528-helper/entry-layout.h.txt` records only the
currently verified MODEL132108 context prefix: **there is no MODEL132108
parent C candidate in this archive**. The separate 5,004-byte family and
other leads remain without a source candidate because their required runtime
data or source evidence is unresolved.

Superseded probe variants, generated permuter inputs, disassemblies, raw
payloads, and temporary build artifacts are excluded. None of these snapshots
changes the authoritative French function/layout inventory or the production
candidate manifest. The four private exact entries still require an
independent raw-layout ownership check and the complete French production
acceptance gate before integration.
