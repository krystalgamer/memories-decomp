# Card preview detail-panel renderer

`CardPreview_DrawDetailPanel` is the resident renderer formerly named
`func_80028B08`. The complete function submits the preview frame and card
image, encodes and draws the ATK and DEF digit rows separately, repeats the
level icons, and draws the card-detail attribute tile through
`DisplayObject_SubmitPacket`.

The producer is the paired resource constructor in
`src/game/duel_effect_resource_setup.c`. It installs this callback on the
secondary object, publishes that object as `secondary_object`, and links its
`field_54` to the allocated-second primary window. The renderer follows
that link and the resource-slot index. The existing
`CardPreview_UpdateVariant` callback establishes the shared subsystem
name; this is not just a stat-only renderer or a new allocation API.

| Configuration | Address | Size | Owner |
| --- | --- | --- | --- |
| `slus_01411` | `0x80028B08` | `0x600` | main C |
| `slpm_86398` | `0x800288E8` | `0x600` | main C |
| `sles_03947` | `0x80028AC8` | `0x674` | European wrapper |
| `sles_03948` | `0x80028C0C` | `0x674` | European wrapper |
| `sles_03949` | `0x80028C0C` | `0x674` | European wrapper |
| `sles_03950` | `0x80028C0C` | `0x674` | European wrapper |
| `sles_03951` | `0x80028C0C` | `0x674` | European wrapper |

All entries retain `gcc_2_8_1_g8_split`. The main owner is
`src/game/card_preview_draw_detail_panel.c`, with
`src/game/european/card_preview_draw_detail_panel.c` supplying the original
European constants and conditional body. The obsolete European function
alias is removed from this wrapper and its constructor because both now
refer to the same canonical logical symbol.

## Deliberately retained boundaries

At the naming checkpoint, only the function identifier changed in the main C
body. Subsequent confirmed stat-member names are documented separately in
`duel-resource-constructor-return.md`; no operation changes. Its
early visibility returns, projected path, fixed scratchpad addresses,
argument forwarding, ordered submissions, clamps and digit loops are
unchanged. The three documented compiler-steering constructs, including
guarded write-backs and the single-pass extent loop, remain byte-for-byte
as written. Their presence is not a reason to simplify this renderer.

The opaque second callback argument remains `s32 arg1`; no ordering-table
or depth type is inferred. The address-based scratchpad helper types were
unchanged at that checkpoint; the later packet-origin view is described
below. Regional macro names remain unchanged. No overlay implementation, display
layout or frozen French-options fingerprint input is changed. Historical
attempt evidence and optional generated reports are not rewritten.

The semantic-map append preserves every previous row, including historical
duplicate keys. Regional symbol edits are scoped to this function's address
and preserve pre-existing aliases at other addresses.

## Controls

The supported compiler's main assembly differs only in `.file` and the
function symbol directives/label. All instructions, frame and scheduling
remain identical; incremental full-executable matching succeeds.

Focused checks cover seven-region owner/address/size/profile bindings and
the public callback type. The existing incompatible sprite-record negative
control follows the moved owner and keeps its oracle unchanged. These
checks do not execute the renderer, emulate scratchpad/GTE behavior, or
claim a modern-native runtime witness for its compiler-steering C.
Final clean complete-executable matching remains the acceptance gate;
regional/cloud execution stays with normal PR validation.

## Canonical packet-origin local

The eight bytes at scratchpad `0x1F800398` are now borrowed as
`DisplayObjectPacketOrigin`: signed halfword X/Y at `+0/+2`, then the
signed word `divisions` at `+4`. This is the existing record the packet
dispatcher actually consumes in `display_object_helpers.c`, not a new
allocation, layout or broader dispatcher ABI.

The projection helper's explicit `ProjectionOut *` view is retained. Its
three stores publish the same X/Y and zero-extended object `field_65` into
that same eight-byte shape. The renderer passes this origin to its frame
submissions, then clears the complete division word before the card/detail
submissions. The backend subtracts X/Y before projection and uses the
division count to select/pass `DivideFT4`.

The local and measured card-list fifth-argument type now state the actual
record, and `field_4 = 0` becomes `divisions = 0`. Every other renderer
token, fixed scratch address, projection cast, call, expression, guard,
uninitialized steering temporary and order is retained. Default byte and
raw-attribute dispatcher views remain unchanged. The old Extra shell stays
as historical type evidence, not the active local/argument view.

Supported-compiler controls check the record's sizes, offsets and signed
fields, accept all three measured API views, and reject the legacy/byte
card-list extra types and wrong arity. A separate native witness executes
only the actual three projection-prefix stores and the actual reset
statement: 262,144 producer/reset pairs at each `-O0`/`-O2`, with all
coordinate halfword patterns, four high-word patterns, all count bytes
and full-word reset inputs under complete byte/guard oracles. Negatives
reject a wrong coordinate, signed count-byte extension and partial reset.

That witness does not execute the renderer, GTE, packet dispatcher or GPU.
Complete renderer compiler/MASPSX output remains identical; full retail
matching and normal regional PR checks remain the acceptance gates.
