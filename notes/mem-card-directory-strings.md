# Memory-card directory patterns and diagnostics

| Former identity | Canonical name | Consumer view |
| --- | --- | --- |
| `D_8009AF6C` | `gMemCard_szDirectoryEntryFormat` | mutable `char[]` |
| `D_8009AF70` | `gMemCardDialog_szAllFilesPattern` | mutable `u8[]` |
| `D_8009AF7C` | `gMemCard_szAllFilesPattern` | mutable `u8[]` |
| `D_80010378` | `gMemCard_szFreeBlockCountFormat` | mutable `char[]` |
| `D_80010398` | `gMemCard_szMakeFileTrace` | mutable `char[]` |
| `D_800103A4` | `gMemCard_szFailFileTrace` | mutable `char[]` |

## Directory enumeration and diagnostic arguments

Save and trade-save dialog states pass the dialog wildcard to
`MemCardGetDirentry`. The request/event driver independently passes its
wildcard to `MemCard_FindFiles` in `MemCard_DoLoadDirectory`, then computes
the free-block count from the loaded directory.

Both wildcard initializers contain `"*"`, but they are distinct objects.
Neither the shared text nor their consumers justify folding their storage
or assigning one name to the other.

Under its existing diagnostic flag, `MemCardWork_UpdateOperation` prints
each directory-entry address using the `"%s\n"` format. The SDK
`DIRENTRY.name` member starts at offset zero, as independently asserted by
`mem_card_directory.h`. The same path prints the selected work slot's
free-block byte using `"\nFREE %d\n\n"`. Its argument is not the global
free-block word directly; the existing narrowing and snapshot remain.

## Create-file traces are not a new result policy

The create-file operation cases retain their first-entry latch and
free-block comparison. On subsequent calls, the non-European code prints
`"Make File\n"` only when the existing operation-result byte is zero,
then prints `"Fail File\n"` unconditionally.

The names identify those literal traces, not exclusive success/failure
branches. No missing `else`, result normalization, new initialization,
request retry, message or early return is introduced.

All four diagnostics remain behind their existing `VERSION_EUROPE`
guards. PAL still has the old diagnostic declarations but no compiled
diagnostic references. Only the two active wildcard identities receive
PAL bindings; unused format providers are not invented for symmetry.

## Initialized ownership and incomplete consumer views

The NA directory-entry format and two wildcard objects remain actual
initialized definitions in `src/game/data_8009af6c.c`: `char[4]`,
`u8[4]` and `u8[4]`, each explicitly in `.sdata`. The wildcard definitions
retain padding beyond their terminators. The directory-entry format
already occupies four bytes including its terminator.

The full `0x1C`-byte window remains one unit in its original
`4 + 4 + 8 + 4 + 8` order. The unrelated volatile depth table, its
interior aliases and the MIDI marker retain their definitions, values,
padding, qualifiers and placement. No compiler profile or source group
changes, and no object is split, deduplicated or reallocated.

The NA initialized objects therefore use **old identity = canonical
C definition** compatibility aliases. Assigning the canonical name over
the old provider would instead override initialized ownership. All other
active regional providers use **canonical = original external provider**.
Existing NA data and absolute symbol classes are preserved, not all
assumed to be absolute.

Consumers remain incomplete mutable arrays, not fixed-size or `const`
declarations. The pattern header follows the existing directory owner;
the diagnostic header follows the existing work-controller owner.
The driver does not gain unrelated diagnostic declarations. Old headers,
selectors, SDK structures and regional wrappers remain unchanged.

## Regional identities and conservation

The Japanese dialog wrapper already selects literal providers
`D_8009AEB4`, `D_8009AEB8`, `D_800102F4`, `D_80010314` and
`D_80010320`. Their existing imported identities remain the alias targets.
PAL wildcards retain their `gEuropean_` providers. NA-number suffixes are
not used to guess other images' addresses.

All 22 active native roles preserve their original provider identities.
Local maps independently establish 17 numeric bindings; the other five
Japanese providers are retained symbolically, not presented as a complete
local numeric or retail-image proof. Fresh PR-side regional full-image
checks remain mandatory.

The existing controller witness changes only its four diagnostic
identifiers in definitions and pointer comparisons. Its fixture strings,
cases, assertions, mutations, setup and linkage remain unchanged. The
legacy ownership controls stay frozen. No harness, fixture, probe or test
case is added, and cloud/regional/overlay/witness suites are not executed
locally.

Conservation compares complete ordinary compiler outputs, all eight
actual source/profile compiler and MASPSX output pairs, initialized data
bytes, fallback assembly and whole source/control files. Only the scoped
original identifiers are reversed in assembly; instructions, directives,
relocations, order and filenames are not filtered.

## Driver path format and transfer retries

| Former identity | Canonical name | Preserved view |
| --- | --- | --- |
| `D_80010538` | `gMemCard_szDevicePathFormat` | mutable incomplete `u8[]` |
| `D_8009B436` | `gMemCard_bTransferRetriesRemaining` | plain nonvolatile `u8` |

The provided NA executable contains `"bu%02X:%s"`. Read, write and
create-file requests format the existing request-path buffer with their
original channel and name arguments. `MemCard_FindFiles` uses the same
format for its local channel-and-pattern path.

`%02X` retains uppercase hexadecimal with a minimum width of two. It does
not add a channel bound or normalization. The existing integer arguments,
pointer casts, mutable `u8[]` declaration and unknown format-object extent
remain, without a new `const` or fixed-size view.

Only the file read/write directory-success stage initializes the separate
transfer retry byte to `0x14`. The shared asynchronous retry tail leaves
it unchanged for a zero I/O result; otherwise it decrements the byte and
tests its `s8` view for negative before returning to transfer step 1.
The byte wrap, pre-decrement order and exact branch remain.

Raw-sector requests share that tail without a new initialization of this
counter. Existing stale values are preserved. The independent
`gMemCard_bRetries` stage budget, open/seek/transfer retry loops and
zero-result interpretation are not merged or repaired.

The new request-input header follows the late `unmatched.h` format owner
and the existing `mem_card.h` scalar owner. Both old headers and all
regional wrappers stay unchanged. These two identities add 14 active
native bindings, all independently mapped numerically, using canonical
aliases equal to the original external providers.

The existing I/O-controller witness changes only its four retry-byte
identifier occurrences. Its setup values, cases, assertions and mutations
remain. No local witness execution or new harness is introduced. The full
component also retains the separate
[work-state and checksum contract](mem-card-work-state.md).
