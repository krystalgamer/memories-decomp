# Duel session transition controls

These five existing scalar/view identities connect new-duel entry,
animated-battle re-entry and post-duel routing. They are not a new packed
session structure: PAL order and gaps differ from NA.

| Former identity | Canonical name | Preserved view |
| --- | --- | --- |
| `D_8009B368` | `gDuel_bPostDuelMainMode` | `u8`, plain / `.data` |
| `D_8009B369` | `gDuel_bResumeFromAnimatedBattle` | `u8`, plain / `.data` |
| `D_8009B370` | `gDuel_wCampaignSceneOnWin` | `u16` or incomplete `u8[]` |
| `D_8009B372` | `gDuel_wCampaignSceneOnLoss` | independent `u16` |
| `D_8009B374` | `gDuel_wAnimatedBattleBgmCommand` | `u16`, plain / `.data` |

## New entry versus model re-entry

`Main_StartDuel` and `Text_StartCampaignDuel` clear the re-entry byte.
Battle and Exodia-result handoffs set it to 1 before entering
`MAIN_MODE_ANIMATED_BATTLE` (1), with `MAIN_MODE_DUEL` (3) as the return
mode. `Main_RunAnimatedBattle` handles either the ordinary model pair or
the special Exodia presentation, then returns through that next-mode byte.

`Main_RunDuel` uses a zero/nonzero test to decide whether an ordinary
opponent needs pre-duel Build Deck setup. `Duel_InitScene` separately uses
`!= 1` for two fresh-duel blocks: card/selection/side reset and
deck/save-window initialization. It initially selects phase 11
(`DuelScene_UpdateResume`); fresh setup instead selects phase 1
(`DuelScene_UpdateStartup`). Both callback indices are confirmed by the
existing scene-handler table.

The byte is retained until later new-duel preparation. It is not a
playback-enable flag or a newly cleared one-shot event. Re-entry still
rebuilds UI and resets other fields; no claim that all duel state remains
untouched is introduced. The distinct zero/nonzero and `!= 1` predicates
remain, including their existing behavior for other byte values.

## Exit mode and campaign return operands

At duel completion, `Main_RunDuel` performs its existing fade/sound/frame/
transfer cleanup and copies the post-duel mode into the active main mode.
Existing producers choose Debug (0), Campaign (2), Free Duel (6) or Menu
(8). Free Duel's overlay still writes the numeric legacy identity; it
binds to the same byte and is not retargeted.

Only when returning to Campaign does the runner select a scene from the
win/loss operands. It reads the **low byte** at the first halfword's base
plus `gDuel_bPlayerLost * 2`: the zero/non-loss path uses the first word,
the loss path the second. Names do not introduce a new draw policy.

`Main_StartDuel` stores its full `arg2/arg3` values as halfwords.
`Text_StartCampaignDuel` reads one byte for each but still performs
halfword stores. Debug setup supplies `0x8000` while returning to Debug,
where those operands are ignored. The sixteen-bit storage and high bits
are preserved, not narrowed or normalized into validated scene IDs.

The first operand keeps the old incomplete byte-array view when
`D_8009B370_AS_BYTE_ARRAY` is selected, otherwise its scalar `u16` view.
Other old `_AS_ARRAY`/`_AS_ABSOLUTE_ARRAY` tokens are not substituted for
that actual declaration guard. Both halfwords retain independent
relocation providers; no two-word array or new structure replaces them.

## Ordinary animated-battle BGM selection

`Main_StartDuel` sets the command to `0x7280`.
`Text_StartCampaignDuel` starts with `0x72A0`, then chooses `0x7280` for
opponents 9 through 16, `0x7290` for 17 and `0x72B0` for 38, under the
original unsigned-range and signed-byte tests and store order.
Battle passes the unchanged whole `u16` to `SD_BGMPreload` and
`Model_SetPresentationBgmCommand` before model handoff.

This is separate from ordinary duel music `gDuel_wBgmId`. Exodia uses its
own `0x7310` literal, so the named word does not claim to control every
model presentation or Exodia music. No command mask or class rewrite is
added.

## Native providers and preservation

| Image | Post mode | Re-entry | Win scene | Loss scene | Animated BGM |
| --- | --- | --- | --- | --- | --- |
| NA | `8009B368` | `8009B369` | `8009B370` | `8009B372` | `8009B374` |
| Japan | `8009B258` | `8009B259` | `8009B260` | `8009B262` | `8009B264` |
| SLES_039.47 | `8009C2DA` | `8009C2DC` | `8009C2CC` | `8009C2CE` | `8009C2D6` |
| Other PAL | `8009C6FA` | `8009C6FC` | `8009C6EC` | `8009C6EE` | `8009C6F6` |

Canonical aliases preserve the old logical providers, including Japan's
actual `gJapanese_*` identities. Japanese `Main_StartDuel` has its own
implementation; its actual identifiers and CPP right-hand sides migrate
mechanically rather than assuming a common-source wrapper.

The isolated header mirrors the original section and byte-array guards.
Old headers, overlays/imports, profiles, grouped functions, widths,
store/read order, predicates and input behavior remain. No allocation
extent, packed layout, pointer reinterpretation or new guard is inferred.

The existing campaign-command witness receives only literal namespace
substitution, with full inverse-file reconstruction. Its 16,384 cases,
optimization levels, assertions, mutations, generators, call-boundary
observations and compiler setup stay intact. No new harness, C fixture,
probe or case is added, and cloud/regional/overlay/witness execution
remains PR-side validation.
