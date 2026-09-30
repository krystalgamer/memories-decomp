import csv
import json
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from progress import load_overlay_inventories
from verify_inputs import load_checksum_manifest


class ModelVariantToolchainTests(unittest.TestCase):
    config = ROOT / "config/slus_01411"

    def profiles(self):
        return json.loads((self.config / "compiler_profiles.json").read_text())["profiles"]

    def test_cdk_release_is_pinned(self):
        release = json.loads((ROOT / "tools/bootstrap/old_gcc_272_prebuilt.json").read_text())
        self.assertEqual(release["release"], "0.17")
        self.assertTrue(release["url"].endswith("/0.17/gcc-2.7.2-cdk.tar.gz"))
        self.assertEqual(release["lock"]["prefix"], "tools/toolchains/gcc-2.7.2-cdk")
        self.assertEqual(release["dumpversion"], "cygnus-2.7.2-970404 SN32.3.7.0004")
        self.assertEqual(set(release["files"]), {"gcc", "cc1", "cpp"})

    def test_profile_differs_from_gcc_2_8_1_g0_only_in_compiler(self):
        profiles = self.profiles()
        cdk = dict(profiles["gcc_2_7_2_cdk_g0"])
        base = dict(profiles["gcc_2_8_1_g0"])
        self.assertEqual(cdk.pop("compiler"), "tools/toolchains/gcc-2.7.2-cdk/bin/mips-sony-psx-gcc")
        base.pop("compiler")
        self.assertEqual(cdk, base)

    def test_only_model_variant_units_use_the_cdk_profile(self):
        users = set()
        for path in sorted((self.config / "overlays").glob("*_matching_c.json")):
            for entry in json.loads(path.read_text())["functions"]:
                if entry["profile"].startswith("gcc_2_7_2_cdk_"):
                    users.add(entry["source"])
        self.assertEqual(users, {
            "src/overlays/model_variant/variant320_rings.c",
            "src/overlays/model_variant/variant320_rings_slot1.c",
            "src/overlays/model_variant/variant321_rings.c",
            "src/overlays/model_variant/variant321_rings_slot1.c",
            "src/overlays/model_variant/variant321_strand.c",
            "src/overlays/model_variant/variant321_strand_slot1.c",
            "src/overlays/model_variant/variant324_draw.c",
            "src/overlays/model_variant/variant324_draw_slot1.c",
            "src/overlays/model_variant/variant324_rings.c",
            "src/overlays/model_variant/variant324_rings_slot1.c",
            "src/overlays/model_variant/variant324_spokes.c",
            "src/overlays/model_variant/variant324_spokes_slot1.c",
            "src/overlays/model_variant/variant376_rings.c",
            "src/overlays/model_variant/variant376_rings_slot1.c",
            "src/overlays/model_variant/variant391_draw.c",
            "src/overlays/model_variant/variant391_draw_slot1.c",
            "src/overlays/model_variant/variant391_entry.c",
            "src/overlays/model_variant/variant391_entry_slot1.c",
            "src/overlays/model_variant/variant391_update.c",
            "src/overlays/model_variant/variant391_update_slot1.c",
            "src/overlays/model_variant/variant397_quad.c",
            "src/overlays/model_variant/variant397_quad_slot1.c",
            "src/overlays/model_variant/variant397_rings.c",
            "src/overlays/model_variant/variant397_rings_slot1.c",
            "src/overlays/model_variant/variant397_sheets.c",
            "src/overlays/model_variant/variant397_sheets_slot1.c",
            "src/overlays/model_variant/variant397_spokes.c",
            "src/overlays/model_variant/variant397_spokes_slot1.c",
            "src/overlays/model_variant/variant397_webs.c",
            "src/overlays/model_variant/variant397_webs_slot1.c",
            "src/overlays/model_variant/variant398_sheets.c",
            "src/overlays/model_variant/variant398_sheets_slot1.c",
            "src/overlays/model_variant/variant398_webs.c",
            "src/overlays/model_variant/variant398_webs_slot1.c",
            "src/overlays/model_variant/variant401_quad.c",
            "src/overlays/model_variant/variant401_quad_slot1.c",
            "src/overlays/model_variant/variant401_rings.c",
            "src/overlays/model_variant/variant401_rings_slot1.c",
            "src/overlays/model_variant/variant401_spokes.c",
            "src/overlays/model_variant/variant401_spokes_slot1.c",
            "src/overlays/model_variant/variant404_quad.c",
            "src/overlays/model_variant/variant404_quad_slot1.c",
            "src/overlays/model_variant/variant404_rings.c",
            "src/overlays/model_variant/variant404_rings_slot1.c",
            "src/overlays/model_variant/variant404_sheets.c",
            "src/overlays/model_variant/variant404_sheets_slot1.c",
            "src/overlays/model_variant/variant404_spokes.c",
            "src/overlays/model_variant/variant404_spokes_slot1.c",
            "src/overlays/model_variant/variant404_webs.c",
            "src/overlays/model_variant/variant404_webs_slot1.c",
            "src/overlays/model_variant/variant405_bands.c",
            "src/overlays/model_variant/variant405_bands_slot1.c",
            "src/overlays/model_variant/variant405_quad.c",
            "src/overlays/model_variant/variant405_quad_slot1.c",
            "src/overlays/model_variant/variant405_rings.c",
            "src/overlays/model_variant/variant405_rings_slot1.c",
            "src/overlays/model_variant/variant405_spokes.c",
            "src/overlays/model_variant/variant405_spokes_slot1.c",
            "src/overlays/model_variant/variant405_webs.c",
            "src/overlays/model_variant/variant405_webs_slot1.c",
            "src/overlays/model_variant/variant407_petals.c",
            "src/overlays/model_variant/variant407_petals_slot1.c",
            "src/overlays/model_variant/variant414_rings.c",
            "src/overlays/model_variant/variant414_rings_slot1.c",
            "src/overlays/model_variant/variant414_spokes.c",
            "src/overlays/model_variant/variant414_spokes_slot1.c",
            "src/overlays/model_variant/variant415_bands.c",
            "src/overlays/model_variant/variant415_bands_slot1.c",
            "src/overlays/model_variant/variant415_draw.c",
            "src/overlays/model_variant/variant415_draw_slot1.c",
            "src/overlays/model_variant/variant415_layers.c",
            "src/overlays/model_variant/variant415_layers_slot1.c",
            "src/overlays/model_variant/variant416_quad.c",
            "src/overlays/model_variant/variant416_quad_slot1.c",
            "src/overlays/model_variant/variant416_rings.c",
            "src/overlays/model_variant/variant416_rings_slot1.c",
            "src/overlays/model_variant/variant416_spokes.c",
            "src/overlays/model_variant/variant416_spokes_slot1.c",
            "src/overlays/model_variant/variant418_bands.c",
            "src/overlays/model_variant/variant418_bands_slot1.c",
            "src/overlays/model_variant/variant418_quad.c",
            "src/overlays/model_variant/variant418_quad_slot1.c",
            "src/overlays/model_variant/variant418_ribbons.c",
            "src/overlays/model_variant/variant418_ribbons_slot1.c",
            "src/overlays/model_variant/variant418_rings.c",
            "src/overlays/model_variant/variant418_rings_slot1.c",
            "src/overlays/model_variant/variant418_sheet.c",
            "src/overlays/model_variant/variant418_sheet_slot1.c",
            "src/overlays/model_variant/variant418_spokes.c",
            "src/overlays/model_variant/variant418_spokes_slot1.c",
            "src/overlays/model_variant/variant418_webs.c",
            "src/overlays/model_variant/variant418_webs_slot1.c",
            "src/overlays/model_variant/variant422_curtains.c",
            "src/overlays/model_variant/variant422_curtains_slot1.c",
            "src/overlays/model_variant/variant422_sheets.c",
            "src/overlays/model_variant/variant422_sheets_slot1.c",
            "src/overlays/model_variant/variant422_webs.c",
            "src/overlays/model_variant/variant422_webs_slot1.c",
            "src/overlays/model_variant/variant423_quad.c",
            "src/overlays/model_variant/variant423_quad_slot1.c",
            "src/overlays/model_variant/variant423_rings.c",
            "src/overlays/model_variant/variant423_rings_slot1.c",
            "src/overlays/model_variant/variant423_spokes.c",
            "src/overlays/model_variant/variant423_spokes_slot1.c",
            "src/overlays/model_variant/variant425_bands.c",
            "src/overlays/model_variant/variant425_bands_slot1.c",
            "src/overlays/model_variant/variant425_quad.c",
            "src/overlays/model_variant/variant425_quad_slot1.c",
            "src/overlays/model_variant/variant425_rings.c",
            "src/overlays/model_variant/variant425_rings_slot1.c",
            "src/overlays/model_variant/variant425_spokes.c",
            "src/overlays/model_variant/variant425_spokes_slot1.c",
            "src/overlays/model_variant/variant425_webs.c",
            "src/overlays/model_variant/variant425_webs_slot1.c",
            "src/overlays/model_variant/variant428_quad.c",
            "src/overlays/model_variant/variant428_quad_slot1.c",
            "src/overlays/model_variant/variant428_rings.c",
            "src/overlays/model_variant/variant428_rings_slot1.c",
            "src/overlays/model_variant/variant428_sheets.c",
            "src/overlays/model_variant/variant428_sheets_slot1.c",
            "src/overlays/model_variant/variant428_spokes.c",
            "src/overlays/model_variant/variant428_spokes_slot1.c",
            "src/overlays/model_variant/variant428_webs.c",
            "src/overlays/model_variant/variant428_webs_slot1.c",
            "src/overlays/model_variant/variant443_sheets.c",
            "src/overlays/model_variant/variant443_sheets_slot1.c",
            "src/overlays/model_variant/variant443_strand.c",
            "src/overlays/model_variant/variant443_strand_slot1.c",
            "src/overlays/model_variant/variant448_quad.c",
            "src/overlays/model_variant/variant448_quad_slot1.c",
            "src/overlays/model_variant/variant448_rings.c",
            "src/overlays/model_variant/variant448_rings_slot1.c",
            "src/overlays/model_variant/variant448_spokes.c",
            "src/overlays/model_variant/variant448_spokes_slot1.c",
            "src/overlays/model_variant/variant448_webs.c",
            "src/overlays/model_variant/variant448_webs_slot1.c",
            "src/overlays/model_variant/variant458_quads.c",
            "src/overlays/model_variant/variant458_quads_slot1.c",
            "src/overlays/model_variant/variant458_sheets.c",
            "src/overlays/model_variant/variant458_sheets_slot1.c",
            "src/overlays/model_variant/variant458_strand.c",
            "src/overlays/model_variant/variant458_strand_slot1.c",
            "src/overlays/model_variant/variant459_curtains.c",
            "src/overlays/model_variant/variant459_curtains_slot1.c",
            "src/overlays/model_variant/variant459_sheet.c",
            "src/overlays/model_variant/variant459_sheet_slot1.c",
            "src/overlays/model_variant/variant459_webs.c",
            "src/overlays/model_variant/variant459_webs_slot1.c",
        })

    def test_first_variant_image(self):
        manifest = json.loads((self.config / "overlays.json").read_text())
        (module,) = [m for m in manifest["modules"] if m["name"] == "model_variant_1_pos0_slot0"]
        checksums = load_checksum_manifest(self.config / "files.sha256")
        self.assertEqual(module["archive"], "game/DATA/MODEL.MRG")
        self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
        # Record-relative sectors 180-189 of record 1.
        self.assertEqual(module["sector_offset"], 1 * 276 + 180)
        self.assertEqual(module["sector_count"], 10)
        self.assertEqual(module["load_address"], "0x8013B000")
        layout = ROOT / module["layout"]
        with layout.with_name(layout.stem + "_functions.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 9)
        self.assertEqual(
            [(r["address"], r["status"]) for r in rows if r["status"] == "matching_c"],
            [("0x8013CD04", "matching_c"), ("0x8013D8FC", "matching_c"), ("0x8013DE54", "matching_c"),
             ("0x8013E168", "matching_c"), ("0x8013E4E8", "matching_c")],
        )
        counts = load_overlay_inventories(ROOT)["model_variant_1_pos0_slot0"]
        self.assertEqual(counts["function_count"], 9)
        self.assertEqual(counts["matching_c_function_count"], 5)
        self.assertEqual(counts["matching_c_bytes"], 0x70C + 0x558 + 0x314 + 0x380 + 0x368)

    @unittest.skipUnless((ROOT / "game/DATA/MODEL.MRG").is_file(), "requires the retail MODEL input")
    def test_header_families_share_text(self):
        manifest = json.loads((self.config / "overlays.json").read_text())
        modules = {m["name"]: m for m in manifest["modules"] if m["name"].startswith("model_variant_")}
        def names(pos0, stage9=(), stage10_slot1=(), stage7=(), stage8_slot1=()):
            return ({f"model_variant_{m}_pos0_slot0" for m in pos0} | {f"model_variant_{m}_stage9_slot0" for m in stage9}
                    | {f"model_variant_{m}_stage10_slot1" for m in stage10_slot1}
                    | {f"model_variant_{m}_stage7_slot0" for m in stage7}
                    | {f"model_variant_{m}_stage8_slot1" for m in stage8_slot1})

        families = {
            397: (names({2, 20, 87, 108, 138, 193, 573}, {152, 168, 170, 388, 427}), 0x2DE4, 5),
            404: (names({84, 162}, {88, 114, 184, 369}), 0x40FC, 5),
            405: (names({1, 550}), 0x3850, 5),
            418: (names({34, 71, 124, 182, 279, 361, 491, 580, 640}, {166, 275, 469, 590}), 0x396C, 7),
            428: (names({187, 596}, {239, 361, 368, 478}), 0x359C, 5),
            401: (names(set(), {410}), 0x3124, 3),
            414: (names({401}), 0x2F58, 2),
            416: (names({180, 440}), 0x30D0, 3),
            423: (names(set(), {262, 631}), 0x2B38, 3),
            425: (names({259, 630}), 0x43FC, 5),
            448: (names(set(), {108, 573}), 0x43E4, 4),
            443: (names({70, 460, 469, 704}, {44, 98, 161, 370, 400, 458, 462, 558}), 0x2888, 2),
            (443, "model 125"): (names({125}), 0x4D0C, 2),
            # The Spanish header-408 source, built for North America.
            391: (names(set(), {54}), 0x146C, 3),
            541: (names(set(), stage10_slot1={54}), 0x146C, 3),
            # The Spanish header-337 and header-432 sources, built for North America.
            320: (names(set(), {110, 159}, stage7={410}), 0x1F44, 1),
            470: (names(set(), stage10_slot1={110, 159}, stage8_slot1={410}), 0x1F44, 1),
            415: (names(set(), {401}), 0x1F7C, 3),
            565: (names(set(), stage10_slot1={401}), 0x1F7C, 3),
            # Slot-1 images at 0x8017B000: each header is its slot-0 header plus 150,
            # built from the same C.
            547: (names(set(), stage8_slot1={2, 20, 87, 108, 138, 193, 573}, stage10_slot1={152, 168, 170, 388, 427}), 0x2DE4, 5),
            551: (names(set(), stage8_slot1=set(), stage10_slot1={410}), 0x3124, 3),
            554: (names(set(), stage8_slot1={84, 162}, stage10_slot1={88, 114, 184, 369}), 0x40FC, 5),
            555: (names(set(), stage8_slot1={1, 550}, stage10_slot1=set()), 0x3850, 5),
            564: (names(set(), stage8_slot1={401}, stage10_slot1=set()), 0x2F58, 2),
            566: (names(set(), stage8_slot1={180, 440}, stage10_slot1=set()), 0x30D0, 3),
            568: (names(set(), stage8_slot1={34, 71, 124, 182, 279, 361, 491, 580, 640}, stage10_slot1={166, 275, 469, 590}), 0x396C, 7),
            573: (names(set(), stage8_slot1=set(), stage10_slot1={262, 631}), 0x2B38, 3),
            575: (names(set(), stage8_slot1={259, 630}, stage10_slot1=set()), 0x43FC, 5),
            578: (names(set(), stage8_slot1={187, 596}, stage10_slot1={239, 361, 368, 478}), 0x359C, 5),
            593: (names(set(), stage8_slot1={70, 460, 469, 704}, stage10_slot1={44, 98, 161, 370, 400, 458, 462, 558}), 0x2888, 2),
            (593, "model 125"): (names(set(), stage8_slot1={125}), 0x4D0C, 2),
            # Header 459: the header-418 webs with header-425 constants.
            459: (names(set(), {712}, stage10_slot1=set(), stage8_slot1=set()), 0x3338, 3),
            609: (names(set(), set(), stage10_slot1={712}, stage8_slot1=set()), 0x3338, 3),
            # Header 407: the petals helper, decompiled here.
            407: (names({68, 96, 186, 297, 376, 595}, {165, 242, 294, 352, 358, 399, 465, 520, 621}, stage10_slot1=set(), stage8_slot1=set()), 0x18C8, 1),
            557: (names(set(), set(), stage10_slot1={165, 242, 294, 352, 358, 399, 465, 520, 621}, stage8_slot1={68, 96, 186, 297, 376, 595}), 0x18C8, 1),
            # Header 324: the header-397 rings, header-405 spokes and header-432 draw as siblings.
            324: (names({7, 552}, set(), stage10_slot1=set(), stage8_slot1=set()), 0x2D64, 3),
            474: (names(set(), set(), stage10_slot1=set(), stage8_slot1={7, 552}), 0x2D64, 3),
            # The header-443 strand and header-337 rings as sibling bodies.
            321: (names({164, 165, 210, 424, 609}, {34, 443, 459}, stage10_slot1=set(), stage8_slot1=set()), 0x2690, 2),
            376: (names({427, 458, 459}, {190, 217, 221, 296, 457, 598, 612, 647}, stage10_slot1=set(), stage8_slot1=set()), 0x2338, 1),
            471: (names(set(), set(), stage10_slot1={34, 443, 459}, stage8_slot1={164, 165, 210, 424, 609}), 0x2690, 2),
            526: (names(set(), set(), stage10_slot1={190, 217, 221, 296, 457, 598, 612, 647}, stage8_slot1={427, 458, 459}), 0x2338, 1),
            # Sibling bodies ported from the header-397, 405 and 443 helpers.
            398: (names(set(), {102, 282, 288, 642, 645}, stage10_slot1=set(), stage8_slot1=set()), 0x2FE0, 2),
            422: (names({185, 391, 436, 504, 594}, {367, 395}, stage10_slot1=set(), stage8_slot1=set()), 0x2CA8, 3),
            458: (names({116, 576}, set(), stage10_slot1=set(), stage8_slot1=set()), 0x2CD4, 3),
            548: (names(set(), set(), stage10_slot1={102, 282, 288, 642, 645}, stage8_slot1=set()), 0x2FE0, 2),
            572: (names(set(), set(), stage10_slot1={367, 395}, stage8_slot1={185, 391, 436, 504, 594}), 0x2CA8, 3),
            608: (names(set(), set(), stage10_slot1=set(), stage8_slot1={116, 576}), 0x2CD4, 3),
            598: (names(set(), stage8_slot1=set(), stage10_slot1={108, 573}), 0x43E4, 4),
        }
        archive = (ROOT / "game/DATA/MODEL.MRG").read_bytes()
        registered = set()
        for key, (family, text_end, c_count) in families.items():
            header = key[0] if isinstance(key, tuple) else key
            texts = set()
            for name in family:
                module = modules[name]
                registered.add(name)
                start = module["sector_offset"] * 2048
                image = archive[start:start + module["sector_count"] * 2048]
                self.assertEqual(int.from_bytes(image[:4], "little"), header, name)
                texts.add(image[4:text_end])
                entries = json.loads((self.config / "overlays" / f"{name}_matching_c.json").read_text())
                self.assertEqual(len(entries["functions"]), c_count, name)
            self.assertEqual(len(texts), 1, key)
        self.assertEqual(registered, set(modules))
        self.assertEqual(len(registered), 236)


if __name__ == "__main__":
    unittest.main()
