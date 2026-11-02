# Shared transfer workspaces

## Stored shared-work pointer

`gRuntime_pSharedWorkBase` names the four-byte pointer word at
`0x80010000` (formerly `D_80010000`), not the payload at the address stored in that word. The supplied
North American image initializes it to `0x80100000`. Its scalar consumers
reuse that address for model and package payloads, campaign portraits,
display-effect records, duel-effect work and the movie VLC work area.
`Runtime` groups those observed uses; it does not assign exclusive ownership
to the file loader, model runtime or frontend.

The canonical declaration retains `u8 *G32`, the existing plain-pointer or
`HIGH_MEMORY_ADDRESSES_BASE_IN_DATA` `.data` view, and every original cast.
It adds no storage, allocation size, record layout, lifetime, synchronization
or mutability guarantee. The adjacent words and the distinctions documented
in [high-memory-load-addresses.md](high-memory-load-addresses.md) remain.

`Main_RunDuel`, `Duel_InitModelScene` and `Main_RunBuildDeckMenu` formerly
read the word through an incomplete `HighMemoryModelAddressPrefix
D_80010000[]` view, but only ever its first payload word. They now use the
forced `.data` scalar view, which emits the same absolute load of the same
word, and the prefix view is gone. Every image binds the name directly at
`0x80010000`; no `D_80010000` provider or alias remains. The block is still
not one arena array: the neighbouring words keep their own names.

## VRAM transfer rectangle scratch

`gGraphics_aImageTransferRects` names the existing `RECT[2]` written view
originally called `D_800E9D70`. Callers fill rectangle coordinates and pass
their addresses to `LoadImage`, `LoadImage2`, `StoreImage`, `StoreImage2` or
`MoveImage`. Campaign and Free Duel portrait uploads use image/CLUT pairs;
PAL Options uses the same two records for language-image readback and copy.
Library's local `gStageRect` macro still selects the first record. Card-list
code retains its existing `s16 *` view and field-store ordering.

These are scratch records, not stored pointers and not an image/CLUT-specific
record type. Two eight-byte rectangles describe the bytes used by these
callers, not the allocation's proven extent: the next North American label
is at `+0x20`, leaving sixteen unclassified bytes. No new bounds, clearing,
locking, ownership or GPU-completion behavior follows.

## Native providers and compatibility

Both names are aliases of the existing external image providers. The old
headers, symbols, numeric bindings and generated storage remain unchanged;
the narrow canonical headers are included after the old declaration owners.
The Japanese display-effect slot-ID macro receives the same pointer-identity
migration as its scalar caller.

The rectangle's original resident providers are North American
`D_800E9D70`, Japanese `D_800E9D70` at `0x800E9C50`, UK
`D_800E9D70` at `0x8009C418`, and other PAL `D_800E9D70` at `0x8009C838`.
Japanese Free Duel instead keeps its original `D_800E9D70` overlay import
at `0x800E9C50`. Canonical aliases are image-local and preserve that
distinction; automatic original providers are not replaced with guessed
tracked addresses.

The original compiler-expanded views cover 120 source/profile contexts and
590 function registrations. They distinguish 35 imported scalar-pointer
views, 23 imported rectangle views, seven imported legacy-prefix views,
48 retained-unused scalar declarations and twelve retained-unused rectangle
declarations. The North American menu-record reset has neither pointer
declaration nor reference under its existing Japanese-only guard.

Existing stage/model controls receive identifier/include migrations only.
PAL Options dependency sets follow the new rectangle import, while raw
resident ownership tuples and the independently pinned Spanish numeric
provider dictionary stay on their old physical identities. Legacy prefix,
alias, split, unmatched-contract and global-usage controls remain unchanged.
No new harness, fixture, probe or testcase is introduced.
