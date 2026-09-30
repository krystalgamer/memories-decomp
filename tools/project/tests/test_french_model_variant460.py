import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant460Tests(family435.FrenchModelVariant435Tests):
    family = 460
    source_family = 443
    module_count = 28
    distinct_images = 28
    binding_count = 34
    tail_start = 0x28CC
    spans = ((4, 0xCFC), (0xCFC, 0x17EC), (0x17EC, 0x1D74),
             (0x1D74, 0x20B4), (0x20B4, 0x28CC))
    helpers = ((0x17EC, 1416, "sheets", "func_8013C808"),
               (0x1D74, 832, "strand", "func_8013CD84"))
    reachable_helpers = {0x17EC}
    local_call_targets = {0xCFC, 0x17EC}
    models_by_stage = ((7, (70, 125, 168, 460, 469, 704)),
                       (9, (44, 98, 161, 370, 400, 458, 462, 558)))
    entry_anchors = {0x10: 0x0080B021, 0x14: 0x26D81740, 0x30: 0x02C0B821,
                     0x34: 0xAFB80080, 0x94: 0x00191980, 0x98: 0x00621821,
                     0x9C: 0xAEC32E7C, 0x590: 0x2A820008, 0x598: 0x26F702E8,
                     0x59C: 0x00008021, 0x740: 0x26100001, 0x74C: 0x8FB90080,
                     0x754: 0x2739009C, 0x758: 0xAFB90080, 0x770: 0x2A020010}

    def test_command_selected_sheet_configuration_bounds(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                base = int(module["load_address"], 0)
                config = base + 0x29C8
                self.assertEqual(struct.unpack_from("<I", data, 0x88)[0],
                                 0x3C020000 | ((config + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x90)[0],
                                 0x24420000 | (config & 0xFFFF))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110 +
                             4 * ((int(row["stage"]) - 7) // 2))
                command = struct.unpack("<i", archive.read(4))[0]
                self.assertEqual(command, int(row["command_word"]))
                self.assertGreaterEqual(command, 0)
                start = 0x29C8 + command % 1000 * 64
                self.assertGreaterEqual(start, self.tail_start)
                self.assertLessEqual(start + 64, len(data))
                n, mode, kind = struct.unpack_from("<3i", data, start + 0x20)
                self.assertEqual(mode, 1)
                self.assertIn(kind, (0, 1, 2))
                count, first = ((n + 1, 1) if kind == 0 else
                                (n + 2, 2) if kind == 2 else (2 * n, n))
                self.assertGreater(first, 0)
                self.assertLessEqual(first, 8)
                self.assertLessEqual(first, count)
                self.assertLessEqual(count, 16)
                self.assertLessEqual(count - first, 8)
