import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant198Tests(family435.FrenchModelVariant435Tests):
    family = 198
    slot_header_delta = 130
    module_count = 2
    distinct_images = 2
    binding_count = 25
    tail_start = 0xDBC
    spans = ((4, 0xD74),)
    helpers = ((4, 3440, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((9, (508,)),)
    entry_anchors = {
        4: 0x27BDFC18,
        0x470: 0x0C013DF0,
        0x548: 0x0C021F12,
        0x80C: 0x27A40090,
        0x838: 0x00501823,
        0x83C: 0x00034200,
        0x840: 0x01034023,
        0x86C: 0x00004012,
        0xD0C: 0xA3C20330,
        0xD10: 0x304400FF,
        0xD20: 0x24020001,
        0xD28: 0x24020004,
        0xD6C: 0x03E00008,
        0xD70: 0x27BD03E8,
        0xD74: 4096,
        0xD78: 4096,
        0xD7C: 4096,
        0xD80: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant198_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BD74 D_8017BD74\n'
                         '#define D_8013BD84 D_8017BD84\n'
                         '#define D_8013BDBC D_8017BDBC\n'
                         '#include "variant198_entry.c"\n')
        source = (directory / "variant198_entry.c").read_text()
        header = (directory / "variant198_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__)\b")
        self.assertIn("const VECTOR D_8013BD74 = {4096, 4096, 4096, 0};", source)
        self.assertIn("extern GsIMAGE D_8013BD84[];", header)
        self.assertIn("extern Model198Config D_8013BDBC[];", header)
        self.assertIn("func_8005F7B0(50, (s16)(config->phase_spacing / 3));", source)
        self.assertIn("work->triggered++;", source)
        self.assertIn("scale_value = i + 33;", source)
        self.assertIn("color_numerator = remaining * 256;\n            color_numerator -= remaining;", source)
        self.assertEqual(source.count("color_numerator = config->"), 6)
        self.assertIn("for (k = 1; k < config->phase_count; k++)", source)
        self.assertIn("point += 2;", source)
        self.assertIn("work->completed == i", source)
        self.assertIn("work->completed == config->phase_count", source)
        with (family435.ROOT / "notes/overlays/french-model-variant198-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 24)
        self.assertEqual(len({(row["attempt"], row["slot"]) for row in rows}), 24)
        expected = {
            "native-first-candidate": ("3436", "499", "mismatch"),
            "native-direct-ring-indexing": ("3444", "455", "mismatch"),
            "native-phase-local-scalars": ("3444", "455", "mismatch"),
            "ring-shared-scalar-separate-flash": ("3440", "15", "mismatch"),
            "byte-width-flash-result": ("3440", "15", "mismatch"),
            "explicit-flash-remaining-lifetime": ("3440", "15", "mismatch"),
            "flash-packet-pointer-lifetime": ("3444", "416", "mismatch"),
            "direct-flash-channel-expressions": ("3576", "353", "mismatch"),
            "chained-grayscale-assignment": ("3440", "15", "mismatch"),
            "shared-color-numerator": ("3440", "10", "mismatch"),
            "shared-flash-scale-intermediate": ("3440", "0", "text_exact"),
        }
        terminal = [row for row in rows if row["result"] == "matched"]
        self.assertEqual({row["slot"] for row in terminal}, {"0", "1"})
        for row in rows:
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
            if row["result"] != "matched":
                self.assertEqual((row["instruction_bytes"], row["different_words"], row["result"]),
                                 expected[row["attempt"]])
        for row in terminal:
            path = directory / ("variant198_entry" + ("_slot1" if row["slot"] == "1" else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("3440", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0xD84:X} = 0x{base + 0xD84:X}; // type:u8 size:0x38 defined:true", symbols)
            self.assertIn(f"D_{base + 0xD74:X} = 0x{base + 0xD74:X}; // type:u32 size:0x10 defined:true", symbols)
            slot = int(module["name"][-1])
            source = "overlays/french_model_variant/variant198_entry" + ("_slot1" if slot else "")
            self.assertIn(f"[0xD74, .rodata, {source}]", layout.read_text())
            self.assertIn(f"[0xD84, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptor_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                command = int(row["command_word"])
                self.assertEqual(command, 129000)
                descriptor = struct.unpack_from("<8B16h", data, 0xDBC + command % 1000 * 40)
                self.assertEqual(descriptor, (128, 128, 96, 128, 128, 128, 6, 0, 1000, 200,
                                             300, 2000, 500, -70, 60, 20, 40, 60, 2, 160,
                                             2, 110, 390, 200))
                self.assertEqual(0xD84 + 56, 0xDBC)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                self.assertIn("func_8005F7B0 = 0x8004F7C0;", bindings)
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<860I", data[4:0xD74])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 39)
                self.assertEqual(set(calls), addresses)
                slot = int(row["slot"])
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 0x334 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model198Config)": 40, "sizeof(Model198State)": 0x334,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(POLY_FT4)": 40, "sizeof(POLY_F4)": 24,
            "sizeof(GsIMAGE)": 28, "sizeof(DVECTOR)": 4,
        }
        for typename, fields in (
            ("Model198State", (("rays", 4), ("opponent", 0x104), ("ring", 0x10C),
                               ("origin", 0x31C), ("texture", 0x324), ("elapsed", 0x32C),
                               ("completed", 0x330), ("triggered", 0x331))),
            ("Model198Config", (("ray_height", 8), ("ray_half_width", 10), ("spread", 12),
                                ("ring_radius", 14), ("ring_width", 16), ("ring_y", 18),
                                ("ray_duration", 20), ("ray_delay", 22), ("ring_duration", 24),
                                ("flash_duration", 26), ("ray_spacing", 28), ("phase_spacing", 30),
                                ("phase_count", 32), ("start_delay", 34))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 31)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant198_entry.h")
