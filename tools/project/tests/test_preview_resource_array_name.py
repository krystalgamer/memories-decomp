"""Check preview records and actual Password data imports without building overlays."""

import csv
import json
import os
from pathlib import Path
import re
import struct
import subprocess
import tempfile
import unittest

from tools.project.tests.test_card_preview_resource_request_name import (
    ASSEMBLER, BINUTILS, LINKER, NM, _absolute_binding_pattern,
)


ROOT = Path(__file__).resolve().parents[3]
NAME = "gCardPreview_Resources"
REGIONS = (
    ("slus_01411", 0x800EA0E8, "c_symbols.ld"),
    ("slpm_86398", 0x800E9FC8, None),
    ("sles_03947", 0x800EB050, None),
    ("sles_03948", 0x800EB470, "link_symbols.ld"),
    ("sles_03949", 0x800EB470, "link_symbols.ld"),
    ("sles_03950", 0x800EB470, "link_symbols.ld"),
    ("sles_03951", 0x800EB470, "link_symbols.ld"),
)
GCC = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
OBJCOPY = BINUTILS / "mipsel-none-elf-objcopy"


class PreviewResourceArrayNameTests(unittest.TestCase):
    def test_seven_main_and_password_bindings_preserve_aliases_and_manifests(self):
        for config, address, linker in REGIONS:
            with self.subTest(config=config):
                directory = ROOT / "config" / config
                self.assertRegex((directory / "symbols.txt").read_text(),
                                 rf"(?m)^{NAME} = 0x{address:08X};$")
                if linker:
                    text = (directory / linker).read_text()
                    self.assertIn(f"D_800EA0E8 = 0x{address:08X};", text)
                    self.assertIn(f"{NAME} = 0x{address:08X};", text)
                imports = directory / "overlays/password_linker_symbols.txt"
                self.assertIn(f"{NAME} = 0x{address:08X};", imports.read_text())
                if config != "slus_01411":
                    self.assertIn(f"D_800EA0E8 = 0x{address:08X};", imports.read_text())
                modules = json.loads((directory / "overlays.json").read_text())["modules"]
                clients = [module for module in modules if "password" in module["name"]]
                self.assertEqual(len(clients), 1 if config in ("slus_01411", "slpm_86398") else 2)
                for module in clients:
                    self.assertEqual(module["linker_symbols"], str(imports.relative_to(ROOT)))
        french = (ROOT / "config/sles_03948/symbols.txt").read_text()
        self.assertIn("D_800EA0E8_French = 0x800EA0E8;", french)
        self.assertNotIn(f"{NAME} = 0x800EA0E8;", french)
        with (ROOT / "notes/semantic-symbol-map.csv").open(newline="") as handle:
            entry, = [row for row in csv.DictReader(handle) if row["name"] == NAME]
        self.assertEqual((entry["kind"], entry["address"], entry["confidence"]),
                         ("global", "0x800EA0E8", "confirmed"))

    @unittest.skipUnless(all(path.is_file() for path in (ASSEMBLER, LINKER, NM, OBJCOPY)),
                         "requires project MIPS binutils")
    def test_actual_import_data_relocations_and_missing_binding_negatives(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="preview-array-import-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            environment = dict(os.environ, TMPDIR=str(directory))
            source = directory / "probe.s"
            source.write_text(f".data\n.globl import_probe\nimport_probe:\n.word {NAME}\n")
            obj = directory / "probe.o"
            result = subprocess.run([str(ASSEMBLER), "-EL", "-o", str(obj), str(source)],
                                    cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60)
            self.assertEqual(result.returncode, 0, result.stderr)
            layout = directory / "layout.ld"
            layout.write_text("gPassword_ModuleState = 0x8016D400;\n"
                              "SECTIONS { . = 0x80168000; .data : { *(.data) } }\n")
            for config, address, _ in REGIONS:
                imports = ROOT / "config" / config / "overlays/password_linker_symbols.txt"
                for missing in (False, True):
                    with self.subTest(config=config, missing=missing):
                        selected = imports
                        if missing:
                            selected = directory / "missing.txt"
                            selected.write_text(re.sub(rf"(?m)^{NAME}[^\n]*\n", "", imports.read_text()))
                        elf = directory / "probe.elf"
                        result = subprocess.run(
                            [str(LINKER), "-EL", "-T", str(layout), "-T", str(selected),
                             "-o", str(elf), str(obj)], cwd=ROOT, env=environment,
                            text=True, capture_output=True, timeout=60,
                        )
                        if missing:
                            self.assertNotEqual(result.returncode, 0)
                            self.assertIn("undefined reference", result.stderr)
                            self.assertIn(NAME, result.stderr)
                        else:
                            self.assertEqual(result.returncode, 0, result.stderr)
                            symbols = subprocess.check_output(
                                [str(NM), str(elf)], cwd=ROOT, env=environment, text=True, timeout=60,
                            )
                            self.assertRegex(symbols, _absolute_binding_pattern(address, NAME))
                            binary = directory / "data.bin"
                            result = subprocess.run(
                                [str(OBJCOPY), "-O", "binary", "--only-section=.data",
                                 str(elf), str(binary)], cwd=ROOT, env=environment,
                                text=True, capture_output=True, timeout=60,
                            )
                            self.assertEqual(result.returncode, 0, result.stderr)
                            data = binary.read_bytes()
                            self.assertGreaterEqual(len(data), 4)
                            self.assertEqual(data, struct.pack("<I", address) + bytes(len(data) - 4))

    @unittest.skipUnless(GCC.is_file(), "requires supported GCC 2.8.1")
    def test_actual_record_element_and_incomplete_array_contract(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        header = (ROOT / "src/game/duel_effect_resource_record.h").read_text()
        self.assertIn(f"extern DuelEffectResourceRecord {NAME}[];", header)
        with tempfile.TemporaryDirectory(prefix="preview-array-owner-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for pointer, extent, accepted in (
                ("DuelEffectResourceRecord", False, True),
                ("struct DisplayObject", False, False),
                ("DuelEffectResourceRecord", True, False),
            ):
                with self.subTest(pointer=pointer, extent=extent):
                    source = directory / "probe.c"
                    source.write_text(
                        '#include "../../src/game/duel_effect_resource_record.h"\n'
                        f"typedef char element_size[sizeof({NAME}[0]) == 0x40 ? 1 : -1];\n"
                        f"{pointer} *G32 checked = {NAME};\n"
                        + (f"typedef char forbidden_bound[sizeof({NAME})];\n" if extent else "")
                    )
                    result = subprocess.run(
                        [str(GCC), "-S", "-O2", "-G8", "-Werror", "-o",
                         str(directory / "probe.s"), str(source)],
                        cwd=ROOT, env=dict(os.environ, TMPDIR=str(directory)),
                        text=True, capture_output=True, timeout=60,
                    )
                    if accepted:
                        self.assertEqual(result.returncode, 0, result.stderr)
                    else:
                        self.assertNotEqual(result.returncode, 0, result.stderr)
                        self.assertIn("incomplete" if extent else "incompatible pointer type", result.stderr)


if __name__ == "__main__":
    unittest.main()
