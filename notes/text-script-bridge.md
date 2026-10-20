# Text-originated image and viewport-tween script dispatch

Two text-state callbacks stage work for the existing scene-script engine.
They share the engine's command word/table and viewport state; they do not
create a per-channel command queue or call the scene handlers directly.

| Text slot | API | Published scene command / text state |
|---|---|---|
| 5 | `Text_StageScriptImage` | Command 5 (`Script_OpShowImage`), text wait state 10 |
| 11 | `Text_StartViewportTween` | Command 7 (`Script_UpdateViewportTween`), text state 0 or 10 |
| 10 | Existing `TextBox_WaitForScriptCompletion` | Clears text state when the shared dispatch gate is zero |

Both producers execute their operand reads/stores on every invocation;
neither introduces an initialization-latch test.

## Image text operand format

The callback clears shared target Y, then target X, consumes one
little-endian image word and stores that word unchanged through the
existing unsized image-ID view. With image bit `0x8000` set it then:

1. consumes one unsigned byte and publishes the stream advance;
2. stores that byte to the same-address `Base2_8009B2AA` target-Y view;
3. consumes one little-endian halfword and stores target X through
   `Base2_8009B2A8`.

This is **Y8 then X16**. It is not the scene-script
`Script_OpStageImage` encoding, which takes two coordinate halfwords.
Do not widen, reorder or normalize either format. The image word's high
bits remain for the existing image handler's interpretation.

The callback then stores gate 5, command word 5 through its unsigned array
view, and text state 10 in that order. Actual image setup, transfer/fade
polling and command completion remain in `Script_OpShowImage`.

## Tween operands and optional text waiting

The tween callback first stores text state zero. It consumes three
halfwords into the existing signed absolute target-X, target-Y and
third-operand views. It then preserves two distinct reads of Y: a signed
word-sized local and a separately cast unsigned-halfword local.

Only when the signed Y value is at least `0x1000` does it subtract
`0x1000` from the unsigned raw value, store the narrowed result back to Y
and select text wait state 10. Otherwise text remains in state zero.
This is a signed threshold, not a bit test or a low-twelve-bit mask:
raw `0x9000` reads negative and does not take the wait path, whereas
`0x2000` takes it and becomes `0x1000`.

Finally it stores gate 7 and command 7 through the existing signed
absolute command-word alias. The third operand remains unvalidated,
including zero and its signed interpretation in the updater's divisions.
No count clamp, eager tween call, new flag or duration normalization is
introduced.

## Shared gate and command table

`gText_ScriptDispatchGate` remains one unsigned byte shared across text
channels. Its producers store 5/7, but the builder only tests nonzero;
the gate itself is not the dispatch index or a countdown. New-box setup
still clears it.

Before the text-state callback, `TextBox_BuildStep` dispatches
`gScript_apfnCommandHandlers[*(u8 *)&gScript_wCommandWord]()` through the
existing target low-byte view. Only after that call does it clear the
gate if the **whole** command word is zero. It does not move completion
testing before dispatch or reduce it to a low-byte comparison.

The ordinary `Script_RunTick` uses the same table but retains its
`gScript_wCommandWord & 0x1F` index. Keep both distinct expressions,
the existing void-handler ABI, unsized declaration and all 23 entries
of the initialized NA table. A masked/raw index domain does not justify
a new declaration bound, extra table, dispatcher helper or validation.

The text state-10 callback observes the gate, not a new per-object
completion token. All current shared-state lifetime and multi-channel
interactions remain.

## ABI and same-address views

Retain `SCRIPT_STATE_TEXT_CALLBACK_VIEWS`, the unsigned unsized arrays,
signed `.data` scalar aliases, the command-word ASM binding and the
Japanese scalar selector. Keep the `Base2` identifiers distinct even where
they have the same addresses as the ordinary targets: those spellings
preserve address materialization and liveness across calls.

No target-coordinate/operand declaration is globally renamed, merged into
a structure or strengthened to a single signedness. The callback locals,
read/store order, source grouping, compiler profiles and every enclosing
table position stay unchanged.

## Native identities and unrelated SDK alias

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Image producer | `0x8003767C` | `0x80036E34` | `0x80037414` | `0x80037614` |
| Tween producer | `0x8003771C` | `0x80036ED4` | `0x800374B4` | `0x800376B4` |
| Dispatch gate | `0x8009B357` | `0x8009B247` | `0x8009C2B2` | `0x8009C6D2` |
| Handler table | `0x80090C50` | `0x80090B00` | `0x80091C54` | `0x80092068` |

Remove only the six obsolete producer/gate wrapper macros after wiring
these native bindings. The unrelated French `D_80090C50_French` symbol and
SDK inventory row at numeric `0x80090C50` remain untouched; the native
French command table is at `0x80092068`.

All complete compiler/MASPSX outputs are compared after reversing only four
identifiers, without filename, instruction, directive or relocation
filtering. Full clean retail executable matching remains mandatory.
All other inventory fields/rows, native symbols/imports, initialized
tables, manifests, layouts and profiles remain. The complete accepted
ledger gains four evidence/caveat rows. No tests, controls or captures
change; regional/cloud validation remains PR-side.
