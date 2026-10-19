# Model-slot animation table lifecycle

These names describe resident game helpers, not recovered original symbols
or Psy-Q entry points. The supporting code is the matched
`src/game/model_slot_row_tables.c` group and its independently typed
`ModelSlot`, `ModelSlotRow` and `ModelSlotPart` records.

| Address | Name | Confidence |
|---|---|---|
| `0x8004D58C` | `Model_ResetSlotAnimationTables` | High |
| `0x8004D75C` | `Model_BuildSlotAnimationTables` | High |
| `0x8004D914` | `Model_LinkSlotAnimationRows` | High |

## Reset and import

`Model_InitSlot` maps the incoming model through `GsMapUnit`, then calls
`Model_ResetSlotAnimationTables`. The reset clears all ten accumulator rows,
their 58 per-part duration halfwords and row maxima, and initializes all
`field_2C8[row][part]` entry keys to `0xFFFF`. It clears the eight-byte part
bitfield, the four command/transfer pointers at `0xDD8..0xDE4`, and related
animation counters.

It also traverses the mapped owner chain at input `+0x10`, selects enabled
owner blocks of kinds 2 and 3, and imports their payload pointers. Kind 3
supplies the per-part flags, command list and source transfer pointer;
kind 2 supplies the other two transfer pointers. The name intentionally
does not promise to reset the entire model slot or just clear memory: it
resets and imports this animation lookup state.

## Build the indexed rows and lengths

`Model_BuildSlotAnimationTables` starts at each part's `start` key in
`field_DD8`, initially on row 1, and sets the part's `start_sid` to that row.
`ModelSlotPart` mirrors `GsSEQ`; the `GsLinkAnim` consumer and layout
assertions are documented in `notes/model-structures.md`.

Negative command words select a row through bits `16..22`; row zero ends
the scan. First visits install that row's entry key and follow its command
offset, whereas an already indexed row advances to the next word.
Nonnegative words contribute byte `+2` to that part's row duration.
The final pass stores each row's largest per-part duration in
`field_750[row].max`.

The animation interpretation is established by consumers, not just the
record stride: `Model_ControlSlotAnimation` selects the corresponding
`field_2C8[animation][part]` entry keys and refuses a row with zero maximum;
`Model_GetSlotAnimationLength` returns the selected row's maximum.

## Link after building

`Model_LinkSlotAnimationRows` uses the populated entry-key table to walk
the command rows, copies row bits into their linked form, and rewrites
halfword command links, including paired termination links. It returns
without a command list or without parts. This is an in-place game command
relink pass, not `GsLinkAnim`'s binding of part sequence records.

The order is explicit in both callers:

- `func_80056828` in `src/game/model_slot_control.c`: phase 1 loads/maps and
  resets, phase 2 builds, phase 3 links, then later phases prepare and rearm
  the parts.
- `Model_LoadSlot` in `src/game/model_slot_control.c`: build, link, derive
  transfer limits, then rearm each part from `start`/`start_sid`.

The former relinker header's statement that it ran *between* reset and
build was incorrect; both callers place it after build.

## Limits and matching

The functions' mechanical roles are high confidence. Individual command
opcode names, the full owner-block schema and the unrelated decoder at
`0x8004D134` remain address-based or offset-based where their semantics are
not established. No opcode, type, member layout or algorithm is changed.

The three functions remain in their original contiguous grouped source,
under the existing `gcc_2_8_1_g0` profile. The semantic registry records
their original North American addresses. Existing European macro adapters
are updated only on their shared-name side, retaining every regional
address-based target; no overlay or regional implementation is added.
