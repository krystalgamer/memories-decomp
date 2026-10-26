"""Guard the named requester's grouped bindings and unchanged descriptor API."""

import csv
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
NAME = "CardPreview_RequestResourceTransfer"
REGIONS = (
    ("slus_01411", 0x80029164, 0x7C, 0x348, ""),
    ("slpm_86398", 0x80028F44, 0x7C, 0x348, "japanese/"),
    ("sles_03947", 0x80029198, 0x78, 0x36C, "european/"),
    ("sles_03948", 0x800292DC, 0x78, 0x36C, "european/"),
    ("sles_03949", 0x800292DC, 0x78, 0x36C, "european/"),
    ("sles_03950", 0x800292DC, 0x78, 0x36C, "european/"),
    ("sles_03951", 0x800292DC, 0x78, 0x36C, "european/"),
)
COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
BINUTILS = ROOT / "tools/toolchains/binutils-2.42/bin"
ASSEMBLER = BINUTILS / "mipsel-none-elf-as"
LINKER = BINUTILS / "mipsel-none-elf-ld"
NM = BINUTILS / "mipsel-none-elf-nm"


def _absolute_binding_pattern(address, name):
    return rf"(?m)^(?:ffffffff)?{address:08x}\s+A\s+{name}$"


class CardPreviewResourceRequestNameTests(unittest.TestCase):
    def test_nm_formats_preserve_exact_address_and_absolute_symbol_type(self):
        for config, address, _, _, _ in REGIONS:
            with self.subTest(config=config):
                pattern = _absolute_binding_pattern(address, NAME)
                for prefix in ("", "ffffffff"):
                    self.assertRegex(f"{prefix}{address:08x} A {NAME}\n", pattern)
                for invalid in (
                    f"{address + 4:08x} A {NAME}",
                    f"ffffffff{address + 4:08x} A {NAME}",
                    f"fffffffe{address:08x} A {NAME}",
                    f"00000000{address:08x} A {NAME}",
                    f"ffffffff{address:08x} T {NAME}",
                    f"{address:08x} a {NAME}",
                    f"{address:08x} A {NAME}_wrong",
                ):
                    self.assertNotRegex(invalid + "\n", pattern)

    def test_password_manifests_select_the_named_resident_import(self):
        for config, address, _, _, _ in REGIONS:
            with self.subTest(config=config):
                directory = ROOT / "config" / config
                path = directory / "overlays/password_linker_symbols.txt"
                self.assertRegex(path.read_text(),
                                 rf"(?m)^{NAME}\s*=\s*0x{address:08X};$")
                if config != "slus_01411":
                    self.assertRegex(path.read_text(),
                                     rf"(?m)^func_80029164\s*=\s*0x{address:08X};$")
                modules = json.loads((directory / "overlays.json").read_text())["modules"]
                clients = [module for module in modules if "password" in module["name"]]
                self.assertEqual(len(clients), 1 if config in ("slus_01411", "slpm_86398") else 2)
                for module in clients:
                    self.assertEqual(module["linker_symbols"], str(path.relative_to(ROOT)))

    @unittest.skipUnless(all(path.is_file() for path in (ASSEMBLER, LINKER, NM)),
                         "requires project MIPS binutils")
    def test_actual_import_scripts_link_and_missing_binding_is_rejected(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="preview-request-import-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            source = directory / "probe.s"
            source.write_text(f".set noreorder\n.text\n.globl probe\nprobe:\nj {NAME}\nnop\n")
            obj = directory / "probe.o"
            environment = dict(os.environ, TMPDIR=str(directory))
            result = subprocess.run([str(ASSEMBLER), "-EL", "-o", str(obj), str(source)],
                                    cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60)
            self.assertEqual(result.returncode, 0, result.stderr)
            layout = directory / "layout.ld"
            layout.write_text("gPassword_ModuleState = 0x8016D400;\n"
                              "SECTIONS { . = 0x80168000; .text : { *(.text) } }\n")
            for config, address, _, _, _ in REGIONS:
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
                            self.assertRegex(symbols, _absolute_binding_pattern(address, NAME))

    def test_seven_region_names_preserve_grouped_owner_and_profile(self):
        for config, address, size, constructor_size, tier in REGIONS:
            with self.subTest(config=config):
                directory = ROOT / "config" / config
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
                    [(address, size, "gcc_2_8_1_g8_split"),
                     (address + size, constructor_size, "gcc_2_8_1_g8_split")],
                )
                self.assertTrue((ROOT / owner).is_file())
                self.assertRegex((directory / "symbols.txt").read_text(),
                                 rf"(?m)^{NAME}\s*=\s*0x{address:08X};.*type:func")
                self.assertNotRegex((directory / "symbols.txt").read_text(),
                                    r"(?m)^func_80029164\s*=")
                if config != "slus_01411":
                    self.assertIn(owner.removeprefix("src/").removesuffix(".c"),
                                  (directory / "split.yaml").read_text())

    def test_registered_callback_and_existing_regional_disc_math(self):
        japanese = (ROOT / "src/game/japanese/duel_effect_resource_setup.c").read_text()
        european = (ROOT / "src/game/european/duel_effect_resource_setup.c").read_text()
        for wrapper in (japanese, european):
            self.assertIn('#include "../duel_effect_resource_setup.c"', wrapper)
        with (ROOT / "notes/semantic-symbol-map.csv").open(newline="") as handle:
            entries = [row for row in csv.DictReader(handle) if row["name"] == NAME]
        entry, = entries
        self.assertEqual((entry["kind"], entry["address"], entry["confidence"]),
                         ("function", "0x80029164", "confirmed"))

    @unittest.skipUnless(COMPILER.is_file(), "requires supported GCC 2.8.1")
    def test_descriptor_return_and_full_word_argument_contract(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="preview-request-name-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for result_type, parameters, accepted in (
                ("FileTransferDescriptor *", "s32, s32", True),
                ("void *", "s32, s32", False),
                ("u8 *", "s32, s32", False),
                ("FileTransferDescriptor *", "s32, u16", False),
                ("FileTransferDescriptor *", "s32", False),
            ):
                with self.subTest(result_type=result_type, parameters=parameters):
                    probe = directory / "probe.c"
                    probe.write_text(
                        '#include "../../src/game/duel_effect_resource_setup.h"\n'
                        f"{result_type} (*checked)({parameters}) = {NAME};\n"
                    )
                    result = subprocess.run(
                        [str(COMPILER), "-S", "-O2", "-G8", "-Werror", "-o",
                         str(directory / "probe.s"), str(probe)],
                        cwd=ROOT, env=dict(os.environ, TMPDIR=str(directory)),
                        text=True, capture_output=True, timeout=60,
                    )
                    if accepted:
                        self.assertEqual(result.returncode, 0, result.stderr)
                    else:
                        self.assertNotEqual(result.returncode, 0, result.stderr)
                        self.assertIn("incompatible pointer type", result.stderr)


if __name__ == "__main__":
    unittest.main()
