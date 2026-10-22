# Model wireframe renderer family

The twelve game-owned handwritten callbacks returned by
`Model_MapToWireframePrimitiveHandler` now have source-format-based names.
They remain `handwritten_asm`, not new C implementations or SDK renames.

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

## Handwritten ABI and packet preservation

The existing `void (void)` declarations are address-use shapes only.
`wireframe_renderers.h` preserves them so the mapper can retain its exact
`s32` function-address casts. They are not a newly recovered callable C ABI.
No `GsARGUNIT *` signature, wrapper or direct C call is added.

The actual bodies save registers in the caller scratch object at `0x20` or
`0x28`, not ordinary C stack frames. Every scratch offset, record advance,
clipping/sentinel branch, depth/ordering-table operation, packet terminator,
return-header calculation and packet-work-cursor store remains unchanged.
The full-word load/comparison at the byte-named current-draw-mode symbol is
not narrowed to a byte operation.

Equivalent-looking variants keep their distinct addresses and bodies.
No generated assembly is manually edited, no instruction is rewritten and
no C source/profile or attempt-ledger entry is added for a renderer.

## Native bindings and acceptance

All eighty-four function records across the seven images are explicitly
game-owned `handwritten_asm`. Regional C-reference bindings are resolved
before selecting each actual native boundary: some inventories use raw
native-address names or borrowed-country labels rather than the NA suffix.
No address is guessed from a reference name, and the template inventory is
not treated as an executable image.

The mapper's twelve address references use canonical declarations while
every pre-existing header remains byte-identical. Original logical
reference names remain available as linker compatibility bindings. All
matching manifests, profiles, overlay C/import maps/metadata, tests, accepted
traces and historical fingerprints remain unchanged.

Validation compares complete caller compiler/MASPSX output and complete
generated fallback assembly with only declared identities reversed, preserving
instructions and directives rather than filtering them. Final acceptance
requires a clean byte-identical resident executable and the PR's fresh
regional/overlay image checks; local source probes are not substitutes.
