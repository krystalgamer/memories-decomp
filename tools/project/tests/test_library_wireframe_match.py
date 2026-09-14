"""Recompile the wireframe against a legally supplied, already built image."""

import hashlib
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from build_baseline import compile_c, tool
from integrate_verified_match import (
    preprocess_source,
    uses_disallowed_psyq_rtps_asm,
    validate_effective_profile,
)


class LibraryWireframeMatchTests(unittest.TestCase):
    def test_complete_text_and_official_macro_policy(self):
        target = ROOT / "game/SLUS_014.11"
        baseline = ROOT / "tmp/project-build/SLUS_014.11.elf"
        if not target.is_file() or not baseline.is_file():
            self.skipTest("requires legal game/SLUS_014.11 and a completed make match")
        retail = target.read_bytes()
        self.assertEqual(
            hashlib.sha256(retail).hexdigest(),
            "84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88",
        )
        manifest = json.loads((ROOT / "config/slus_01411/matching_c.json").read_text())
        entries = [entry for entry in manifest["functions"] if entry["address"] == "0x80029934"]
        self.assertEqual(len(entries), 1)
        entry = entries[0]
        self.assertEqual(int(entry["size"], 0), 0x57C)
        profiles = json.loads((ROOT / "config/slus_01411/compiler_profiles.json").read_text())["profiles"]
        profile = profiles[entry["profile"]]
        validate_effective_profile(profile, entry["profile"])
        self.assertTrue(profile.get("allow_psyq_inline_macros"))
        preprocessed = preprocess_source(ROOT, ROOT / entry["source"], profile)
        self.assertFalse(uses_disallowed_psyq_rtps_asm(preprocessed))

        def output(*command):
            return subprocess.check_output([str(arg) for arg in command], cwd=ROOT, text=True)

        symbols = {}
        for line in output(tool(ROOT, "nm"), "-n", baseline).splitlines():
            fields = line.split()
            if len(fields) == 3:
                symbols[fields[2]] = int(fields[0], 16)
        with tempfile.TemporaryDirectory(prefix="wireframe-match-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            relative = str(directory.relative_to(ROOT))
            obj = compile_c(
                ROOT, tool(ROOT, "as"),
                {"source": entry["source"], "profile": entry["profile"], "object": "wireframe.o"},
                profiles, object_directory=relative, asm_directory=relative,
            )
            undefined = [line.split()[-1] for line in output(tool(ROOT, "nm"), "-u", obj).splitlines()]
            script = directory / "wireframe.ld"
            script.write_text(
                "OUTPUT_ARCH(mips)\nSECTIONS {\n"
                " .text 0x80029934 : SUBALIGN(2) { *(.text) }\n"
                " .rodata : { *(.rodata) *(.rdata) }\n"
                " /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.comment) }\n}\n"
                + "\n".join(f"{symbol} = 0x{symbols[symbol]:X};" for symbol in undefined)
                + f"\n_gp = 0x{symbols['_gp']:X};\n"
            )
            elf, text, rodata = (directory / item for item in ("wireframe.elf", "text.bin", "rodata.bin"))
            output(tool(ROOT, "ld"), "-EL", "--no-relax", "-T", script, "-o", elf, obj)
            output(tool(ROOT, "objcopy"), "-O", "binary", "-j", ".text", elf, text)
            output(tool(ROOT, "objcopy"), "-O", "binary", "-j", ".rodata", elf, rodata)
            offset = 0x80029934 - 0x80010000 + 0x800
            self.assertEqual(text.read_bytes(), retail[offset:offset + 0x57C])
            self.assertEqual(rodata.read_bytes(), b"")


if __name__ == "__main__":
    unittest.main()
