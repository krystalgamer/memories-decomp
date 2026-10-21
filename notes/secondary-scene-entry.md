# Secondary text commands and Campaign Map entry

The existing seven-function `duel_effect_sound_commands.c` object ends with
two empty secondary handlers and a Campaign Map request. These identities
complete that object without changing its four accepted sound handlers.

## Secondary command identities

| North American address | Identity | Secondary opcode |
| --- | --- | --- |
| `0x80038888` | `Text_NoOpSecondary12` | `0x12` |
| `0x80038890` | `Text_NoOpSecondary13` | `0x13` |
| `0x80038898` | `Text_OpenCampaignMap` | `0x14` |

The first two bodies are empty, each occupies eight bytes and each retains
its own address. Both still have `void (void)` prototypes and the dispatch
table still explicitly casts them to its channel-pointer callback type.
The names describe the observed no-op behavior and hexadecimal table slot,
not an inferred original purpose or a claim that the opcodes are unused.
Neither handler consumes an operand or changes command disposition.

`Text_OpenCampaignMap` consumes one unsigned byte from the channel's selected
stream, advancing that slot once. It writes frontend mode `5` and then
publishes the byte to `gCampaignMap_bEntryLocation[0]`. The separate
`Script_OpOpenCampaignMap` command makes the same request using the story
script cursor. Both original expressions, unsigned operand types and store
orders remain intact.

## Entry location and loaded overlay

`Main_RunCampaignMap` loads the Egypt overworld package and passes the entry
byte to the overlay function already named `CampaignMap_SetLocation`.
That function stores the argument in `gCampaignMap_Location`, initializes
the scene, and uses the location for the camera, label and exit objects
(`src/overlays/overworld/set_location.c:298-363`).

All fourteen registered before/after-coup variants across the seven images
have the matching `0x250`-byte initializer at `0x8016866C`. The resident call
now uses its accepted canonical name; each resident image receives an
explicit binding at that proven entry address. Existing numeric bindings
are preserved, and no duplicate semantic function row is added for the
already named overlay function.

The request byte is an entry location, not a package selector or an alias
of the overlay's live current-location state. `DebugMenu_UpdateMapEntry`
explicitly requests location zero; its separate remembered debug map ID is
not forwarded as this operand.

`gCampaignMap_bEntryLocation` keeps the original incomplete `u8[]`
declaration and `[0]` accesses. The incomplete array is an absolute-addressing
input under G8, not evidence of a longer buffer. Each image's alias refers
to its existing `D_8009B363` provider. The French symbol
`D_8009B363_French`, which names the literal address `0x8009B363`, is a
different native identity and is not substituted.

## Preservation boundary

Canonical declarations are isolated in `secondary_scene_commands.h`.
All pre-existing headers, every overlay C body, existing test files and
overlay metadata remain unchanged. There is no overlay implementation
rewrite, new allocation, validation, guard, timeout or frame advance.

All seven native command groups keep the same complete source/profile
mapping, source definition order and sizes
`0x28/0xE0/0x68/0x88/0x8/0x8/0x40`, using `gcc_2_8_1_g8`.
The Japanese mode-state alias and all sound operands/state remain intact.
Full compiler and MASPSX output comparisons permit only the declared
identity substitutions; the final clean resident image must still match
retail exactly. Regional/overlay full-image acceptance remains on the PR,
not replaced by local source probes.
