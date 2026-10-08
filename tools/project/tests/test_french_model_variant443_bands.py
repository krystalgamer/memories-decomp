import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments

CONFIG = ROOT / "config/sles_03948"
STEM = "src/overlays/french_model_variant/variant443_bands"
MODULES = {"french_model_variant_62_stage9_slot0", "french_model_variant_62_stage10_slot1"}


class FrenchModelVariant443BandsTests(unittest.TestCase):
    def setUp(self):
        manifest = json.loads((CONFIG / "overlays.json").read_text())["modules"]
        self.modules = [module for module in manifest if module["name"] in MODULES]

    def test_registrations_and_resident_bindings(self):
        self.assertEqual({module["name"] for module in self.modules}, MODULES)
        with (CONFIG / "functions.csv").open() as handle:
            resident = {int(row["address"], 0) for row in csv.DictReader(handle)}
        for module in self.modules:
            with self.subTest(module=module["name"]):
                base = int(module["load_address"], 0)
                source = STEM + ("_slot1" if base == 0x8017B000 else "") + ".c"
                layout = ROOT / module["layout"]
                functions = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())["functions"]
                self.assertIn({"address": f"0x{base + 0x3EF0:X}", "size": "0x6B8",
                               "profile": "gcc_2_8_1_g0_split", "source": source}, functions)
                self.assertIn(source, [segment["source"] for segment in c_segments(ROOT, layout)])
                with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                    row = next(row for row in csv.DictReader(handle)
                               if int(row["address"], 0) == base + 0x3EF0)
                self.assertEqual((row["status"], row["size"]), ("matching_c", "0x6B8"))
                bindings = (ROOT / module["linker_symbols"]).read_text()
                self.assertIn("RotTransPers3 = 0x80087898;", bindings)
                for address in re.findall(r"^\w+\s*=\s*(0x[0-9A-Fa-f]+);", bindings, re.M):
                    self.assertIn(int(address, 0), resident)

    def test_source_semantics_and_slot_wrapper(self):
        body = (ROOT / (STEM + ".c")).read_text()
        for expression in ("PSXLONG flag[3][2];",
                           "state->size * (sheet->size / 256) / 1024",
                           "state->size * (sheet->size * 24 / 4096) / 1024",
                           "i < 3", "j < 2", "j < 1",
                           "state->origins[i].vx + state->directions[i].vx * j",
                           "state->screen_y[i], state->screen_x[i]",
                           "if (band->otz[j] >= 0 && flag[i][j] >= 0)",
                           "if (state->size < 1024 && state->phase == 1)",
                           "state->timing->grow_end - state->timing->grow_start"):
            self.assertIn(expression, body)
        self.assertLess(body.index("quad = &state->quad;"), body.index("ot = func_80058F10();"))
        self.assertNotRegex(body, r"\b(?:asm|__asm__|register|volatile)\b")
        self.assertEqual((ROOT / (STEM + "_slot1.c")).read_text(),
                         '#include "../../types.h"\n'
                         "#define func_8013EEF0 func_8017EEF0\n"
                         '#include "variant443_bands.c"\n')

    def test_target_compiler_layout(self):
        from build_baseline import compile_c, load_compiler_profiles, tool

        if not (ROOT / "tools/toolchains/binutils-2.42/bin/mipsel-none-elf-as").exists():
            self.skipTest("Set up the project toolchain before compiling layout assertions")
        fields = {
            ("Band443", "b"): 0x10, ("Band443", "c"): 0x20,
            ("Band443", "sa"): 0x30, ("Band443", "sb"): 0x38,
            ("Band443", "sc"): 0x40, ("Band443", "ca"): 0x48,
            ("Band443", "cb"): 0x50, ("Band443", "otz"): 0x70,
            ("Bands443Timing", "grow_start"): 0x3C, ("Bands443Timing", "grow_end"): 0x40,
            ("Bands443State", "bands"): 0x2900, ("Bands443State", "sheets"): 0x2A68,
            ("Bands443State", "quad"): 0x33D0, ("Bands443State", "origins"): 0x3570,
            ("Bands443State", "directions"): 0x35C8, ("Bands443State", "screen_x"): 0x35FC,
            ("Bands443State", "screen_y"): 0x3602, ("Bands443State", "frame"): 0x3620,
            ("Bands443State", "time"): 0x3624, ("Bands443State", "timing"): 0x3634,
            ("Bands443State", "size"): 0x3678, ("Bands443State", "phase"): 0x3698,
        }
        lines = ['#include "../../src/overlays/french_model_variant/variant443_bands.h"',
                 "typedef char record_size[(sizeof(Band443) == 0x78) ? 1 : -1];"]
        for index, ((record, field), offset) in enumerate(fields.items()):
            lines.append(f"typedef char offset_{index}[((u32)&(({record} *)0)->{field} == {offset}) ? 1 : -1];")
        lines.append("s32 bands_layout_checked;")
        with tempfile.TemporaryDirectory(prefix="bands443-layout-", dir=ROOT / "tmp") as directory:
            directory = Path(directory)
            source = directory / "layout.c"
            source.write_text("\n".join(lines) + "\n")
            compile_c(ROOT, tool(ROOT, "as"),
                      {"source": source.relative_to(ROOT).as_posix(), "profile": "gcc_2_8_1_g0_split",
                       "object": "layout.o"}, load_compiler_profiles(ROOT),
                      object_directory=(directory / "obj").relative_to(ROOT).as_posix(),
                      asm_directory=(directory / "asm").relative_to(ROOT).as_posix())

    def test_terminal_fingerprints(self):
        with (ROOT / "notes/overlays/french-model-variant443-bands-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 2)
        for slot, row in enumerate(rows):
            source = ROOT / (STEM + ("_slot1" if slot else "") + ".c")
            self.assertEqual((row["slot"], row["result"], row["instruction_bytes"],
                              row["different_words"], row["profile"]),
                             (str(slot), "matched", "1720", "0", "gcc_2_8_1_g0_split"))
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual(row["dependency_fingerprint"],
                             hashlib.sha256((ROOT / (STEM + ".h")).read_bytes()).hexdigest())

    def test_complete_images_and_sized_c_owners(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required for linked owner checks")
        from elftools.elf.elffile import ELFFile

        for module in self.modules:
            name = module["name"]
            build = ROOT / "tmp/overlays" / name / "build"
            binary = build / f"{name}.bin"
            if not binary.exists():
                self.skipTest("Build French overlays before checking linked owners")
            with self.subTest(module=name):
                self.assertEqual(hashlib.sha256(binary.read_bytes()).hexdigest(), module["sha256"])
                base = int(module["load_address"], 0)
                symbol = f"func_{base + 0x3EF0:X}"
                segment = next(segment for segment in c_segments(ROOT, ROOT / module["layout"])
                               if segment["source"].startswith(STEM))
                with (build / segment["object"]).open("rb") as handle:
                    owner, = ELFFile(handle).get_section_by_name(".symtab").get_symbol_by_name(symbol)
                    self.assertEqual((owner["st_value"], owner["st_size"], owner["st_info"]["type"]),
                                     (0, 0x6B8, "STT_FUNC"))
                with (build / f"{name}.elf").open("rb") as handle:
                    table = ELFFile(handle).get_section_by_name(".symtab")
                    linked, = table.get_symbol_by_name(symbol)
                    self.assertEqual((linked["st_value"], linked["st_size"]), (base + 0x3EF0, 0x6B8))
                    self.assertIsNone(table.get_symbol_by_name(symbol + ".NON_MATCHING"))
