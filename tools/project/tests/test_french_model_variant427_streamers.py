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
FAMILIES = {
    "src/overlays/french_model_variant/variant427_streamers": (0x2DF8, {379}),
}


class FrenchModelVariant427StreamersTests(unittest.TestCase):
    def setUp(self):
        manifest = json.loads((FRENCH / "overlays.json").read_text())["modules"]
        self.modules = {source: [m for m in manifest
                                 if any(c["source"].startswith(source + ".c") or
                                        c["source"].startswith(source + "_slot1.c")
                                        for c in self.functions(m))]
                        for source in FAMILIES}

    @staticmethod
    def functions(module):
        layout = ROOT / module["layout"]
        path = layout.with_name(layout.stem + "_matching_c.json")
        return json.loads(path.read_text())["functions"] if path.exists() else []

    def test_both_images_use_the_exact_helper(self):
        self.assertEqual({source: len(modules) for source, modules in self.modules.items()},
                         {source: 2 * len(models) for source, (_off, models) in FAMILIES.items()})
        for source_stem, (offset, models) in FAMILIES.items():
            modules = self.modules[source_stem]
            self.assertEqual({int(m["name"].split("_")[3]) for m in modules}, models)
            for module in modules:
                with self.subTest(module=module["name"]):
                    base = int(module["load_address"], 0)
                    slot = (base - 0x8013B000) // 0x40000
                    self.assertIn(slot, (0, 1))
                    source = source_stem + ("_slot1" if slot else "") + ".c"
                    self.assertIn({"address": f"0x{base + offset:X}", "profile": "gcc_2_8_1_g0_split",
                                   "size": "0x7C4", "source": source}, self.functions(module))
                    self.assertIn(source, [s["source"] for s in c_segments(ROOT, ROOT / module["layout"])])
                    layout = ROOT / module["layout"]
                    with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                        row = next(r for r in csv.DictReader(handle) if int(r["address"], 0) == base + offset)
                    self.assertEqual((row["status"], row["size"]), ("matching_c", "0x7C4"))
                    self.assertIn("first recovery of this body in any release", row["notes"])

    def test_bindings_are_french_resident_functions(self):
        with (FRENCH / "functions.csv").open() as handle:
            resident = {int(r["address"], 0) for r in csv.DictReader(handle)}
        bindings = dict(re.findall(r"^(\w+)\s*=\s*(0x[0-9A-Fa-f]+);",
                                   (FRENCH / "overlays/model_variant427_linker_symbols.txt").read_text(), re.M))
        self.assertEqual((bindings["RotTransPers"], bindings["ratan2"]), ("0x80087868", "0x80089928"))
        for address in bindings.values():
            self.assertIn(int(address, 0), resident)

    def test_source_is_plain_c_with_the_slot_wrapper(self):
        stem = "src/overlays/french_model_variant/variant427_streamers"
        text = (ROOT / (stem + ".c")).read_text()
        self.assertIn('#include "../../types.h"', text)
        self.assertIn("    reach = 0xC0;", text)
        self.assertNotIn("0x800", text)
        self.assertNotRegex(text, r"\b(?:asm|__asm__|register)\b")
        header = (ROOT / (stem + ".h")).read_text()
        self.assertIn("    u8 unknown_2AC[0x44];\n    s32 otz[17];", header)
        wrapper = (ROOT / (stem + "_slot1.c")).read_text()
        self.assertIn("#define func_8013DDF8 func_8017DDF8", wrapper)

    def test_attempt_ledger_ends_with_the_exact_source(self):
        with (ROOT / "notes/overlays/french-model-variant427-streamers-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        stem = ROOT / "src/overlays/french_model_variant/variant427_streamers"
        self.assertEqual((rows[-1]["result"], rows[-1]["different_words"]), ("matched", "0"))
        self.assertEqual(rows[-1]["fingerprint"], hashlib.sha256(stem.with_suffix(".c").read_bytes()).hexdigest())
        self.assertEqual(rows[-1]["dependency_fingerprint"],
                         hashlib.sha256(stem.with_suffix(".h").read_bytes()).hexdigest())

    def test_built_images_are_exact_with_the_c_owner(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for source_stem, (offset, _models) in FAMILIES.items():
            for module in self.modules[source_stem]:
                name = module["name"]
                directory = ROOT / "tmp/overlays" / name
                binary = directory / f"build/{name}.bin"
                if not binary.exists():
                    self.skipTest("Build French overlays before checking linked owners")
                with self.subTest(module=name):
                    self.assertEqual(hashlib.sha256(binary.read_bytes()).hexdigest(), module["sha256"])
                    base = int(module["load_address"], 0)
                    segment = next(s for s in c_segments(ROOT, ROOT / module["layout"])
                                   if s["source"].startswith(source_stem))
                    obj = directory / "build" / segment["object"]
                    with obj.open("rb") as handle:
                        symbols = ELFFile(handle).get_section_by_name(".symtab")
                        own, = symbols.get_symbol_by_name(f"func_{base + offset:X}")
                        self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                         (0, 0x7C4, "STT_FUNC"))
                    with (directory / f"build/{name}.elf").open("rb") as handle:
                        linked, = ELFFile(handle).get_section_by_name(".symtab").get_symbol_by_name(
                            f"func_{base + offset:X}")
                        self.assertEqual((linked["st_value"], linked["st_size"]), (base + offset, 0x7C4))

if __name__ == "__main__":
    unittest.main()
