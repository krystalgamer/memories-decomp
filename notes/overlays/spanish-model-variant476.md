# Spanish MODEL476: sheet, spiral, webs, curtains, globe and screen grid

## Scope and independent evidence

Spanish model 712, compact record 612, selects two distinct ten-sector
images at stages 9/10, slots 0/1. Headers are 476/626; archive sectors
are 169112/169122 and loads are `0x8013B000/0x8017B000`. Both actual
configuration commands are 642000, selecting descriptor zero at image
`0x3470` with a bounded 56-byte stride. The
[instance census](spanish-model-variant476-instances.csv) records actual
Spanish hashes and command words.

Accepted local [French MODEL476](french-model-variant476.md) and US459
sources supplied structural leads, not Spanish registration or ownership.
Twelve fresh GCC 2.8.1/MASPSX 2.81 objects using the existing
`gcc_2_8_1_g0_split` profile independently reproduce all six helper spans
in both Spanish images. C, headers, SDK declarations and compiler profiles
are unchanged. The [attempt ledger](spanish-model-variant476-attempts.csv)
records source fingerprints and exact results.

| Image range | Bytes | Owner |
| --- | ---: | --- |
| `0x0004..0x135C` | 4,952 | Game-owned entry assembly |
| `0x135C..0x170C` | 944 | Sheet C |
| `0x170C..0x20BC` | 2,480 | Spiral C |
| `0x20BC..0x247C` | 960 | Webs C |
| `0x247C..0x2848` | 972 | Curtains C |
| `0x2848..0x2DD4` | 1,420 | Globe C |
| `0x2DD4..0x3374` | 1,440 | Screen-grid C |
| `0x3374..0x5000` | 7,308 | Unclassified preserved suffix |

Both whole-image scratch links match: twelve C owners / 16,432 bytes,
two assembly owners / 9,904 bytes and four raw owners / 14,624 bytes
cover all 40,960 bytes. The raw owners include each four-byte header.
All fourteen function instances are inventoried. Webs is retained C with
no demonstrated direct-entry call path, not an entry-called renderer.
The unclassified suffix is not counted as recovered code.

## Retained helper contexts and target layouts

The 272-byte entry frame captures `a0` in `s3` at image `0xC`, then in
`s8` at `0x14`. `s3` also has initialization uses: a blanket assertion
that it never changes would be wrong. Delay-slot-aware reaching-definition
analysis independently proves the original capture reaches every call at
`0x10A0/0x10D8/0x1138/0x1178/0x1194`; each delay slot passes `s3` as
`a0`. `s8` remains the original context until restoration.

Fresh target compilation establishes 137 constants in exactly 548 bytes
of read-only data, including shared packet/coordinate declarations,
all five private views and GT4 texture fields. Actual Spanish instructions
independently confirm 408 literal anchors, eight relocated anchors and
21 complete register-write sets per image.

Three 608-byte web records occupy `0..0x720`. Each has two six-by-six
eight-byte point grids at relative `0/0x120`, color at `0x240` and scale
at `0x254`. Its helper advances the original `s3` cursor by `0x260`,
but preserves the initial context separately at `sp+0xD8`; no overlapping
direct stack store rewrites that spill.

The single 152-byte sheet occupies `0xF60..0xFF8`. Globe begins exactly
at `0xFF8`: nine rows of seventeen eight-byte points, row stride `0x88`,
followed by nine four-byte color entries at relative `0x4C8`. Its
1,260-byte view ends at `0x14E4`.

The 1,332-byte screen grid occupies `0x14E4..0x1A18`: two nine-by-nine
point grids at relative `0/0x288`, with nine color entries at `0x510`.
Five 428-byte curtains then occupy `0x1A18..0x2274`. Each curtain uses
seventeen-point rows at relative `0/0x88`, rotation at `0x198` and scale
at `0x1A0`. The helper does not assign a shared type to the intervening
`0x110..0x198` bytes or the trailing eight bytes.

These are measured local views, not a monolithic allocation declaration.
The entry's directly observed minimum is `0x2864`. Actual resident
pointer-storage owners select `0x80136000/0x80176000`; the measured views
do not overlap the relevant slot image ranges. This is not a claim about
the allocator's complete extent or whole-game isolation.

## Packets, timing and projection order

Helper frames are 256/296/304/280/304 bytes for
sheet/webs/curtains/globe/screen grid. Projection outputs occupy
`sp+0xD0..0xD8`, except curtains at `sp+0xF8..0x100`; direct stores remain
inside their frames and do not overlap those output pairs. Curtains keep
the observed reserved stack interval and separate colors at
`sp+0xE8/0xF0`.

GT4 packet starts are `0x22E8` for sheet, `0x2704` for curtains,
`0x26D0` for globe and `0x2390` for screen grid. Each is 52 bytes.
The web line is twenty bytes at `0x237C`. Complete register-write sets
and direct-store footprints prove packet pointers, RGB fields, line
fields and the screen-grid UV/tpage fields stay within these views.
Globe's packet ends exactly where the curtain packet begins.

The selected descriptor's seven timing words at relative `0x1C..0x34`
are 80, 144, 152, 260, 270, 284 and 420. The sheet entry gate uses
unsigned frames `[80,270)`; its first timing denominator is the positive
`144 - 80`. The screen grid is called at frames 270 through 288,
inclusive, refreshing its angle through frame 274. Curtains are called
from frame 284; globe follows while phase `0x284C` is below six.
The late deadline comparison is strictly beyond 420, not inclusive.

Sheet and screen-grid translations use words `0x274C/0x2750/0x2754`;
curtains and globe use halfwords `0x2758/0x275A/0x275C`. Globe draws
eight-by-sixteen quads; screen grid draws eight-by-eight. The screen-grid
context starts in `s1`, but that register is later reused for tpage at
`0x317C/0x31E8` and projection depth at `0x32A8`. The packet is already
captured in `s0` at `0x2EE8` and remains stable through both projections.

At `0x314C`, projection of the second grid writes the four packet XY
words. Signed `x0 < 160` selects `GetTPage(2,1,320,0)`; the other path
uses X=448 and subtracts 128 from each UV X before byte truncation.
Both paths call `SetPolyGT4` and set the halfword tpage plus all eight UV
bytes. Their join at `0x3248` leads to the first grid's final geometry
projection at `0x3294`. `SetSemiTrans(poly,0)` and `SetShadeTex(poly,0)`
follow before nonnegative depth/flag checks and low-halfword depth sort.
The retained active-buffer call does not imply distinct page arguments:
the explicit identical source branches compile to the observed folded
paths. No volatile accesses or register-forcing workaround is introduced.

## Actual resident and SDK ownership

All 35 bindings were independently assembled from accepted Spanish
MODEL435/338/442/408/415 metadata, then checked against actual resident
function extents, executable sections and selected link inputs. Three
matching resident caller owners and both context-pointer storage owners
were separately checked and archived. The initial owner receipt deliberately
did not authorize promotion before the deeper lifetime proof.

The sixteen-byte `GsGetActiveBuff` candidate alone is ambiguous.
Additional actual Spanish SDK context establishes its role without
renaming any resident inventory row or adding a global declaration:

| Actual SDK interval | Measured relation |
| --- | --- |
| `0x80084D58`, 116 bytes | Clears shared halfword `0x800FF454` and invokes draw setup |
| `0x800852A8`, 16 bytes | Returns the same signed halfword |
| `0x800852B8`, 264 bytes | Uses it to select paired draw offsets |
| `0x80085488`, 164 bytes | Selects display offsets, toggles it with `sltiu`, and invokes draw setup |

Twenty-seven literal resident anchors and all four actual SDK
section/input owners independently support this alias. These remain SDK
assembly, not game-owned matching candidates or attempt-ledger entries.

## Initial integration boundary

Research began from accepted
`6a236f4ff898d240f8adb623afc681d7c165bcb3`. A guarded clean fast-forward
to accepted `7532a3a72b3ce21216bbd2785b3c59bb233b5be6` preserved every
calibrated source, declaration, compiler-profile and Spanish binding
fingerprint before registration. No pending branch or unpublished report
was included.

All 186 prior module records remain unchanged. This registration adds two
images and ten matching C instances / 11,472 bytes, yielding 188 configured
Spanish images, 1,018/1,272 C instances and 1,187,804 C bytes. These are
configured partial-runtime totals, not exhaustive Spanish or seven-release
campaign completion.

Local production acceptance passed for all 188 complete Spanish overlay
images and a fresh Spanish resident. Actual final sections and selected
objects retain all ten C owners, four assembly owners and four raw owners;
all 137 target constants were freshly recompiled. Fresh resident checks
archive 35 callees, three matching callers, both context-pointer owners
and all four supplemental SDK context owners before the clean North
American match. All six policy gates, 61 focused tests and 1,357 full-suite
tests pass without skips. Maintainer acceptance remains separate from
these local gates.

## Independently recovered spiral

The subsequent spiral proof starts independently from accepted MODEL402
coverage, excluding then-pending MODEL341 entries. Both unchanged local
slot sources reproduce the actual 2,480-byte Spanish spans at `0x170C`.
All five earlier helpers are freshly compiled as well. Complete scratch
images use actual generated entry assembly: all 1,238 annotated words per
image agree with retail before assembly, never executable `incbin`.
Entry code and each 7,308-byte suffix remain unrecovered.

Fresh target compilation establishes 173 constants in 692 read-only bytes:
the 137 retained constants plus 36 spiral-arm offsets and extents.
Sixteen 132-byte arms occupy `0x720..0xF60`, ending at the sheet.
Each arm has two eight-byte spine points at relative `0x10`, projected
words at `0x20`, angles at `0x28`, displaced points at `0x30`, their
projected words at `0x40` and widths at `0x48`. Flags/depths are at
`0x64/0x6C`; two signed-halfword offsets per axis are at `0x74/0x78`.
The helper does not claim ownership semantics for the opaque bytes or
interpret the unused color rows.

Each actual image supplies 484 family literal anchors, seven relocated
spiral jumps and 25 spiral call sites with actual resident owners.
Ten complete spiral register-write sets prove the original context stays
in `s8`; stable GT4 pointer `s3` stays at `0x22B4`. The arm cursor resets
between generation, projection and rendering, advancing by 132 bytes.
Existing delay-slot-aware entry dataflow independently proves the call at
`0x10D8` passes the original pointer, despite initialization reusing `s3`.

The 304-byte frame keeps projection outputs at `sp+208..216`, without
overlapping direct stores. The ordering-table spill at `sp+216` is written
once. Signed-halfword outer counter `sp+224` resets for all three passes;
the advancing initialization cursor at `sp+256` remains distinct from
the stable projection-output pointer at `sp+248` and per-arm pointer homes.
Sixteen arms and two projected points are checked independently of the
single-segment rendering loop.

Both positive-offset and negative-offset GT4 footprints contain exactly
the eight XY halfwords and twelve RGB bytes. They neither rewrite UVs nor
extend beyond the 52-byte packet, which ends exactly at the sheet packet.
Each submission independently rejects negative depth or flags, then sorts
using the low halfword of depth and the original ordering table. Colors
retain inner 64/64/0, outer 160/160/128 and the zeroed second endpoint.
Signed length division by 64/16 and odd-frame scale division by eight
retain truncation corrections and subsequent signed-halfword narrowing.

Actual command642000 still selects the 56-byte descriptor at `0x3470`.
Spiral timing words are 144/152/260, giving positive denominators 8/108.
The expansion and contraction calculations use unsigned frame comparisons
and unsigned division, then signed clamps to 4,096 and zero respectively.
Rendering precedes these state updates. The helper's complete direct
context-write set contains only scale `0x27E4`, length `0x27EC` and angle
`0x27F0`; angle advances by 48 after the timing logic.

Fresh Spanish resident ownership establishes all 35 bindings, three
matching callers and both context-pointer storage owners. The single
`RotTransPers` alias replaces its address-based overlay name at unchanged
address `0x80087868`, supported by accepted Spanish declarations and the
actual 44-byte SDK owner. No C, header or compiler-profile changes.

After MODEL341 acceptance, a guarded fast-forward preserves 35 immutable
source/declaration/profile/binding dependencies. The spiral registration
preserves all 188 image records and ten earlier MODEL476 C owners, adding
two instances / 4,960 bytes. Provisional configured Spanish totals become
1,078/1,272 C instances / 1,374,220 bytes. Unknown entry code, suffixes and
expanded seven-release runtime recovery remain in scope.

All 188 production Spanish images, a fresh Spanish resident and a clean
North American executable match retail. Final selected input/ELF ownership
retains twelve C functions, two generated entries and four header/suffix
owners; all 173 constants are freshly recompiled. The 35 callees, three
matching callers and both context-pointer owners are archived before the
North American build. All six repository policies, 72 focused regressions
and 1,407 full-suite tests pass without skips. Exact-head CI and maintainer
acceptance remain separate from these local gates.
