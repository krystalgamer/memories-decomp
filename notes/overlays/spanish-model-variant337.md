# Spanish MODEL variant 337/487 entries, rings and ribbons

These six independent ten-sector images share the 1,216-byte ring helper at
offset `0x1278`. Slot zero loads at `0x8013B000`; slot one at `0x8017B000`.
Each slot has its own GCC 2.8.1 / MASPSX 2.81 compilation under the named
`gcc_2_8_1_g0_split` profile. Each full image is separately linked and hashed.

| Model | Compact record | Stages | Archive sectors | Normal command |
|---:|---:|---|---|---:|
| 110 | 110 | 9/10 | 30560/30570 | 1 |
| 159 | 159 | 9/10 | 44084/44094 | 4 |
| 410 | 360 | 7/8 | 99540/99550 | 7 |

Header families span loader stages: model 410's stages 9/10 are a different
family and are not registered by this change. None of these six images is
treated as an identical-image duplicate.

## Boundaries and preserved scope

| Offset | Bytes | Status |
|---|---:|---|
| `0x4` | 2464 | Matching entry C |
| `0x9A4` | 2260 | Matching ribbon C |
| `0x1278` | 1216 | Matching C |
| `0x1738` | 2084 | Game-owned unmatched assembly |

Direct-call traversal reaches every instruction in these contiguous
intervals, with one return per function and no unresolved indirect jump.
This does not prove the absence of other runtime entry points. The header
and complete 12,452-byte suffix at `0x1F5C..0x5000` retain real generated
storage. The suffix remains **unclassified**, not excluded code or C coverage.
The six original rings contribute 7,296 instruction bytes, the six ribbons
13,560 bytes and the six entries 14,784 bytes: eighteen C instances /
35,640 bytes in total. The six secondary function instances / 12,504 bytes
remain explicitly unmatched.

## Local layout and behavior evidence

Two 152-byte ring records start at context `0x3A4`. Each contains four rows
of four canonical `SVECTOR`s, two `CVECTOR`s at offsets 128/132 and a signed
scale at 136; its final twelve bytes remain unknown. The helper reuses a
single canonical `POLY_GT4` at `0xC38` for four projected quads per ring.
Three vertices receive the second color and the fourth the first. Negative
depth or projection flags suppress submission; depth is multiplied by eight
and divided by ten in that order.

The entry passes context `0xCCC` to `GsGetLwUnit`, establishing a canonical
`MATRIX` whose translation starts at `0xCE0`. Its end is the next `SVECTOR`
at `0xCEC`, not an overlapping guessed sixteen-byte position vector.
The first ring uses that translation and the second the target vector.
Their rotations differ by a half turn about Y and the sign of Z rotation.
The shared angle advances by `step * 64` once per ring.

Odd frames add scale divided by eight to each scale component. The first
ring grows to 4096 using unsigned elapsed/configuration interpolation, then
shrinks during state three. The second grows to 8192 at `step * 1024` in
state two and shrinks at `step * 64` in state four. The observed clamps and
state transitions are retained, including unsigned division semantics.

The entry computes its configuration pointer as image base plus `0x2058`
plus command times 36. The final metadata sector of each ordinary model
record supplies requests 503001, 503004 and 503007. The resident's secondary
dispatch passes each request modulo 1000, selecting configuration views
1, 4 and 7 inside the real suffix owner. This establishes those load paths
and access windows, not global array capacity or exclusive runtime use.

Forty-nine target-compiled size/offset values verify the local and SDK
layouts. The partial state size `0xD70` is not caller allocation proof.
All nine helper callees have real selected resident input objects and sized
function symbols whose final bytes match the legal Spanish executable.
No inline assembly, fixed-register declarations or synthetic instruction
storage is used. The two exact source experiments are recorded in
[`spanish-model-variant337-attempts.csv`](spanish-model-variant337-attempts.csv);
fingerprints cover the scratch source/header pair before include-path and
type-prefix promotion. The regional manifests follow existing overlay
integration conventions; the resident-only integrator is North-American-only.

## Independently recovered entry-called ribbons

New accepted local French337 indexing evidence resolves the previously paused
Spanish `0x9A4` helper without repeating its earlier compiler/source guesses.
The existing `variant337_ribbon{,_slot1}.c` wrappers include the canonical
`variant320_ribbon.c` with its measured indexed terminal-point branch.
Both slots compile to exactly 2,260 bytes / 304-byte frame, with zero differing
words in all six Spanish images. No C body, header or compiler profile changes.
The earlier scratch attempts remain distinct evidence; two new canonical
terminal records follow the two unchanged historical ring records.

Four fresh compiler objects reproduce six complete 20,480-byte images,
preserving every accepted ring, remaining assembly span, header and suffix.
Ninety-five freshly compiled constants (380-byte read-only data) combine
49 retained ring/state/SDK checks with 46 ribbon/packet checks. Each Spanish
image independently passes 112 raw anchors and eight additional lifetime
anchors, the full 26-call sequence, and all function-boundary traversals.

One 932-byte ribbon occupies context `0..0x3A4`, ending at the rings.
Seventeen-point arrays start at `a=0`, `sa=0x88`, `angle=0xCC`, `b=0x110`,
`sb=0x198`, `width=0x1DC`, `otz=0x2D8`, `flag=0x31C`, `ox=0x360`,
and `oy=0x382`. Entry initializes RGB at `0x220..0x222` to 192.
The ribbon checkpoint left the fourth color byte and `0x224..0x2D8`
uninterpreted. The entry-only views below recover selected initialization
fields without widening the renderer's existing opaque view.

The negative-command branch at `0x60` reaches `0x698`, preserving the
original context in `s2`. Unsigned clock `0xD24` is compared with descriptor
field `0x10` before the call at `0x818`; its delay slot supplies that context.
The helper holds context in `s7`. Its record-cursor writes, packet-pointer
increments/decrements and stack-pointer lifetime are independently verified.
The 304-byte frame saves registers at `264..304` and spills the ordering-table
pointer at `216..220`.

Two 40-byte FT4 packets occupy `0xC6C..0xCBC`, alternating by segment parity.
Only RGB and eight coordinate halfwords are directly written, ending at
packet byte 35. Both signed depth and flag must be nonnegative to sort,
using the low sixteen depth bits. The indexed terminal endpoint remains
`k` while the preceding endpoint remains literal `15`.

All six actual requests select their own 36-byte descriptors at
`0x2058 + command%1000*36`. The entry gate, growth interval `0x14/0x18`
and fade interval `0x1C/0x20` are independently checked. Phase one grows
signed sixteen-bit length to sixteen and advances to phase two; phase three
shrinks signed displacement to zero. Unsigned division semantics and traps
remain exact. Both wave clocks advance even when drawing is disabled.

Fresh Spanish resident verification establishes all 33 actual binding owners,
three matching callers and both context-pointer data owners. The eleven
distinct ribbon callees include independently sized `rsin` (60 bytes),
`rcos` (160), `RotTransPers` (44) and `ratan2` (372); their established SDK
aliases replace address labels without changing addresses. The `rsin`
extent is its own function, not the interval to the next named binding.
Every symbol map retains the other regional labels.

The direct helper view ends at `0xD70`; entry's last observed halfword
at `0xD7E` establishes `0xD80`. These views do not overlap selected model,
primary or secondary loads, but do not establish allocation capacity or
whole-game lifetime isolation.

The independent accepted cutoff is
`2f38ffd754e6e62bf0f6e4a1337638410ce6b2a2`, including accepted MODEL338
ribbons. This batch brings configured Spanish coverage to 900/1,082 C
instances / 1,009,708 bytes across the same 154 images. Entry, the final
helper and unclassified suffixes remained open at that cutoff; this is not runtime or
seven-release completion.

## Independently matched entries

The accepted local French MODEL337 entry provides a new structural lead.
Its unchanged `variant337_entry{,_slot1}.c` and private header compile under
`gcc_2_8_1_g0_split` to the exact 2,464-byte Spanish entry in all six images.
No C body, header, primitive declaration or compiler profile changes.
Two new terminal attempt records preserve all four prior ring/ribbon records.
The secondary helper at `0x1738` remains genuine generated assembly; paused
experiments are not promoted by this result.

Fresh entry, ribbon and ring compilations produce eighteen real C owners /
35,640 bytes, six real generated-assembly owners / 12,504 bytes and twelve
non-executable header/suffix owners / 74,736 bytes. Every byte of all six
20,480-byte images matches. The independent scratch link freshly assembled
the fallback instructions and checked their 521 annotated words per image
against the retail bytes; it did not encode executable fallback bytes with
`incbin`. Production uses the normal generated-assembly pipeline.

The new target probe verifies 102 constants / 408-byte read-only data:
canonical primitive, pointer, vector, matrix and packet layouts; the 36-byte
descriptor; 932-byte record; two 152-byte rings; two 888-byte streamers; and
the entry's observed `0xD80` minimum state extent. One record ends at `0x3A4`,
two rings at `0x4D4`, and two streamers at `0xBC4`. Packet views begin at
`G3=0xBC4`, `G4=0xBE0`, `GT4=0xC04`, and `FT4=0xC6C`; paired GT4/FT4
strides are 52/40 bytes. Direct texture-coordinate, CLUT and texture-page
stores are independently checked before and after each packet increment.
These are observed access views, not recovered allocation capacity.

Each actual Spanish image passes 124 literal instruction anchors, six
relocated anchors, eight complete register-write sets and all 48 entry call
sites. The original context is captured in `s2` at `0xC` and in `s6` at
`0x14`. Initialization reuses `s2` for a texture result at `0x18C`, so a
whole-function immutable-register claim would be false. Delay-slot-aware
reaching definitions prove that only the original `0xC` definition reaches
the ribbon, ring and secondary calls at `0x818`, `0x820` and `0x83C`.
The initialization jump at `0x690` bypasses those calls; its delay slot
stores the command halfword.

The entry allocates a 192-byte frame. Incoming `a1` is legitimately saved at
`sp+0xC4`, in the caller-provided argument home beyond the allocated frame,
and later reloaded as both a word and a halfword. This is not an out-of-frame
local temporary. Projection outputs occupy `sp+0x70..0x80`: position XY,
interpolation, flag, and target XY. No direct stores overlap them. The
advancing streamer-pointer spill at `0x80` and stable quad-pointer spill at
`0x84` are distinct. Calls at `0x7A4` and `0x7D8` use independently checked
input/output addresses. The first projected X is loaded unsigned and Y
signed; the later halfword delta stores do not justify inventing a signed-X
load.

Actual matching resident callers establish initialization with
`handler(secondary, request % 1000)` and updates with
`handler(secondary, -1)`. The returned value is stored in the model slot's
`field_E0E` and dispatched through its existing control switch. Metadata
requests 503001/503004/503007 select descriptor indices 1/4/7; their complete
36-byte windows and positive growth/fade intervals are checked independently
in each image. Unsigned frame comparison gates ribbon/ring dispatch;
phase at least three gates the secondary helper.

The update increments the frame counter every time. On an animation-frame
change, two separate `Model_GetFrameStep` calls update the accumulated frame
and stored step; their repeated calls are preserved. Before phase six,
positive `field_D78` decreases by 128 and clamps to zero; from phase six it
is assigned `fade << 5`. Phases two through four return four. Phase five
returns one and advances to six. In phase six, fade advances only when it
starts below 64; reaching the threshold clamps to 64 and enters phase seven.
An already-at-least-64 fade does not automatically take that transition.
Phase seven returns two; other cases return zero.

A fresh exact Spanish resident link establishes all 33 actual callee owners,
three matching caller owners and both context-pointer storage owners before
the North American gate replaces the build output. Eleven entry SDK aliases
are independently grounded in accepted Spanish MODEL408 bindings and the
actual Spanish input/final function sections: `GetTPage`, `GetClut`,
`SetSemiTrans`, `SetShadeTex`, `SetPolyG3`, `SetPolyFT4`, `SetPolyG4`,
`SetPolyGT4`, `SquareRoot0`, `Square0`, and `GsGetLwUnit`. Only overlay
bindings and their symbol maps change; resident inventories and global names
are untouched.

The independent accepted integration base is
`ea0fca231fd658f5b10c32317f1a230048250a73`, including accepted MODEL476.
All 188 image records and the twelve prior MODEL337 C instances are
preserved. The six new entries bring configured Spanish coverage to
1,024/1,272 C instances / 1,202,588 bytes. The secondary helper, every unknown
suffix and expanded seven-release runtime recovery remain open.
