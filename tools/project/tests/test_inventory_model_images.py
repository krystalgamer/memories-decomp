from pathlib import Path
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

import inventory_model_images as inventory
import overlay_function_inventory as census
import register_model_images as registration

BASE = 0x8013B000


def image() -> bytes:
    data = bytearray(0x200)
    words = {
        # Switch whose table base is hoisted into $s3 before the bound check.
        0x04: 0x3C028014, 0x08: 0x2453B100, 0x0C: 0x2C820002, 0x10: 0x10400006,
        0x14: 0x00041080, 0x18: 0x00531021, 0x1C: 0x8C420000, 0x20: 0x00000000,
        0x24: 0x00400008, 0x28: 0x00000000, 0x2C: 0x03E00008, 0x30: 0x00000000,
        0x34: 0x03E00008, 0x38: 0x00000000,
        # Framed tail jump to a resident function.
        0x40: 0x27BDFFF8, 0x44: 0x08004000, 0x48: 0x27BD0008,
        0x100: BASE + 0x2C, 0x104: BASE + 0x34,
    }
    for offset, word in words.items():
        struct.pack_into("<I", data, offset, word)
    return bytes(data)


class InventoryModelImagesTests(unittest.TestCase):
    def test_published_census_rules_are_unchanged_by_default(self):
        data = image()
        self.assertIn("indirect_jump@0x24", census.walk_function(data, BASE, 4)["problems"])
        self.assertIn("tail_target@0x80010000", census.walk_function(data, BASE, 0x40)["problems"])

    def test_flow_resolution_follows_hoisted_switch_tables_and_tail_jumps(self):
        data = image()
        self.assertEqual(census.jump_table_targets(data, BASE, 4, 0x24), [0x2C, 0x34])
        switch = census.walk_function(data, BASE, 4, resolve_flow=True)
        self.assertTrue(switch["closed"])
        self.assertEqual(switch["extent"], 0x38)
        tail = census.walk_function(data, BASE, 0x40, resolve_flow=True)
        self.assertTrue(tail["closed"])
        self.assertEqual(tail["external"], {0x80010000})

    def test_flow_resolution_rejects_targets_outside_main_ram(self):
        data = bytearray(image())
        struct.pack_into("<I", data, 0x44, 0x0814F60C)
        tail = census.walk_function(bytes(data), BASE, 0x40, resolve_flow=True)
        self.assertFalse(tail["closed"])
        self.assertIn("invalid_target@0x8053d830", tail["problems"])

    def test_switch_table_must_agree_with_its_bound(self):
        data = bytearray(image())
        struct.pack_into("<I", data, 0x104, 0x12345678)
        self.assertIsNone(census.jump_table_targets(bytes(data), BASE, 4, 0x24))

    def test_functions_classify_entry_and_unreached_closed_bodies(self):
        rows = inventory.image_functions(image(), BASE, {4}, set())
        self.assertEqual([(offset, size) for offset, size, _ in rows], [(4, 0x38), (0x40, 0xC)])
        self.assertTrue(rows[0][2].startswith("Loader entry"))
        self.assertIn("reachability is unresolved", rows[1][2])

    def test_layout_keeps_gaps_raw_and_covers_the_image(self):
        module = {"name": "model_image_model_1_8013b000", "load_address": "0x8013B000",
                  "sector_count": 1, "output": "tmp/overlays/model_image_model_1_8013b000/module.bin"}
        sha1 = "0" * 40
        text = inventory.inventory_layout_text(module, sha1, [(4, 0x38), (0x40, 8)])
        self.assertFalse(inventory.baseline_text(module, text))
        self.assertTrue(inventory.baseline_text(module, registration.layout_text(module, sha1)))
        kinds = [line.split(", ")[1] for line in text.splitlines() if line.startswith("      - [")]
        self.assertEqual(kinds, ["data", "asm", "bin", "asm", "bin"])
        self.assertIn("unclassified_3c", text)
        self.assertTrue(text.endswith("  - [0x800]\n"))
        with self.assertRaisesRegex(inventory.InventoryError, "loader entry"):
            inventory.inventory_layout_text(module, sha1, [(8, 4)])
        with self.assertRaisesRegex(inventory.InventoryError, "invalid function span"):
            inventory.inventory_layout_text(module, sha1, [(4, 8), (8, 4)])


if __name__ == "__main__":
    unittest.main()
