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

    def test_switch_table_rejects_a_clobbered_jump_register(self):
        data = bytearray(image())
        struct.pack_into("<I", data, 0x20, 0x24020000)  # addiu v0, zero, 0
        self.assertIsNone(census.jump_table_targets(bytes(data), BASE, 4, 0x24))
        switch = census.walk_function(bytes(data), BASE, 4, resolve_flow=True)
        self.assertFalse(switch["closed"])
        self.assertIn("indirect_jump@0x24", switch["problems"])

    def test_switch_table_requires_a_guarded_bound_on_its_selector(self):
        for offset, word in ((0x0C, 0x00000000),   # no sltiu
                             (0x0C, 0x2CA20002),   # sltiu bounds a1, not the a0 selector
                             (0x10, 0x00000000),   # no beqz guard on the bound flag
                             (0x10, 0x14400006),   # inverted bnez: out of range falls into the table read
                             (0x10, 0x10400001)):  # beqz exit lands inside the dispatch
            data = bytearray(image())
            struct.pack_into("<I", data, offset, word)
            with self.subTest(offset=offset, word=word):
                self.assertIsNone(census.jump_table_targets(bytes(data), BASE, 4, 0x24))

    def test_switch_rejects_paths_that_rejoin_the_guarded_dispatch(self):
        data = bytearray(image())
        struct.pack_into("<I", data, 0x34, 0x1000FFF8)  # case 1: b 0x18, past the bound check
        switch = census.walk_function(bytes(data), BASE, 4, resolve_flow=True)
        self.assertFalse(switch["closed"])
        self.assertIn("unproven_switch@0x24", switch["problems"])

    def test_switch_accepts_guard_entries_that_recompute_the_bound(self):
        data = bytearray(image())
        struct.pack_into("<I", data, 0x34, 0x1000FFF6)  # case 1: b 0x10 (the guard)
        struct.pack_into("<I", data, 0x38, 0x2C820002)  # delay slot: sltiu v0, a0, 2
        self.assertTrue(census.walk_function(bytes(data), BASE, 4, resolve_flow=True)["closed"])
        struct.pack_into("<I", data, 0x38, 0x00000000)  # without it the flag is stale
        switch = census.walk_function(bytes(data), BASE, 4, resolve_flow=True)
        self.assertIn("unproven_switch@0x24", switch["problems"])

    def test_switch_guard_entries_must_preserve_the_table_base(self):
        data = bytearray(image())
        struct.pack_into("<I", data, 0x34, 0x24130000)  # case 1: addiu s3, zero, 0
        struct.pack_into("<I", data, 0x38, 0x1000FFF5)  # b 0x10 (the guard)
        struct.pack_into("<I", data, 0x3C, 0x2C820002)  # delay slot: sltiu v0, a0, 2
        self.assertEqual(census.jump_table_targets(bytes(data), BASE, 4, 0x24), [0x2C, 0x34])
        switch = census.walk_function(bytes(data), BASE, 4, resolve_flow=True)
        self.assertFalse(switch["closed"])
        self.assertIn("unproven_switch@0x24", switch["problems"])

    def test_switch_rejects_a_selector_changed_after_its_bound(self):
        data = bytearray(image())
        struct.pack_into("<I", data, 0x34, 0x2C820002)  # case 1: sltiu v0, a0, 2
        struct.pack_into("<I", data, 0x38, 0x1000FFF5)  # b 0x10 (the guard)
        struct.pack_into("<I", data, 0x3C, 0x24840005)  # delay slot: addiu a0, a0, 5
        self.assertEqual(census.jump_table_targets(bytes(data), BASE, 4, 0x24), [0x2C, 0x34])
        switch = census.walk_function(bytes(data), BASE, 4, resolve_flow=True)
        self.assertFalse(switch["closed"])
        self.assertIn("unproven_switch@0x24", switch["problems"])

    def test_switch_rejects_a_bound_on_the_selector_in_its_load_delay_slot(self):
        for load in (0x80A40000, 0x84A40000, 0x8CA40000, 0x90A40000, 0x94A40000):  # lb lh lw lbu lhu a0, 0(a1)
            data = bytearray(0x200)
            for offset, word in {
                0x04: 0x3C028014, 0x08: 0x2453B100, 0x0C: load, 0x10: 0x2C820002,
                0x14: 0x10400006, 0x18: 0x00041080, 0x1C: 0x00531021, 0x20: 0x8C420000,
                0x28: 0x00400008, 0x30: 0x03E00008, 0x38: 0x03E00008,
                0x100: BASE + 0x30, 0x104: BASE + 0x38,
            }.items():
                struct.pack_into("<I", data, offset, word)
            with self.subTest(load=load):
                self.assertEqual(census.jump_table_targets(bytes(data), BASE, 4, 0x28), [0x30, 0x38])
                switch = census.walk_function(bytes(data), BASE, 4, resolve_flow=True)
                self.assertIn("unproven_switch@0x28", switch["problems"])
                # Loading the selector before the table base keeps it out of the delay slot.
                struct.pack_into("<I", data, 0x04, load)
                struct.pack_into("<I", data, 0x08, 0x3C028014)
                struct.pack_into("<I", data, 0x0C, 0x2453B100)
                self.assertTrue(census.walk_function(bytes(data), BASE, 4, resolve_flow=True)["closed"])

    def test_switch_rejects_a_jump_register_read_in_the_load_delay_slot(self):
        data = bytearray(image())
        struct.pack_into("<I", data, 0x18, 0x00000000)
        struct.pack_into("<I", data, 0x1C, 0x00531021)  # addu v0, v0, s3
        struct.pack_into("<I", data, 0x20, 0x8C420000)  # lw v0, 0(v0)
        self.assertEqual(census.jump_table_targets(bytes(data), BASE, 4, 0x24), [0x2C, 0x34])
        switch = census.walk_function(bytes(data), BASE, 4, resolve_flow=True)
        self.assertIn("unproven_switch@0x24", switch["problems"])

    def test_reachability_only_follows_edges_from_rooted_bodies(self):
        data = bytearray(image())
        for offset, word in {
            # U takes V's address but is itself unreachable.
            0x40: 0x27BDFFF8, 0x44: 0x3C048014, 0x48: 0x2484B060, 0x4C: 0x03E00008, 0x50: 0x27BD0008,
            0x60: 0x27BDFFF8, 0x64: 0x03E00008, 0x68: 0x27BD0008,
        }.items():
            struct.pack_into("<I", data, offset, word)
        notes = {offset: note for offset, _, note in inventory.image_functions(bytes(data), BASE, {4}, set())}
        self.assertIn("reachability is unresolved", notes[0x40])
        self.assertIn("reachability is unresolved", notes[0x60])
        notes = {offset: note for offset, _, note in inventory.image_functions(bytes(data), BASE, {0x40}, set())}
        self.assertTrue(notes[0x60].startswith("Reached from the loader entry"))
        notes = {offset: note for offset, _, note in
                 inventory.image_functions(bytes(data), BASE, {4}, {BASE + 0x40})}
        self.assertTrue(notes[0x40].startswith("Address referenced by resident code"))
        self.assertTrue(notes[0x60].startswith("Reached from a resident-referenced body"))

    def test_functions_classify_entry_and_unreached_closed_bodies(self):
        rows = inventory.image_functions(image(), BASE, {4}, set())
        self.assertEqual([(offset, size) for offset, size, _ in rows], [(4, 0x38), (0x40, 0xC)])
        self.assertTrue(rows[0][2].startswith("Loader entry"))
        self.assertIn("reachability is unresolved", rows[1][2])

    def test_preamble_falling_through_into_a_framed_body_is_split(self):
        data = bytearray(image())
        for offset, word in {0x80: 0x00002021, 0x84: 0x27BDFFF8, 0x88: 0x03E00008,
                             0x8C: 0x27BD0008}.items():
            struct.pack_into("<I", data, offset, word)
        rows = inventory.image_functions(bytes(data), BASE, {4, 0x80}, set())
        self.assertIn((0x80, 4), [(offset, size) for offset, size, _ in rows])
        self.assertIn((0x84, 0xC), [(offset, size) for offset, size, _ in rows])
        self.assertIn("Falls through into func_8013B084.", dict((o, n) for o, _, n in rows)[0x80])
        notes = {offset: note for offset, _, note in inventory.image_functions(bytes(data), BASE, {0x80}, set())}
        self.assertTrue(notes[0x84].startswith("Reached from the loader entry"))

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
        with self.assertRaisesRegex(inventory.InventoryError, "module header"):
            inventory.inventory_layout_text(module, sha1, [(0, 8)])
        with self.assertRaisesRegex(inventory.InventoryError, "module header"):
            inventory.inventory_layout_text(module, sha1, [])
        late = inventory.inventory_layout_text(module, sha1, [(0xF4, 8)])
        self.assertIn("[0x4, bin, overlays/model_image_model_1_8013b000/unclassified_4]", late)
        with self.assertRaisesRegex(inventory.InventoryError, "invalid function span"):
            inventory.inventory_layout_text(module, sha1, [(4, 8), (8, 4)])


if __name__ == "__main__":
    unittest.main()
