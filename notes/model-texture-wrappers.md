# Model texture wrapper interfaces

`ModelTexture_UploadTimFile` is the former `func_800599FC`.
It supplies the original eight-word local scratch to `ModelTexture_LoadTim`,
forwards the file path and six integers, and discards both the descriptor
and packed return value. The callee reads the host file, decodes TIM
information and performs the existing pixel/CLUT uploads.

The wrapper's `void` return, `char *` path, six `int` arguments, scratch cast
and `s32[8]` allocation remain exact. A retail `GsIMAGE` is 28 bytes;
the 32-byte scratch has one extra word, not the four stated by the old
comment. No smaller typed local, new status return, pointer qualifier or
different failure/upload behavior is introduced. Its original profile
remains `gcc_2_8_1_g0` and its native extent remains `0x54`.

`ModelTexture_PackPageClutCopy` is the former `func_80059A50`.
It copies the caller's complete `GsIMAGE` into a local, calls
`ModelTexture_PackPageClut` with that local and returns the packed word.
The provider changes `px`, `cx` and `cy` while applying side/region biases;
the copy therefore isolates those changes from repeated queries of an
overlay's shared image table.

The original `s32` arguments, non-const image pointer, whole-struct copy,
`u32` return, `gcc_2_8_1_g8` profile and `0x58` extent are unchanged.
The provider's biases, masks, mutation order and packed page/CLUT layout
are not rewritten.

Both standalone sources have semantic paths, selected by every native
manifest and applicable static split. The existing texture owner header
publishes their exact interfaces. One old-spelling bridge there preserves
all 54 historical matching overlay C bodies and their fingerprints; those
bodies already reach the owning header through their existing include
chains. No overlay source is renamed or edited.

Resident and per-image import/linker/Splat maps publish canonical names
at unchanged addresses while preserving every other binding and attribute.
Existing controls migrate identifiers/include paths only. Historical
attempts, reuse records, compiler claims, profiles and the full Lua tree
remain unchanged; no new source-string or synthetic harness is added.
