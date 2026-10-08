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
STEM = "src/overlays/french_model_variant/variant417_feedback"
MODELS = {134, 232, 354, 535}


class FrenchModelVariant417FeedbackTests(unittest.TestCase):
    def setUp(self):
        manifest = json.loads((CONFIG / "overlays.json").read_text())["modules"]
        self.modules = [m for m in manifest if m.get("linker_symbols") ==
                        "config/sles_03948/overlays/model_variant417_linker_symbols.txt"]

    def test_all_eight_instances_have_real_c_registration(self):
        self.assertEqual(len(self.modules), 8)
        self.assertEqual({int(m["name"].split("_")[3]) for m in self.modules}, MODELS)
        for module in self.modules:
            with self.subTest(module=module["name"]):
                base = int(module["load_address"], 0)
                source = STEM + ("_slot1" if base == 0x8017B000 else "") + ".c"
                layout = ROOT / module["layout"]
                functions = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())["functions"]
                self.assertIn({"address": f"0x{base + 0x30E4:X}", "size": "0x66C",
                               "profile": "gcc_2_8_1_g0_split", "source": source}, functions)
                self.assertIn(source, [s["source"] for s in c_segments(ROOT, layout)])
                with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                    row = next(r for r in csv.DictReader(handle)
                               if int(r["address"], 0) == base + 0x30E4)
                self.assertEqual((row["status"], row["size"]), ("matching_c", "0x66C"))
                self.assertIn("direct-entry reachable", row["notes"])

    def test_source_keeps_feedback_and_two_cycle_transition(self):
        body = (ROOT / (STEM + ".c")).read_text()
        for expression in ("width = record->size / 128;",
                           "radius = record->size * 100 / 8192;",
                           "if (state->phase >= 5)",
                           "z = -width;",
                           "group->progress += state->step * 48;",
                           "group->progress -= 2048;",
                           "if (state->phase == 4)",
                           "group->cycles++;",
                           "if (group->cycles >= 2)",
                           "state->phase = 5;",
                           "if (depth >= 0 && flag >= 0)"):
            self.assertIn(expression, body)
        self.assertNotRegex(body, r"\b(?:asm|__asm__|register|volatile)\b")
        self.assertEqual((ROOT / (STEM + "_slot1.c")).read_text(),
                         '#include "../../types.h"\n'
                         "#define func_8013E0E4 func_8017E0E4\n"
                         '#include "variant417_feedback.c"\n')

    def test_new_bindings_are_resident_function_starts(self):
        with (CONFIG / "functions.csv").open() as handle:
            resident = {int(row["address"], 0) for row in csv.DictReader(handle)}
        bindings = dict(re.findall(r"^(\w+)\s*=\s*(0x[0-9A-Fa-f]+);",
                                   (CONFIG / "overlays/model_variant417_linker_symbols.txt").read_text(), re.M))
        for name, address in {"GsGetActiveBuff": 0x800852A8, "GetTPage": 0x80082CE8,
                              "SetPolyGT4": 0x80082EE8, "SetSemiTrans": 0x80082DA8,
                              "SetShadeTex": 0x80082DD8}.items():
            self.assertEqual(int(bindings[name], 0), address)
            self.assertIn(address, resident)

    def test_target_compiler_layout_matches_accessed_offsets(self):
        from build_baseline import compile_c, load_compiler_profiles, tool

        if not (ROOT / "tools/toolchains/binutils-2.42/bin/mipsel-none-elf-as").exists():
            self.skipTest("Set up the project toolchain before compiling layout assertions")
        fields = {
            ("Feedback417", "points[1]"): 0x88,
            ("Feedback417", "points[2]"): 0x110,
            ("Feedback417", "inner"): 0x198,
            ("Feedback417", "outer"): 0x19C,
            ("Feedback417", "progress"): 0x1A0,
            ("Feedback417", "cycles"): 0x1A4,
            ("Feedback417SizeRecord", "size"): 0x120,
            ("Feedback417State", "groups"): 0xF28,
            ("Feedback417State", "quad"): 0x2028,
            ("Feedback417State", "origin"): 0x20F0,
            ("Feedback417State", "direction"): 0x20F8,
            ("Feedback417State", "step"): 0x2130,
            ("Feedback417State", "phase"): 0x2170,
        }
        lines = ['#include "../../src/overlays/french_model_variant/variant417_feedback.h"',
                 "typedef char record_size[(sizeof(Feedback417) == 0x1A8) ? 1 : -1];"]
        for index, ((record, field), offset) in enumerate(fields.items()):
            lines.append(f"typedef char offset_{index}[((u32)&(({record} *)0)->{field} == {offset}) ? 1 : -1];")
        lines.append("s32 feedback_layout_checked;")
        with tempfile.TemporaryDirectory(prefix="feedback417-layout-", dir=ROOT / "tmp") as directory:
            directory = Path(directory)
            source = directory / "layout.c"
            source.write_text("\n".join(lines) + "\n")
            compile_c(ROOT, tool(ROOT, "as"),
                      {"source": source.relative_to(ROOT).as_posix(), "profile": "gcc_2_8_1_g0_split",
                       "object": "layout.o"}, load_compiler_profiles(ROOT),
                      object_directory=(directory / "obj").relative_to(ROOT).as_posix(),
                      asm_directory=(directory / "asm").relative_to(ROOT).as_posix())

    def test_terminal_ledger_records_both_slot_sources(self):
        with (ROOT / "notes/overlays/french-model-variant417-feedback-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 2)
        for slot, row in enumerate(rows):
            source = ROOT / (STEM + ("_slot1" if slot else "") + ".c")
            self.assertEqual((row["slot"], row["result"], row["instruction_bytes"],
                              row["different_words"], row["profile"]),
                             (str(slot), "matched", "1644", "0", "gcc_2_8_1_g0_split"))
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual(row["dependency_fingerprint"],
                             hashlib.sha256((ROOT / (STEM + ".h")).read_bytes()).hexdigest())

    def test_full_images_and_sized_c_owners(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required for linked owner checks")
        from elftools.elf.elffile import ELFFile

        for module in self.modules:
            name = module["name"]
            directory = ROOT / "tmp/overlays" / name / "build"
            binary = directory / f"{name}.bin"
            if not binary.exists():
                self.skipTest("Build French overlays before checking linked owners")
            with self.subTest(module=name):
                self.assertEqual(hashlib.sha256(binary.read_bytes()).hexdigest(), module["sha256"])
                base = int(module["load_address"], 0)
                symbol = f"func_{base + 0x30E4:X}"
                segment = next(s for s in c_segments(ROOT, ROOT / module["layout"])
                               if s["source"].startswith(STEM))
                with (directory / segment["object"]).open("rb") as handle:
                    owner, = ELFFile(handle).get_section_by_name(".symtab").get_symbol_by_name(symbol)
                    self.assertEqual((owner["st_value"], owner["st_size"], owner["st_info"]["type"]),
                                     (0, 0x66C, "STT_FUNC"))
                with (directory / f"{name}.elf").open("rb") as handle:
                    table = ELFFile(handle).get_section_by_name(".symtab")
                    linked, = table.get_symbol_by_name(symbol)
                    self.assertEqual((linked["st_value"], linked["st_size"]), (base + 0x30E4, 0x66C))
                    self.assertIsNone(table.get_symbol_by_name(symbol + ".NON_MATCHING"))
