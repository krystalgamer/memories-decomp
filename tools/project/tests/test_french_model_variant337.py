import csv
import hashlib
import re
import struct

from tools.project.tests import test_spanish_model_variant337 as spanish337


class FrenchModelVariant337Tests(spanish337.SpanishModelVariant337Tests):
    region = "french"
    config_path = "config/sles_03948"
    archive_path = "game/france/DATA/MODEL.MRG"
    c_helpers = (
        (0x4, 2464, "french_model_variant/variant337_entry"),
        (0x9A4, 2260, "french_model_variant/variant337_ribbon"),
        (0x1278, 1216, "spanish_model_variant/variant337_rings"),
    )
    ribbon_anchors = {
        0x000c: 0x00809021,
        0x0014: 0x0240B021,
        0x001c: 0x26D303A4,
        0x0020: 0x26D804D4,
        0x0030: 0x26D50C6C,
        0x0038: 0x02C08021,
        0x0084: 0x001810C0,
        0x0088: 0x00581021,
        0x008c: 0x00021080,
        0x0090: 0x00431021,
        0x0094: 0xAEC20D34,
        0x00d8: 0x26C50CCC,
        0x00e0: 0x261702D0,
        0x036c: 0x241800C0,
        0x037c: 0xA2F8FF50,
        0x0380: 0xA2F8FF51,
        0x0384: 0xA2F8FF52,
        0x0414: 0x1A80FFD1,
        0x0418: 0x26F703A4,
        0x0634: 0xA6C20D46,
        0x0648: 0xA6C00D40,
        0x0654: 0xAEC00D5C,
        0x0658: 0xAEC00D60,
        0x0680: 0xAEC00D6C,
        0x0694: 0xA6D80D7E,
        0x0800: 0x8C630010,
        0x0804: 0x8EC20D24,
        0x080c: 0x0043102B,
        0x0810: 0x14400005,
        0x081c: 0x02402021,
        0x09a4: 0x27BDFED0,
        0x09ac: 0x0080B821,
        0x09d8: 0x8EE40D10,
        0x09dc: 0x8EE50D08,
        0x09e8: 0x8EE40D0C,
        0x09fc: 0x8EE30D6C,
        0x0a0c: 0x8EE20D20,
        0x0a20: 0x86E20D46,
        0x0a3c: 0x86E30D46,
        0x0a60: 0x02E09821,
        0x0a7c: 0x8EE20D5C,
        0x0ab4: 0x8EE30D60,
        0x0ae8: 0x8EE30CF4,
        0x0b60: 0x8EE30CF8,
        0x0bb0: 0x8EE20CFC,
        0x0bfc: 0xA6220110,
        0x0c04: 0x26300110,
        0x0c1c: 0x27DE0300,
        0x0c34: 0x28420011,
        0x0c50: 0x267303A4,
        0x0c6c: 0x000212C0,
        0x0c98: 0x8EE20CE0,
        0x0ca4: 0x8EE20CE4,
        0x0cb0: 0x8EE30CE8,
        0x0d3c: 0x266B0078,
        0x0d40: 0x26680080,
        0x0d44: 0x26720040,
        0x0d48: 0x26690020,
        0x0d80: 0x2662035C,
        0x0da0: 0x26640190,
        0x0da4: 0x266501D8,
        0x0db4: 0xAE4202D8,
        0x0dd8: 0xAE4200CC,
        0x0ddc: 0x86420198,
        0x0de0: 0x86430088,
        0x0df0: 0xAE4201DC,
        0x0e0c: 0xA5220360,
        0x0e38: 0xA5220382,
        0x0e6c: 0x2602031C,
        0x0ea8: 0xAE0202D8,
        0x0ed8: 0xAE0200CC,
        0x0ef0: 0xAE0201DC,
        0x0f10: 0xA6220360,
        0x0f34: 0xA6220382,
        0x0f48: 0x28420011,
        0x0f54: 0x26D603A4,
        0x0f70: 0x267303A4,
        0x0f7c: 0x26F20222,
        0x0f84: 0x86E20D40,
        0x0f90: 0x26F10C6C,
        0x0f94: 0x26F00C72,
        0x105c: 0x9242FFFE,
        0x1068: 0x9242FFFF,
        0x1074: 0x92420000,
        0x1080: 0x8CE202D8,
        0x1090: 0x8CE2031C,
        0x10a0: 0x94E602D8,
        0x10bc: 0x26100028,
        0x10c4: 0x26310028,
        0x10c8: 0x2610FFD8,
        0x10cc: 0x2631FFD8,
        0x1110: 0x8EE30D6C,
        0x1134: 0x8EE30D34,
        0x113c: 0x8C640014,
        0x1140: 0x8C630018,
        0x1150: 0x0043001B,
        0x1180: 0xA6E20D40,
        0x118c: 0xAEE20D6C,
        0x1198: 0x86E20D46,
        0x11b0: 0x8CA4001C,
        0x11c4: 0x8CA20020,
        0x11d0: 0x0062001B,
        0x11ec: 0xA6E20D46,
        0x1200: 0xA6E00D46,
        0x1204: 0x8EE30D2C,
        0x1234: 0xAEE30D5C,
        0x1244: 0xAEE30D60,
    }

    def test_french_attempt_fingerprints_and_bindings(self):
        root = spanish337.ROOT
        with (root / "notes/overlays/french-model-variant337-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual({int(row["slot"]) for row in attempts}, {0, 1})
        self.assertEqual(len(attempts), 14)
        terminals = [row for row in attempts if row["result"] == "matched"]
        self.assertEqual(len(terminals), 6)
        helpers = {offset: (size, source) for offset, size, source in self.c_helpers}
        for row in terminals:
            slot = int(row["slot"])
            size, helper = helpers[int(row["function_offset"], 0)]
            source = root / ("src/overlays/" + helper +
                             ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
            self.assertEqual(int(row["instruction_bytes"]), size)
            self.assertEqual(int(row["different_words"]), 0)
        for result in ("mismatch", "text_exact"):
            probes = [row for row in attempts if row["result"] == result]
            self.assertEqual({int(row["slot"]) for row in probes}, {0, 1})
            self.assertEqual(len(probes), 4)
            self.assertEqual({(int(row["function_offset"], 0), int(row["slot"])) for row in probes},
                             {(4, 0), (4, 1), (0x9A4, 0), (0x9A4, 1)})
            entries = [row for row in probes if int(row["function_offset"], 0) == 4]
            expected = ("2452", "603") if result == "mismatch" else ("2464", "0")
            self.assertTrue(all((row["instruction_bytes"], row["different_words"]) == expected
                                for row in entries))
        for module, *_ in self.selected():
            bindings = (root / module["linker_symbols"]).read_text()
            self.assertEqual(len(re.findall(r"^\w+ =", bindings, re.M)), 33)
            for name, address in (("ratan2", 0x80089928), ("RotTransPers", 0x80087868),
                                  ("rcos", 0x800866F8), ("rsin", 0x80086628),
                                  ("GetTPage", 0x80082CE8), ("GetClut", 0x80082D28),
                                  ("SetSemiTrans", 0x80082DA8), ("SetShadeTex", 0x80082DD8),
                                  ("SetPolyG3", 0x80082E48), ("SetPolyFT4", 0x80082EA8),
                                  ("SetPolyG4", 0x80082EC8), ("SetPolyGT4", 0x80082EE8),
                                  ("SquareRoot0", 0x80086DD8), ("Square0", 0x80089BC8),
                                  ("GsGetLwUnit", 0x8008A428), ("GsSetLsMatrix", 0x80085558)):
                self.assertIn(f"{name} = 0x{address:X};", bindings)

    def test_entry_access_views_and_slot_wrapper(self):
        directory = spanish337.ROOT / "src/overlays/french_model_variant"
        source = (directory / "variant337_entry.c").read_text()
        header = (directory / "variant337_entry.h").read_text()
        self.assertIn('#include "variant337_entry.h"', source)
        self.assertNotRegex(source, r"\b(?:asm|__asm__|register|volatile|extern)\b")
        self.assertIn("Variant337EntryConfig *G32 config;", header)
        self.assertIn("GsCOORDUNIT *G32 part;", header)
        self.assertIn("Variant337EntryRecord records[1];", header)
        self.assertIn("Variant337EntryRing rings[2];", header)
        self.assertIn("Variant337EntryStreamer streamers[2];", header)
        self.assertIn("angle = 1024 + (i + 1) * 512;", source)
        self.assertIn("Model_CopySlotU16Values(1, (u16 *)&work->target);", source)
        self.assertIn("Model_CopySlotU16Values(0, (u16 *)&work->target);", source)
        self.assertEqual((directory / "variant337_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define func_8013B9A4 func_8017B9A4\n'
                         '#define func_8013C278 func_8017C278\n'
                         '#define func_8013C738 func_8017C738\n'
                         '#define D_8013CF5C D_8017CF5C\n'
                         '#include "variant337_entry.c"\n')

    def test_ribbon_wrappers_reuse_shared_header_and_region_branch(self):
        root = spanish337.ROOT
        shared = (root / "src/overlays/model_variant/variant320_ribbon.c").read_text()
        self.assertIn("#if defined(VERSION_FRENCH)", shared)
        self.assertIn("ribbon->otz[k] = RotTransPers4(&ribbon->a[15], &ribbon->a[k]", shared)
        self.assertIn("ribbon->otz[16] = RotTransPers4(&ribbon->a[15], &ribbon->a[16]", shared)
        self.assertNotRegex(shared, r"\b(?:asm|__asm__|register|volatile|extern)\b")
        for slot in (0, 1):
            source = root / ("src/overlays/french_model_variant/variant337_ribbon" +
                             ("_slot1" if slot else "") + ".c")
            self.assertEqual(source.read_text(), '#include "../../types.h"\n'
                             '#define VERSION_FRENCH\n'
                             f'#define func_8013B9A8 func_{0x8013B9A4 + slot * 0x40000:X}\n'
                             '#include "../model_variant/variant320_ribbon.c"\n')

    def test_french_entry_ownership_and_actual_config_requests(self):
        path = spanish337.ROOT / self.archive_path
        if not path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
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
                self.assertEqual(len(self.ribbon_anchors), 107)
                for offset, word in self.ribbon_anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                self.assertEqual(struct.unpack_from("<I", data, 0x818)[0],
                                 0x0C000000 | ((base + 0x9A4) >> 2 & 0x3FFFFFF))
                config = base + 0x2058
                self.assertEqual(struct.unpack_from("<I", data, 0x70)[0],
                                 0x3C030000 | ((config + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x74)[0], 0x24630000 | (config & 0xFFFF))
                archive.seek((record * 276 + 275) * 2048 + 0x110 + (stage == 9) * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 503000 + command)
                self.assertGreaterEqual(0x2058 + command * 36, 0x1F5C)
                gate, grow_start, grow_end, fade_start, fade_end = struct.unpack_from(
                    "<5I", data, 0x2058 + command * 36 + 16)
                self.assertLessEqual(gate, grow_start)
                self.assertLess(grow_start, grow_end)
                self.assertLess(fade_start, fade_end)
                calls = set()
                for start, size in self.spans:
                    for pc in range(start, start + size, 4):
                        word = struct.unpack_from("<I", data, pc)[0]
                        if word >> 26 == 3:
                            target = ((base + pc + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                            if base <= target < base + len(data):
                                calls.add(target - base)
                self.assertEqual(calls, {0x9A4, 0x1278, 0x1738})
