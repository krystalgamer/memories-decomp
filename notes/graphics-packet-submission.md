# Graphics packet submission

The three contiguous functions in `src/game/gpu_packets.c` submit copied
primitives to an ordering table. The middle function,
`Graphics_SubmitTextureWindowPacket`, already has an accepted identity. Naming
its two siblings completes this small component without changing their shared
source, ABI, native compiler profile, or packet layout.

| North American address | Identity | Submitted command sequence |
| --- | --- | --- |
| `0x8005B260` | `Graphics_SubmitDrawModePacket` | Draw mode, copied primitive |
| `0x8005B36C` | `Graphics_SubmitTextureWindowPacket` | Texture window, copied primitive, window reset |
| `0x8005B4D8` | `Graphics_SubmitMaskWritePacket` | Draw mode, mask-write enable, copied primitive, mask-write reset |

## Evidence and limits

Both newly named helpers read the primitive's tag length, copy its tag and
payload to `gGraphics_pPacketWorkCursor`, and insert a draw-mode word
`0xE1000200 | ((flags & 3) << 5)`. A nonnegative `flags` value additionally
sets bit 1 of the copied primitive's code byte. The signed test is independent
of the low-two-bit blend-mode selection: negative values are not rejected or
normalized. Both link to `ot->org[idx & 0xFFFF]` through `addPrim`.

The first helper increases the tag length by one and advances the cursor by
the original length plus two words. The mask-write helper surrounds the copied
primitive with `0xE6000001` and `0xE6000000`, increases its tag length by three,
and advances by the original length plus four words. That pair enables mask-bit
writes and resets the setting afterward; it does not save and restore prior
GPU state. Preserve tag-length narrowing, unchecked lengths and indices,
pointer arithmetic, local ordering, and the exact command constants.

The draw-mode helper has resident model, effect, library, projection, and
display-object callers, as well as matched overlay callers. These corroborate
packet submission rather than a specific geometry or scene role. The
mask-write helper's body establishes its narrower GPU-command contract, but
there is no matched C caller establishing a higher-level use. Neither is an
SDK reidentification, allocator, bounds checker, or GPU-state stack.

## Native and frozen-call contracts

All seven resident manifests select the same three-function source and
`gcc_2_8_1_g8`. The ordered extents remain `0x10C`, `0x16C`, and `0x124`:

| Image | Draw-mode entry | Mask-write entry |
| --- | --- | --- |
| North America | `0x8005B260` | `0x8005B4D8` |
| Japan | `0x8004CFF8` | `0x8004D270` |
| United Kingdom | `0x8004D3B4` | `0x8004D62C` |
| France, Germany, Italy, Spain | `0x8004D5B8` | `0x8004D830` |

The native inventories and complete resident/overlay binding maps expose the
canonical identities. The manifests, split templates, source paths, compiler
flags, historical attempts, and reuse fingerprints stay intact. The accepted
texture-window sibling remains unchanged.

## Frozen declaration and import boundary

All 70 matched overlay C callers retain their original bytes. Their historical
dependency fingerprints also include the exact bytes of `gpu_packets.h`, so
that header must remain byte-identical, including its legacy declarations.
Canonical resident declarations instead live in
`src/game/graphics_packet_submission.h`. The eight affected resident C units
include that header; there is no macro bridge in the frozen dependency graph.
The two French ribbon headers are also unchanged: their historical implicit
calls must not be silently reclassified as prototype-bearing calls.

The 617 registered physical consumers of those frozen C bodies use 96
independent linker import maps. Each map exposes the canonical resident
address and an explicit `func_8005B260 = Graphics_SubmitDrawModePacket` legacy
import alias. Compatibility thus stays outside the frozen source/header
inputs. This is an overlay-import boundary only: the resident object defines
the canonical functions, not duplicate legacy functions. No guessed
declaration, compiler flag, source rewrite, or fingerprint refresh is needed.
The extra alias is a real additional import-map assignment, not another
physical resident target. Existing complete-binding controls retain every
original numeric-import cardinality. Their shared metadata validator parses
every assignment, rejects malformed or duplicate names, and checks the one
permitted legacy alias separately. The alias must target the canonical
numeric import at `0x8004D5B8`; unknown, duplicate, missing, or dangling aliases
fail. An explicit family/region inventory, checked against the registered
frozen consumers, distinguishes those families from assembly-only users.
The independent Spanish 338/402 and 87 controls use the same validation,
retaining their original numeric counts of 35 and 21. No historical count is
increased, symbolic assignments are not broadly ignored, and all existing
address, raw-extent and binary call-target checks remain.

The original #7713 CI failure exposed two independently selected NA maps that
had relied on the autogenerated numeric resident name:
`password_linker_symbols.txt` and `main_menu_linker_symbols.txt`. Both now bind
`Graphics_SubmitDrawModePacket = 0x8005B260` explicitly, along with the legacy
import alias. Other consumers retain their complete existing canonical maps.
The French ribbon raw-source regression assertion still requires the original
numeric call spelling, while linker-binding assertions check canonical names.

Actual original/final translation-unit comparisons cover the complete shared
resident group and frozen duel-effect, password, Spanish intro, both French
ribbon, and two NA main-menu units, using unchanged manifest-selected profiles.
The frozen overlay compiler and MASPSX outputs compare with zero substitutions;
only the resident group needs the two declared identifier substitutions.
Complete historical dependency inputs and ledger prefixes are preserved, not
rehashed. This supporting evidence does not replace fresh per-image PR checks.
