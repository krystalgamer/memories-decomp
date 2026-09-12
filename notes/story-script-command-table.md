# Story script command table

The resident story-script dispatcher has 23 entries at `D_80090C50`.
`Script_RunTick` selects an entry with the low five bits of `D_8009B27C`.
Handlers that span frames preserve their slot in that word and use bit
`0x8000` as the first-entry latch through `func_8002E3B4`.
`TextBox_BuildStep` can also redispatch the current low byte while script text
is active.

| Slot | Handler | Established behavior |
| ---: | --- | --- |
| 0 | `func_8002E3DC` | Calls the command-busy latch and otherwise has no visible effect. |
| 1 | `func_8002E470` | Loads an image-scene package and waits for its transfer lifecycle. |
| 2 | `Script_OpShowDialog` | Creates a story dialog from a text id and waits for completion. |
| 3 | `Script_OpStoryFlag` | Writes a story flag or conditionally jumps when a flag is set. |
| 4 | `Script_OpStageImage` | Stages an image id and optional viewport, then selects slot 5. |
| 5 | `Script_OpShowImage` | Creates or replaces the staged story image. |
| 6 | `Script_OpViewportTween` | Reads a viewport target and duration, then selects slot 7. |
| 7 | `Script_UpdateViewportTween` | Advances the viewport interpolation until it completes. |
| 8 | `func_8002EB48` | Copies one operand byte into `D_8009B363` and requests mode 5. |
| 9 | `Script_OpShowMenu` | Arms a menu-effect record and waits for its effect step to finish. |
| 10 | `Script_OpSound` | Starts a BGM or sound effect, or updates the retained BGM command. |
| 11 | `Script_OpFadeBgm` | Starts a default or operand-sized BGM fade. |
| 12 | `Script_OpJump` | Jumps to a little-endian offset in the loaded script bank. |
| 13 | `Script_OpSavePrompt` | Runs the save prompt and memory-card dialog state machine. |
| 14 | `func_8002F430` | Empty handler. |
| 15 | `func_8002F438` | Empty handler. |
| 16 | `Script_OpFadeOut` | Starts and optionally waits for a screen fade-out. |
| 17 | `Script_OpDuelResult` | Loads, presents, and releases the scripted duel-result screen. |
| 18 | `Script_OpGameOver` | Transfers control to frontend mode 12. |
| 19 | `Script_OpCredits` | Transfers control to frontend mode 15. |
| 20 | `Script_OpWait` | Counts down a two-byte delay once per script tick. |
| 21 | `Script_OpJumpIfDeckIncomplete` | Jumps when any of the 40 player deck slots is empty. |
| 22 | `Script_OpReturnToMenu` | Requests frontend mode 8 and menu id 5. |

Slots 0, 8, 14, and 15 remain address-named deliberately. Slot 0 exposes
only the generic first-entry latch, slot 8's mode-5 consumer is not yet named,
and slots 14 and 15 are indistinguishable empty placeholders. Naming any of
them by table position alone would assert semantics not established by a
consumer, script operand, or observed behavior.
