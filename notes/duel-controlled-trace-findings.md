# Controlled duel trace findings

Two completed human-operated traces resolve independent questions at the
campaign-to-duel boundary and in duel rank accounting.

## Forest Mage home terrain

Entering the mandatory Forest Shrine gate duel changed main mode from campaign
to duel with opponent ID 23 and `gDuel_bTerrain == 1`. The field was visibly
Forest, no cheat or card effect changed it, and the value remained stable for
sixty frames. Forest Mage's home field is therefore confirmed rather than a
guide-derived reading.

`campaign_duel_home_terrain.lua` remains active for the unresolved Sea,
Mountain, Desert, Meadow, and finale assignments. It now skips opponent 23 so
a later run cannot spend its single capture on the resolved Forest pair.

## Face-down and fusion rank counters

The face-down/fusion trace captured two player turns against Duel Master K:

| Turn | Action | `face_down_plays` | `fusions_initiated` |
|---|---|---:|---:|
| 1 | Place one ordinary card face-down | +1 | 0 |
| 2 | Fuse two cards and place the result | 0 | +1 |

The interface does not allow a fusion result to be placed face-down. The
second turn's Cyber Soldier result was forced face-up, so the unchanged
face-down counter is expected and the fusion counter independently records the
successful hand fusion. This resolves the static-code uncertainty about a
possible alternate face-down accounting path for multi-card selections.

The completed `duel_face_down_fusion_counter` script, harness, and result are
retired; the measured contract is durable here and in
`notes/research/the-game.md`.
