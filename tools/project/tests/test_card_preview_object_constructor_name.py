"""Guard the paired constructor's semantic bindings without running the pool."""

import csv
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile
import unittest

from tools.project.tests.test_card_preview_resource_request_name import (
    ASSEMBLER, LINKER, NM, _absolute_binding_pattern,
)


ROOT = Path(__file__).resolve().parents[3]
NAME = "CardPreview_CreateResourceObjects"
REGIONS = (
    ("slus_01411", 0x80029164, 0x7C, 0x348, ""),
    ("slpm_86398", 0x80028F44, 0x7C, 0x348, "japanese/"),
    ("sles_03947", 0x80029198, 0x78, 0x36C, "european/"),
    ("sles_03948", 0x800292DC, 0x78, 0x36C, "european/"),
    ("sles_03949", 0x800292DC, 0x78, 0x36C, "european/"),
    ("sles_03950", 0x800292DC, 0x78, 0x36C, "european/"),
    ("sles_03951", 0x800292DC, 0x78, 0x36C, "european/"),
)


class CardPreviewObjectConstructorNameTests(unittest.TestCase):
    def test_password_clients_select_correct_named_and_legacy_imports(self):
        for config, start, request_size, _, _ in REGIONS:
            with self.subTest(config=config):
                directory = ROOT / "config" / config
                address = start + request_size
                imports = directory / "overlays/password_linker_symbols.txt"
                self.assertRegex(imports.read_text(), rf"(?m)^{NAME} = 0x{address:08X};$")
                if config != "slus_01411":
                    self.assertRegex(imports.read_text(), rf"(?m)^func_800291E0 = 0x{address:08X};$")
                modules = json.loads((directory / "overlays.json").read_text())["modules"]
                clients = [module for module in modules if "password" in module["name"]]
                self.assertEqual(len(clients), 1 if config in ("slus_01411", "slpm_86398") else 2)
                for module in clients:
                    self.assertEqual(module["linker_symbols"], str(imports.relative_to(ROOT)))

    @unittest.skipUnless(all(path.is_file() for path in (ASSEMBLER, LINKER, NM)),
                         "requires project MIPS binutils")
    def test_actual_constructor_import_link_and_missing_binding_negative(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="preview-object-import-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            environment = dict(os.environ, TMPDIR=str(directory))
            source = directory / "probe.s"
            source.write_text(f".set noreorder\n.text\n.globl probe\nprobe:\nj {NAME}\nnop\n")
            obj = directory / "probe.o"
            result = subprocess.run([str(ASSEMBLER), "-EL", "-o", str(obj), str(source)],
                                    cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60)
            self.assertEqual(result.returncode, 0, result.stderr)
            layout = directory / "layout.ld"
            layout.write_text("gPassword_ModuleState = 0x8016D400;\n"
                              "SECTIONS { . = 0x80168000; .text : { *(.text) } }\n")
            for config, start, request_size, _, _ in REGIONS:
                imports = ROOT / "config" / config / "overlays/password_linker_symbols.txt"
                for missing in (False, True):
                    with self.subTest(config=config, missing=missing):
                        selected = imports
                        if missing:
                            selected = directory / "missing.txt"
                            selected.write_text(re.sub(rf"(?m)^{NAME}[^\n]*\n", "", imports.read_text()))
                        binary = directory / "probe.elf"
                        result = subprocess.run(
                            [str(LINKER), "-EL", "-T", str(layout), "-T", str(selected),
                             "-o", str(binary), str(obj)], cwd=ROOT, env=environment,
                            text=True, capture_output=True, timeout=60,
                        )
                        if missing:
                            self.assertNotEqual(result.returncode, 0)
                            self.assertIn("undefined reference", result.stderr)
                            self.assertIn(NAME, result.stderr)
                        else:
                            self.assertEqual(result.returncode, 0, result.stderr)
                            symbols = subprocess.check_output(
                                [str(NM), str(binary)], cwd=ROOT, env=environment, text=True, timeout=60,
                            )
                            self.assertRegex(symbols, _absolute_binding_pattern(start + request_size, NAME))

    def test_seven_region_constructor_bindings_preserve_complete_group(self):
        for config, start, request_size, size, tier in REGIONS:
            with self.subTest(config=config):
                directory = ROOT / "config" / config
                address = start + request_size
                with (directory / "functions.csv").open(newline="") as handle:
                    rows = list(csv.DictReader(handle))
                entry, = [row for row in rows if int(row["address"], 0) == address]
                self.assertEqual((entry["name"], entry["status"], entry["module"]),
                                 (NAME, "matching_c", "game"))
                self.assertEqual(int(entry["size"], 0), size)
                matching = json.loads((directory / "matching_c.json").read_text())["functions"]
                owner = "src/game/" + tier + "duel_effect_resource_setup.c"
                # The request/constructor pair opens the resource-setup unit;
                # the resource-object helpers follow it.
                group = [row for row in matching if row["source"] == owner]
                self.assertEqual(
                    [(int(row["address"], 0), int(row["size"], 0), row["profile"]) for row in group[:2]],
                    [(start, request_size, "gcc_2_8_1_g8_split"),
                     (address, size, "gcc_2_8_1_g8_split")],
                )
                self.assertRegex((directory / "symbols.txt").read_text(),
                                 rf"(?m)^{NAME}\s*=\s*0x{address:08X};.*type:func")
                self.assertNotRegex((directory / "symbols.txt").read_text(),
                                    r"(?m)^func_800291E0\s*=")


    def test_canonical_objects_header_and_confirmed_evidence_row(self):
        self.assertFalse((ROOT / "src/game/func_800291E0.h").exists())
        with (ROOT / "notes/semantic-symbol-map.csv").open(newline="") as handle:
            entry, = [row for row in csv.DictReader(handle) if row["name"] == NAME]
        self.assertEqual((entry["kind"], entry["address"], entry["confidence"]),
                         ("function", "0x800291E0", "confirmed"))


if __name__ == "__main__":
    unittest.main()
