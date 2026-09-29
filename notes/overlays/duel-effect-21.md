# Spanish two-slot duel effect 21

`func_8014E3EC` owns the complete **1,680-byte** range
`8014E3EC..8014EA7C`, using `gcc_2_8_1_g0_split`.
The work argument is intentionally ignored: two `0x80`-byte state views
occupy offsets `0x2800` and `0x2880` from the existing resident payload
pointer `D_80010000`. No new global storage or payload allocation is created.

The canonical `high_memory_addresses.h`, `screen_projection.h` and resident
request declarations supply the real owners. The matrix getter binds to
resident `Model_GetLightSourceMatrix` at Spanish `8005C328`, rather than a
guessed overlay alias or an independently redeclared function.

Each state has ten vectors at `00`, ten tile selectors at `50`, active count
at `64`, deliberately unaligned color at `66`, enabled halfword at `6A`,
and ten vertical offsets at `6C`. The two slots exactly fill `2800..2900`.
The initial vector `D_80146158` is 16 bytes; the ten interleaved two-side
vectors at `D_8015AB68` occupy 160 bytes.

Nonnegative phases select a side with signed absolute-value/remainder
expressions. An already enabled side is cancelled; otherwise its ten
positions are copied, eight random swaps performed, and its descent/count
state initialized. Negative phases select `abs(phase + 1) % 2`, update and
draw tiles, publish the observed resident status values, optionally fade,
restore matrices and signal completion only through the original predicates.

The original initializes **only selector eight** after the eight-swap loop.
Other selectors retain their prior values. This unusual behavior is preserved,
not silently replaced by a guessed ten-element initialization. Likewise,
the active-count comparison must consume the incremented halfword expression:
splitting the increment and comparison causes an extra pointer/field reload.

The initial missing request-header dependency was corrected before the first
compiled candidate. The first complete candidate was 1,684 bytes with 50
reported differing words after the count increment. Combining the increment
with its comparison produces all **1,680 bytes / zero differing words**.
Snapshots remain under `tmp/effect-21-probe/`.

The independent whole-bank receipt at `tmp/effect-21-full/verified.json`
reproduces all 90,112 bytes with SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Together with independently matched effect 2, the branch adds **3,948 bytes**
across two complete routines. Against accepted `f52efc378` this is
**64/85 bank C functions / 27,428 bytes**, **21 assembly boundaries**, and
**188/209 configured-overlay C instances / 83,280 bytes**. All 62 prior
manifest entries remain unchanged; the separate pending effect-6/geometry
batch is excluded. The wider runtime campaign remains open.

Production Spanish images, all seven bank copies, metadata, target-GCC slot
layout and real data-owner/retail-byte checks pass. The slot size and all six
field offsets are verified independently of the image hash. All six distinct
overlay callees resolve to real code. Resident binding checks consult both
the symbol inventory and supplemental linker symbols, where the canonical
payload pointer is recorded.

After incorporating accepted #6566 and regional work through `78fc1686f`,
the combined batch reaches **67/85 bank C functions / 31,096 bytes**,
**18 explicit assembly boundaries**, and **191/209 configured-overlay C
instances / 86,948 bytes**. All 65 accepted Spanish manifest entries,
including the already accepted complete geometry groups, remain unchanged.

Final combined acceptance passes 88 targeted tests and sequential complete
Spanish, French, English PAL, Japanese and North American overlay matching.
Spanish copies, metadata, both new layouts/data owners, accepted effect-6
ownership and dispatcher ownership are rechecked after integration.
