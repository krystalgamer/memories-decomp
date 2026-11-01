# Secondary voice lifecycle and reuse ranking

This component names the coupled allocation/silent-envelope stage and reuse
counter of `SDSecondaryObject`, and the helper that updates the counter. It
does not introduce a lifecycle implementation, alter the sound driver, or
unify its two different cleanup routines.

| Original name | Canonical name | Existing view |
| --- | --- | --- |
| `tent_release_stage` | `lifecycle_stage` | `u8` at object `+0x0D` |
| `tent_age_countdown` | `reuse_countdown` | `u16` at object `+0x1E` |
| `tent_SD_AgeSecondaryObjects` | `SD_UpdateSecondaryObjectReuseCountdowns` | `void(void)` |

The fields retain their positions in the existing `0x28`-byte object.
Their names do not introduce enum types, Boolean conversions, new storage,
or fixed global addresses for runtime-allocated object members.

## Reuse ranking is not voice expiry

`SD_KeyOnSecondaryNote` seeds `reuse_countdown` with `0xFFFF`.
The update helper walks the original `object_count` records. It decrements
a nonzero counter only when `channel_index < SD_SEQUENCE_CHANNEL_COUNT`;
otherwise it writes zero to that counter. It does not clear the record,
detach its channel, change `key_held`, or request key-off. A counter reaching
zero therefore does not expire a sound.

The consumers establish its ranking role:

- `SD_FindLowestPrioritySecondaryObject` filters by channel and nonzero
  `lifecycle_stage`, then accepts a counter less than **or equal to** the
  current best. The later equal candidate wins.
- `SD_SelectSecondaryObject` preserves all earlier free/inactive/matching-note
  passes. Its final fallback accepts only a **strictly lower** counter, keeping
  the first minimum; if no counter is below `0xFFFF`, it returns `-1`.

`SD_ResetSecondaryPlayback` and the main sweep's `SPU_OFF` cleanup clear the counter.
Neither that clearing nor the ranking readers establishes a playback
duration or time unit.

## Stage, held key, and channel attachment are separate

Note-on sets `lifecycle_stage` to 1. Nonzero stage gates allocation-sensitive
selection, channel-level spatialization/volume updates, and mute/restore
work. The main `SD_UpdateSecondaryObjectVoices` sweep also uses it to track
silent-envelope observations:

- On `SPU_OFF`, a nonzero stage permits the existing channel-usage decrement,
  stage/counter clearing, and channel detachment.
- On `SPU_ON_ENV_OFF`, a nonzero stage below 2 advances. At stage 2 or higher,
  the sweep requests key-off through `SD_KeyOffSecondaryObject`.

These are not necessarily consecutive silent observations: other SPU
statuses do not reset the stage. The stage is not the SDK's key-status enum.
`key_held` remains independent, including the sweep's separate forced-off
path for a cleared held-key flag.

The unused `tent_SD_KeyOffSilentSecondaryObjects` helper deliberately keeps
its tentative function name. The existing research found no caller in the
resident executable, SU.MRG, or WA_MRG.MRG. Unlike the main sweep, it has no
channel-range guard, clears the stage immediately after its own key-off
loop, and leaves `key_held`, channel ownership, and usage untouched. Its
field references adopt the same member names, but its behavior is not
merged with the main cleanup path. In particular, stage zero does not
universally imply that a channel is already detached.

## Cadence and compatibility

`SD_UpdateSecondarySequence` calls the ranking helper immediately before
the voice sweep. `SD_SequenceTimerCallback` calls the same pair when its
existing `field_0508` gate reaches 11 inside the eight-iteration track loop.
Those paths do not justify calling each decrement a video frame or a unit
of wall-clock time.

| Resident image | Helper address |
| --- | --- |
| North America | `0x8004C84C` |
| Japan | `0x8004BF38` |
| United Kingdom | `0x8004CAD8` |
| France, Germany, Italy, Spain | `0x8004CCDC` |

Each inventory and primary symbol names the same matching C entry.
The canonical declaration uses the existing assembler-name view pattern
(`sound_sequence_timing.h` provides another example) to retain the native
`tent_SD_AgeSecondaryObjects` entry. The `__asm__` spelling also parses in
strict-C99 consumers of the shared header. That preserves its original function
class, address, and `0x7C` ELF size without adding code or size directives.
Each image exports the canonical name as a linker alias to that provider;
the alias naturally has the same class/address without an ELF size.
The legacy C declaration also remains explicitly `extern`, so the existing
source-contract checker recognizes the retained native provider as a tracked
alias target through its header-declaration inventory. The source
path, compiler profile, source-group boundaries, declaration selectors,
pointer annotations, field offsets, raw offset expressions, comparison
order, and SPU calls remain unchanged. Overlay consumers of the shared
header receive the canonical declaration without a new import unless they
actually call it.

The existing semantic-map record at `function/0x8004C84C` is promoted in
place, rather than duplicating its kind/address. Other historical evidence
rows retain the old member/helper spellings as historical descriptions.
There is no new SDK attribution, shared cleanup policy, callback, timer,
storage provider, or reference-derived declaration.

## Evidence and confidence

The member roles are high-confidence from their local producers, ranking
readers, SPU-status sweep, control-change updates, and mute/restore consumers.
The helper's name describes its complete counter-only body and both actual
callers, replacing the older tentative per-frame description.

Primary evidence is in `sound.h`, `sound_sequence_tracks.c`,
`sound_sequence_channel_events.c`, `sound_secondary_object_selection.c`,
`sd_update_secondary_object_voices.c`, `sound_voice_setup.c`,
`sound_secondary_commands.c`, `sound_sequence_runtime.c`,
`sound_secondary_voice_mute.c`, and `sound_secondary_silent_key_off.c`.

## Classified helper source filename

The unchanged source at func_80049920.c now lives at
sound_secondary_silent_key_off.c. Its namespace, SPU status reads and
existing body establish the secondary-voice/silent-envelope grouping;
this filename is not a promotion of tent_SD_KeyOffSilentSecondaryObjects
or new evidence that anything calls it. The tentative symbol, no-caller
caveat and differences from SD_UpdateSecondaryObjectVoices remain intact.

All seven resident matching manifests select the same original function,
address, size and named compiler profile through the new path. The six
static regional split C entries use that same source identity, preserving
their offsets, segment types and adjacent boundaries; the North American
split derives it from its manifest. The C bytes, includes, source-group
boundary, types and every statement are unchanged.
Compiler/MASPSX assembly changes only the source filename directive. Raw ELF
objects differ in the corresponding local STT_FILE string and its required
string-table/layout offsets; instructions, data, relocation entries, native
symbol identities and disassembly are unchanged.
Historical external-attempt records and optional generated global-usage
output keep their original paths rather than being rewritten as new evidence.

The empty func_800136D4 and func_800283EC units retain their address-based
filenames: no supported subsystem or purpose has been established for them.
No stub name is invented merely to remove the func_ prefix.
