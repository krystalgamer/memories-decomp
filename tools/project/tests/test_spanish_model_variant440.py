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
    source_directories = {"webs": "spanish_model_variant"}
    helpers = ((0x1BD4, 1372, "webs", "func_8013CBDC"), *family440.FrenchModelVariant440Tests.helpers)
    reachable_helpers = {0x1BD4}
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
