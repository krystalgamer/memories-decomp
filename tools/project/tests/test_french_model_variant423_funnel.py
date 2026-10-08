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
STEM = "src/overlays/french_model_variant/variant423_funnel"
MODULES = {"french_model_variant_385_stage7_slot0", "french_model_variant_385_stage8_slot1"}


class FrenchModelVariant423FunnelTests(unittest.TestCase):
    def setUp(self):
        manifest = json.loads((CONFIG / "overlays.json").read_text())["modules"]
        self.modules = [m for m in manifest if m["name"] in MODULES]

    def test_both_images_have_exact_c_registration(self):
        self.assertEqual({m["name"] for m in self.modules}, MODULES)
        for module in self.modules:
            base = int(module["load_address"], 0)
            source = STEM + ("_slot1" if base == 0x8017B000 else "") + ".c"
            layout = ROOT / module["layout"]
            functions = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())["functions"]
            with self.subTest(module=module["name"]):
                self.assertIn({"address": f"0x{base + 0x3214:X}", "size": "0x5C0",
                               "profile": "gcc_2_8_1_g0_split", "source": source}, functions)
                self.assertIn(source, [segment["source"] for segment in c_segments(ROOT, layout)])
                with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                    row = next(row for row in csv.DictReader(handle)
                               if int(row["address"], 0) == base + 0x3214)
                self.assertEqual((row["status"], row["size"]), ("matching_c", "0x5C0"))
                self.assertIn("direct-entry reachable", row["notes"])

    def test_source_preserves_four_records_and_signed_attenuation(self):
        body = (ROOT / (STEM + ".c")).read_text()
        for expression in ("i < 4", "radius = rsin(1300) * 384 >> 12;",
                           "height = -(bias + 512);", "reach = radius + (bias + 160);",
                           "size = size_y + extra;", "scale.vy = size_y;",
                           "r -= r * funnel->size / 8192;",
                           "outer_b -= outer_b * funnel->size / 8192;",
                           "funnel->size += state->step << 7;",
                           "funnel->size -= 8192;", "state->spin += state->step * 80;",
                           "if (depth >= 0 && flag >= 0)"):
            self.assertIn(expression, body)
        self.assertNotRegex(body, r"\b(?:asm|__asm__|register|volatile)\b")
        self.assertEqual((ROOT / (STEM + "_slot1.c")).read_text(),
                         '#include "../../types.h"\n'
                         "#define func_8013E214 func_8017E214\n"
                         '#include "variant423_funnel.c"\n')

    def test_target_compiler_layout(self):
        from build_baseline import compile_c, load_compiler_profiles, tool

        if not (ROOT / "tools/toolchains/binutils-2.42/bin/mipsel-none-elf-as").exists():
            self.skipTest("Set up the project toolchain before compiling layout assertions")
        fields = {("Funnel423", "outer"): 0x88, ("Funnel423", "size"): 0x110,
                  ("Funnel423State", "sheets"): 0xE1C,
                  ("Funnel423State", "funnels"): 0x15B4,
                  ("Funnel423State", "quad"): 0x1B60,
                  ("Funnel423State", "position"): 0x1D58,
                  ("Funnel423State", "direction"): 0x1D60,
                  ("Funnel423State", "frame"): 0x1D8C,
                  ("Funnel423State", "step"): 0x1D98,
                  ("Funnel423State", "size"): 0x1DB8,
                  ("Funnel423State", "fade"): 0x1DC8,
                  ("Funnel423State", "inner_color"): 0x1DF0,
                  ("Funnel423State", "outer_color"): 0x1DF4,
                  ("Funnel423State", "spin"): 0x1DF8,
                  ("Funnel423State", "phase"): 0x1E0C}
        lines = ['#include "../../src/overlays/french_model_variant/variant423_funnel.h"',
                 "typedef char record_size[(sizeof(Funnel423) == 0x118) ? 1 : -1];"]
        for index, ((record, field), offset) in enumerate(fields.items()):
            lines.append(f"typedef char offset_{index}[((u32)&(({record} *)0)->{field} == {offset}) ? 1 : -1];")
        lines.append("s32 funnel_layout_checked;")
        with tempfile.TemporaryDirectory(prefix="funnel423-layout-", dir=ROOT / "tmp") as directory:
            directory = Path(directory)
            source = directory / "layout.c"
            source.write_text("\n".join(lines) + "\n")
            compile_c(ROOT, tool(ROOT, "as"),
                      {"source": source.relative_to(ROOT).as_posix(), "profile": "gcc_2_8_1_g0_split",
                       "object": "layout.o"}, load_compiler_profiles(ROOT),
                      object_directory=(directory / "obj").relative_to(ROOT).as_posix(),
                      asm_directory=(directory / "asm").relative_to(ROOT).as_posix())

    def test_terminal_ledger_fingerprints(self):
        with (ROOT / "notes/overlays/french-model-variant423-funnel-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 2)
        for slot, row in enumerate(rows):
            source = ROOT / (STEM + ("_slot1" if slot else "") + ".c")
            self.assertEqual((row["slot"], row["result"], row["instruction_bytes"],
                              row["different_words"], row["profile"]),
                             (str(slot), "matched", "1472", "0", "gcc_2_8_1_g0_split"))
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual(row["dependency_fingerprint"],
                             hashlib.sha256((ROOT / (STEM + ".h")).read_bytes()).hexdigest())

    def test_full_images_and_sized_c_owners(self):
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
                symbol = f"func_{base + 0x3214:X}"
                segment = next(segment for segment in c_segments(ROOT, ROOT / module["layout"])
                               if segment["source"].startswith(STEM))
                with (build / segment["object"]).open("rb") as handle:
                    owner, = ELFFile(handle).get_section_by_name(".symtab").get_symbol_by_name(symbol)
                    self.assertEqual((owner["st_value"], owner["st_size"], owner["st_info"]["type"]),
                                     (0, 0x5C0, "STT_FUNC"))
                with (build / f"{name}.elf").open("rb") as handle:
                    table = ELFFile(handle).get_section_by_name(".symtab")
                    linked, = table.get_symbol_by_name(symbol)
                    self.assertEqual((linked["st_value"], linked["st_size"]), (base + 0x3214, 0x5C0))
                    self.assertIsNone(table.get_symbol_by_name(symbol + ".NON_MATCHING"))
