# Model-load dispatcher at 0x80056828

`src/game/model_load_step.c` reproduces the complete eleven-stage dispatcher:
341 instructions (1,364 bytes) and the eleven-entry, 44-byte table at
0x8001170C, under the existing `gcc_2_8_1_g8_split` profile. The four bytes
between that table and the existing model-texture-transfer table remain raw.
No register bindings, statement assembly, or new production profiles are used.

The function keeps its void interface. All observed callers discard the
return register; scratch experiments with a return-value view were not
integrated.

## State and callback boundaries

The captured slot is `D_800F2C40[index]`, with valid indices 0 through 2.
Every invocation calls `VSync(1)` before reading the state byte at +0xE14.
States 0 and 255 return immediately, without a second timing call, a
diagnostic, or a state write.

| State | Operation |
| --- | --- |
| 1 | Choose the individually labeled payload base; its first word overrides the default 0xC000 size when nonzero; call slot setup with final argument -1 |
| 2 | Build the slot's row tables |
| 3 | Call `func_8004D914(index)` |
| 4 | Sum row-one part widths for slots 0/1, publish both cursor limits for **all three slots**, and apply the slot-one 0x800 offset |
| 5 | Rearm each part's sequence fields, preserving unrelated fields |
| 6 | Publish +0xE1F = 1 around `func_800590DC`, then clear it |
| 7 | Apply mode-dependent speed and finish slots at index 2 or higher |
| 8 | Forward tint and level fields to `func_800582C0` |
| 9 | Activate the selected control module when its command is nonnegative, then seek the slot's parts |
| 10 | Submit the slot's 0x800-byte-stride sound-data address unless +0xE1D is nonzero |
| 11 | Set +0xE1E when enabled and always set the finish latch |

Other non-idle byte values take only the common timing, diagnostic, and
advance path. That path calls `VSync(1)` before reading the live state for
`printf`. After the diagnostic callback it reloads the finish latch and,
when needed, the state. A nonzero latch selects 255; otherwise the new
state is the live byte plus one, narrowed on publication.

## Layout and activation details

State 4 reads row one of `ModelSlot.field_2C8` and skips 0xFFFF entries.
Every bit test is relative to the **slot base** at +0xBEC, not to an
advancing key cursor. A set bit contributes 0x14 bytes, otherwise 0xC.
Slot 2 skips the sum but still gets +0xDF0/+0xDF4 populated before
`func_8005A4C4`.

State 5 retains the repeated part-pointer loads between writes. It sets
`ii` and `aframe` to 0xFFFF, restores `ti` and `sid` from `start` and
`start_sid`, clears `rframe`, and sets speed to 0x10. Its count is reloaded
on the back edge.

State 9 captures the initial signed command at +0xD10 and module context at
+0xDE8 before publishing `D_8009AFA0`. A negative command skips the module
call but not the subsequent sequence setup. The module entry is four bytes
past the captured primary-module base. Its ABI is the existing
`ModelControlHandler(context, command)` contract: no extra parameter is
invented from a live scratch argument register.

The command is reloaded after `func_8005F198(1)` and reduced with signed
`% 1000`. Context and module identity remain captured. After the closing
gate, the sequence selection and full-word, zero-extended +0xE06 position
are captured. When the captured selection is zero, each part uses the live
selection byte from the **slot base**. Nonzero captured selections override
the part's `sid` and are restored after the loop. The part count remains
live across seek callbacks.

## Matching discriminators

A signed-halfword inline phase reader preserves the dispatch copy. This
is a promoted selector view, not a claim that +0xE14 occupies two bytes.
The neighboring layout routine's shared layout/speed work value recovers
the speed branch allocation; an explicit missing-key sentinel preserves
the loop-preheader ordering. A local next-state result with the finish
condition first gives the original common store without register pins.

The six historical canonical attempts remain intact. The new terminal
post-resolution record refers to the complete verified source. The old
asm-assisted structural candidate and its target snapshot are retired;
they are not presented as equivalent reference C.

## Behavioral evidence

`test_model_load_step.py` compiles the actual dispatcher as a freestanding
ILP32 program at native O0 and O2. Its oracle uses numeric record offsets
and independently structured state handling. Native executable module
headers provide real callable entries at base +4.

Each optimization covers 1,443 cases, 10,071 ordered callbacks, 222 module
calls, 4,887 seeks, 99 layout calls, and 117 callback-mutation cases.
The corpus includes every state byte for each valid slot, all 58 part
positions, sentinel/bitfield patterns, aliased part pointers, speed modes,
signed module commands, full halfword positions, disabled-state behavior,
and timing/printf/module/seek mutations. Complete slot and part snapshots,
global argument identities, and guards are compared.

Four actual-source negative controls reject with semantic exit 81:
incorrect idle handling, an advancing bitfield base, omitted slot-2 cursor
publication, and an advancing base for the live sequence byte.

The SDK/model helpers are bounded stubs, not a rendering or sound-hardware
accuracy test. Callback mutations deliberately stress capture/reload order
beyond the ordinary effects of some helpers; they do not assert that those
helpers currently perform every synthetic mutation.
