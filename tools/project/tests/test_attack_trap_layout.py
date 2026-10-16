"""Check the trap scratch layout and typed attacker contract with GCC 2.8.1."""

import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from build_baseline import compile_c, tool

COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"


@unittest.skipUnless(COMPILER.is_file(), "requires supported GCC 2.8.1")
class AttackTrapLayoutTests(unittest.TestCase):
    def test_scratch_arrays_word_aliases_and_alignment(self):
        profiles = json.loads(
            (ROOT / "config/slus_01411/compiler_profiles.json").read_text()
        )["profiles"]
        with tempfile.TemporaryDirectory(
            prefix="attack-trap-layout-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            relative = str(directory.relative_to(ROOT))
            source = directory / "layout.c"
            source.write_text(
                '#include "../../src/types.h"\n'
                '#include "../../src/game/duel_card_staging.h"\n'
                "#define OFFSET(member) "
                "((u32)&((DuelAttackTrapScratchBlock *)0)->member)\n"
                "const u32 layout[] = {\n"
                " sizeof(DuelAttackTrapTable), sizeof(DuelAttackTrapScratchBlock),\n"
                " __alignof__(DuelAttackTrapScratchBlock),\n"
                " OFFSET(table.entries.card_ids[0]),\n"
                " OFFSET(table.entries.card_ids[5]),\n"
                " OFFSET(table.entries.record_indices[0]),\n"
                " OFFSET(table.entries.record_indices[5]),\n"
                " OFFSET(table.words[16]), OFFSET(table.words[21]),\n"
                " DUEL_CARD_STAGING_TRAP_BASE_OFFSET +\n"
                " OFFSET(table.entries.card_ids[0]),\n"
                " DUEL_CARD_STAGING_TRAP_BASE_OFFSET +\n"
                " sizeof(DuelAttackTrapScratchBlock), DUEL_ATTACK_TRAP_COUNT\n"
                "};\n"
            )
            obj = compile_c(
                ROOT, tool(ROOT, "as"),
                {
                    "source": str(source.relative_to(ROOT)),
                    "profile": "gcc_2_8_1_g0", "object": "layout.o",
                },
                profiles, object_directory=relative, asm_directory=relative,
            )
            data = directory / "layout.bin"
            subprocess.run(
                [
                    str(tool(ROOT, "objcopy")), "-O", "binary",
                    "-j", ".rodata", "-j", ".rdata", str(obj), str(data),
                ],
                cwd=ROOT, check=True,
            )
            expected = (
                0x2C, 0x3C94, 2, 0x3C68, 0x3C72, 0x3C88,
                0x3C92, 0x3C88, 0x3C92, 0x1BC68, 0x1BC94, 6,
            )
            compiled = data.read_bytes()
            length = len(expected) * 4
            self.assertEqual(compiled[:length], struct.pack("<12I", *expected))
            self.assertEqual(compiled[length:], bytes(len(compiled) - length))

    def test_attacker_contract_accepts_object_and_rejects_bytes(self):
        with tempfile.TemporaryDirectory(
            prefix="attack-trap-contract-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            for argument, accepted in (
                ("const struct DisplayObject *", True), ("u8 *", False),
            ):
                with self.subTest(argument=argument):
                    source = directory / "probe.c"
                    source.write_text(
                        '#include "../../src/types.h"\n'
                        '#include "../../src/game/duel_trap_resolution.h"\n'
                        f"s32 (*checked)({argument}) = Duel_SelectAttackTrap;\n"
                    )
                    result = subprocess.run(
                        [
                            str(COMPILER), "-S", "-O2", "-G0", "-Werror",
                            "-o", str(directory / "probe.s"), str(source),
                        ],
                        cwd=ROOT, env=environment, text=True,
                        capture_output=True, timeout=60,
                    )
                    if accepted:
                        self.assertEqual(result.returncode, 0, result.stderr)
                    else:
                        self.assertNotEqual(result.returncode, 0, result.stderr)
                        self.assertIn("incompatible pointer type", result.stderr)


if __name__ == "__main__":
    unittest.main()
