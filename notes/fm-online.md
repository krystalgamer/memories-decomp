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
turn variable.

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
| Guardian-star sound/cursor | `0x800370E4`, `0x800370EC`, `0x800371D0`, `0x8003725C` | `func_8003700C`, `func_800371A8` |
| Guardian-star symbols | `0x80037FF4` | `func_80037DA4` |
| Guardian-star text | `0x80039730` | `func_800393B0` |
| Allow equal duelist codes | `0x8003FAE8`, `0x8003FAF0` | `func_8003F8D4` |

Most visibility patches switch calls between their retail instruction and
`nop` according to whose turn it is. FM-Online also writes `0x00` to the
current player's card-view byte and `0xFF` to the other player's byte. Together
these changes show that opponent information is not controlled by one privacy
flag: card faces, triangle preview, guardian-star cursor, symbols, text, sound,
and automatic card rotation are separate presentation paths.

The duelist-code patch replaces the store at `0x8003FAE8` with `nop` and the
return value at `0x8003FAF0` with `li v0, 1`. This indicates that
`func_8003F8D4` contains the equal-code rejection path and returns a Boolean-like
success value after setting an error or scene byte.

The life-point width adjustment reads `RAM::lp[i] - 2`, so it tests the
displayed values at `0x800EA002` and `0x800EA022`. Its loading and end-of-duel
checks read `RAM::lp[i]` directly and therefore test the actual values at
`0x800EA004` and `0x800EA024`. The companion preserves the game's distinction
between animated display totals and gameplay totals rather than treating each
player's pair as one field.

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
