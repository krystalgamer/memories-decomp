# Model wireframe renderer family

The twelve game-owned handwritten callbacks returned by
`Model_MapToWireframePrimitiveHandler` have source-format-based names. In the
North American resident executable they are matched in C by
`src/game/wireframe_renderers.c` under the named
`gcc_2_8_1_g0_handwritten_asm` profile; the regional images keep their
`handwritten_asm` records.

## Source formats, not line shading

| NA entry | Canonical name | Input kind | Stride |
| --- | --- | --- | --- |
| `0x80069E44` | `Model_DrawFt3Wireframes` | FT3 | `0x14` |
| `0x80069F94` | `Model_DrawGt3Wireframes` | GT3 | `0x18` |
| `0x8006A0E8` | `Model_DrawFt4Wireframes` | FT4 | `0x18` |
| `0x8006A268` | `Model_DrawGt4Wireframes` | GT4 | `0x1C` |
| `0x8006A3F0` | `Model_DrawFt3WindowRecordWireframes` | FT3 plus window word | `0x18` |
| `0x8006A540` | `Model_DrawGt3WindowRecordWireframes` | GT3 plus window word | `0x1C` |
| `0x8006A694` | `Model_DrawFt4WindowRecordWireframes` | FT4 plus window word | `0x1C` |
| `0x8006A814` | `Model_DrawGt4WindowRecordWireframes` | GT4 plus window word | `0x20` |
| `0x8006A99C` | `Model_DrawSharedGt3Wireframes` | Shared GT3 | `0x18` |
| `0x8006AAFC` | `Model_DrawSharedGt4Wireframes` | Shared GT4 | `0x1C` |
| `0x8006AC88` | `Model_DrawSharedGt3WindowRecordWireframes` | Shared GT3 plus window word | `0x1C` |
| `0x8006ADE8` | `Model_DrawSharedGt4WindowRecordWireframes` | Shared GT4 plus window word | `0x20` |

FT/GT describes the input primitive record, independently identified through
the corresponding filled renderer and its `gModel_dwPolyFt3/Ft4/Gt3/Gt4Template`
reference. It does not claim textured or Gouraud-shaded line output.
All twelve wireframe targets use the existing flat line templates.
For example, the ordinary triangle path writes
`SXY0/SXY1/SXY2/SXY0` into a closed line packet.

The `WindowRecord` variants consume the source layout with an extra
texture-window control word, accounting for the four-byte stride difference.
The filled counterparts emit the E2 texture-window command and reset word,
identified by the local `DR_TWIN` SDK declaration. The wireframe routines
skip that word and emit no E2 command. These names do not assert a texture
window is applied to lines or that kind bit `0x200` means double-sided:
the filled counterpart still performs its existing NCLIP rejection.

`Shared` identifies the four paths that read cached screen/depth entries
instead of transforming XYZ vertices. They have no RTPT/RTPS, retain the
cached-depth `-1` sentinel and populate the GTE depth FIFO from those entries.
No cache layout or structure declaration is introduced.

## Handwritten ABI and its C form

Each routine is an HMD primitive driver, `u32 *(GsARGUNIT_NORMAL *)` or
`u32 *(GsARGUNIT_SHARED *)`, returning the next primitive block
(`primp + 2`) in `$v0`. The mapper keeps its exact `s32` address casts.

The originals have no stack frame. They park the caller's s0-s2 in scratch
words of the argument block (`0x20`-`0x28`, or `0x28`-`0x30` for the shared
paths), keep the line, draw-mode and `0x55555555` terminator words in those
registers, and use `$at` directly for `beq $13,1` and `rol $13,$13,24`, which
GCC never allocates. The C therefore relies on:

- register declarations for every live value (file-scope global register
  variables for s0-s2 and `$at`, local `register ... asm("$N")` for the rest);
- inline assembly only for the s0-s2 save/restore and GTE operations, from
  `src/game/handwritten_asm.h`, validated by
  `tools/project/handwritten_asm_contract.py`;
- the profile's `-fno-schedule-insns -fno-schedule-insns2`, so statements
  keep the original instruction order;
- `HW_KEEP_LABEL`, which keeps a label in front of the final return. GCC's
  delay-slot filler stops at labels, so the epilogue keeps its
  `addiu $v0,$v0,8; jr $ra; nop` order. The label address is stored in a
  `.discard` section that the linker script drops, so no data is emitted.

Without register declarations no C form matches: the `$at` macro expansions
and the use of s0-s2 while `$t4` stays free cannot come from GCC's allocator.

Every scratch offset, record advance, clipping/sentinel branch,
depth/ordering-table operation, packet terminator, return-header calculation
and packet-work-cursor store is reproduced exactly. Triangle variants use
`gModel_dwLineF4Template`; quad variants use `gModel_dwLineF3Template`.

## Native bindings and acceptance

Outside the North American resident executable, the remaining seventy-two
function records across the other images stay game-owned `handwritten_asm`.
Regional C-reference bindings are resolved before selecting each actual
native boundary: some inventories use raw
native-address names or borrowed-country labels rather than the NA suffix.
No address is guessed from a reference name, and the template inventory is
not treated as an executable image.

The mapper's twelve address references use the declarations in
`wireframe_renderers.h`, now the driver signatures above. Original logical
reference names remain available as linker compatibility bindings.

Validation compares complete caller compiler/MASPSX output and complete
generated fallback assembly with only declared identities reversed, preserving
instructions and directives rather than filtering them. Final acceptance
requires a clean byte-identical resident executable and the PR's fresh
regional/overlay image checks; local source probes are not substitutes.
