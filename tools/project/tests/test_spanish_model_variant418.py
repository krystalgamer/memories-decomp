import re
import struct

from tools.project.tests import test_french_model_variant418 as family418
from tools.project.tests import test_french_model_variant435 as family435
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant418Tests(family418.FrenchModelVariant418Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    load_inventories = staticmethod(load_spanish_overlay_inventories)

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
                table = base + 0x3218
                self.assertEqual(struct.unpack_from("<II", data, 0xA0),
                                 (0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF),
                                  0x24420000 | (table & 0xFFFF)))
                self.assertEqual(struct.unpack_from("<5I", data, 0xB0),
                                 (0x00181840, 0x00781821, 0x00031900, 0x00621821, 0xAEC31AF4))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x114)
                request, = struct.unpack("<i", archive.read(4))
                self.assertEqual(request, 584002)
                self.assertEqual(request, int(row["command_word"]))
                descriptor = 0x3218 + request % 1000 * 48
                self.assertEqual(descriptor, 0x3278)
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 48, len(data))
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack("<1015I", data[4:0xFE0])
                            if word >> 26 in widths and word >> 21 & 31 == 22
                            and word & 0x8000 == 0]
                self.assertEqual(max(accesses), 0x1B2C)
                context = pointers[9 + slot]
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048), (base, 10 * 2048)):
                    self.assertTrue(context + max(accesses) <= start or start + size <= context)
