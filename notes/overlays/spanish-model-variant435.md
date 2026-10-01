# Spanish MODEL headers 435 and 585

Twenty-six Spanish images, with 23 distinct complete hashes, reuse the
unchanged accepted `variant418_{sheet,webs,ribbons,bands,spokes,rings,quad}.c`
bodies through the existing French435 wrappers. The fourteen canonical
compiler objects independently provide 182 C instances / 204,776 instruction
bytes under named `gcc_2_8_1_g0_split`, GCC 2.8.1 and MASPSX 2.81.
No shared implementation, type declaration or compiler profile is changed.

## Loader and code boundaries

Models34/71/124/182/279/361/491/580/640 select stages7/8;
models166/275/469/590 select stages9/10. Compact records omit the loader's
reserved model ranges. Each stage selects a ten-sector slice at
`record * 276 + 180 + (stage - 7) * 10`, loaded at
`0x8013B000` or `0x8017B000`. The matched resident controller calls
offset4 with its context and decoded initial command, then update command-1.
The [instance ledger](spanish-model-variant435-instances.csv) records
each actual Spanish slice, header, request and complete SHA-256.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `4..1084` | 4224 | generated assembly | yes |
| `1084..1AD4` | 2640 | generated assembly | yes |
| `1AD4..1E7C` | 936 | sheet C | yes |
| `1E7C..2258` | 988 | webs C | yes |
| `2258..28A4` | 1612 | ribbons C | no |
| `28A4..2FB0` | 1804 | bands C | no |
| `2FB0..32B8` | 776 | spokes C | no |
| `32B8..3634` | 892 | rings C | no |
| `3634..3998` | 868 | quad C | no |

Strict walks cover every instruction and terminal return in all nine
spans, including the retained ribbon helper at `+0x2258`. Sheet and
webs are entry-call reachable. The other five C helpers remain retained
code without a demonstrated direct entry-call path.

Fifty-two assembly instances / 178,464 bytes remain untranslated.
Real input and final storage owners preserve every four-byte header and
5,736-byte suffix, covering all 532,480 image bytes. The 149,136 suffix
bytes remain unclassified, not established non-code.

## Independently verified views

Entry captures `a0 -> s2 -> s6` and initializes three 608-byte web
records from context0 through `+0x720`, not from `+0x720`. Each contains
two six-by-six `SVECTOR` grids at0 and `+0x120`, with 48-byte rows,
color at `+0x240` and scale at `+0x254`. The helper receives the same
context through the call at module `+0xF14`, with `a0 = s2` in its
delay slot. Sixty Spanish instruction anchors per image check context
capture, initialization, counters, record advances, helper access and
descriptor construction.

One 492-byte padded band begins at `+0x10F0`, followed by one 152-byte
sheet at `+0x12DC`, six 144-byte rings at `+0x1374`, four 144-byte
spoke records at `+0x16D4` and one 144-byte quad at `+0x1914`.
The spokes timing input is the sheet size at `0x12DC + 0x88 = 0x1364`.
Band vectors and projected coordinates have distinct verified strides;
colors are four-byte entries, not packed three-byte arrays.

The band and sheet use `POLY_GT4` packets at `+0x19E4/+0x1A18`.
The quad uses `POLY_G4` at `+0x19C0`; line helpers use `GsGLINE`
at `+0x1AE0`. Translation, step and phase views include
`+0x1B08/+0x1B0C/+0x1B10`, `+0x1B64` and `+0x1B9C`.

The 147 independently target-compiled constants cover all seven canonical
record types and accessed fields, grid/array endpoints, SDK vectors,
matrices, stored coordinate pointers, `GsOT`, polygon formats,
packed line colors and four-byte pointer/integer widths. Their actual
owner is a 588-byte `.rodata` section containing arrays at offsets 0,
292 and 448. The original 112 values are preserved alongside 35 ribbon
and `POLY_G3` values. GCC emits size-zero NOTYPE labels, so verification checks the
real section extent and every constant rather than inventing symbol sizes.

Actual requests select commands0/1/2/3/5/6/7/9/10/11. The selected
68-byte descriptor is at module `+0x3A94 + (command % 1000) * 68`,
and entry stores its pointer at context `+0x1B74`. Every selected window
fits its preserved suffix owner. Direct entry accesses establish a minimum
context view through `+0x1BB4` (7,092 bytes).

Fresh exact Spanish resident proofs verify 37 real callee input/final
owners, three matching initializer/controller/loader owners and both
context-pointer storage owners at `0x80010024/28`. Those pointers belong
to input `.data` in `spanish_raw_80010000.o`, despite the mixed executable
output `.main`. The contexts `0x80136000/0x80176000` and their minimum
views do not overlap the selected MODEL, primary or secondary loads.
Neither this separation nor initialized record counts prove allocation
capacity or whole-game lifetime isolation.

## Exactness and scope

The [attempt ledger](spanish-model-variant435-attempts.csv) records fourteen
terminal canonical-wrapper matches. Full unmasked links, actual selected
compiler/assembly/raw owners, sized functions, dependency fingerprints,
target layouts and resident owners are checked independently.

Regional regressions reuse the accepted French source/boundary fixture
and add Spanish fallback-binding, actual descriptor and minimum-context
checks. All twelve retained compiler objects were freshly rebuilt against
current dependency fingerprints before the combined proof.
Existing accepted module records are preserved. This extension adds
26 C instances / 41,912 bytes, yielding 840/1,082 C instances and 868,804
bytes across 154 configured images. Progress snapshots
remain separate; unknown game code, further Spanish runtime discovery and
the expanded seven-release campaign remain open.

## Retained ribbons

The unchanged wrapper's `VERSION_FRENCH` selects the measured indexed
projection expressions, not a release-name restriction. Both 1,612-byte
objects match all 13 Spanish images per slot. No direct `jal` to this
helper occurs in the module's code spans; this does not rule out unknown
indirect invocation or establish whole-game reachability.

Eight 116-byte records occupy `[0xD50,0x10F0)`, ending at the padded band.
Each holds two-point spines at 0 and 32, projected coordinates at 16 and 48,
32-bit angles at 24, widths at 56, depths at 100, and unsigned 16-bit screen
offsets at 108/112. The range `[64,100)` remains opaque. Spine radii are
40/150, adjacent records turn by 512, and the sideways length is 16.
The flag's low bit selects the far point's z displacement 160/192.
Translation preserves signed division by 1024; scale reads the sheet's
size at `0x12DC + 136`.

The fixed 28-byte `POLY_G3` packet is `[0x19A4,0x19C0)`. Entry constructs
that address and passes it to the real `SetPolyG3` SDK owner at `+0x260`.
The helper draws one triangle per record. Sorting requires signed depth
at least zero; the projection flag output is not read. The final angle
update adds `step << 5` only when signed frame-halfword plus one equals
the selected unsigned halfword at descriptor+32. All ten actual request
values select a halfword value of 1; full descriptor hashes are preserved.

The 296-byte frame separates coordinate `[128,208)`, projection output
`[208,212)`, flag `[212,216)`, sheet pointer `[216,220)` and ordering-table
pointer `[220,224)`. Index storage begins at 224; turn/tilt/z-step words begin
at 232/236/240, projected-spine temporary pointers at 244/248, and saves at 256.
Context, packet and ribbon pointer writes are checked across the complete
helper. Its direct context minimum `+0x1BB2` remains below the entry's
`+0x1BB4`, not an allocation-capacity or lifetime guarantee.

The raw proof checks 185 retained/ribbon instruction anchors per image.
Twenty static calls resolve to eleven real resident helpers. The existing
44-byte SDK owner at `0x80087868` is now named `RotTransPers` in the shared
bindings and all 26 symbol maps, with byte identity and real selected owners
verified. No binding address, SDK classification or resident inventory changes.
