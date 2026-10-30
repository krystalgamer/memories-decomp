# French MODEL207/607 sparse data tails

The alleged `0x42C`-byte functions at file offset `0x3520` in three French
MODEL images are data, not unresolved helpers. Their inventory notes claimed
a closed CFG and stack prologue, but the registered retail sectors contain
neither a prologue nor a return. Each span contains 267 words: 265 zero words
and only the following two nonzero words.

| MODEL image | Sector | Loaded address | Word at file `0x3600` | Word at file `0x3800` |
|---|---:|---|---|---|
| 207, stage 7, slot 0 | 57312 | `0x8013E520` | `0x011419C9` | `0x011519C9` |
| 207, stage 8, slot 1 | 57322 | `0x8017E520` | `0x013C19C9` | `0x013D19C9` |
| 607, stage 7, slot 0 | 153912 | `0x8013E520` | `0x05031B8E` | `0x05041B8E` |

These are direct file-to-loaded-address mappings, not compressed or relocated
code. The accepted `variant685_entry` sources do not reference the alleged
helpers. MODEL607 stage 8 slot 1 already treats the corresponding range as
part of its raw `unclassified_tail`; the three corrected layouts use that
same boundary and representation.

The false function rows are removed. Their function symbols and the old
`0x394C` tail boundary are replaced by one address-based data symbol covering
`0x3520..0x5000` (`0x1AE0` bytes). This does not assign a semantic type or name
to the nonzero values, change the accepted C definitions, or claim new C
decompilation. The stale wrapper comment is corrected; all tail bytes remain
represented as raw data.

All four complete 20,480-byte images reproduce their registered retail hashes
before and after the correction:

| MODEL image | SHA-256 |
|---|---|
| 207, stage 7, slot 0 | `46ae8cd49069ac2ffb8c61c076d2950365e70f0d405da09fe532df09215a0f99` |
| 207, stage 8, slot 1 | `cd5d942e940873e5c023eb3042aabcebf9a10b3d509d05f08f30a089abd73792` |
| 607, stage 7, slot 0 | `059b8a51bb025ebf6158a753b748ef0a82c7d62c04806c148407d07d254b3988` |
| 607, stage 8, slot 1 | `e13cad9a0f2ec86d65131c6cd7c0f37f402aba5d163022d4fddf378f744c524c` |
