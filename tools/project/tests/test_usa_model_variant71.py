"""USA loader records and callee contracts for the shared French MODEL88 C."""
import csv
import hashlib
import json
from pathlib import Path
import re
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))
from overlay_sources import c_segments
from progress import load_overlay_inventories


class USAModelVariant71Tests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        config = ROOT / "config/slus_01411"
        with (ROOT / "notes/overlays/usa-model-variant71-instances.csv").open() as handle:
            cls.instances = {row["module"]: row for row in csv.DictReader(handle)}
        cls.modules = {m["name"]: m for m in json.loads((config / "overlays.json").read_text())["modules"]
                       if m.get("linker_symbols") == "config/slus_01411/overlays/model_variant71_linker_symbols.txt"}
        cls.bindings = dict((name, int(address, 16)) for name, address in re.findall(
            r"^(\w+) = (0x[0-9A-F]+);", (config / "overlays/model_variant71_linker_symbols.txt").read_text(), re.M))
        cls.archive = ROOT / "game/DATA/MODEL.MRG"

    def test_registration_uses_shared_c_and_keeps_suffix_unclassified(self):
        self.assertEqual(set(self.modules), set(self.instances))
        self.assertEqual(len(self.modules), 14)
        counts = load_overlay_inventories(ROOT)
        for name, module in self.modules.items():
            with self.subTest(module=name):
                slot = int(self.instances[name]["slot"])
                source = "src/overlays/french_model_variant/variant88_entry" + ("_slot1" if slot else "") + ".c"
                segments = c_segments(ROOT, ROOT / module["layout"])
                self.assertEqual([(s["source"], s["profile"]) for s in segments], [(source, "gcc_2_8_1_g0_split")])
                self.assertEqual(counts[name]["function_count"], 1)
                self.assertEqual(counts[name]["matching_c_bytes"], 2384)
                layout = (ROOT / module["layout"]).read_text()
                self.assertIn(f"[0x954, data, overlays/{name}/unclassified_tail]", layout)
                self.assertIn("  - [0x5000]", layout)

    def test_callees_are_canonical_resident_function_starts(self):
        with (ROOT / "config/slus_01411/functions.csv").open() as handle:
            resident = {row["name"]: int(row["address"], 16) for row in csv.DictReader(handle)}
        self.assertEqual(len(self.bindings), 19)
        for name, address in self.bindings.items():
            self.assertEqual(address, resident[name], name)

    def test_retail_loader_commands_descriptors_and_calls(self):
        if not self.archive.exists():
            self.skipTest("requires the legal USA MODEL archive")
        commands, products = set(), set()
        with self.archive.open("rb") as archive:
            for name, module in self.modules.items():
                with self.subTest(module=name):
                    row = self.instances[name]
                    model, record, stage, slot = (int(row[key]) for key in ("model", "record", "stage", "slot"))
                    self.assertEqual(record, model - (50 if model >= 400 else 0))
                    sector = record * 276 + 180 + (stage - 7) * 10
                    self.assertEqual(module["sector_offset"], sector)
                    self.assertEqual(int(row["sector_offset"]), sector)
                    self.assertEqual(module["load_address"], f"0x{0x8013B000 + slot * 0x40000:08X}")
                    archive.seek(sector * 2048)
                    image = archive.read(20480)
                    self.assertEqual(hashlib.sha256(image).hexdigest(), module["sha256"])
                    self.assertEqual(module["sha256"], row["sha256"])
                    self.assertEqual(struct.unpack_from("<I", image)[0], 71 + slot * 130)
                    words = struct.unpack_from("<596I", image, 4)
                    calls = [0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3]
                    self.assertEqual(len(calls), 23)
                    self.assertEqual(set(calls), set(self.bindings.values()))
                    archive.seek((record * 276 + 275) * 2048 + 0x110 + (stage - 7) // 2 * 4)
                    command, = struct.unpack("<i", archive.read(4))
                    self.assertEqual(command, int(row["command_word"]))
                    commands.add(command)
                    descriptor = struct.unpack_from("<10B5h4B", image, 0x970 + command % 1000 * 24)
                    groups, count, period = descriptor[3], descriptor[4], descriptor[12]
                    products.add(groups * count)
                    self.assertTrue(0 < groups <= 5)
                    self.assertTrue(0 < groups * count <= 512)
                    self.assertGreater(period, 0)
        self.assertEqual(commands, {18000, 18002, 18003, 18004, 18005, 18006, 18007})
        self.assertEqual(products, {20, 30, 32, 40, 48, 60})


if __name__ == "__main__":
    unittest.main()
