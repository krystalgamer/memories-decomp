# Memory-card directory patterns and diagnostics

The memory-card units write their directory patterns, diagnostics, device
path format and card file name as string literals at the call sites. Each
literal lands in its unit's `.sdata` (eight bytes or fewer) or `.rodata`
in first-use order, which is the retail layout.

| Retail bytes (NA) | Literal | Unit and first use |
| --- | --- | --- |
| `0x8009AF6C` | `"%s\n"` | `mem_card.c`, `MemCardWork_UpdateOperation` |
| `0x8009AF70` | `"*"` | `mem_card.c`, `MemCardDialog_UpdateSave` |
| `0x8009AF7C` | `"*"` | `mem_card_driver.c`, `MemCard_DoLoadDirectory` |
| `0x80010378` | `"\nFREE %d\n\n"` | `mem_card.c`, `MemCardWork_UpdateOperation` |
| `0x80010384` | `"BASLUS-01411-YUGIOH"` | `mem_card.c`, `MemCardWork_UpdateOperation` |
| `0x80010398` | `"Make File\n"` | `mem_card.c`, `MemCardWork_UpdateOperation` |
| `0x800103A4` | `"Fail File\n"` | `mem_card.c`, `MemCardWork_UpdateOperation` |
| `0x80010538` | `"bu%02X:%s"` | `mem_card_driver.c`, `MemCard_ReqReadFile` |

`mem_card.c` owns `.rodata` from its first string, ahead of the
`MemCardWork_UpdateOperation` jump tables, and `.sdata` through the
dialog wildcard. `mem_card_driver.c` owns the device path format ahead of
its `MemCard_ProcessRequest` tables and the driver wildcard in `.sdata`.
The depth table between the two wildcards is the only `.sdata` of
`display_object_system.c`, and the `"MTrk"` marker after them is the
literal `SD_FindMidiTrackChunk` (`sound_find_midi_track_chunk.c`) compares.

## Directory enumeration and diagnostic arguments

Save and trade-save dialog states pass their wildcard to
`MemCardGetDirentry`. The request/event driver independently passes its
own to `MemCard_FindFiles` in `MemCard_DoLoadDirectory`, then computes
the free-block count from the loaded directory. The two `"*"` literals are
separate copies because they belong to different translation units.

Under its existing diagnostic flag, `MemCardWork_UpdateOperation` prints
each directory-entry address using `"%s\n"`. The SDK `DIRENTRY.name`
member starts at offset zero, as independently asserted by
`mem_card_driver.h` (formerly `mem_card_directory.h`). The same path
prints the selected work slot's free-block byte using `"\nFREE %d\n\n"`.
Its argument is not the global free-block word directly; the existing
narrowing and snapshot remain.

## Create-file traces are not a new result policy

The create-file operation cases retain their first-entry latch and
free-block comparison. On subsequent calls, the non-European code prints
`"Make File\n"` only when the existing operation-result byte is zero,
then prints `"Fail File\n"` unconditionally. These are literal traces,
not exclusive success/failure branches.

All four diagnostics remain behind their existing `VERSION_EUROPE`
guards, so the PAL images contain none of them.

## Card file names and regional builds

`mem_card.c` spells the card file name through `MEM_CARD_SAVE_FILE_NAME`,
whose default is the North American literal. Each PAL image carries its
own product code, so each has its own wrapper: `european/mem_card.c`
(`"BESLES-03947-YUGIOH"`), `spanish/mem_card.c` (03951), and the
`french`, `german` and `italian` wrappers, which include the Spanish one
with 03948, 03949 and 03950.

Japan cannot take the literals. Its image puts the Japanese-only options
file name (`"BISLPMP86398-YUGIOH"` at `0x8001032C`) after
`"Fail File\n"` and before `MemCardWork_UpdateOperation`'s first jump
table, inside the unit's `.rodata`; the four bytes of padding before that
table at `0x80010340` keep it eight-byte aligned within the object. The
string's only user, `SaveData_RequestOptionsWrite`, comes after
`MemCardWork_UpdateOperation`, so a literal there would follow the tables.
`japanese/mem_card.c` therefore maps the save name and the three
`.rodata` diagnostics to linker symbols bound in
`config/slpm_86398/symbols.txt`: `gMemCard_szFreeBlockCountFormat`
(`0x800102F4`), `gMemCard_szSaveFileName` (`"BISLPM-86398-YUGIOH"` at
`0x80010300`), `gMemCard_szMakeFileTrace` (`0x80010314`),
`gMemCard_szFailFileTrace` (`0x80010320`) and
`gMemCard_szOptionsFileName`. The options payload format remains unknown.
Japan's `.sdata` strings and device path format are literals as
elsewhere.

## Driver path format and transfer retries

`"bu%02X:%s"` formats the request-path buffer for read, write and
create-file requests with their original channel and name arguments, and
`MemCard_FindFiles` uses it for its local channel-and-pattern path.
`%02X` retains uppercase hexadecimal with a minimum width of two. It does
not add a channel bound or normalization.

`gMemCard_bTransferRetriesRemaining` (former `D_8009B436`) is a plain
nonvolatile `u8` defined in `mem_card_driver.c`.

Only the file read/write directory-success stage initializes the separate
transfer retry byte to `0x14`. The shared asynchronous retry tail leaves
it unchanged for a zero I/O result; otherwise it decrements the byte and
tests its `s8` view for negative before returning to transfer step 1.
The byte wrap, pre-decrement order and exact branch remain.

Raw-sector requests share that tail without a new initialization of this
counter. Existing stale values are preserved. The independent
`gMemCard_bRetries` stage budget, open/seek/transfer retry loops and
zero-result interpretation are not merged or repaired.

The full component also retains the separate
[work-state and checksum contract](mem-card-work-state.md).
