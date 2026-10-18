import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant88 as shared
from tools.project.tests import test_french_model_variant435 as family435
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant88Tests(shared.FrenchModelVariant88Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    attempt_count = 29
    mismatch_count = 25
    isolated_exact_count = 2

    def test_spanish_resident_bindings_and_context_data(self):
        with (self.config / "functions.csv").open() as handle:
            inventory = {int(row["address"], 0): row for row in csv.DictReader(handle)}
        bindings = {name: int(address, 16) for name, address in re.findall(
            r"^(\w+) = (0x[0-9A-F]+);$",
            (self.config / "overlays/model_variant88_linker_symbols.txt").read_text(), re.M)}
        self.assertEqual(len(bindings), 19)
        self.assertTrue(set(bindings.values()) <= inventory.keys())
        for address in bindings.values():
            self.assertGreater(int(inventory[address]["size"], 0), 0)
        for name, address in (
            ("Model_GetActiveSlotIndex", 0x8005BED4), ("Model_GetFrameStep", 0x8005BF24),
            ("Model_GetSlotDataEntry", 0x8005C028), ("func_80059A50", 0x8005CB58),
            ("func_8005B260", 0x8004D5B8), ("memset", 0x8008F548),
        ):
            self.assertEqual(bindings[name], address)
        path = family435.ROOT / "game/spain/SLES_039.51"
        if not path.exists():
            self.skipTest("legal Spanish resident input required")
        data = path.read_bytes()
        self.assertEqual(hashlib.sha256(data).hexdigest(),
                         "b0fefd88b6510f49af4f01e6180e40371652b7ceaa5f31dcb938c942316fc790")
        self.assertEqual(struct.unpack_from("<2I", data, 0x814), (0x8013B000, 0x8017B000))
        self.assertEqual(struct.unpack_from("<2I", data, 0x824), (0x80136000, 0x80176000))
        self.assertEqual(inventory[0x80058B4C]["name"], "func_800559D4")
        self.assertEqual(inventory[0x80058B4C]["status"], "matching_c")
