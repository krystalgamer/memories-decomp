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
    "src/overlays/french_model_variant/variant423_streamers": (0x2A50, {385}),
    "src/overlays/french_model_variant/variant473_streamers": (0x30A8, {385}),
}


class FrenchModelVariant423StreamersTests(unittest.TestCase):
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

    def test_all_four_images_use_the_exact_helper(self):
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
        for model in (423, 473):
            bindings = dict(re.findall(r"^(\w+)\s*=\s*(0x[0-9A-Fa-f]+);",
                                       (FRENCH / f"overlays/model_variant{model}_linker_symbols.txt").read_text(),
                                       re.M))
            self.assertEqual(bindings["RotTransPers"], "0x80087868")
            for address in bindings.values():
                self.assertIn(int(address, 0), resident)

    def test_sources_share_one_body_with_model_bases(self):
        stem = "src/overlays/french_model_variant/"
        text = (ROOT / (stem + "variant423_streamers.c")).read_text()
        self.assertIn('#include "../../types.h"', text)
        self.assertIn("    reach = 0xC0;", text)
        self.assertNotIn("0x800", text)
        self.assertNotRegex(text, r"\b(?:asm|__asm__|register)\b")
        wrappers = {
            "variant423_streamers_slot1.c": ("#define func_8013DA50 func_8017DA50",),
            "variant473_streamers.c": ("#define STREAMERS_FIELDS 0x3BC4", "#define func_8013DA50 func_8013E0A8"),
            "variant473_streamers_slot1.c": ("#define STREAMERS_LIST 0x2D14", "#define func_8013DA50 func_8017E0A8"),
        }
        for name, lines in wrappers.items():
            wrapper = (ROOT / (stem + name)).read_text()
            self.assertIn('#include "variant423_streamers.c"', wrapper)
            for line in lines:
                self.assertIn(line, wrapper)

    def test_attempt_ledger_ends_with_the_exact_sources(self):
        with (ROOT / "notes/overlays/french-model-variant423-streamers-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        stem = ROOT / "src/overlays/french_model_variant"
        header = hashlib.sha256((ROOT / "src/overlays/model_variant/variant458_streamers.h").read_bytes()).hexdigest()
        self.assertEqual([r["result"] for r in rows[-2:]], ["matched", "matched"])
        self.assertEqual(rows[-2]["fingerprint"],
                         hashlib.sha256((stem / "variant423_streamers.c").read_bytes()).hexdigest())
        self.assertEqual(rows[-1]["fingerprint"],
                         hashlib.sha256((stem / "variant473_streamers.c").read_bytes()).hexdigest())
        self.assertEqual({r["dependency_fingerprint"] for r in rows}, {header})

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
