"""Compile the three save-seal views with the supported 32-bit toolchain."""

import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from build_baseline import compile_c, tool


class SaveIntegrityLayoutTests(unittest.TestCase):
    def test_record_offsets_and_short_trade_prefix(self):
        profiles = json.loads(
            (ROOT / "config/slus_01411/compiler_profiles.json").read_text()
        )["profiles"]
        if not (ROOT / profiles["gcc_2_8_1_g0"]["compiler"]).is_file():
            self.skipTest("requires the supported local GCC 2.8.1 toolchain")
        with tempfile.TemporaryDirectory(
            prefix="save-integrity-layout-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            relative = str(directory.relative_to(ROOT))
            source = directory / "layout.c"
            source.write_text(
                '#include "../../src/types.h"\n'
                '#include "../../src/game/save_data.h"\n'
                "#define OFFSET(type, member) ((u32)&((type *)0)->member)\n"
                "const u32 layout[] = {\n"
                " sizeof(SaveDataPrimaryIntegrity),\n"
                " OFFSET(SaveDataPrimaryIntegrity, mask_words),\n"
                " OFFSET(SaveDataPrimaryIntegrity, checksum[0]),\n"
                " OFFSET(SaveDataPrimaryIntegrity, checksum[1]),\n"
                " sizeof(SaveDataSecondaryIntegrity),\n"
                " OFFSET(SaveDataSecondaryIntegrity, mask_words),\n"
                " OFFSET(SaveDataSecondaryIntegrity, checksum[0]),\n"
                " OFFSET(SaveDataSecondaryIntegrity, checksum[1]),\n"
                " sizeof(SaveDataTertiaryIntegrity),\n"
                " OFFSET(SaveDataTertiaryIntegrity, checksum),\n"
                " OFFSET(SaveDataTertiaryIntegrity, mask_words[0]),\n"
                " OFFSET(SaveDataTertiaryIntegrity, mask_words[7]),\n"
                " sizeof(SaveDataPrimaryIntegrity) +\n"
                " sizeof(SaveDataSecondaryIntegrity),\n"
                " SAVE_DATA_TERTIARY_OFFSET + sizeof(SaveDataTertiaryIntegrity),\n"
                " sizeof(SaveDataState)\n"
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
                0x380, 0x340, 0x37C, 0x37E,
                0x80, 0x6C, 0x7C, 0x7E,
                0x228, 0x204, 0x208, 0x224, 0x400, 0x628, 0x5E4,
            )
            compiled = data.read_bytes()
            length = len(expected) * 4
            self.assertEqual(compiled[:length], struct.pack("<15I", *expected))
            self.assertEqual(compiled[length:], bytes(len(compiled) - length))


if __name__ == "__main__":
    unittest.main()
