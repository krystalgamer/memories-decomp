# maspsx: reorder-mode branch delay slots are padded, never filled

**Status:** suspected divergence from ASPSX, submitted for verification.
**Reporter:** Copilot
**maspsx commit:** `746b895f02929ecd148af7b1f4ff05b69f973878` (pinned in
`tools/bootstrap/tools.json`, checkout unmodified)

## Summary

In `.set reorder` mode maspsx emits an unconditional `nop` after every branch
and jump instead of moving the following instruction into the delay slot. ASPSX
fills the slot. The retail executable contains the filled form, so any function
whose GCC output leaves a branch in reorder mode with a fillable successor
builds one instruction too long and cannot match.

## Exact invocation

```
tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc -S \
    -O2 -G8 -mel -mips1 -mcpu=R3000 -msoft-float -fno-builtin \
    -mgas -msplit-addresses -Dpsx -D__psx__ -D__psx \
    -o out.s src/game/func_80012DB4.c

python3 tools/vendor/maspsx/maspsx.py \
    --aspsx-version=2.81 --expand-div -G8 --use-comm-section \
    < out.s > out.maspsx.s

tools/toolchains/binutils-2.42/bin/mipsel-none-elf-as \
    -EL -mips1 -G8 -o out.o out.maspsx.s
```

Profile: `gcc_2_8_1_g8_split_comm`. The same behaviour appears without
`--use-comm-section` and on every other profile in
`config/slus_01411/compiler_profiles.json`.

## Minimal reproducer

Input (`repro.s`), reduced from GCC's own output for `func_80012DB4`:

```
	.text
	.globl	f
	.ent	f
f:
	andi	$2,$2,0x00ff
	beq	$2,$0,$L7
	li	$2,1
	sb	$2,D_8009B0C1
$L7:
	lbu	$2,D_8009B0C1
	j	$31
	.end	f
	.comm	D_8009B0C1,1
```

Command:

```
python3 tools/vendor/maspsx/maspsx.py \
    --aspsx-version=2.81 --expand-div -G8 --use-comm-section < repro.s
```

Actual output:

```
beq	$2,$0,$L7
nop  # DEBUG: branch/jump          <-- slot padded
li	$2,1
sb	$2,%gp_rel(D_8009B0C1)($gp)
```

Expected (what retail contains):

```
beq	$2,$0,$L7
li	$2,1                           <-- slot filled from the next instruction
sb	$2,%gp_rel(D_8009B0C1)($gp)
```

## Retail evidence

`func_80012DB4` at `0x80012DB4`, bytes read straight from `game/SLUS_014.11`:

```
80012DF4  304200FF  andi $v0, $v0, 0xFF
80012DF8  02004010  beqz $v0, 0x80012E04
80012DFC  01000224  addiu $v0, $zero, 0x1      <-- in the delay slot
80012E00  B90182A3  sb   $v0, 441($gp)
80012E04  B9018293  lbu  $v0, 441($gp)
```

The move is safe and ASPSX made it: the delay-slot instruction executes on both
paths, and on the taken path `$v0` is immediately overwritten by the `lbu` at
`0x80012E04`, so the write is dead there.

## Where it comes from

`maspsx/__init__.py`:

```python
elif op in branch_mnemonics or op in jump_mnemonics:
    res.append(line)
    if self.is_reorder:
        res.append("nop  # DEBUG: branch/jump")
```

The slot is padded unconditionally; there is no attempt to hoist the successor,
and no command-line option changes it.

Note GCC does fill some slots itself — elsewhere in this same function it emits
`.set noreorder` around `bne $3,$0,$L10` with the slot already filled, and
maspsx correctly leaves those alone. The gap is only the reorder-mode path that
GCC deliberately leaves to the assembler. Here GCC cannot fill it because the
only candidate before the branch is the `andi` that computes the branch operand,
so it emits reorder mode and defers to the assembler, as ASPSX expects.

## What was ruled out first

- All 30 profiles in `compiler_profiles.json`: every one builds 43 instructions
  against the target's 42.
- Four source spellings of the clamp (`!= 0`, bare truth test, `(u8)` cast,
  inverted `if`/`else`): all identical output. GCC always emits
  `andi; beq; li; sb` in reorder mode, which is also exactly the target's
  instruction order, so no source shape can move the fill responsibility back
  to GCC.
- This is *not* the small-extern load-delay case. That one turned out to be
  faithful emulation with a source-side fix — define the global the unit owns
  and build with `--use-comm-section`; see `func_80025028`. Applying that fix
  here is what produces the `nop` at `0x80012DEC` correctly and leaves only this
  branch-slot difference.

## Effect

`func_80012DB4` is otherwise exact: with the owned-global fix applied it
reproduces every instruction, register and relocation of the target except this
one slot, building 43 instructions against 42.

Scan for others with:

```sh
grep -n 'branch/jump' tmp/project-build/asm/*.maspsx.s
```
