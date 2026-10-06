# French MODEL117 runtime entries

Six runtime images use headers 117/247: models 139 and 146 at stages 7/8,
and model 15 at stages 9/10. Each contains a closed 4,232-byte entry and
a 16-byte C-owned unit-scale VECTOR. The six instances contribute 25,392
C instruction bytes and 96 literal bytes. The 96,360 suffix bytes remain
unclassified; this is not exhaustive runtime completion.

## Image and ownership evidence

| Offset | Bytes | Owner |
| --- | ---: | --- |
| `0x0000` | 4 | Raw module header |
| `0x0004` | 4,232 | C entry |
| `0x108C` | 16 | C unit-scale literal |
| `0x109C` | 168 | Raw six-element GsIMAGE view |
| `0x1144` | 16,060 | Unclassified suffix, including selected descriptors |

Loads are `0x8013B000` and `0x8017B000`; contexts are `0x80136000` and
`0x80176000`. Commands are `48000 + argument`: model 15 selects 0, model
146 selects 1, and model 139 selects 3. Both slot entry variants are compared
without masks. Complete-image proofs use actual sized C function definitions,
exclusive C literal sections and three disjoint raw owners per image, with
22 resident destinations checked against the exact resident ELF.

## Independently recovered declarations

The descriptor stride is 30 bytes: four color/part bytes, eleven signed
halfwords, and four trailing bytes. The byte at `+0x1A` remains unnamed.
Count is at `+0x0E`; row count, travel duration, fade duration, spacing and
initial delay follow at `+0x10` through `+0x18`. Texture tile size and frame
count are bytes at `+0x1B` and `+0x1C`.

The observed state extent is `0x724`. Positions, targets and radial endpoints
begin at `+4`, `+0x204` and `+0x404`. The maximum selected count is 40, so
each declaration exposes only 40 SVECTORs and leaves the following 192 bytes
unknown. Row and frame views likewise expose 40 bytes at `+0x604` and
`+0x644`, with 24-byte gaps. Forty scale halfwords at `+0x684` precede a
48-byte unknown gap. Six packed texture results begin at `+0x704`; the
completion byte and elapsed word are at `+0x71C` and `+0x720`.

Twenty-nine target-compiled layout assertions protect these views and SDK
sizes. All traversals stay within their selected count. SDK declarations,
including `ccos`, `csin`, `GsGetLwUnit`, and packet/projection helpers, come
from existing local headers. The cached view yaw is read as the signed
halfword at offset 2 of `Model_GetViewMetricsBuffer`.

## Native behavior

Construction clears three position components without claiming or clearing
SVECTOR padding. Three random calls select each texture row, frame and
scale (`rand() % 4096 + 2048`). A second loop generates target points around
Y=-350 and slot-directed Z=450. A third generates radial endpoints using
`ccos`/`csin`, signed division by 4096 and Z=`direction * (450-radius)`.
Six images are uploaded; elapsed starts at negative descriptor delay.

The negative-elapsed path preserves an unusual native behavior: it passes
the uninitialized local base MATRIX to `GsSetLsMatrix`, obtains the active
slot's coordinate-unit transform and stores the translation in position 0.
It then advances elapsed by the frame step and returns zero. Initializing
that local would change the original instruction stream; this reconstruction
does not assert that the native path is well-defined C.

Normal updates copy the light-source matrix and prepare a textured quad.
Each staggered particle uses `time = elapsed - i*spacing`. At time <= 0,
its position follows the selected coordinate unit. During travel, each
component moves toward its target by twice the signed quotient of the
remaining displacement and remaining time. During fade it instead moves
toward the radial endpoint and fades half-intensity RGB. These updates
use constant factor 2, not the queried frame step.

Rendering uses the individual random scale. During travel, the displayed Y
also subtracts a sine arc; this displacement does not alter stored position.
The active slot and cached yaw choose normal or reversed projection output
order. Non-64 tiles use the selected row/frame and row-specific CLUT;
64-pixel tiles use fixed UVs and CLUT 1. Rendering requires the native time,
flag and nonnegative depth checks. Even after the fade interval, projection,
UV setup and frame advancement still occur, though submission is suppressed.

A second pass draws six-frame flashes at the target points with CLUT 3.
After matrix restoration and elapsed advancement, the function returns 0
before travel ends, 4 through the final spacing/fade threshold, then 1 once
and 2 thereafter. The final comparison is strictly greater-than.

## Matching experiments

The local `gcc_2_8_1_g0_split` GCC 2.8.1/MASPSX 2.81 profile is unchanged.
The ledger retains three paired experiments and two canonical matches:

| Experiment | Entry bytes / frame | Differing words per slot |
| --- | --- | ---: |
| Native two-phase particles | 4232 / 280 | 860 |
| Bounded offset and SDK UV expressions | 4232 / 280 | 118 |
| Target-before-end cursor initialization | 4232 / 280 | 0 |

Narrowing the constructor's Y offset to signed 16-bit, preserving SDK UV
width expressions, assigning flash CLUT before UV stores and advancing the
flash cursor before its index recover native scheduling. All selected Y
offsets are within signed 16-bit range. Initializing the target cursor before
packet setup and the endpoint cursor after quad setup recovers native saved
register versus stack allocation without forced registers, artificial stores
or fake dependencies.
