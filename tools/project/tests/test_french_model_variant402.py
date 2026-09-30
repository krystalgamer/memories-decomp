import csv
import hashlib
import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant402Tests(family435.FrenchModelVariant435Tests):
    family = 402
    module_count = 4
    distinct_images = 4
    binding_count = 35
    tail_start = 0x2060
    spans = ((4, 0xA5C), (0xA5C, 0x1048), (0x1048, 0x169C),
             (0x169C, 0x1B38), (0x1B38, 0x2060))
    helpers = ((0x169C, 1180, "rings", "func_8013C69C"),
               (0x1B38, 1320, "bands", "func_8013CB38"))
    reachable_helpers = {0x1B38}
    local_call_targets = {0x1B38}
    models_by_stage = ((7, (6, 551)),)
    entry_anchors = {
        0x0C: 0x00809021, 0x14: 0x0240F021, 0x1C: 0x27D50058,
        0x94: 0x00181880, 0x98: 0x00781821, 0x9C: 0x00031880,
        0xA4: 0xAFC3087C, 0x438: 0x26A30088,
        0x440: 0x26B00020, 0x444: 0x26AF0040, 0x448: 0x26AE0060,
        0x554: 0x24840001, 0x558: 0x28820004, 0x564: 0x26D60001,
        0x568: 0xAC60FFF8, 0x56C: 0xAC60FFFC, 0x570: 0xAC600000,
        0x574: 0x24630090, 0x578: 0x2AC20002, 0x580: 0x26B50090,
        0x694: 0xAFC008AC,
        0x16A4: 0x00808021, 0x16AC: 0x261E0058, 0x16D4: 0x261106DC,
        0x16DC: 0x261200D8, 0x18AC: 0x03D42821, 0x18B0: 0x03D53021,
        0x1964: 0x2AE20004, 0x1AEC: 0x26520090, 0x1AF4: 0x27DE0090,
        0x1AFC: 0x29020002,
        0x20: 0x27D80438, 0x40: 0xAFB80084, 0x590: 0x27130114,
        0x5E0: 0xA6030088, 0x608: 0x2A420011, 0x614: 0xAE74FFFC,
        0x620: 0xAE600000, 0x624: 0x26730118, 0x62C: 0x2AC20002,
        0x630: 0x27390118, 0x8E4: 0x02402021,
        0x1B40: 0x0080A021, 0x1B44: 0x268C0438,
        0x1BBC: 0x8FAC00E8, 0x1BF0: 0x25AD0114,
        0x1CBC: 0x2A620011, 0x1F84: 0x2A620010,
        0x2014: 0x258C0118, 0x2018: 0x25AD0118, 0x201C: 0x29E20002,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        for label, symbol0, symbol1 in (("rings", "func_8013C69C", "func_8017C69C"),
                                       ("bands", "func_8013CB38", "func_8017CB38")):
            self.assertEqual((directory / f"variant402_{label}_slot1.c").read_text(),
                             '#include "../../types.h"\n'
                             f'#define {symbol0} {symbol1}\n'
                             f'#include "variant402_{label}.c"\n')
            body = (directory / f"variant402_{label}.c").read_text()
            self.assertIn(f'#include "variant402_{label}.h"', body)
            self.assertNotRegex(body, r"\b(?:extern|asm|__asm__)\b")
        with (family435.ROOT / "notes/overlays/french-model-variant402-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 12)
        self.assertEqual([row["result"] for row in attempts[:4]], ["mismatch", "text_exact", "matched", "matched"])
        self.assertEqual([row["result"] for row in attempts[4:10]], ["mismatch"] * 5 + ["text_exact"])
        self.assertEqual(attempts[0]["different_words"], "11")
        terminal = [row for row in attempts if row["result"] == "matched"]
        self.assertEqual(len(terminal), 4)
        for row in terminal:
            label, size = ("rings", "1180") if row["function_offset"] == "0x169C" else ("bands", "1320")
            source = directory / (f"variant402_{label}" + ("_slot1" if row["slot"] == "1" else "") + ".c")
            self.assertEqual(hashlib.sha256(source.read_bytes()).hexdigest(), row["fingerprint"])
            self.assertEqual((row["instruction_bytes"], row["different_words"], row["profile"]),
                             (size, "0", "gcc_2_8_1_g0_split"))

    def test_selected_ring_descriptor_and_context_separation(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(row["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x215C
                self.assertEqual(struct.unpack_from("<I", data, 0x84)[0],
                                 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x88)[0],
                                 0x24420000 | (table & 0xFFFF))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, 568000)
                self.assertEqual(command, int(row["command_word"]))
                descriptor = 0x215C + command % 1000 * 20
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 20, len(data))
                self.assertEqual(struct.unpack_from("<H", data, descriptor + 12)[0], 1)
                self.assertEqual(struct.unpack_from("<I", data, descriptor + 16)[0], 92)
                self.assertEqual(0x58 + 2 * 144, 0x178)
                self.assertEqual(0x438 + 2 * 280, 0x668)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + 0x8C4 <= start or start + size <= context)

    def test_named_band_imports_preserve_existing_addresses(self):
        for module in self.modules:
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for name, address in (("rcos", 0x800866F8), ("rsin", 0x80086628), ("ratan2", 0x80089928)):
                self.assertIn(f"{name} = 0x{address:X};", bindings)
                self.assertIn(f"{name} = 0x{address:X}; // type:func absolute:true", symbols)
                self.assertNotIn(f"func_{address:X} =", bindings)
                self.assertNotIn(f"func_{address:X} =", symbols)
