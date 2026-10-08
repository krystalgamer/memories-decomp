import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import unittest

from tools.project.overlay_sources import c_segments

ROOT = Path(__file__).resolve().parents[3]
SECTORS = {48224, 48234}


class GermanModel450TakeoverTests(unittest.TestCase):
    def setUp(self):
        self.all_modules = json.loads(
            (ROOT / "config/sles_03949/overlays.json").read_text())["modules"]
        self.modules = [m for m in self.all_modules if m["sector_offset"] in SECTORS]

    def test_existing_physical_registrations_are_not_duplicated(self):
        self.assertEqual(len(self.all_modules), 3584)
        self.assertEqual(len(self.modules), 2)
        keys = [(m["archive"], m["sector_offset"], m["load_address"])
                for m in self.modules]
        self.assertEqual(len(keys), len(set(keys)))
        self.assertEqual({m["name"] for m in self.modules},
                         {"german_model_image_model_48224_8013b000",
                          "german_model_image_model_48234_8017b000"})
        self.assertFalse(any("model_variant_174_" in m["name"] for m in self.all_modules))

    def test_selected_owners_are_already_accepted_in_french(self):
        for module in self.modules:
            base = int(module["load_address"], 0)
            slot = int(base == 0x8017B000)
            layout = ROOT / module["layout"]
            functions = json.loads(layout.with_name(
                layout.stem + "_matching_c.json").read_text())["functions"]
            self.assertEqual(len(functions), 4)
            self.assertEqual({int(f["address"], 0) - base for f in functions},
                             {0xDD8, 0x1794, 0x27C0, 0x2D88})
            french = json.loads((ROOT / "config/sles_03948/overlays" /
                                 f"model_variant_174_stage{10 if slot else 9}_slot{slot}_matching_c.json"
                                 ).read_text())["functions"]
            self.assertTrue(all(function in french for function in functions))
            with layout.with_name(layout.stem + "_functions.csv").open() as stream:
                rows = list(csv.DictReader(stream))
            self.assertEqual(len(rows), 7)
            self.assertEqual({int(r["address"], 0) - base for r in rows
                              if r["status"] == "unmatched_asm"}, {4, 0x1ECC, 0x310C})
            self.assertEqual(sum(int(f["size"], 0) for f in functions), 6720)
            self.assertIn("start: 14656", layout.read_text())

    def test_preserved_instance_and_terminal_fingerprints(self):
        with (ROOT / "notes/overlays/german-model-variant450-instances.csv").open() as stream:
            instances = list(csv.DictReader(stream))
        self.assertEqual({r["module"] for r in instances}, {m["name"] for m in self.modules})
        with (ROOT / "notes/overlays/german-model-variant450-attempts.csv").open() as stream:
            attempts = list(csv.DictReader(stream))
        self.assertEqual(len(attempts), 8)
        sources = {}
        for module in self.modules:
            base = int(module["load_address"], 0)
            slot = str(int(base == 0x8017B000))
            layout = ROOT / module["layout"]
            for function in json.loads(layout.with_name(
                    layout.stem + "_matching_c.json").read_text())["functions"]:
                sources[(int(function["address"], 0) - base, slot)] = ROOT / function["source"]
        for row in attempts:
            self.assertEqual((row["profile"], row["result"], row["different_words"]),
                             ("gcc_2_8_1_g0_split", "matched", "0"))
            source = sources[(int(row["function_offset"], 0), row["slot"])]
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())

    def test_complete_images_and_actual_sized_c_owners(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module in self.modules:
            name = module["name"]
            directory = ROOT / "tmp/overlays" / name
            binary = directory / "build" / f"{name}.bin"
            if not binary.exists():
                self.skipTest("Build German overlays before checking owners")
            data = binary.read_bytes()
            self.assertEqual(len(data), 0x5000)
            self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
            layout = ROOT / module["layout"]
            segments = {s["source"]: s for s in c_segments(ROOT, layout)}
            functions = json.loads(layout.with_name(
                layout.stem + "_matching_c.json").read_text())["functions"]
            for function in functions:
                symbol = f"func_{int(function['address'], 0):X}"
                size = int(function["size"], 0)
                obj = directory / "build" / segments[function["source"]]["object"]
                elf = directory / "build" / f"{name}.elf"
                for path, expected in ((obj, 0), (elf, int(function["address"], 0))):
                    with path.open("rb") as stream:
                        image = ELFFile(stream)
                        own, = image.get_section_by_name(".symtab").get_symbol_by_name(symbol)
                        self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                         (expected, size, "STT_FUNC"))
                        self.assertIsInstance(own["st_shndx"], int)
                        self.assertNotEqual(image.get_section(own["st_shndx"]).name, "*ABS*")
