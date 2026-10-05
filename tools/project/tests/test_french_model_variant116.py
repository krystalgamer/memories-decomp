import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant116Tests(family435.FrenchModelVariant435Tests):
    family = 116
    slot_header_delta = 130
    module_count = 22
    distinct_images = 22
    binding_count = 22
    tail_start = 0x11C0
    spans = ((4, 0x115C),)
    helpers = ((4, 4440, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (27, 422, 556)),
                       (9, (22, 73, 127, 454, 484, 504, 581, 701)))
    entry_anchors = {
        4: 0x27BDF9D8,
        0xD4: 0x00158200, 0xD8: 0x0C021ACE, 0xDC: 0x02002021,
        0x1154: 0x03E00008, 0x1158: 0x27BD0628,
        0x115C: 4096, 0x1160: 4096, 0x1164: 4096, 0x1168: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant116_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013C15C D_8017C15C\n'
                         '#define D_8013C16C D_8017C16C\n'
                         '#define D_8013C1C0 D_8017C1C0\n'
                         '#include "variant116_entry.c"\n')
        source = (directory / "variant116_entry.c").read_text()
        header = (directory / "variant116_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__)\b")
        for declaration in (
            "extern GsIMAGE D_8013C16C[];", "extern Model116Config D_8013C1C0[];",
            "Model116Config *G32 config;", "SVECTOR ring[16];",
            "SVECTOR dots[128];", "SVECTOR sprites[64];", "s32 texture[3];",
            "u16 frame_toggle;",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "static const VECTOR D_8013C15C = {4096, 4096, 4096, 0};",
            "&D_8013C1C0[command]", "config->ring_radius * csin(i * 256)",
            "config->ring_radius * -ccos(i * 256)",
            "(radius * ccos(value) / 4096) * csin(elevation) / 4096",
            "radius * ccos(elevation) / 4096",
            "(radius * csin(value) / 4096) * csin(elevation) / 4096",
            "work->frame_toggle ^= 1;", "setVector(&vertices[0], 0, 0, 0);",
            "dot.attribute = 0x50000000;", "dot.w = 1;", "dot.h = 1;",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("MulMatrix2(&base, &matrix);"), 2)
        self.assertEqual(source.count("depth = RotAverage4("), 2)
        self.assertEqual(source.count("work->elapsed += Model_GetFrameStep();"), 2)

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        with (family435.ROOT / "notes/overlays/french-model-variant116-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 35)
        self.assertEqual(len({(row["attempt"], row["slot"]) for row in rows}), 35)
        expected = {
            "verified-trigonometric-call-semantics": ("4432", "658"),
            "native-dot-projection-walkers": ("4424", "623"),
            "shared-spherical-azimuth-and-scale-value": ("4436", "132"),
            "phase-local-dot-projection-walkers": ("4436", "132"),
            "signed-word-terminal-maximum": ("4440", "101"),
            "native-box-and-projection-setup": ("4440", "99"),
            "shared-radial-progress-work-value": ("4440", "158"),
            "phase-local-ring-and-sprite-rgb": ("4440", "83"),
            "shared-ring-angle-value": ("4448", "615"),
            "ring-local-relative-time": ("4440", "64"),
            "dot-local-elapsed-time": ("4440", "85"),
            "post-projection-dot-cursors": ("4440", "25"),
            "shared-time-after-projection-recovery": ("4440", "8"),
            "compound-first-terminal-maximum": ("4440", "2"),
            "loop-local-initial-ring-angle": ("4440", "2"),
            "direct-ring-trigonometric-angle": ("4440", "0"),
        }
        experiments = [row for row in rows if row["result"] in ("mismatch", "text_exact")]
        self.assertEqual({(row["attempt"], row["slot"]) for row in experiments},
                         {(name, slot) for name in expected for slot in ("0", "1")})
        for row in rows:
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
        for row in experiments:
            self.assertEqual((row["instruction_bytes"], row["different_words"]),
                             expected[row["attempt"]])
            self.assertEqual(row["result"], "text_exact" if row["different_words"] == "0" else "mismatch")
            frame = 1584 if row["attempt"] == "shared-ring-angle-value" else 1576
            self.assertIn(f"Frame {frame}/1576;", row["reason"])
        failure, = [row for row in rows if row["result"] == "blocked_ordered_call_check"]
        self.assertEqual((failure["attempt"], failure["slot"]), ("native-first-candidate", "0"))
        self.assertEqual((failure["instruction_bytes"], failure["different_words"]), ("4432", ""))
        self.assertIn("swapped trigonometric meanings", failure["reason"])
        self.assertIn("No full comparison or slot1 probe completed", failure["reason"])
        terminal = [row for row in rows if row["result"] == "matched"]
        self.assertEqual({row["slot"] for row in terminal}, {"0", "1"})
        for row in terminal:
            source = directory / ("variant116_entry" + ("_slot1" if row["slot"] == "1" else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("4440", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0x116C:X} = 0x{base + 0x116C:X}; // type:u8 size:0x54 defined:true", symbols)
            self.assertIn(f"D_{base + 0x115C:X} = 0x{base + 0x115C:X}; // type:u32 size:0x10 defined:true", symbols)
            slot = int(module["name"][-1])
            source = "overlays/french_model_variant/variant116_entry" + ("_slot1" if slot else "")
            self.assertIn(f"[0x115C, .rodata, {source}]", layout.read_text())
            self.assertIn(f"[0x116C, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (128, 128, 128, 255, 255, 128, 128, 64, 64, 0, 200, 300, 400, 600, 150, 400, 2, 64, 32, 30, 40, 30, 6, 50),
            1: (128, 128, 128, 255, 255, 128, 128, 160, 32, 0, 300, 450, 500, 600, 200, 500, 4, 64, 32, 20, 40, 60, 6, 306),
            2: (160, 160, 128, 255, 255, 128, 96, 64, 32, 0, 300, 350, 250, 600, 150, 300, 3, 64, 32, 20, 40, 30, 6, 76),
            3: (160, 160, 192, 0, 0, 0, 32, 32, 0, 0, 200, 90, 190, 400, 100, 250, 2, 64, 32, 18, 40, 30, 4, 226),
            4: (128, 128, 160, 0, 255, 255, 80, 32, 0, 0, 250, 200, 400, 600, 200, 450, 3, 96, 16, 30, 60, 60, 4, 50),
            7: (128, 192, 192, 0, 255, 255, 0, 32, 80, 0, 250, 200, 350, 600, 200, 450, 4, 96, 24, 40, 60, 50, 4, 60),
            8: (192, 192, 128, 255, 255, 0, 64, 16, 16, 0, 400, 300, 450, 600, 200, 600, 3, 96, 32, 30, 60, 60, 4, 120),
            9: (192, 128, 192, 255, 255, 0, 64, 16, 48, 0, 200, 200, 350, 600, 250, 600, 3, 96, 16, 40, 60, 60, 6, 100),
            10: (192, 128, 128, 255, 255, 0, 48, 48, 32, 0, 200, 200, 350, 600, 250, 600, 2, 96, 16, 40, 60, 60, 4, 96),
        }
        arguments = {(7, 27): 0, (7, 422): 4, (7, 556): 0,
                     (9, 22): 10, (9, 73): 2, (9, 127): 3, (9, 454): 1,
                     (9, 484): 9, (9, 504): 7, (9, 581): 3, (9, 701): 8}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot = int(row["slot"])
                argument = arguments[int(row["stage"]) - slot, int(row["model"])]
                self.assertEqual(int(row["command_word"]), 47000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                observed.add(argument)
                self.assertEqual(struct.unpack_from("<10B14h", data, 0x11C0 + 38 * argument),
                                 descriptors[argument])
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                self.assertIn("ccos = 0x800868A8;", bindings)
                self.assertIn("csin = 0x80086B38;", bindings)
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1110I", data[4:0x115C])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 56)
                self.assertEqual(set(calls), addresses)
                self.assertEqual(calls[3:5], [0x80086B38, 0x800868A8])
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 0x698 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model116Config)": 38, "sizeof(Model116State)": 0x698,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(POLY_FT4)": 40, "sizeof(GsBOXF)": 16, "sizeof(GsIMAGE)": 28,
            "sizeof(DVECTOR)": 4, "sizeof(PSXLONG)": 4,
        }
        for typename, fields in (
            ("Model116State", (("ring", 4), ("dots", 0x84), ("sprites", 0x484),
                               ("texture", 0x684), ("frame_toggle", 0x690), ("elapsed", 0x694))),
            ("Model116Config", (("ring_height", 0xA), ("ring_width", 0xC), ("ring_radius", 0xE),
                                ("dot_radius", 0x10), ("sprite_size", 0x12), ("sprite_radius", 0x14),
                                ("ring_count", 0x16), ("dot_count", 0x18), ("sprite_count", 0x1A),
                                ("ring_duration", 0x1C), ("dot_duration", 0x1E),
                                ("sprite_duration", 0x20), ("ring_stagger", 0x22),
                                ("initial_delay", 0x24))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 30)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant116_entry.h")
