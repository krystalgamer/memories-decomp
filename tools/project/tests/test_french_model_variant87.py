import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant87Tests(family435.FrenchModelVariant435Tests):
    family = 87
    slot_header_delta = 130
    module_count = 4
    distinct_images = 4
    binding_count = 21
    tail_start = 0xA1C
    spans = ((4, 0xA1C),)
    helpers = ((4, 2584, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((9, (140, 584)),)
    entry_anchors = {4: 0x27BDFE10, 0x298: 0x2484FE3E, 0x29C: 0x00641823,
                     0xA14: 0x03E00008, 0xA18: 0x27BD01F0}

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant87_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BA1C D_8017BA1C\n'
                         '#include "variant87_entry.c"\n')
        body = (directory / "variant87_entry.c").read_text()
        self.assertIn('#include "variant87_entry.h"', body)
        self.assertNotRegex(body, r"\b(?:extern|asm|__asm__)\b")
        with (family435.ROOT / "notes/overlays/french-model-variant87-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 40)
        self.assertEqual(len({row["attempt"] for row in attempts}), 40)
        self.assertEqual(sum(row["result"] == "mismatch" for row in attempts), 36)
        self.assertEqual(sum(row["result"] == "link_blocked" for row in attempts), 1)
        self.assertEqual(sum(row["result"] == "text_exact" for row in attempts), 1)
        terminal = [row for row in attempts if row["result"] == "matched"]
        self.assertEqual({row["slot"] for row in terminal}, {"0", "1"})
        for row in terminal:
            source = directory / ("variant87_entry" + ("_slot1" if row["slot"] == "1" else "") + ".c")
            self.assertEqual(hashlib.sha256(source.read_bytes()).hexdigest(), row["fingerprint"])
            self.assertEqual((row["instruction_bytes"], row["different_words"], row["profile"]),
                             ("2584", "0", "gcc_2_8_1_g0_split"))

    def test_selected_descriptor_and_accessed_context_bounds(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<I", data)[0], int(row["header"]))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x114)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, 17001)
                offset = self.tail_start + command % 1000 * 22
                self.assertEqual(offset, 0xA32)
                descriptor = struct.unpack_from("<8B7h", data, offset)
                self.assertEqual(descriptor, (64, 0, 64, 176, 0, 176, 11, 16,
                                              70, 300, 10, 30, 10, 2, 40))
                self.assertLessEqual(4 + 25 * 8 + 6, 0xD4)
                self.assertLessEqual(0xD4 + (descriptor[7] - 1) * 8 + 6, 0x1D4)
                slot = int(row["slot"])
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 473 <= start or start + size <= context)

    def test_all_direct_calls_have_resident_bindings(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                archive.seek(module["sector_offset"] * 2048 + 4)
                words = struct.unpack("<646I", archive.read(2584))
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 29)
                self.assertEqual(set(calls), addresses)
