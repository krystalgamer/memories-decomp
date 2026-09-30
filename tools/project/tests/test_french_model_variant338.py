import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant338Tests(family435.FrenchModelVariant435Tests):
    family = 338
    source_family = 321
    module_count = 16
    distinct_images = 16
    binding_count = 35
    tail_start = 0x270C
    spans = ((4, 0xBA0), (0xBA0, 0x16D8), (0x16D8, 0x1B90),
             (0x1B90, 0x1ED4), (0x1ED4, 0x270C))
    helpers = ((0x16D8, 1208, "rings", "func_8013C6A8"),
               (0x1B90, 836, "strand", "func_8013CB64"))
    reachable_helpers = {0x16D8}
    local_call_targets = {0xBA0, 0x16D8, 0x1ED4}
    models_by_stage = ((7, (164, 165, 210, 424, 609)), (9, (34, 443, 459)))
    entry_anchors = {
        0x0C: 0x00809821, 0x14: 0x0260B021, 0x1C: 0x26D21234, 0x20: 0x26D8167C,
        0x8C: 0x00191040, 0x90: 0x00591021, 0x94: 0x00021080,
        0x98: 0x00591021, 0x9C: 0x00021080, 0xA4: 0xAEC21F3C,
        0x110: 0x26C71EE4, 0x138: 0x26C51EC4,
        0x5F0: 0x26500020, 0x5F4: 0x264F0040, 0x5F8: 0x264E0060,
        0x784: 0x26520098, 0x798: 0x2A220002, 0x7A0: 0x24630098,
        0x834: 0xAEC01F68, 0xA20: 0x02602021,
        0x1B98: 0x0080A021, 0x1BA0: 0x26971364, 0x1BD4: 0x26951EB4,
        0x1D0C: 0x2A42000D, 0x1D34: 0x2AC20006, 0x1D3C: 0x26F70084,
        0x1D54: 0x86921F4A, 0x1D58: 0x86821F4C,
        0x1E14: 0x2AC20006, 0x1E1C: 0x26F70084,
    }

    def test_selected_timing_records_and_context_separation(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<I", data)[0], int(row["header"]))
                base, slot = int(module["load_address"], 0), int(row["slot"])
                config = base + 0x2808
                self.assertEqual(struct.unpack_from("<I", data, 0x78)[0],
                                 0x3C030000 | ((config + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x7C)[0],
                                 0x24630000 | (config & 0xFFFF))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110 +
                             (int(row["stage"]) - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(row["command_word"]))
                offset = 0x2808 + command % 1000 * 52
                self.assertGreaterEqual(offset, self.tail_start)
                self.assertLessEqual(offset + 52, len(data))
                start, end, _, fade_start, fade_end = struct.unpack_from("<5I", data, offset + 32)
                self.assertLess(start, end)
                self.assertLess(fade_start, fade_end)
                self.assertEqual(0x1234 + 2 * 152, 0x1364)
                self.assertEqual(0x1364 + 6 * 132, 0x167C)
                self.assertEqual(struct.unpack_from("<I", data, 0x848)[0] & 0xFFFF, 0x1F7A)
                context = 0x80136000 + slot * 0x40000
                for load, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                   (0x8013A000 + slot * 0x40000, 2 * 2048),
                                   (base, 10 * 2048)):
                    self.assertTrue(context + 0x1F7C <= load or load + size <= context)
