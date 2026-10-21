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

Only the function identities in the native inventories and complete existing
resident/overlay binding maps change. The manifests, split templates, source
paths, compiler flags, historical attempts, and reuse fingerprints stay intact.
The accepted texture-window sibling remains unchanged.

All 70 matched overlay C callers retain their original bytes. One legacy
spelling bridge lives beside the canonical declaration in `gpu_packets.h`.
Two French ribbon headers (`variant373_ribbons.h` and
`variant383_ribbons.h`) previously reached neither that declaration nor a
private prototype; their calls were implicit. They now include the owning
header, so the same bridge covers them without changing the frozen C bodies
or adding guessed declarations. Every affected frozen caller's quoted-include
chain consequently reaches the owner.

Actual original/final translation-unit comparisons cover the complete shared
resident group and frozen duel-effect, password, Spanish intro, and both
French ribbon units, using each manifest's unchanged named profile. Complete
compiler and MASPSX outputs are compared with only the two declared identifier
substitutions. This supporting evidence does not replace per-image PR checks.
