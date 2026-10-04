import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as family373
from tools.project.tests import test_spanish_model_variant450 as spanish_lines
from tools.project.tests import test_spanish_model_variant450_quads as spanish_quads


class FrenchModelVariant450Tests(family435.FrenchModelVariant435Tests):
    family = 450
    module_count = 2
    distinct_images = 2
    binding_count = 35
    tail_start = 0x3940
    spans = ((4, 0xDD8), (0xDD8, 0x1794), (0x1794, 0x1ECC), (0x1ECC, 0x27C0),
             (0x27C0, 0x2D88), (0x2D88, 0x310C), (0x310C, 0x3940))
    helpers = ((0xDD8, 2492, "ribbons", "func_8013BDD8"),
               (0x1794, 1848, "bands", "func_8013C794"),
               (0x27C0, 1480, "quads", "func_8013D7C0"),
               (0x2D88, 900, "lines", "func_8013DD88"))
    source_directories = {"quads": "spanish_model_variant", "lines": "spanish_model_variant"}
    reachable_helpers = {0xDD8, 0x27C0, 0x2D88}
    local_call_targets = {0xDD8, 0x1ECC, 0x27C0, 0x2D88}
    models_by_stage = ((9, (174,)),)
    entry_anchors = {0xC38: 0x02602021, 0xC40: 0x02602021,
                     0x27C0: 0x27BDFEF8, 0x2D88: 0x27BDFEE0,
                     0x20: 0x26D80F00, 0x24: 0xAFB80088, 0x50: 0x26D541E0,
                     0x374: 0x0C020BAA, 0x378: 0x02A02021,
                     0x3D4: 0x26B50028, 0x3D8: 0x0C020BAA,
                     0x644: 0x8FB80088, 0x64C: 0x27110190,
                     0x688: 0xA234FF90, 0x68C: 0xA234FF91, 0x690: 0xA234FF92,
                     0x718: 0x26310208, 0x720: 0x2A420003, 0x724: 0x27180208,
                     0x1794: 0x27BDFE68, 0xDD8: 0x27BDFDE8, 0xC74: 0x02602021,
                     0x18: 0x26D80270, 0x1C: 0xAFB80084,
                     0x4FC: 0x00009021, 0x508: 0x241E1000, 0x50C: 0x24170080,
                     0x514: 0x0240A021, 0x518: 0x27110210, 0x528: 0x00101240,
                     0x52C: 0x00021023, 0x53C: 0x000210C3, 0x544: 0xAC6201D0,
                     0x54C: 0x2A020009, 0x55C: 0x240200C0, 0x56C: 0xA237FF10,
                     0x570: 0xA237FF11, 0x574: 0xA222FF12, 0x594: 0xAE20FFE4,
                     0x59C: 0x2694FE00, 0x5F8: 0x27180218, 0x630: 0x2A420006,
                     0x638: 0x26310218}

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/spanish_model_variant"
        for label, symbol in (("quads", "func_8013D7C0"), ("lines", "func_8013DD88")):
            self.assertEqual((directory / f"variant450_{label}_slot1.c").read_text(),
                             '#include "../../types.h"\n'
                             f'#define {symbol} {symbol.replace("8013", "8017")}\n'
                             f'#include "variant450_{label}.c"\n')
            self.assertNotRegex((directory / f"variant450_{label}.c").read_text(),
                                r"\b(?:extern|asm|__asm__|register|volatile)\b")
        with (family435.ROOT / "notes/overlays/french-model-variant450-attempts.csv").open() as handle:
            rows = [row for row in csv.DictReader(handle) if row["function_offset"] in ("0x27C0", "0x2D88")]
        self.assertEqual(len(rows), 8)
        self.assertEqual([row["result"] for row in rows], ["text_exact"] * 4 + ["matched"] * 4)
        for row in rows:
            offset, slot = int(row["function_offset"], 0), int(row["slot"])
            label, size = ("quads", 1480) if offset == 0x27C0 else ("lines", 900)
            self.assertIn(offset, (0x27C0, 0x2D88))
            self.assertIn(slot, (0, 1))
            source = directory / f"variant450_{label}{'_slot1' if slot else ''}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((int(row["instruction_bytes"]), row["different_words"], row["profile"]),
                             (size, "0", "gcc_2_8_1_g0_split"))
        expected = {("0x27C0", "0"), ("0x27C0", "1"), ("0x2D88", "0"), ("0x2D88", "1")}
        self.assertEqual({(row["function_offset"], row["slot"]) for row in rows[:4]}, expected)
        self.assertEqual({(row["function_offset"], row["slot"]) for row in rows[4:]}, expected)

    def test_french_caller_callees_and_measured_overlap(self):
        root = family435.ROOT
        archive_path, resident_path = root / "game/france/DATA/MODEL.MRG", root / "game/france/SLES_039.48"
        if not archive_path.exists() or not resident_path.exists():
            self.skipTest("legal French MODEL and resident inputs required")
        resident = resident_path.read_bytes()
        self.assertEqual(hashlib.sha256(resident).hexdigest(),
                         "57ecdfb9a9e1faf8b342fe7c7304c23723810861f3ab2fa3bef9eb27b5146b44")
        pointers = struct.unpack_from("<14I", resident, 0x800)
        with (root / "config/sles_03948/functions.csv").open() as handle:
            starts = {int(row["address"], 0) for row in csv.DictReader(handle)}
        with archive_path.open("rb") as archive:
            for module in self.modules:
                slot, base = int(self.instances[module["name"]]["slot"]), int(module["load_address"], 0)
                self.assertEqual(int(self.instances[module["name"]]["command_word"]), 616000)
                self.assertEqual(pointers[5 + slot], base)
                context = pointers[9 + slot]
                self.assertEqual(context, 0x80136000 + slot * 0x40000)
                start, end = pointers[3 + slot], pointers[3 + slot] + 4096
                for extent, overlap in ((0x42F8, 760), (0x42FC, 764), (0x42D0, 720)):
                    self.assertEqual(max(0, min(context + extent, end) - max(context, start)), overlap)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                for call, target in ((0xC34, 0x2D88), (0xC3C, 0x27C0), (0xC70, 0xDD8)):
                    self.assertEqual(struct.unpack_from("<II", data, call),
                                     (0x0C000000 | ((base + target) >> 2 & 0x3FFFFFF), 0x02602021))
                external = set()
                for start, end in self.spans:
                    for word, in struct.iter_unpack("<I", data[start:end]):
                        if word >> 26 == 3:
                            target = 0x80000000 | ((word & 0x3FFFFFF) << 2)
                            if not base <= target < base + 20480:
                                external.add(target)
                bindings = (root / module["linker_symbols"]).read_text()
                declared = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                self.assertEqual(external, declared)
                self.assertTrue(external <= starts)
                self.assertIn("GsSortPoly = 0x800842A8;", bindings)
                self.assertIn("RotTransPers4 = 0x80087958;", bindings)
                self.assertNotIn("func_spanish_", bindings)

    def test_band_canonical_bindings_agree_with_splat_symbols(self):
        root = family435.ROOT
        for module in self.modules:
            bindings = (root / module["linker_symbols"]).read_text()
            layout = root / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for canonical, alias, address in (
                ("rcos", "func_french_800866F8", 0x800866F8),
                ("rsin", "func_french_80086628", 0x80086628),
                ("RotTransPers", "func_french_80087868", 0x80087868),
            ):
                self.assertIn(f"{canonical} = 0x{address:X};", bindings)
                self.assertIn(f"{canonical} = 0x{address:X}; // type:func absolute:true", symbols)
                self.assertNotIn(alias + " =", bindings)
                self.assertNotIn(alias + " =", symbols)

    def test_band_attempts_and_symbol_only_wrapper(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        with (family435.ROOT / "notes/overlays/french-model-variant450-attempts.csv").open() as handle:
            all_rows = list(csv.DictReader(handle))
        self.assertEqual(len(all_rows), 34)
        rows = [row for row in all_rows if row["function_offset"] == "0x1794"]
        self.assertEqual([row["result"] for row in rows],
                         ["mismatch"] * 10 + ["text_exact"] * 2 + ["matched"] * 2)
        expected = [(1808, 377), (1848, 12), (1848, 8), (1808, 373),
                    (1848, 2), (1848, 0), (1848, 0)]
        self.assertEqual([(int(row["instruction_bytes"]), int(row["different_words"])) for row in rows],
                         [pair for pair in expected for _ in range(2)])
        self.assertEqual([row["slot"] for row in rows], ["0", "1"] * 7)
        for slot, row in enumerate(rows[-2:]):
            source = directory / f"variant450_bands{'_slot1' if slot else ''}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
        self.assertEqual((directory / "variant450_bands_slot1.c").read_text(),
                         '#include "../../types.h"\n\n#define func_8013C794 func_8017C794\n'
                         '#include "variant450_bands.c"\n')
        self.assertNotRegex((directory / "variant450_bands.c").read_text(),
                            r"\b(?:extern|asm|__asm__|register|volatile)\b")

    def test_band_projection_and_alternating_submission(self):
        text = (family435.ROOT / "src/overlays/french_model_variant/variant450_bands.c").read_text()
        for expression in (
            "PSXLONG flags[3][9];", "s16 i, j, width;",
            "ratan2(work->view_direction[2], work->view_direction[0]) + 3072",
            "ratan2(work->view_direction[1], work->view_direction[0]);",
            "width = work->radius / 128;", "width = work->radius * 12 / 1024;",
            "i < 3", "j < 9", "angle = j * 512",
            "work->delta[0] * i / 3 + (rcos(angle) * 256 >> 12)",
            "work->delta[1] * i / 3 + (rsin(angle) * 256 >> 12)",
            "work->delta[2] * i / 3", "if (j == 8)",
            "band->depth[j] = RotTransPers4(", "&flags[i][8]", "&flags[i][j]",
            "band->angle[j] = ratan2(dy, dx) - 1024;",
            "band->x_offset[8] = rcos(band->angle[j]) * band->width[j] >> 12;",
            "band->y_offset[8] = rsin(band->angle[j]) * band->width[j] >> 12;",
            "for (j = 0, quad = work->packets; j < 8; j++)",
            "band->depth[j] >= 0 && flags[i][j] >= 0",
            "GsSortPoly(quad, ot, (u16)band->depth[j]);",
            "if (!(j & 1))", "quad++;", "quad--;",
        ):
            self.assertIn(expression, text)
        self.assertEqual(text.count(".packed >> 16"), 4)
        self.assertEqual(text.count("GsSortPoly("), 1)
        self.assertEqual(text.count("RotTransPers4("), 2)
        self.assertNotIn("phase", text)

    def test_target_compiled_band_views(self):
        checks = {
            "sizeof(SVECTOR)": 8, "sizeof(Model450Screen)": 4,
            "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(GsCOORDINATE2)": 80, "sizeof(PSXLONG)": 4,
            "sizeof(CVECTOR)": 4, "sizeof(POLY_FT4)": 40,
            "sizeof(Model450Band)": 520, "sizeof(Model450BandView)": 0x42D0,
        }
        for typename, fields in (
            ("Model450Band", (
                ("points", 0), ("screen", 0x48), ("angle", 0x6C),
                ("edges", 0x90), ("edge_screen", 0xD8), ("width", 0xFC),
                ("color", 0x120), ("depth", 0x198),
                ("x_offset", 0x1BC), ("y_offset", 0x1CE))),
            ("Model450BandView", (
                ("bands", 0xF00), ("packets", 0x41E0), ("origin", 0x4268),
                ("delta", 0x427C), ("view_direction", 0x4290),
                ("flags", 0x42A8), ("radius", 0x42CE))),
            ("POLY_FT4", (("x0", 8), ("x1", 16), ("x2", 24), ("x3", 32))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 31)
        family373.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant450_bands.h")

    def test_target_compiled_line_views(self):
        spanish_lines.SpanishModelVariant450Tests.test_target_compiled_partial_view_layout(self)

    def test_target_compiled_quad_views(self):
        spanish_quads.SpanishModelVariant450QuadTests.test_target_compiled_quad_view(self)

    def test_ribbon_attempts_and_symbol_only_wrapper(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        with (family435.ROOT / "notes/overlays/french-model-variant450-attempts.csv").open() as handle:
            rows = [row for row in csv.DictReader(handle) if row["function_offset"] == "0xDD8"]
        self.assertEqual([row["result"] for row in rows],
                         ["mismatch"] * 8 + ["text_exact"] * 2 + ["matched"] * 2)
        expected = [(2536, 586), (2496, 580), (2544, 588), (2492, 13), (2492, 0), (2492, 0)]
        self.assertEqual([(int(row["instruction_bytes"]), int(row["different_words"])) for row in rows],
                         [pair for pair in expected for _ in range(2)])
        self.assertEqual([row["slot"] for row in rows], ["0", "1"] * 6)
        for slot, row in enumerate(rows[-2:]):
            source = directory / f"variant450_ribbons{'_slot1' if slot else ''}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
        self.assertEqual((directory / "variant450_ribbons_slot1.c").read_text(),
                         '#include "../../types.h"\n\n#define func_8013BDD8 func_8017BDD8\n'
                         '#include "variant450_ribbons.c"\n')
        self.assertNotRegex((directory / "variant450_ribbons.c").read_text(),
                            r"\b(?:extern|asm|__asm__|register|volatile)\b")

    def test_ribbon_phase_and_submission(self):
        text = (family435.ROOT / "src/overlays/french_model_variant/variant450_ribbons.c").read_text()
        for expression in (
            "PSXLONG flags[6][9];", "s32 bend_angle;", "bend_angle = progress * 2;",
            "ribbon->tail.delta.vx * progress / 1024",
            "ribbon->tail.delta.vy * progress / 1024",
            "ribbon->tail.delta.vz * progress / 1024",
            "if (ribbon->phase[j] > 0)", "if (ribbon->phase[j] < progress)",
            "if (ribbon->phase[j] <= 1024)", "ribbon->phase[j] += work->frame_step << 6;",
            "if (i == 0 && work->state == 1)", "ribbon->tail.completed = 1;",
            "wave = -work->ripple", "wave += 1300", "if (j == 8)",
            "&flags[i][8]", "&flags[i][j]",
            "ribbon->x_offset[8] = rcos(ribbon->angle[j]) * ribbon->width[j] >> 12;",
            "ribbon->y_offset[8] = rsin(ribbon->angle[j]) * ribbon->width[j] >> 12;",
            "if ((s16)(j % 2) == (work->flags & 1))",
            "quad = &work->packets[0];", "quad = &work->packets[1];",
            "ribbon->depth[j] > 0 && flags[i][j] >= 0 && !ribbon->tail.completed",
            "GsSortPoly(quad, ot, (u16)ribbon->depth[j]);",
            "work->ripple += work->frame_step * 850;",
            "work->ripple2 += work->frame_step << 7;",
        ):
            self.assertIn(expression, text)
        self.assertEqual(text.count(".packed >> 16"), 4)
        self.assertEqual(text.count("RotTransPers4("), 2)
        self.assertEqual(text.count("GsSortPoly("), 1)

    def test_target_compiled_ribbon_views(self):
        checks = {
            "sizeof(SVECTOR)": 8, "sizeof(Model450Screen)": 4,
            "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(GsCOORDINATE2)": 80, "sizeof(PSXLONG)": 4,
            "sizeof(CVECTOR)": 4, "sizeof(POLY_FT4)": 40,
            "sizeof(Model450RibbonTail)": 0x24,
            "sizeof(Model450Ribbon)": 0x218, "sizeof(Model450RibbonView)": 0x42FC,
        }
        for typename, fields in (
            ("Model450RibbonTail", (("completed", 0), ("target", 4), ("delta", 0x14))),
            ("Model450Ribbon", (
                ("points", 0), ("screen", 0x48), ("angle", 0x6C),
                ("edges", 0x90), ("edge_screen", 0xD8), ("width", 0xFC),
                ("color", 0x120), ("secondary_color", 0x124), ("depth", 0x188),
                ("x_offset", 0x1AC), ("y_offset", 0x1BE), ("phase", 0x1D0), ("tail", 0x1F4))),
            ("Model450RibbonView", (
                ("ribbons", 0x270), ("packets", 0x41E0), ("origin", 0x4268),
                ("view_direction", 0x4290), ("flags", 0x42A8), ("frame_step", 0x42B4),
                ("radius", 0x42CE), ("ripple", 0x42E4), ("ripple2", 0x42E8), ("state", 0x42F8))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 37)
        family373.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant450_ribbons.h")
