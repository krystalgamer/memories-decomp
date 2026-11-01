# PAL glyph spiral and linear fades

Six connected European callbacks use evidence-backed names, bound
directly at their own address in every PAL symbol file:

| Former label | Name | UK / other PAL address | Bytes |
|---|---|---|---|
| `func_80039B3C` | `DuelEffect_UpdateGlyphSpiralFade` | `80039B3C` / `80039D3C` |488 |
| `func_80039D24` | `DuelEffect_StartGlyphSpiralReveal` | `80039D24` / `80039F24` |96 |
| `func_80039D84` | `DuelEffect_StartGlyphSpiralConceal` | `80039D84` / `80039F84` |116 |
| `func_80039DF8` | `DuelEffect_UpdateGlyphLinearFade` | `80039DF8` / `80039FF8` |144 |
| `func_80039E88` | `DuelEffect_UpdateGlyphFadeIn` | `80039E88` / `8003A088` |32 |
| `func_80039EA8` | `DuelEffect_UpdateGlyphFadeOut` | `80039EA8` / `8003A0A8` |36 |

The complete six-function family spans 912 instruction bytes in each PAL
release. Existing wrappers, grouped E88/EA8 definition order and named
profiles remain intact; the wrapper file names still carry the UK addresses.

## Renderer evidence: draw mode and raw intensity

`EuropeanDuelEffectEntry` is the existing two-byte-aligned, `0x16`-byte
record. Its `handler_state` is
at `0x11`, `field_14` at `0x12`, and `render_mode` at `0x13`. The callbacks'
`EU_ENTRY_FADE_VALUE` is instead the raw byte at physical `0x15`.
It must not be substituted with the member named `render_mode`.

The actual PAL `TextBox_DrawEntries` in `text_box_draw_entries.c` starts its
cursor at the entry's physical `0x15`. It reads mode from `p[-2]`
(physical `0x13`), and case 3 writes `p[0]` to all three sprite RGB
components before two `GsSortFastSprite` submissions. The first uses
attribute bits `0x50000000`, the second `0x60000000` and CLUT `(0x2A0,0xFF)`.
This establishes glyph blend-intensity fades, not a guessed alpha field,
universal opacity formula or global screen fade.

## Spiral setup, per-call motion and termination

The two start wrappers preserve both random calls and both direction-byte
writes, even though the second write replaces the first. The final speed
comes from `(rand() & 0x1F) + 32`; removing the first random call changes
the RNG stream. Conceal retains the original `| -0x80` expressions.
Both write selector 4 and invoke the spiral core with the original context.

First-invocation setup retains the occupancy-latch helper, mode 3, random
rotation-direction bit, saved origin halfwords, random angle and radius.
Reveal starts at radius `0x100`, contracts by ten per invocation, and
restores the origin and clears mode/latch on completion. Conceal starts
at zero, expands by ten, and releases the entry at radius `0x140`.
No frame-time multiplier, extra terminal update or clamping is added.

Intermediate updates keep the radius-dependent intensity expression,
the original `0xA0` clamp, angle mask `0xFFF`, direction/speed masks,
`rcos`/`rsin` arithmetic and position-store order. Terminal returns
precede those stores exactly as before. The two-argument signatures,
unused context and all halfword casts/private record views remain.

## Linear ramp and wrapper directions

The one-argument linear updater also selects mode 3 on initialization.
Direction zero starts the raw RGB byte at zero and increases its local
value by five per invocation; any nonzero direction starts at `0x80`
and decreases it by five. This is not scaled by elapsed frames.
The two wrappers retain direction writes 0 and `0x80` before each call.

Fade-in clears mode/latch once the computed value reaches `0x80`;
fade-out releases once it reaches zero. Neither terminal branch stores
the computed endpoint back to the raw intensity byte. In particular,
do not add a terminal 0/128 write or a clamp. Release clears the entry's
occupied flag and requests compaction, not a heap free; PAL does not clear
the separate occupancy-value map.

`DuelEffect_ProcessEntries` retains its two-argument function-pointer
dispatch through the existing unsized interior
`gDuelEffect_EntryHandlers` view, with the original selector mask.
The linear callbacks still have their original one-argument definitions;
no callback-arity unification, cast change, new table or inferred bound is
introduced. This component does not claim a local inspection of complete
PAL table contents or a different dispatch allocation.

## Native conservation and evidence scope

A separate `VERSION_EUROPE`-guarded header,
`european_glyph_fade_callbacks.h`, declares the six names with their
original signatures. It used to map them onto the `func_` labels with
`__asm__` aliases and kept a separate `func_80039B3C.h`; both are gone, and
the symbol files bind the names directly.
The complete common C changes only ten definition/call identifiers and
one guarded include. Existing NA callback declarations, secondary-command
table, renderer, record layout, latch/release helpers and all controls
stay byte-for-byte unchanged.

All eight actual source/profile routes are compared against untouched
baselines: one complete NA unit, one Japanese unit, one existing PAL
glyph-callback unit and five PAL family units. NA/Japan each retain three
functions / 616 bytes; each PAL manifest covers nine functions / 1,544
bytes, including this 912-byte family. The existing glyph units retain
`gcc_2_8_1_g0`; the five family units retain `gcc_2_8_1_g8`.
Complete compiler/MASPSX output, symbols and disassembly/relocations
remain identical, not just selected instructions.

The clean complete NA resident retains retail SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
This is not local full PAL-image execution; ordinary applicable exact-head
regional PR image/regression gates remain mandatory. No new tests,
synthetic/generated-C/source-string harness, local cloud/regional/witness
suite or manual CI rerun is introduced.

## Position names preserve the original local space

The PAL entry's old logical field_0C/field_0E now spell local_x/local_y,
still u16 at physical +0x02/+0x04, not +0x0C/+0x0E. The spiral core
keeps its existing halfword saved-origin views, RNG calls, direction and
radius masks, signed trigonometric arithmetic, terminal restore and
X-before-Y writes. The raw renderer still reads these same local values
and adds the display-object origin. Its record size and signed/unsigned
consumer views are unchanged. Only qualified coordinate identifiers in
the callback change; origin storage, field_04/field_08/field_14 meanings,
raw intensity +0x15, two-argument ABI and table ownership stay intact.
See [text-glyph-renderer.md](text-glyph-renderer.md).
