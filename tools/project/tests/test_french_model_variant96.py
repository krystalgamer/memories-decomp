import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant96Tests(family435.FrenchModelVariant435Tests):
    family = 96
    slot_header_delta = 130
    module_count = 4
    distinct_images = 4
    binding_count = 23
    tail_start = 0xC78
    spans = ((4, 0xC4C),)
    helpers = ((4, 3144, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((9, (192, 196)),)
    entry_anchors = {
        4: 0x27BDFA28,
        0xA0: 0xAFA90584,
        0x440: 0x256C041C,
        0x448: 0xAFAC0584,
        0xC00: 0x24420001,
        0xC44: 0x03E00008,
        0xC48: 0x27BD05D8,
        0xC4C: 4096,
        0xC50: 4096,
        0xC54: 4096,
        0xC58: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant96_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BC4C D_8017BC4C\n'
                         '#define D_8013BC5C D_8017BC5C\n'
                         '#define D_8013BC78 D_8017BC78\n'
                         '#include "variant96_entry.c"\n')
        source = (directory / "variant96_entry.c").read_text()
        header = (directory / "variant96_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__)\b")
        self.assertIn("const VECTOR D_8013BC4C = {4096, 4096, 4096, 0};", source)
        self.assertIn("extern GsIMAGE D_8013BC5C[];", header)
        self.assertIn("extern Model96Config D_8013BC78[];", header)
        self.assertIn("position = work->outer;", source)
        self.assertIn("for (i = 0; i <= config->segments; position++, inner++, i++)", source)
        self.assertIn("position = work->positions;\n        GsSetLsMatrix(&base);", source)
        self.assertIn("quad.r0 = quad.r1 = config->r * (6144 - *factor) / 2048;", source)
        with (family435.ROOT / "notes/overlays/french-model-variant96-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 6)
        self.assertEqual(len({(row["attempt"], row["slot"]) for row in rows}), 6)
        self.assertEqual(sum(row["result"] == "mismatch" for row in rows), 2)
        self.assertEqual(sum(row["result"] == "text_exact" for row in rows), 2)
        terminal = [row for row in rows if row["result"] == "matched"]
        self.assertEqual({row["slot"] for row in terminal}, {"0", "1"})
        for row in rows:
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
        for row in terminal:
            path = directory / ("variant96_entry" + ("_slot1" if row["slot"] == "1" else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("3144", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0xC5C:X} = 0x{base + 0xC5C:X}; // type:u8 size:0x1C defined:true", symbols)
            self.assertIn(f"D_{base + 0xC4C:X} = 0x{base + 0xC4C:X}; // type:u32 size:0x10 defined:true", symbols)
            slot = int(module["name"][-1])
            source = "overlays/french_model_variant/variant96_entry" + ("_slot1" if slot else "")
            self.assertIn(f"[0xC4C, .rodata, {source}]", layout.read_text())
            self.assertIn(f"[0xC5C, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptor_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            192: (96, 96, 64, 5, 100, 350, 32, 16, 30, 20, 2, 100),
            196: (128, 96, 96, 3, 200, 380, 16, 16, 20, 20, 2, 40),
        }
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                command = int(row["command_word"])
                self.assertEqual(command, {192: 27002, 196: 27004}[int(row["model"])])
                config = struct.unpack_from("<4B8h", data, 0xC78 + command % 1000 * 20)
                self.assertEqual(config, descriptors[int(row["model"])])
                self.assertTrue(0 < config[6] <= 64)
                self.assertTrue(0 < config[7] <= 128)
                self.assertGreater(config[8], 0)
                self.assertGreater(config[9], 0)
                self.assertEqual(0xC5C + 28, 0xC78)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<786I", data[4:0xC4C])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 39)
                self.assertEqual(set(calls), addresses)
                slot = int(row["slot"])
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 0xA34 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model96Config)": 20, "sizeof(Model96State)": 0xA34,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(POLY_GT4)": 52, "sizeof(POLY_F4)": 24,
            "sizeof(GsIMAGE)": 28, "sizeof(DVECTOR)": 4,
        }
        for typename, fields in (
            ("Model96State", (("outer", 0xC), ("inner", 0x214), ("positions", 0x41C),
                              ("scales", 0x81C), ("rotation", 0xA1C), ("prepared", 0xA24),
                              ("texture", 0xA28), ("completed", 0xA2C), ("elapsed", 0xA30))),
            ("Model96Config", (("outer_radius", 4), ("inner_radius", 6), ("segments", 8),
                               ("count", 10), ("duration", 12), ("flash_duration", 14),
                               ("spacing", 16), ("delay", 18))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 26)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant96_entry.h")
