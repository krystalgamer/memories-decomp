# Spanish MODEL headers 421 and 571

Twelve independently extracted Spanish secondary images select seven unchanged
local MODEL421 helpers. Each complete 20,480-byte image matches its own retail
slice. Entry and companion functions remain generated assembly; suffix storage
is not claimed as recovered C.

## Actual records and ownership

| Model | Compact record | Stages | Sectors | Actual command |
| --- | --- | --- | --- | --- |
| 84 | 84 | 7/8 | 23364/23374 | 587005 |
| 162 | 162 | 7/8 | 44892/44902 | 587001 |
| 88 | 88 | 9/10 | 24488/24498 | 587000 |
| 114 | 114 | 9/10 | 31664/31674 | 587002 |
| 184 | 184 | 9/10 | 50984/50994 | 587003 |
| 369 | 319 | 9/10 | 88244/88254 | 587004 |

The [instance ledger](spanish-model-variant421-instances.csv) records each slot,
sector, header, command and distinct image hash. The
[attempt ledger](spanish-model-variant421-attempts.csv) identifies all 84 exact
experiments by module, not merely by slot.

| Image-relative span | Selected owner | Bytes per image |
| --- | --- | --- |
| `0..4` | Raw header | 4 |
| `4..11D0` | Generated entry assembly | 4,556 |
| `11D0..1860` | Ribbons C | 1,680 |
| `1860..247C` | Generated companion assembly | 3,100 |
| `247C..2C28` | Bands C | 1,964 |
| `2C28..310C` | Sheets C | 1,252 |
| `310C..367C` | Webs C | 1,392 |
| `367C..398C` | Spokes C | 784 |
| `398C..3D08` | Rings C | 892 |
| `3D08..406C` | Quad C | 868 |
| `406C..5000` | Unclassified raw suffix | 3,988 |

All seven roles use `gcc_2_8_1_g0_split`, the authoritative GCC 2.8.1/MASPSX
2.81 pipeline. The accepted bands wrapper's `VERSION_FRENCH` selects existing
expression structure that independently matches Spanish; it is not a new
regional patch. No C, header, profile or French production metadata changes
are needed.

The twelve images contain 105,984 C bytes, 91,872 generated instruction bytes
and 47,904 raw bytes, covering all 245,760 bytes. Each image has seven real
compiler owners, two generated assembly owners and two sized raw owners.
All 1,139 entry words and 775 companion words per image were compared with
retail. Input objects, final ELF symbols/sections and actual linker selection
establish ownership, rather than an equivalent blob replacing code.

Only entry, ribbons, companion, bands, sheets and webs are directly reachable.
Spokes, rings and quad remain retained code without a direct-entry path.

## Loader, descriptors and resident ownership

All 36 bindings were independently grounded in accepted Spanish metadata and
checked against a freshly matching resident's selected objects, together with
three callers and both context-pointer storage owners.

The selected loader at `0x8005967C`, transfer callback at `0x80059EF4`, slot
setup at `0x8004FC2C` and dispatcher at `0x80058B4C` establish both alternates.
`Model_LoadMonsterMerge` writes `p4 != 0` to `field_DFE` when `p4 >= 0`, then
passes the slot and alternate through `callback_data` and `position`. The
callback consumes those actual fields. Stage counts are
`(96,48,2,1,16,1,16,10,10,10,10,2,2,1,50,1)`, totaling 276 sectors.
Stages 7/8 select alternate zero, stages 9/10 alternate one; their starts are
180/190/200/210 within a record. Skipped stages still consume their sectors.

Load-pointer storage `0x80010014/18` selects `0x8013B000/0x8017B000`; context
storage `0x80010024/28` selects `0x80136000/0x80176000`. Final record-sector
words `+0x110/+0x114` reach slot `+0xD08/+0xD0C`. The command view indexes
`commands[field_DFE]`, initialization receives `command % 1000` (all values
zero through five occur), and updates receive minus one.

Descriptors start at image `+0x4168`, stride 60, inside retained raw storage.
Unsigned halfword `+0x18` is one for all six selected descriptors.

| Command | Words `+24/+28/+2C/+30` | Radius `+38` |
| --- | --- | --- |
| 587000 | 126/130/200/238 | 64 |
| 587001 | 56/68/200/250 | 64 |
| 587002 | 136/148/280/320 | 64 |
| 587003 | 56/64/136/164 | 64 |
| 587004 | 56/64/160/200 | 32 |
| 587005 | 224/236/320/360 | 64 |

## Layout and lifetime evidence

A fresh target compilation checks 195 constants in 780 read-only bytes:
primitive and pointer widths, SDK vector/matrix/coordinate/OT and packet
layouts, helper fields and the following extents.

| Context interval | Contents |
| --- | --- |
| `0..4E0` | Three 416-byte narrow webs |
| `4E0..AB0` | Unclassified gap |
| `AB0..E10` | Eight 108-byte short ribbons |
| `E10..FD8` | One 456-byte band |
| `FD8..1108` | Two 152-byte sheets |
| `1108..1468` | Six 144-byte rings |
| `1468..16A8` | Four 144-byte spokes |
| `16A8..1738` | One 144-byte quad |
| `1738..1754` | Ribbon G3 packet |
| `1754..1778` | Retained quad G4 packet |
| `1778..17AC` | Shared companion/band GT4 packet |
| `17AC..17E0` | Sheets GT4 packet |
| `1874..1888` | Line packet |

The first sheet's size at `FD8+88=1060` also supplies ribbon/web timing or
scale; it is not another ribbon/web field. Entry accesses establish a minimum
context of `0x1978` bytes, not a complete allocation boundary or whole-game
lifetime-isolation proof.

Regressions freeze 90 complete register-write sets per image: `s0..s7`, stack
pointer and frame pointer across all nine functions. Frames in image order
are 232/360/416/296/256/296/272/264/248 bytes. The entry command store at
`sp+236` is a legitimate caller argument home above its 232-byte frame.

The original context goes to `s2` at `+0xC`, then `s6` at `+0x14`; initialization
later reuses `s2`. Delay-slot-aware control flow proves the original definition
reaches all five update calls, at `1024/102C/1044/104C/1068`, targeting
sheets/webs/ribbons/bands/companion respectively. All five argument moves occur
in delay slots. Entry has 77 call sites and 25 distinct resident callees.

Ribbon root `s8` and packet `s7` remain stable. The saved `PSXLONG[8][2]`
projection-status grid occupies stack 208..272, separate from `p` at 272 and
the offset-point flag at 276. Two four-point projections and one offset-point
projection preserve that distinction. Ribbon offsets at record 100/102/104/106
are unsigned halfwords. Eight ribbons each emit one triangle, with nine direct
RGB byte stores and six XY halfwords. Outer colors are 128/128/128 and the
middle color is 64/96/255. Negative depth or saved status is skipped, not
clamped. The custom emitter receives low-16-bit depth and argument three equal
to one. Only `+0x182C` writes context phase-related field `+0x195C`, gated by
signed halfword `+0x194C + 1` equaling descriptor unsigned halfword `+0x18`.
Increment is `step << 5`; radii are 40/150, far Z is 160/192, and both unused
initial angle results remain.

Bands preserve original root `s3` and packet `s2`. Their nine-word projection
flag grid at stack 200..236 is separate from `p` at 240; a grid pointer is saved
at 252. Nine projected points produce eight strips. Unlike ribbons, bands
clamp negative depth to zero and explicitly clear projection flags, twice per
strip. Sort calls pass low-16-bit depth. Both divisions and the clock comparison
are unsigned.

Companion and bands share the GT4 at `+0x1778`, each directly writing twelve
RGB bytes and eight XY halfwords. Sheets directly write twelve RGB bytes;
webs/spokes/rings write a line attribute word and six color bytes; quad writes
twelve RGB bytes. Sheets/webs use separate projection outputs at stack 208/212.
These direct footprints are not claims about every SDK output write.
Inherited family checks cover 177 literal anchors per image, actual descriptor
selection, boundaries, bindings, wrapper ownership and entry reachability.

## Integration scope

All 194 previously configured Spanish image records are preserved. The twelve
new images add 108 inventoried functions and 84 C instances, producing branch
totals of **206 images, 1,206/1,430 C instances and 1,537,996 C instruction
bytes**. External acceptance is separate from local registration. README and
global-usage reports are not refreshed as part of this matching change.

Local acceptance passes all 206 configured Spanish images, the fresh Spanish
resident, final selected C/assembly/raw owners, a fresh target layout probe,
actual loader/pointer ownership and a clean North American executable.
Six repository policy gates, 64 focused regressions and 1,456 full regressions
pass with no skips. Spanish owner artifacts were archived before the clean
North American build. Exact-head regional CI and external acceptance remain
separate publication gates.
