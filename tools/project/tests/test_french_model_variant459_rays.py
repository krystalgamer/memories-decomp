import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments

FRENCH = ROOT / "config/sles_03948"
SOURCE = "src/overlays/spanish_model_variant/variant459_rays"
MODELS = (47, 231, 238, 411, 417, 620)


class FrenchModelVariant459RaysTests(unittest.TestCase):
    def setUp(self):
        manifest = json.loads((FRENCH / "overlays.json").read_text())["modules"]
        self.modules = [m for m in manifest
                        if any(c["source"].startswith(SOURCE) for c in self.functions(m))]

    @staticmethod
    def functions(module):
        layout = ROOT / module["layout"]
        path = layout.with_name(layout.stem + "_matching_c.json")
        return json.loads(path.read_text())["functions"] if path.exists() else []

    def test_all_eleven_layouts_use_the_exact_helper(self):
        self.assertEqual(len(self.modules), 11)
        self.assertEqual({int(m["name"].split("_")[3]) for m in self.modules}, set(MODELS))
        for module in self.modules:
            with self.subTest(module=module["name"]):
                base = int(module["load_address"], 0)
                slot = (base - 0x8013B000) // 0x40000
                self.assertIn(slot, (0, 1))
                source = SOURCE + ("_slot1" if slot else "") + ".c"
                self.assertIn({"address": f"0x{base + 0x1FB8:X}", "profile": "gcc_2_8_1_g0_split",
                               "size": "0x64C", "source": source}, self.functions(module))
                self.assertIn(source, [s["source"] for s in c_segments(ROOT, ROOT / module["layout"])])
                layout = ROOT / module["layout"]
                with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                    row = next(r for r in csv.DictReader(handle) if int(r["address"], 0) == base + 0x1FB8)
                self.assertEqual((row["status"], row["size"]), ("matching_c", "0x64C"))

    def test_bindings_are_french_resident_functions(self):
        with (FRENCH / "functions.csv").open() as handle:
            resident = {int(r["address"], 0) for r in csv.DictReader(handle)}
        bindings = dict(re.findall(r"^(\w+)\s*=\s*(0x[0-9A-Fa-f]+);",
                                   (FRENCH / "overlays/model_variant459_linker_symbols.txt").read_text(), re.M))
        for name, address in (("rcos", "0x800866F8"), ("rsin", "0x80086628"), ("RotTransPers", "0x80087868")):
            self.assertEqual(bindings[name], address)
        for address in bindings.values():
            self.assertIn(int(address, 0), resident)

    def test_source_is_plain_c_with_the_slot_wrapper(self):
        text = (ROOT / (SOURCE + ".c")).read_text()
        self.assertIn('#include "../../types.h"', text)
        self.assertIn("radius = 150;", text)
        self.assertIn("width = 16;", text)
        self.assertNotRegex(text, r"\b(?:asm|__asm__|register)\b")
        wrapper = (ROOT / (SOURCE + "_slot1.c")).read_text()
        self.assertIn("#define func_8013CFB8 func_8017CFB8", wrapper)

    def test_attempt_ledger_ends_with_the_exact_source(self):
        with (ROOT / "notes/overlays/french-model-variant459-rays-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(rows[-1]["result"], "matched")
        self.assertEqual(rows[-1]["different_words"], "0")
        self.assertEqual(rows[-1]["fingerprint"],
                         hashlib.sha256((ROOT / (SOURCE + ".c")).read_bytes()).hexdigest())
        self.assertEqual(rows[-1]["dependency_fingerprint"],
                         hashlib.sha256((ROOT / (SOURCE + ".h")).read_bytes()).hexdigest())

    def test_built_images_are_exact_with_the_c_owner(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module in self.modules:
            name = module["name"]
            directory = ROOT / "tmp/overlays" / name
            binary = directory / f"build/{name}.bin"
            if not binary.exists():
                self.skipTest("Build French overlays before checking linked owners")
            with self.subTest(module=name):
                self.assertEqual(hashlib.sha256(binary.read_bytes()).hexdigest(), module["sha256"])
                base = int(module["load_address"], 0)
                segment = next(s for s in c_segments(ROOT, ROOT / module["layout"])
                               if s["source"].startswith(SOURCE))
                obj = directory / "build" / segment["object"]
                with obj.open("rb") as handle:
                    symbols = ELFFile(handle).get_section_by_name(".symtab")
                    own, = symbols.get_symbol_by_name(f"func_{base + 0x1FB8:X}")
                    self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                     (0, 0x64C, "STT_FUNC"))
                with (directory / f"build/{name}.elf").open("rb") as handle:
                    linked, = ELFFile(handle).get_section_by_name(".symtab").get_symbol_by_name(
                        f"func_{base + 0x1FB8:X}")
                    self.assertEqual((linked["st_value"], linked["st_size"]), (base + 0x1FB8, 0x64C))


if __name__ == "__main__":
    unittest.main()
