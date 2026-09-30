import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant476Tests(family435.FrenchModelVariant435Tests):
    family = 476
    source_family = 459
    module_count = 2
    distinct_images = 2
    binding_count = 35
    tail_start = 0x3374
    spans = ((4, 0x135C), (0x135C, 0x170C), (0x170C, 0x20BC),
             (0x20BC, 0x247C), (0x247C, 0x2848), (0x2848, 0x2DD4), (0x2DD4, 0x3374))
    helpers = ((0x20BC, 960, "webs", "func_8013D064"),)
    reachable_helpers = set()
    local_call_targets = {0x135C, 0x170C, 0x247C, 0x2848, 0x2DD4}
    models_by_stage = ((9, (712,)),)
    entry_anchors = {
        0x0C: 0x00809821, 0x14: 0x0260F021, 0x24: 0x27D80720, 0x88: 0xAFBE008C,
        0xB8: 0x001918C0, 0xBC: 0x00791823, 0xC0: 0x000318C0, 0xC8: 0xAFC327C0,
        0xB60: 0x26500120, 0xBB8: 0x2A620006, 0xBFC: 0x2B220006,
        0xC58: 0x27180260, 0xC90: 0x2B220003, 0xC98: 0x26B50260,
    }

    def test_selected_web_descriptor_and_context_separation(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(row["slot"])
                self.assertEqual(int(row["model"]), 712)
                self.assertEqual(int(row["record"]), 612)
                self.assertEqual(module["sector_offset"], 169112 + slot * 10)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x3470
                self.assertEqual(struct.unpack_from("<I", data, 0xA8)[0],
                                 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xAC)[0],
                                 0x24420000 | (table & 0xFFFF))
                archive.seek((612 * 276 + 275) * 2048 + 0x114)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, 642000)
                self.assertEqual(command, int(row["command_word"]))
                descriptor = 0x3470 + command % 1000 * 56
                self.assertEqual(descriptor, 0x3470)
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 56, len(data))
                self.assertEqual(6 * 6 * 8, 0x120)
                self.assertEqual(3 * 0x260, 0x720)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + 0x2864 <= start or start + size <= context)
