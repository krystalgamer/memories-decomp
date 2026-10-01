import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant440 as family440
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant440Tests(family440.FrenchModelVariant440Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    source_directories = {"sheets": "spanish_model_variant", "webs": "spanish_model_variant"}
    helpers = (
        (0x16E0, 1268, "sheets", "func_8013C6E4"),
        (0x1BD4, 1372, "webs", "func_8013CBDC"),
        (0x2130, 784, "spokes", "func_8013D13C"),
        (0x2440, 892, "rings", "func_8013D450"),
        (0x27BC, 868, "quad", "func_8013D7D0"),
    )
    reachable_helpers = {0x16E0, 0x1BD4}
    entry_anchors = {
        **family440.FrenchModelVariant440Tests.entry_anchors,
        0x7C: 0xAFB60094, 0x758: 0x8FB80094, 0x764: 0x2714019C,
        0x77C: 0xAFB800B8, 0x7E4: 0x8FB200B8, 0x844: 0x265000C0,
        0x880: 0x26520008, 0x89C: 0x2A620006, 0x8C8: 0x27180030,
        0x8FC: 0x2B020004, 0x924: 0xA282FFE4, 0x94C: 0x271801A0,
        0x964: 0xAE80FFFC, 0x97C: 0xAE82FFF8, 0x980: 0x2AE20003,
        0x988: 0x269401A0, 0x1BDC: 0xAFA400D0, 0x1BE0: 0x0080A021,
        0x1C34: 0x26910E84, 0x1C58: 0x26920194, 0x1C5C: 0x8E820F24,
        0x2048: 0x8E820EF8, 0x20D4: 0x265201A0, 0x20E4: 0x252901A0,
        0x16E0: 0x27BDFF00, 0x16E8: 0x00808821, 0x16F0: 0x263505E8,
        0x171C: 0x26320DBC, 0x1728: 0x26300670, 0x172C: 0x8E220EEC,
        0x1774: 0x8E220EAC, 0x1780: 0x8E220EB0, 0x178C: 0x8E220EB4,
        0x1798: 0x86230F1C, 0x17AC: 0x8E220EC0, 0x17DC: 0x8E220EC4,
        0x180C: 0x8E220EC8, 0x1838: 0x86220EB8, 0x1844: 0x86220EBA,
        0x1850: 0x86220EBC, 0x1920: 0x24E50020, 0x1928: 0x24E60040,
        0x1930: 0x26420008, 0x1938: 0x26420014, 0x1940: 0x26420020,
        0x1948: 0x2642002C, 0x1950: 0x27A200D0, 0x1958: 0x27A200D4,
        0x195C: 0x24E70060, 0x196C: 0x9203FFFC, 0x19D8: 0x9203FFF8,
        0x1A0C: 0x8FA200D4, 0x1A30: 0x2A620004, 0x1A38: 0x26940008,
        0x1A44: 0x8E220F24, 0x1A68: 0x8E230F00, 0x1A6C: 0x8E220EF0,
        0x1A70: 0x8C64001C, 0x1A74: 0x8C630020, 0x1AB8: 0x86220F18,
        0x1B0C: 0x8E220EF8, 0x1B6C: 0x8E220EF8, 0x1B8C: 0xAE220F24,
        0x1B94: 0x26100098, 0x1B98: 0x2AE20002, 0x1BA0: 0x26B50098,
    }

    def test_web_projection_bindings_keep_verified_resident_addresses(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            for name, address in (("RotTransPers3", 0x80087898), ("ratan2", 0x80089928)):
                self.assertIn(f"{name} = 0x{address:X};", bindings)
                self.assertIn(f"{name} = 0x{address:X}; // type:func absolute:true", symbols)
                self.assertNotIn(f"func_french_{address:X}", symbols)

    def test_bindings_cover_fallback_functions_as_well_as_c(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)

    def test_selected_descriptors_and_minimum_context_extent(self):
        archive_path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        resident_path = family435.ROOT / "game/spain/SLES_039.51"
        if not archive_path.exists() or not resident_path.exists():
            self.skipTest("legal Spanish MODEL and resident inputs required")
        pointers = struct.unpack_from("<14I", resident_path.read_bytes(), 0x800)
        self.assertEqual(pointers[9:11], (0x80136000, 0x80176000))
        widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
        with archive_path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, base = int(row["slot"]), int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x2C1C
                self.assertEqual(struct.unpack_from("<II", data, 0x98),
                                 (0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF),
                                  0x24420000 | (table & 0xFFFF)))
                self.assertEqual(struct.unpack_from("<5I", data, 0xA8),
                                 (0x00181840, 0x00781821, 0x00031900, 0x00621821, 0xAEC30F00))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110 +
                             (int(row["stage"]) - 7) // 2 * 4)
                request, = struct.unpack("<i", archive.read(4))
                self.assertEqual(request, 606000)
                self.assertEqual(request, int(row["command_word"]))
                descriptor = 0x2C1C + request % 1000 * 48
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 48, len(data))
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack("<964I", data[4:0xF14])
                            if word >> 26 in widths and word >> 21 & 31 == 22
                            and word & 0x8000 == 0]
                self.assertEqual(max(accesses), 0xF38)
                context = pointers[9 + slot]
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048), (base, 10 * 2048)):
                    self.assertTrue(context + max(accesses) <= start or start + size <= context)

    def test_sheet_context_packet_and_stack_ownership(self):
        path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        self.assertEqual(0x5E8 + 2 * 152, 0x718)
        self.assertEqual(0xDBC + 52, 0xDF0)
        self.assertEqual(128 + 80, 208)
        self.assertEqual(208 + 4, 212)
        self.assertEqual(212 + 4, 216)
        widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<I", data, 0x16E0)[0], 0x27BDFF00)
                self.assertEqual(struct.unpack_from("<I", data, 0x1BC8)[0], 0x8FB000D8)
                self.assertEqual(struct.unpack_from("<I", data, 0x1BD0)[0], 0x27BD0100)
                for register, capture, restore, extent in (
                        (17, 0x16E8, 0x1BC4, 0xF28),
                        (18, 0x171C, 0x1BC0, None)):
                    writes, accesses = [], []
                    for offset in range(0x16E0, 0x1BD4, 4):
                        word, = struct.unpack_from("<I", data, offset)
                        op = word >> 26
                        destination = word >> 11 & 31 if op == 0 else word >> 16 & 31 if op in (
                            8, 9, 10, 11, 12, 13, 14, 15, 32, 33, 35, 36, 37) else None
                        if destination == register:
                            writes.append(offset)
                        if op in widths and word >> 21 & 31 == register and not word & 0x8000:
                            accesses.append((word & 0xFFFF) + widths[op])
                    self.assertEqual(writes, [capture, restore])
                    restored, = struct.unpack_from("<I", data, restore)
                    self.assertEqual((restored >> 26, restored >> 21 & 31), (35, 29))
                    if extent is not None:
                        self.assertEqual(max(accesses), extent)
