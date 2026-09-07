# maspsx: reorder-mode branch delay slots are always padded, never filled

**Status:** suspected divergence from ASPSX, submitted for verification.
**maspsx commit:** `746b895f02929ecd148af7b1f4ff05b69f973878` (pinned in
`tools/bootstrap/tools.json`; checkout unmodified).

## Summary

When GCC leaves a branch in `.set reorder` and expects the assembler to fill the
delay slot, maspsx emits an unconditional `nop` and leaves the successor where
it was. ASPSX moves the successor into the slot when it is safe to do so.

`func_80012DB4` (`0x80012DB4`, 0xA8, 42 instructions) is otherwise an exact
match. With this one slot filled it would be byte-exact, so a fix here is a
free match rather than a diagnostic.

## Files

| file | what it is |
| --- | --- |
| `func_80012DB4.c` | my attempt — the C I believe is correct, self-contained (no project headers) |
| `func_80012DB4.s` | the original game assembly, spimdisasm output via splat |
| `build.sh` | compile → maspsx → assemble, with every flag |

`build.sh` writes all intermediates to `tmp/` and prints what GCC handed the
assembler, what maspsx produced, the built disassembly, and the original game
assembly. It reports **172 bytes built against the game's 168**.

If the slot is filled, `func_80012DB4.c` is byte-exact and this is a match, not
just a diagnostic.

## Exact invocation

Profile `gcc_2_8_1_g8_split_comm` from `config/slus_01411/compiler_profiles.json`:

```sh
tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc -S \
    -O2 -G8 -mel -mips1 -mcpu=R3000 -msoft-float -fno-builtin \
    -mgas -msplit-addresses -Dpsx -D__psx__ -D__psx \
    -o tmp/maspsx_bug_reorder/out.s \
    tools/maspsx_bugs/reorder-branch-delay-slot/func_80012DB4.c

python3 tools/vendor/maspsx/maspsx.py \
    --aspsx-version=2.81 --expand-div -G8 --use-comm-section \
    < tmp/maspsx_bug_reorder/out.s > tmp/maspsx_bug_reorder/out.maspsx.s

tools/toolchains/binutils-2.42/bin/mipsel-none-elf-as \
    -EL -mips1 -G8 -o tmp/maspsx_bug_reorder/out.o \
    tmp/maspsx_bug_reorder/out.maspsx.s
```

## The difference

GCC emits, with **no** `.set noreorder` around the branch:

```
	andi	$2,$2,0x00ff
	beq	$2,$0,$L7
	li	$2,1			# 0x00000001
	sb	$2,D_8009B0C1
$L7:
	lbu	$2,D_8009B0C1
```

maspsx produces:

```
beq	$2,$0,$L7
nop  # DEBUG: branch/jump      <-- slot padded, li left in place
li	$2,1
sb	$2,%gp_rel(D_8009B0C1)($gp)
```

Retail, straight from `game/SLUS_014.11`:

```
80012DF4  304200FF  andi $v0, $v0, 0xFF
80012DF8  02004010  beqz $v0, 0x80012E04
80012DFC  01000224  addiu $v0, $zero, 0x1     <-- successor moved into the slot
80012E00  B90182A3  sb   $v0, 441($gp)
80012E04  B9018293  lbu  $v0, 441($gp)
```

The move is safe: a delay-slot instruction runs on both paths, and on the taken
path `$v0` is immediately overwritten by the `lbu` at `0x80012E04`.

## ASPSX's rule is conditional; maspsx's is not

This matters for any fix, and the same function shows both halves.

The spin loop a few instructions earlier is *also* left in reorder mode by GCC:

```
	bne	$2,$0,$L3
	#.set	volatile
	lw	$2,D_8009B0C8
```

and retail leaves **that** slot as a `nop` (`0x80012DE4`), because the successor
is a volatile load and must not move. maspsx pads it, which is correct.

So ASPSX forward-fills a reorder-mode slot only when the successor is safe to
move, and pads otherwise. maspsx pads unconditionally:

```python
elif op in branch_mnemonics or op in jump_mnemonics:
    res.append(line)
    if self.is_reorder:
        res.append("nop  # DEBUG: branch/jump")
```

Both behaviours appear in this one function, so it exercises the rule and its
exception together.

## Ruled out before filing

- **All 30 profiles** in `compiler_profiles.json`: every one builds 43
  instructions against 42.
- **Four spellings of the clamp** (`!= 0`, bare truth test, `(u8)` cast,
  inverted `if`/`else`): identical output. GCC always emits `andi; beq; li; sb`,
  which is also exactly retail's instruction order, so no source shape moves the
  fill responsibility back to GCC.
- **GCC does fill slots when it can** — in this same function it wraps
  `bne $3,$0,$L10` in `.set noreorder` with `li $2,2` already in the slot, and
  maspsx correctly passes that through. The gap is only the reorder-mode path.
- **Not the small-extern load-delay case.** That one turned out to be faithful
  emulation with a source-side fix: define the global the unit owns and build
  with `--use-comm-section`. That fix is already applied in `func_80012DB4.c`
  (`D_8009B0C1` is defined, not `extern`) and is what produces the correct `nop`
  at `0x80012DEC`. It leaves only this branch slot.
- **A reduced test case does not reproduce it.** Cut down far enough, GCC fills
  the slot itself and emits `.set noreorder`, and maspsx passes it through
  correctly. That is why this report ships the real function rather than a
  minimal example.

## Finding others

```sh
grep -n 'branch/jump' tmp/project-build/asm/*.maspsx.s
```
