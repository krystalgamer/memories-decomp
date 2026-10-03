import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant88Tests(family435.FrenchModelVariant435Tests):
    family = 88
    slot_header_delta = 130
    module_count = 14
    distinct_images = 14
    binding_count = 19
    tail_start = 0x954
    spans = ((4, 0x954),)
    helpers = ((4, 2384, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    attempt_count = 45
    mismatch_count = 42
    isolated_exact_count = 1
    models_by_stage = ((7, (290, 295, 501, 518)), (9, (31, 408, 531)))
    entry_anchors = {
        4: 0x27BDFEF8,
        0x910: 0x24020001,
        0x94C: 0x03E00008,
        0x950: 0x27BD0108,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant88_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013B954 D_8017B954\n'
                         '#include "variant88_entry.c"\n')
        source = (directory / "variant88_entry.c").read_text()
        header = (directory / "variant88_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__)\b")
        self.assertIn("extern u8 D_8013B954[];", header)
        self.assertNotIn("D_8013B970", source + header)
        self.assertIn("(Family88Config *)(D_8013B954 + 0x1C)", source)
        self.assertIn("(GsIMAGE *)D_8013B954", source)
        self.assertIn("work->frame >= config->duration + config->period", source)
        with (family435.ROOT / f"notes/overlays/{self.module_prefix}-model-variant88-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), self.attempt_count)
        self.assertEqual(len({row["attempt"] for row in attempts}), self.attempt_count)
        self.assertEqual(sum(row["result"] == "mismatch" for row in attempts), self.mismatch_count)
        self.assertEqual(sum(row["result"] == "text_exact" for row in attempts), self.isolated_exact_count)
        for row in attempts:
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
            if not row["profile"]:
                self.assertIn("Original receipt omitted profile", row["reason"])
        terminal = [row for row in attempts if row["result"] == "matched"]
        self.assertEqual({row["slot"] for row in terminal}, {"0", "1"})
        for row in terminal:
            path = directory / ("variant88_entry" + ("_slot1" if row["slot"] == "1" else "") + ".c")
            self.assertEqual(hashlib.sha256(path.read_bytes()).hexdigest(), row["fingerprint"])
            self.assertEqual((row["instruction_bytes"], row["different_words"], row["profile"]),
                             ("2384", "0", "gcc_2_8_1_g0_split"))

    def test_selected_descriptors_and_overlapping_views(self):
        path = family435.ROOT / f"game/{self.region}/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
        commands, counts = set(), set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                archive.seek((int(row["record"]) * 276 + 275) * 2048 +
                             0x110 + (int(row["stage"]) - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(row["command_word"]))
                commands.add(command)
                offset = 0x970 + command % 1000 * 24
                config = struct.unpack_from("<10B5h4B", data, offset)
                used = config[3] * config[4]
                counts.add(used)
                self.assertTrue(0 < used <= 512)
                self.assertTrue(0 < config[3] <= 5)
                self.assertGreater(config[12], 0)
                self.assertLessEqual(4 + used * 8, 0x1004)
                self.assertLessEqual(0x1004 + used * 2, 0x1404)
                self.assertEqual(0x954 + 28, 0x970)
                self.assertEqual(len(data[0x970:0x98C]), 28)
                slot = int(row["slot"])
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 0x1410 <= start or start + size <= context)
        self.assertEqual(commands, {18000, 18002, 18003, 18004, 18005, 18006, 18007})
        self.assertEqual(counts, {20, 30, 32, 40, 48, 60})

    def test_all_direct_calls_have_resident_bindings(self):
        path = family435.ROOT / f"game/{self.region}/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                archive.seek(module["sector_offset"] * 2048 + 4)
                words = struct.unpack("<596I", archive.read(2384))
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 23)
                self.assertEqual(set(calls), addresses)

    def test_target_compiled_context_and_overlapping_views(self):
        checks = {
            "sizeof(Family88Config)": 24, "sizeof(Family88State)": 0x1410,
            "sizeof(SVECTOR)": 8, "sizeof(MATRIX)": 32, "sizeof(VECTOR)": 16,
            "sizeof(GsIMAGE)": 28, "sizeof(POLY_FT4)": 40,
        }
        for typename, fields in (
            ("Family88Config", (("groups", 3), ("count", 4), ("parts", 5), ("size", 10),
                                ("amplitude", 12), ("period", 14), ("delay", 16),
                                ("duration", 18), ("r1", 20))),
            ("Family88State", (("config", 0), ("points", 4), ("timers", 0x1004),
                               ("frame", 0x1404), ("shared", 0x1408),
                               ("shared.state.done", 0x140C))),
            ("POLY_FT4", (("clut", 14), ("tpage", 22), ("x0", 8))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 25)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant88_entry.h")
