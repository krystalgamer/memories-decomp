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
import register_model_images as registration

FRENCH = ROOT / "config/sles_03948"
SPANISH = ROOT / "config/sles_03951"


def rename(text):
    return text.replace("spanish_model_variant_", "french_model_variant_").replace(
        "config/sles_03951/", "config/sles_03948/")


class FrenchModelSpanishReuseTests(unittest.TestCase):
    """French images identical to Spanish ones reuse the accepted Spanish C."""

    def setUp(self):
        self.french = {m["name"]: m for m in json.loads((FRENCH / "overlays.json").read_text())["modules"]}
        self.spanish = {m["name"]: m for m in json.loads((SPANISH / "overlays.json").read_text())["modules"]}
        with (ROOT / "notes/overlays/french-model-spanish-reuse.csv").open() as handle:
            self.rows = list(csv.DictReader(handle))

    def test_ledger_matches_manifest_and_physical_registration(self):
        self.assertEqual(len(self.rows), 121)
        self.assertEqual(sum(len(row["sectors"].split(";")) for row in self.rows), 124)
        self.assertEqual(sum(int(row["c_bytes"]) for row in self.rows), 357292)
        mapped = registration.registered_keys(list(self.french.values()))
        for row in self.rows:
            module, donor = self.french[row["module"]], self.spanish[row["donor"]]
            self.assertNotIn(row["replaced"], self.french)
            self.assertEqual(module["name"], rename(donor["name"]))
            self.assertEqual((module["sha256"], module["load_address"]),
                             (donor["sha256"], donor["load_address"]))
            self.assertEqual(module["sha256"], row["sha256"])
            sectors = [int(s) for s in row["sectors"].split(";")]
            self.assertEqual([module["sector_offset"], *module.get("duplicate_sector_offsets", [])], sectors)
            for sector in sectors:
                key = (module["archive"], sector, module["sector_count"], int(module["load_address"], 0))
                self.assertIs(mapped[key], module)

    def test_c_metadata_follows_the_accepted_spanish_donors(self):
        # Spain may add C later; France may lag, but must never claim C the donor lacks.
        with (FRENCH / "functions.csv").open() as handle:
            resident = {int(r["address"], 0) for r in csv.DictReader(handle)}
        binding = re.compile(r"^(\w+)\s*=\s*(0x[0-9A-Fa-f]+);", re.M)
        for row in self.rows:
            module, donor = self.french[row["module"]], self.spanish[row["donor"]]
            with self.subTest(module=module["name"]):
                source, target = ROOT / donor["layout"], ROOT / module["layout"]
                functions = json.loads(target.with_name(target.stem + "_matching_c.json").read_text())["functions"]
                accepted = json.loads(source.with_name(source.stem + "_matching_c.json").read_text())["functions"]
                self.assertTrue(functions)
                for function in functions:
                    self.assertIn(function, accepted)
                self.assertEqual(sorted(segment["source"] for segment in c_segments(ROOT, target)),
                                 sorted({function["source"] for function in functions}))
                self.assertEqual(len(functions), int(row["c_functions"]))
                self.assertEqual(sum(int(f["size"], 0) for f in functions), int(row["c_bytes"]))
                self.assertEqual(module["linker_symbols"], rename(donor["linker_symbols"]))
                bindings = binding.findall((ROOT / module["linker_symbols"]).read_text())
                self.assertLessEqual(set(bindings),
                                     set(binding.findall((ROOT / donor["linker_symbols"]).read_text())))
                for _name, address in bindings:
                    self.assertIn(int(address, 0), resident)

    def test_built_images_are_exact_with_selected_c_owners(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for row in self.rows:
            module = self.french[row["module"]]
            name = module["name"]
            directory = ROOT / "tmp/overlays" / name
            binary = directory / f"build/{name}.bin"
            if not binary.exists():
                self.skipTest("Build French overlays before checking linked owners")
            with self.subTest(module=name):
                self.assertEqual(hashlib.sha256(binary.read_bytes()).hexdigest(), module["sha256"])
                layout = ROOT / module["layout"]
                segments = {segment["source"]: directory / "build" / segment["object"]
                            for segment in c_segments(ROOT, layout)}
                owners = {}
                for path in (directory / "build").rglob("*.o"):
                    with path.open("rb") as handle:
                        for symbol in ELFFile(handle).get_section_by_name(".symtab").iter_symbols():
                            if symbol["st_info"]["type"] == "STT_FUNC" and isinstance(symbol["st_shndx"], int):
                                owners.setdefault(symbol.name, []).append((path, symbol["st_size"]))
                with (directory / f"build/{name}.elf").open("rb") as handle:
                    linked = ELFFile(handle).get_section_by_name(".symtab")
                    functions = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())["functions"]
                    for function in functions:
                        address, size = int(function["address"], 0), int(function["size"], 0)
                        symbol, = [s for s in linked.iter_symbols()
                                   if s["st_value"] == address and s["st_info"]["type"] == "STT_FUNC"]
                        self.assertEqual(symbol["st_size"], size)
                        self.assertEqual(owners[symbol.name], [(segments[function["source"]], size)])


if __name__ == "__main__":
    unittest.main()
