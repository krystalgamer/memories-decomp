import csv
import hashlib
import json
from pathlib import Path
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments
from progress import load_spanish_overlay_inventories
from verify_inputs import load_checksum_manifest


class SpanishModelVariant432Tests(unittest.TestCase):
    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [row for row in manifest["modules"]
                        if row["name"].startswith("spanish_model_variant_401_")]

    def test_loader_slices_and_independent_images(self):
        self.assertEqual(len(self.modules), 2)
        checksums = load_checksum_manifest(self.config / "files.sha256")
        for slot, module in enumerate(self.modules):
            self.assertEqual(module["name"], f"spanish_model_variant_401_stage{9 + slot}_slot{slot}")
            self.assertEqual(module["archive"], "game/spain/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sector_offset"], 351 * 276 + 200 + slot * 10)
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(int(module["load_address"], 0), 0x8013B000 + slot * 0x40000)
            self.assertNotIn("duplicate_sector_offsets", module)

    def test_two_helpers_are_selected_c_and_other_functions_remain_unmatched(self):
        counts = load_spanish_overlay_inventories(ROOT)
        for slot, module in enumerate(self.modules):
            layout = ROOT / module["layout"]
            base = int(module["load_address"], 0)
            source = "src/overlays/spanish_model_variant/variant432_draw" + ("_slot1" if slot else "") + ".c"
            bands = "src/overlays/spanish_model_variant/variant432_bands" + ("_slot1" if slot else "") + ".c"
            manifest = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(manifest["functions"], [{
                "address": f"0x{base + 0x134C:X}", "profile": "gcc_2_8_1_g0_split",
                "size": "0x350", "source": source,
            }, {
                "address": f"0x{base + 0x1AC4:X}", "profile": "gcc_2_8_1_g0_split",
                "size": "0x41C", "source": bands,
            }])
            selected = c_segments(ROOT, layout)
            self.assertEqual([entry["source"] for entry in selected], [source, bands])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"], 0) - base, int(r["size"], 0)) for r in rows],
                             [(4, 3496), (0xDAC, 1440), (0x134C, 848),
                              (0x169C, 1064), (0x1AC4, 1052)])
            self.assertEqual([r["status"] for r in rows],
                             ["unmatched_asm", "unmatched_asm", "matching_c",
                              "unmatched_asm", "matching_c"])
            self.assertEqual(counts[layout.stem]["function_count"], 5)
            self.assertEqual(counts[layout.stem]["matching_c_function_count"], 2)
            self.assertEqual(counts[layout.stem]["matching_c_bytes"], 1900)
            for offset in (4, 0xDAC, 0x169C):
                self.assertIn(f"[0x{offset:X}, asm,", layout.read_text())

    def test_unclassified_storage_is_real_and_not_a_linker_alias(self):
        for module in self.modules:
            base = int(module["load_address"], 0)
            layout = ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base:X} = 0x{base:X}; // type:u8 size:0x4 defined:true", symbols)
            self.assertIn(f"D_{base + 0x1EE0:X} = 0x{base + 0x1EE0:X}; // type:u8 size:0x3120 defined:true",
                          symbols)
            self.assertIn(f"[0x1EE0, data, overlays/{module['name']}/unclassified_tail]", layout.read_text())
            bindings = (ROOT / module["linker_symbols"]).read_text()
            self.assertNotRegex(bindings, r"=\s*0x801[37]")
            self.assertIn("ReadRotMatrix = 0x800872A8;", bindings)
            self.assertIn("SetRotMatrix = 0x80087738;", bindings)

    def test_source_keeps_canonical_types_and_observed_stack_interval(self):
        directory = ROOT / "src/overlays/spanish_model_variant"
        source = (directory / "variant432_draw.c").read_text()
        header = (directory / "variant432_draw.h").read_text()
        self.assertIn('#include "../../types.h"', source)
        self.assertIn("u8 unknown_stack[16];", source)
        self.assertIn("SVECTOR points[4][4];", header)
        self.assertIn("POLY_GT4 quad;", header)
        self.assertIn('#include "../../psyq/libgte.h"', header)
        self.assertNotRegex(source, r"\b(?:asm|__asm__|register)\b")
        self.assertIn("#define func_8013C34C func_8017C34C",
                      (directory / "variant432_draw_slot1.c").read_text())
        with (ROOT / "notes/overlays/spanish-model-variant432-attempts.csv").open() as handle:
            attempts = [row for row in csv.DictReader(handle) if row["function_offset"] == "0x134C"]
        self.assertEqual([r["result"] for r in attempts], ["nonmatching", "matched"])
        self.assertEqual(attempts[-1]["different_words"], "0")

    def test_band_helper_keeps_closed_rows_and_matching_loop_order(self):
        directory = ROOT / "src/overlays/spanish_model_variant"
        source = (directory / "variant432_bands.c").read_text()
        header = (directory / "variant432_bands.h").read_text()
        self.assertIn('#include "../../types.h"', source)
        self.assertIn("SVECTOR points[2][17];", header)
        self.assertIn("ModelVariant432Band bands[3];", header)
        self.assertIn("POLY_GT4 quads[16];", header)
        self.assertIn("for (j = 0, quad = work->quads; j < 16; j++, quad++)", source)
        self.assertIn("&band->points[1][j + 1]", source)
        self.assertIn("band->phase += work->step * 160;", source)
        self.assertNotRegex(source, r"\b(?:asm|__asm__|register)\b")
        self.assertIn("#define func_8013CAC4 func_8017CAC4",
                      (directory / "variant432_bands_slot1.c").read_text())
        with (ROOT / "notes/overlays/spanish-model-variant432-attempts.csv").open() as handle:
            attempts = [row for row in csv.DictReader(handle) if row["function_offset"] == "0x1AC4"]
        self.assertEqual([row["result"] for row in attempts], ["nonmatching", "matched"])
        self.assertEqual(attempts[-1]["different_words"], "0")

    @unittest.skipUnless((ROOT / "game/spain/DATA/MODEL.MRG").exists(), "legal Spanish MODEL input required")
    def test_legal_images_and_complete_direct_control_flow(self):
        with (ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as archive:
            for slot, module in enumerate(self.modules):
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                self.assertEqual(struct.unpack_from("<I", data)[0], 432 + slot * 150)
                self.assertEqual(struct.unpack_from("<III", data, 0xBD4),
                                 (0x8FA400E8, 0x0C000000 | ((base + 0x134C) >> 2 & 0x3FFFFFF), 0))
                self.assertEqual(struct.unpack_from("<III", data, 0xBE0),
                                 (0x8FA400E8, 0x0C000000 | ((base + 0x1AC4) >> 2 & 0x3FFFFFF), 0))
                spans = [(4, 0xDAC), (0xDAC, 0x134C), (0x134C, 0x169C),
                         (0x169C, 0x1AC4), (0x1AC4, 0x1EE0)]
                local_calls = set()
                for start, end in spans:
                    pending, visited, returns = [start], set(), set()
                    while pending:
                        pc = pending.pop()
                        self.assertTrue(start <= pc < end and pc % 4 == 0)
                        if pc in visited:
                            continue
                        visited.add(pc)
                        word = struct.unpack_from("<I", data, pc)[0]
                        op = word >> 26
                        if op in (1, 2, 3, 4, 5, 6, 7) or word == 0x03E00008:
                            self.assertLess(pc + 4, end)
                            visited.add(pc + 4)
                            delay = struct.unpack_from("<I", data, pc + 4)[0]
                            self.assertNotIn(delay >> 26, (1, 2, 3, 4, 5, 6, 7))
                            self.assertFalse(delay >> 26 == 0 and delay & 63 in (8, 9))
                            if word == 0x03E00008:
                                returns.add(pc)
                            elif op in (2, 3):
                                target = ((base + pc + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                                if op == 2:
                                    pending.append(target - base)
                                else:
                                    pending.append(pc + 8)
                                    if base <= target < base + len(data):
                                        local_calls.add(target - base)
                            else:
                                imm = (word & 0xFFFF) - (0x10000 if word & 0x8000 else 0)
                                pending.extend((pc + 8, pc + 4 + imm * 4))
                        else:
                            self.assertFalse(op == 0 and word & 63 in (8, 9))
                            pending.append(pc + 4)
                    self.assertEqual(visited, set(range(start, end, 4)))
                    self.assertEqual(returns, {end - 8})
                self.assertEqual(local_calls, {start for start, end in spans[1:]})
