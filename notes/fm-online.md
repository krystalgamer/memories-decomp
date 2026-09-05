# FM-Online findings

[FM-Online](https://github.com/mateusfavarin/FM-Online) is a Windows companion
process for Mednafen netplay. It finds the emulator's emulated PlayStation RAM,
then reads game state and rewrites NTSC-U instructions or data while the game
is running. It is useful semantic evidence because each patch names the
behavior its author intended to change, but those names are hypotheses until
confirmed against local control flow.

This review is based on upstream commit `cb08b5c`.

## How it attaches

`memory.cpp` opens `mednafen.exe`, scans its module memory for the disc string
`SLUS_014.11;1`, and treats the match minus `0x9244` as the start of emulated
RAM. All addresses in `fm.cpp` are offsets from that base. Therefore
`RAM::prng = 0x0FE6F8` corresponds to PlayStation address `0x800FE6F8`.

This discovery method is emulator- and build-specific. It does not validate
the executable hash, and the instruction patches assume the NTSC-U
`SLUS-01411` layout.

### Attachment validation limits

The scanner compares the full `SLUS_014.11;1` marker including its terminating
null byte, then derives the RAM base by subtracting `0x9244`. If no marker is
found, `baseAddress` remains zero, but `GetProcessSettings` still returns
success as long as `OpenProcess` succeeded: `FindBaseAddress` and `GetPSXRAM`
do not report scan failure to their caller.

`ReadMem` and `WriteMem` also ignore the return values and transferred-byte
counts from `ReadProcessMemory` and `WriteProcessMemory`. The companion
therefore confirms neither that its derived base is valid nor that an
individual patch was applied. Its labels and offsets remain useful static
evidence, but successful startup is not runtime proof that the expected game
image or emulated RAM mapping was found.

## Confirmed data addresses

Several labels agree with independently tracked repository symbols:

| FM-Online label | Address | Repository evidence |
|---|---:|---|
| `currentScene` | `0x8009B26C` | Main-mode state used by `Main_RunMenu` |
| `currentTurn` | `0x8009B1D5` | Duel turn byte |
| player displayed life points | `0x800EA002` | `gDuel_wPlayerLifePointDisplay` |
| player actual life points | `0x800EA004` | `gDuel_wPlayerLifePoint` |
| opponent displayed life points | `0x800EA022` | `gDuel_wOpponentLifePointDisplay` |
| opponent actual life points | `0x800EA024` | `gDuel_wOpponentLifePoint` |
| PRNG state | `0x800FE6F8` | `gRand_dwSeed` |
| card-view bytes | `0x800EA00F`, `0x800EA02F` | Per-player duel display state |

The life-point and card-view pairs are fields in the same two side records,
not separate parallel allocations. Matching C establishes
`D_800E9FF0[2]` as an array with a `0x20`-byte stride. Relative to each record,
the displayed life points are at `+0x12`, authoritative life points at
`+0x14`, and FM-Online's card-view byte at `+0x1F`:

| Side record | Display LP | Actual LP | Card view |
|---:|---:|---:|---:|
| `0x800E9FF0` | `0x800EA002` | `0x800EA004` | `0x800EA00F` |
| `0x800EA010` | `0x800EA022` | `0x800EA024` | `0x800EA02F` |

This stride explains why each FM-Online address array advances by exactly
`0x20` between players and provides a stable structural anchor for recovering
the still-unnamed display-state field.

FM-Online identifies scene value `0x2D0` as the 2P setup screen and `0x2C3` as
the active duel. These values are runtime observations and should not yet
replace address-based scene-state names.

## Netplay synchronization

The companion displays the current seed when the 2P scene is entered. Between
matches it advances its local copy exactly 255 times with:

```text
seed = seed * 0x41C64E6D + 0x3039
```

It then writes the result to `gRand_dwSeed`. The constants exactly match the
resident Psy-Q `rand` implementation. The fixed 255-step adjustment is strong
evidence that otherwise identical consecutive PvP matches consume or require
a stable PRNG offset, but the project does not document how 255 was measured.
It should be verified with a `rand` call trace before being treated as a game
invariant.

The project also alternates the starting side between matches by changing one
byte at `0x80017AF2`, inside `func_800179F4`. The replacement is `0x80` or
`0x90`; this changes an instruction operand rather than directly writing a
turn variable. The retail four-byte instruction at `0x80017AF0` is
`CD 02 80 A3`, or `sb zero, D_8009B1D5(gp)` in little-endian storage. Replacing
its byte at `+2` with `0x90` changes only the source register, producing
`sb s0, D_8009B1D5(gp)`. `func_800179F4` initializes `s0` to one before this
store, so FM-Online alternates the initial turn byte between zero and one
without changing the store opcode or destination.

## Runtime patches

The following table maps the project's instruction offsets to local resident
functions:

| Purpose in FM-Online | Patch address(es) | Containing function |
|---|---|---|
| Life-point digit width | `0x80016F14`, `0x80016F98` | `func_80016E70` |
| Alternate starting player | `0x80017AF2` | `func_800179F4` |
| Disable automatic trap rotation | `0x8001CA24`, `0x8001CA2C`, `0x8001CA30` | `func_8001BD88` |
| Bypass life-point limit behavior | `0x800251A4` | `func_800250C8` |
| Hide triangle text | `0x800284D8` | `func_800283F4` |
| Hide triangle image | `0x8002946C` | `func_800291E0` |
| Guardian-star sound/cursor | `0x800370E4`, `0x800370EC`, `0x800371D0`, `0x8003725C` | `Dialog_ReadChoiceInput` (`0x8003700C`), `Dialog_UpdateChoice` (`0x800371A8`) |
| Guardian-star symbols | `0x80037FF4` | `func_80037DA4` |
| Guardian-star text | `0x80039730` | `func_800393B0` |
| Allow equal duelist codes | `0x8003FAE8`, `0x8003FAF0` | `func_8003F8D4` |

Most visibility patches switch calls between their retail instruction and
`nop` according to whose turn it is. FM-Online also writes `0x00` to the
current player's card-view byte and `0xFF` to the other player's byte. Together
these changes show that opponent information is not controlled by one privacy
flag: card faces, triangle preview, guardian-star cursor, symbols, text, sound,
and automatic card rotation are separate presentation paths.

The two triangle patches are argument-setup switches rather than draw-call
switches. At `0x800284D8`, retail loads `a2` from `D_8009B24B`; the delay slot
of the later `func_800404CC` call then adds `0x0E`. At `0x8002946C`, retail
loads the constant `4` into `a2` before another `func_800404CC` call.
FM-Online's enable values (`0x93860343` and `0x24060004`) are those original
instructions exactly. Hiding the text or image replaces only that instruction
with `nop`; both calls and their other arguments remain in place. The patch
therefore disrupts each draw command's third argument instead of skipping the
rendering function outright.

The guardian-star group does switch complete call instructions. The enable
words restore these exact retail targets:

| Address | Retail call |
|---:|---|
| `0x800370E4` | `SD_SEPlayFull(6)` |
| `0x800370EC` | `Dialog_HighlightChoice` |
| `0x800371D0` | `func_8004006C` |
| `0x8003725C` | `Dialog_HighlightChoice` |
| `0x80037FF4` | `func_80036C14` |
| `0x80039730` | `func_80036C14` |

The three addresses grouped as `drawGsCursor` are therefore not three copies
of one renderer: two call the accepted dialog-choice highlighter, while
`0x800371D0` calls a separate helper whose return value is consumed by the
following code. FM-Online's labels describe the observed combined effect of
disabling these sites, not interchangeable function identities.

The three automatic-rotation patches suppress one complete retail action in
`func_8001BD88`. The branch is eligible when the packed card type is below
`0x14` (a monster) or exactly `0x15` (Trap), and runs only when byte `0x21` of
the current object is zero. Retail then writes `0x10` to the object's halfword
at offset `0x60`, writes a value with bit `0x20` asserted to `D_8009B174`, and
calls `SD_SEPlayFull(0x0B)`. FM-Online nops those three instructions at
`0x8001CA24`, `0x8001CA2C`, and `0x8001CA30`, while leaving the type and object
state tests intact. This verifies the coordinated field, flag, and sound
suppression; the exact semantic name of object offset `0x60` remains unresolved.

The duelist-code patch replaces the store at `0x8003FAE8` with `nop` and the
return value at `0x8003FAF0` with `li v0, 1`. In the retail branch, the first
instruction is `sb v0, D_8009B3EA(gp)` with `v0` already set to `0x0A`; the
second is the jump delay-slot instruction `addu v0, zero, zero`. The patch
therefore prevents state `0x0A` from being stored and changes that path's
return value from zero to one. This directly identifies the bypassed path as a
failed duelist-code validation result, although the broader semantics of
`D_8009B3EA` remain unresolved.

The life-point width adjustment reads `RAM::lp[i] - 2`, so it tests the
displayed values at `0x800EA002` and `0x800EA022`. Its loading and end-of-duel
checks read `RAM::lp[i]` directly and therefore test the actual values at
`0x800EA004` and `0x800EA024`. The companion preserves the game's distinction
between animated display totals and gameplay totals rather than treating each
player's pair as one field.

The writes at `0x80016F98` for the player and `0x80016F14` for the opponent
each target the first byte of an identical retail instruction:
`04 00 07 24`, or `addiu a3, zero, 4`. Writing `0x05` changes the argument
passed to `func_80016D2C` from four digits to five; restoring `0x04` returns
the retail width. The patch changes only the low byte of the immediate and
does not replace the formatter call or either branch's coordinate setup.

The `0x800251A4` patch nops a retail `sh v1, 0x14(a0)` instruction. Immediately
before it, `func_800250C8` has added a table-selected multiple of 100 to the
signed halfword at offset `0x14`, compared the result with the signed upper
bound at offset `0x16`, and loaded that bound into `v1`. Retail therefore
clamps the active field to the bound when the increment exceeds it; the nop
leaves the over-limit value in place. This verifies FM-Online's patch as a
limit bypass, but the local function's broader field-motion behavior does not
independently establish that these two structure fields are life points.

## Behavioral model

The tool polls at approximately half-frame intervals. During a duel it:

1. Detects a turn transition from `0x8009B1D5`.
2. Exposes the current player's card view and blocks the opponent's.
3. Enables guardian-star feedback only for the active player.
4. Adjusts the life-point text width when a displayed value exceeds 9999.
5. Treats either zero actual life-point value as an end-of-duel signal.

The zero-life-point test is an implementation shortcut, not proof that all
duels end exclusively through life-point depletion. Exodia, surrender, or
other state transitions should be checked before using it to name game-owned
functions.

## Research value

High-value follow-up work is concentrated in the containing functions above:

- Trace `func_8003F8D4` to isolate 2P duelist-code validation.
- Recover the card-view byte structure around `0x800EA000`.
- Split guardian-star selection from its rendering and sound helpers.
- Verify the 255-call inter-match PRNG adjustment.
- Determine whether `func_80016E70` formats both players' life-point strings.

The upstream code is evidence, not a patch source for this repository. Its
hard-coded writes intentionally change retail behavior and must never be
folded into matching C.
