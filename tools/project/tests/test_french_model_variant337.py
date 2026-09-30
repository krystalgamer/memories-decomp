import csv
import hashlib
import re
import struct

from tools.project.tests import test_spanish_model_variant337 as spanish337


class FrenchModelVariant337Tests(spanish337.SpanishModelVariant337Tests):
    region = "french"
    config_path = "config/sles_03948"
    archive_path = "game/france/DATA/MODEL.MRG"

    def test_french_attempt_fingerprints_and_bindings(self):
        root = spanish337.ROOT
        with (root / "notes/overlays/french-model-variant337-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual({int(row["slot"]) for row in attempts}, {0, 1})
        for row in attempts:
            slot = int(row["slot"])
            source = root / ("src/overlays/spanish_model_variant/variant337_rings" +
                             ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
            self.assertEqual(int(row["function_offset"], 0), 0x1278)
        for module, *_ in self.selected():
            bindings = (root / module["linker_symbols"]).read_text()
            self.assertEqual(len(re.findall(r"^\w+ =", bindings, re.M)), 33)

    def test_french_entry_ownership_and_actual_config_requests(self):
        path = spanish337.ROOT / self.archive_path
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        anchors = {0x0C: 0x00809021, 0x14: 0x0240B021, 0x1C: 0x26D303A4,
                   0x5C4: 0x26730098, 0x5D8: 0x2A220002, 0xD8: 0x26C50CCC,
                   0xEC: 0x0C02290A, 0x84: 0x001810C0, 0x88: 0x00581021,
                   0x8C: 0x00021080, 0x90: 0x00431021, 0x94: 0xAEC20D34}
        with path.open("rb") as archive:
            for module, record, stage, slot, command in self.selected():
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                config = base + 0x2058
                self.assertEqual(struct.unpack_from("<I", data, 0x70)[0],
                                 0x3C030000 | ((config + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x74)[0], 0x24630000 | (config & 0xFFFF))
                archive.seek((record * 276 + 275) * 2048 + 0x110 + (stage == 9) * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 503000 + command)
                self.assertGreaterEqual(0x2058 + command * 36, 0x1F5C)
                calls = set()
                for start, size in self.spans:
                    for pc in range(start, start + size, 4):
                        word = struct.unpack_from("<I", data, pc)[0]
                        if word >> 26 == 3:
                            target = ((base + pc + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                            if base <= target < base + len(data):
                                calls.add(target - base)
                self.assertEqual(calls, {0x9A4, 0x1278, 0x1738})
