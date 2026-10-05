import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant162Tests(family435.FrenchModelVariant435Tests):
    family = 162
    slot_header_delta = 130
    module_count = 2
    distinct_images = 2
    binding_count = 27
    tail_start = 0xDD8
    spans = ((4, 0xDAC),)
    helpers = ((4, 3496, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (37,)),)
    entry_anchors = {
        4: 0x27BDFA58,
        0x4C: 0xAFAD056C,
        0x64: 0xAFAE0570,
        0xA8: 0xAFAF0574,
        0xB0: 0xAFA20578,
        0x158: 0x2A420100,
        0x694: 0x2A420040,
        0x6C0: 0x0C021F12,
        0xB14: 0x0C022712,
        0xD68: 0x24420001,
        0xDA4: 0x03E00008,
        0xDA8: 0x27BD05A8,
        0xDAC: 4096,
        0xDB0: 4096,
        0xDB4: 4096,
        0xDB8: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant162_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BDAC D_8017BDAC\n'
                         '#define D_8013BDBC D_8017BDBC\n'
                         '#define D_8013BDD8 D_8017BDD8\n'
                         '#include "variant162_entry.c"\n')
        source = (directory / "variant162_entry.c").read_text()
        header = (directory / "variant162_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__)\b")
        self.assertIn("const VECTOR D_8013BDAC = {4096, 4096, 4096, 0};", source)
        self.assertIn("extern GsIMAGE D_8013BDBC[];", header)
        self.assertIn("extern Model162Config D_8013BDD8[];", header)
        self.assertEqual(source.count("applyVector(&delta, -1, -1, -1, *=);"), 2)
        self.assertIn("Model_GetActiveSlotIndex();\n    {\n        s32 step = Model_GetFrameStep();", source)
        self.assertIn("point->vx = delta.vx * time / config->line_duration * i / 64;", source)
        self.assertIn("addVector(point, &spiral[j / 64]);", source)
        self.assertIn("RotTransSV(point, &position, &flag);\n                copyVector(&position, point);", source)
        self.assertIn("for (i = 0; i < value; point += 2, i += 2)", source)
        self.assertIn("work->completed++;", source)
        with (family435.ROOT / "notes/overlays/french-model-variant162-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 10)
        self.assertEqual(len({(row["attempt"], row["slot"]) for row in rows}), 10)
        expected = {
            "native-first-candidate": ("3452", "667", "mismatch"),
            "native-signed-vector-operations": ("3496", "13", "mismatch"),
            "native-byte-completion-result": ("3496", "8", "mismatch"),
            "frame-step-initialization-scope": ("3496", "0", "text_exact"),
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
            path = directory / ("variant162_entry" + ("_slot1" if row["slot"] == "1" else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("3496", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0xDBC:X} = 0x{base + 0xDBC:X}; // type:u8 size:0x1C defined:true", symbols)
            self.assertIn(f"D_{base + 0xDAC:X} = 0x{base + 0xDAC:X}; // type:u32 size:0x10 defined:true", symbols)
            slot = int(module["name"][-1])
            source = "overlays/french_model_variant/variant162_entry" + ("_slot1" if slot else "")
            self.assertIn(f"[0xDAC, .rodata, {source}]", layout.read_text())
            self.assertIn(f"[0xDBC, data, overlays/{module['name']}/image_view]", layout.read_text())

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
                self.assertEqual(command, 93000)
                descriptor = struct.unpack_from("<8B6h", data, 0xDD8 + command % 1000 * 20)
                self.assertEqual(descriptor, (128, 128, 128, 64, 64, 32, 22, 0, 200, 50, 40, 40, 140, 160))
                self.assertEqual(0xDBC + 28, 0xDD8)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                self.assertIn("RotTransSV = 0x80089C48;", bindings)
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<874I", data[4:0xDAC])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 53)
                self.assertEqual(set(calls), addresses)
                slot = int(row["slot"])
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 0x820 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model162Config)": 20, "sizeof(Model162State)": 0x820,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(POLY_FT4)": 40, "sizeof(POLY_F4)": 24,
            "sizeof(GsIMAGE)": 28, "sizeof(GsLINE)": 16, "sizeof(DVECTOR)": 4,
        }
        for typename, fields in (
            ("Model162State", (("points", 4), ("anchor", 0x804), ("rotation", 0x80C),
                               ("texture", 0x814), ("completed", 0x818), ("elapsed", 0x81C))),
            ("Model162Config", (("radius", 8), ("half_width", 10), ("line_duration", 12),
                                ("quad_duration", 14), ("line_delay", 16), ("quad_delay", 18))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 22)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant162_entry.h")
