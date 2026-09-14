"""Exact text/table regression for the resident card-placement controller."""

import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

from build_baseline import compile_c, tool
from workspace import local_environment


ROOT = Path(__file__).resolve().parents[3]
SOURCE = "src/game/duel_scene_card_placement.c"
PROFILE = "gcc_2_8_1_g8_split_comm"
ADDRESS = 0x80019D18
SIZE = 0x13B4
TABLE_ADDRESS = 0x800100F8
TABLE_SIZE = 52
TARGET_SHA256 = "84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88"


class CardPlacementMatchTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        target = ROOT / "game/SLUS_014.11"
        baseline = ROOT / "tmp/project-build/SLUS_014.11.elf"
        built_image = ROOT / "tmp/project-build/SLUS_014.11"
        compiler = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
        if not all(path.is_file() for path in (target, baseline, built_image, compiler)):
            raise unittest.SkipTest("requires legal retail input, local tools and a matched build")
        cls.retail = target.read_bytes()
        if hashlib.sha256(cls.retail).hexdigest() != TARGET_SHA256:
            raise AssertionError("unexpected retail target")
        if hashlib.sha256(built_image.read_bytes()).hexdigest() != TARGET_SHA256:
            raise AssertionError("symbol baseline must come from a matched executable")
        if Path.cwd().resolve() != ROOT:
            raise AssertionError("run project tests from the repository root")
        scratch = ROOT / "tmp/card-placement-match-tests"
        scratch.mkdir(parents=True, exist_ok=True)
        cls.temporary = tempfile.TemporaryDirectory(prefix="match-", dir=scratch)
        cls.addClassCleanup(cls.temporary.cleanup)
        cls.directory = Path(cls.temporary.name)
        environment = os.environ.copy()
        environment.update(local_environment(ROOT))
        environment["TMPDIR"] = str(ROOT / "tmp")

        def run(arguments):
            return subprocess.check_output(
                [str(value) for value in arguments], cwd=ROOT,
                env=environment, text=True, stderr=subprocess.STDOUT,
            )

        profiles = json.loads((ROOT / "config/slus_01411/compiler_profiles.json").read_text())["profiles"]
        obj = compile_c(
            ROOT, tool(ROOT, "as"),
            {"source": SOURCE, "profile": PROFILE, "object": "placement.o"},
            profiles, object_directory=str(cls.directory.relative_to(ROOT)),
            asm_directory=str(cls.directory.relative_to(ROOT)),
        )
        symbols = {}
        for line in run([tool(ROOT, "nm"), "-n", baseline]).splitlines():
            fields = line.split()
            if len(fields) == 3:
                symbols[fields[2]] = int(fields[0], 16)
        bindings = {line.split()[-1] for line in run([tool(ROOT, "nm"), "-u", obj]).splitlines()}
        for line in run([tool(ROOT, "nm"), "--defined-only", obj]).splitlines():
            fields = line.split()
            if len(fields) == 3 and fields[1] == "C":
                bindings.add(fields[2])
        if not bindings <= symbols.keys():
            raise AssertionError(f"unknown image symbols: {sorted(bindings - symbols.keys())}")
        script = cls.directory / "link.ld"
        script.write_text(
            "OUTPUT_ARCH(mips)\nSECTIONS {\n"
            f" .text 0x{ADDRESS:X} : SUBALIGN(2) {{ *(.text) }}\n"
            f" .rodata 0x{TABLE_ADDRESS:X} : SUBALIGN(2) {{ *(.rodata) *(.rdata) }}\n"
            " /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.comment) }\n}\n"
            + "\n".join(f"{name} = 0x{symbols[name]:X};" for name in sorted(bindings))
            + f"\n_gp = 0x{symbols['_gp']:X};\n"
        )
        elf = cls.directory / "placement.elf"
        run([tool(ROOT, "ld"), "-EL", "--no-relax", "-T", script, "-o", elf, obj])
        cls.sections = {}
        for section in (".text", ".rodata"):
            output = cls.directory / (section[1:] + ".bin")
            run([tool(ROOT, "objcopy"), "-O", "binary", "-j", section, elf, output])
            cls.sections[section] = output.read_bytes()

    def test_all_instruction_bytes_and_relocations(self):
        expected = self.retail[ADDRESS - 0x80010000 + 0x800:ADDRESS - 0x80010000 + 0x800 + SIZE]
        self.assertEqual(len(self.sections[".text"]), SIZE)
        self.assertEqual(self.sections[".text"], expected)

    def test_both_switch_tables(self):
        offset = TABLE_ADDRESS - 0x80010000 + 0x800
        self.assertEqual(len(self.sections[".rodata"]), TABLE_SIZE)
        self.assertEqual(self.sections[".rodata"], self.retail[offset:offset + TABLE_SIZE])

    def test_single_function_manifest_ownership(self):
        manifest = json.loads((ROOT / "config/slus_01411/matching_c.json").read_text())
        entries = [entry for entry in manifest["functions"] if entry["source"] == SOURCE]
        self.assertEqual(len(entries), 1)
        self.assertEqual(int(entries[0]["address"], 16), ADDRESS)
        self.assertEqual(int(entries[0]["size"], 16), SIZE)
        self.assertEqual(entries[0]["profile"], PROFILE)


if __name__ == "__main__":
    unittest.main()
