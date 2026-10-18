# Card preview detail-panel renderer

`CardPreview_DrawDetailPanel` is the resident renderer formerly named
`func_80028B08`. The complete function submits the preview frame and card
image, encodes and draws the ATK and DEF digit rows separately, repeats the
level icons, and draws the card-detail attribute tile through
`DisplayObject_SubmitPacket`.

The producer is the paired resource constructor in
`src/game/duel_effect_resource_setup.c`. It installs this callback on the
secondary object, publishes that object as `object_04`, and links its
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

Only the function identifier changes in the complete main C body. Its
early visibility returns, projected path, fixed scratchpad addresses,
argument forwarding, ordered submissions, clamps and digit loops are
unchanged. The three documented compiler-steering constructs, including
guarded write-backs and the single-pass extent loop, remain byte-for-byte
as written. Their presence is not a reason to simplify this renderer.

The opaque second callback argument remains `s32 arg1`; no ordering-table
or depth type is inferred. The address-based scratchpad helper types and
regional macro names remain unchanged. No overlay implementation, display
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
