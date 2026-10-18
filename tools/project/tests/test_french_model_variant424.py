import csv
import hashlib
import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant424Tests(family435.FrenchModelVariant435Tests):
    family = 424
    source_family = 407
    module_count = 30
    distinct_images = 29
    binding_count = 33
    tail_start = 0x18B4
    spans = ((4, 0x12D8), (0x12D8, 0x18B4))
    helpers = ((0x12D8, 1500, "petals", "func_8013C2DC"),)
    reachable_helpers = {0x12D8}
    local_call_targets = {0x12D8}
    models_by_stage = ((7, (68, 96, 186, 297, 376, 595)),
                       (9, (165, 242, 294, 352, 358, 399, 465, 520, 621)))
    entry_anchors = {
        0xC: 0x00809021, 0x14: 0x0240B021, 0x74: 0x26D92320, 0x94: 0xAFB60080,
        0xC0: 0x00181900, 0xC4: 0x00781821, 0xC8: 0x00031880, 0xD0: 0xAEC329D4,
        0x42C: 0x8FA60080, 0x434: 0x00C02821, 0x440: 0x9442001C,
        0x44C: 0xA4A20000, 0x478: 0xA4A20180, 0x4AC: 0xA4A20300, 0x4D8: 0xA4A20480,
        0x508: 0xA3020780, 0x51C: 0xA3020781, 0x530: 0xA3020782,
        0x534: 0xACC00854, 0x538: 0xACC00914, 0x53C: 0xACC009D4, 0x550: 0xACC20794,
        0x564: 0xA4A20600, 0x58C: 0x24C60004, 0x598: 0x2B020030, 0x5A0: 0x24A50008,
        0xFD4: 0x8E220794, 0xFE4: 0x8E2209D4, 0x1050: 0xAE0223A8,
        0x1084: 0xAE0226A8, 0x10A4: 0x26100010, 0x10AC: 0xAC820008,
        0x10B0: 0x26C20300, 0x10B4: 0x0202102A, 0x10BC: 0x26310004,
        0x1174: 0x02402021, 0x12D8: 0x27BDFEB0, 0x12E0: 0x00808821,
        0x12E8: 0x02209821, 0x12F0: 0x26362320,
        0x1370: 0x02621021, 0x142C: 0x0262A821, 0x1444: 0x9463001E,
        0x1618: 0x001738C0, 0x1620: 0x24E50180, 0x1628: 0x24E60300,
        0x1630: 0x26C20008, 0x1638: 0x26C20010, 0x1640: 0x26C20018, 0x1648: 0x26C20020,
        0x165C: 0x24E70480, 0x1674: 0xA2C80004, 0x1680: 0xA2C90005,
        0x168C: 0x04C0000C, 0x1690: 0xA2CA0006, 0x169C: 0x04400008,
        0x16D0: 0x02622821, 0x16F0: 0x94430020, 0x1704: 0x00081042,
        0x1748: 0x02622021, 0x1768: 0x8C430028, 0x1788: 0xAC8009D4,
        0x17D8: 0x02621021, 0x17EC: 0x24020030, 0x1868: 0x28840030,
    }



    def test_named_import_preserves_resident_address(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn("ratan2 = 0x80089928; // type:func absolute:true", symbols)
            self.assertIn("ratan2 = 0x80089928;",
                          (family435.ROOT / module["linker_symbols"]).read_text())
            self.assertNotIn("func_french_80089928", symbols)

    def test_descriptor_commands_and_direct_context_separation(self):
        archive_path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        resident = family435.ROOT / "game/france/SLES_039.48"
        if not archive_path.exists() or not resident.exists():
            self.skipTest("legal French MODEL and resident inputs required")
        pointers = struct.unpack_from("<14I", resident.read_bytes(), 0x800)
        self.assertEqual((pointers[9], pointers[10]), (0x80136000, 0x80176000))
        commands = set()
        with archive_path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(row["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x19B0
                self.assertEqual(struct.unpack_from("<I", data, 0xB0)[0],
                                 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xB4)[0], 0x24420000 | (table & 0xFFFF))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110 +
                             (int(row["stage"]) - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(row["command_word"]))
                commands.add(command)
                descriptor = 0x19B0 + command % 1000 * 68
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 68, len(data))
                widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack(f"<{(0x12D8 - 4) // 4}I", data[4:0x12D8])
                            if word >> 26 in widths and (word >> 21) & 31 == 22 and word & 0x8000 == 0]
                self.assertEqual(max(accesses), 0x2A10)
                for start, size in ((pointers[slot], 96 * 2048), (pointers[3 + slot], 4096), (base, 20480)):
                    self.assertTrue(pointers[9 + slot] + 0x2A10 <= start or start + size <= pointers[9 + slot])
        self.assertEqual(commands, {590000, 590001, 590002, *range(590004, 590014)})
