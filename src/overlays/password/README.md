# Password Overlay

This directory contains matching source from the password-screen
runtime module.

Verified boundaries:

| Item | Range |
|---|---|
| WA package | sectors `7983-8069` |
| Executable phase | sectors `8054-8069`, `0x7800` bytes |
| Runtime image range | `0x80168000-0x8016F800` |

The [name-entry package](../name_entry/README.md) contains a closely related
`0x7800` phase at sectors `7968-7983`. The two chunks share code but differ in
their final data sector, so they must not be treated as one interchangeable
module image.

Module-scoped symbol evidence is kept in
[`notes/research/Unchiga_Symbols/modules/password.txt`](../../../notes/research/Unchiga_Symbols/modules/password.txt).

The verified executable phase is tracked in
[`config/slus_01411/overlays.json`](../../../config/slus_01411/overlays.json).
`make overlays` extracts it to `tmp/overlays/password/module.bin`, and
`make verify-overlays` checks the archive and payload hashes.

Its accepted build layout is
[`config/slus_01411/overlays/password.yaml`](../../../config/slus_01411/overlays/password.yaml).
`make match-overlays` splits, assembles, links, and verifies the complete
module byte-for-byte. Keep candidate sources, objects, and diffs under `tmp/`
until a function passes this overlay-specific exact-match process. Do not add
this module to the resident `config/slus_01411/matching_c.json`.

## Name-entry selection-frame packets

`NameEntry_Init` installs
[`NameEntry_DrawSelectionFrame`](name_entry_draw_selection_frame.c)
(`0x801681A0`) at callback offset `+0x4C` of the selection cursor stored in
`D_8016D404`. The initializer sets its position to `(22, 24)` and its
dimensions to `16 x 16`; keyboard movement later updates that cursor's
position and width. This establishes a name-entry selection-frame role,
not a general password-screen or input handler.

The matching drawing routine uses the actual Psy-Q line records rather than
byte-offset views of its scratchpad packets:

| Scratchpad address | SDK record | Source payload words | Use |
|---|---|---:|---|
| `0x1F800000` | `LINE_F3` | 5 | Four three-vertex corner brackets |
| `0x1F800040` | `LINE_G2` | 4 | Four callouts to `x=0/320` or `y=0/192` |

The lengths exclude each packet's tag word; the records occupy 24 and 20
bytes respectively. `setLineF3` also supplies the `0x55555555` polyline
terminator. The corner color and first callout color are green; the second
callout color is black. These are input packet colors, not a claim about
opaque final pixels: `func_8005B260` copies the packet and applies draw-mode
and semi-transparency handling.

The caller uses that helper's `u32 *`, `GsOT *`, `s32`, `s32` interface.
Its signed priority load and explicit `u16` conversion at submission retain
the target's access and mask placement. Packed color-word writes and vertex
store order remain unchanged.

[`name_entry_frame.h`](name_entry_frame.h) declares the callback once and
is included by both its definition and `NameEntry_Init`. The shared
`NameEntrySelectionFrameView` names only the proven drawing prefix:
signed priority at `+0x14`, signed x/y at `+0x30/+0x32`, and unsigned
width/height at `+0x3C/+0x3E`. Padding and field widths are unchanged; this
does not claim that the cursor allocation ends at `+0x40`. The installer
retains its existing object writes and callback-slot assignment rather
than pretending that the callback takes no arguments.
