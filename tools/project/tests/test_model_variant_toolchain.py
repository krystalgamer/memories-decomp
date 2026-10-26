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
            "src/overlays/model_variant/variant320_entry.c",
            "src/overlays/model_variant/variant320_entry_slot1.c",
            "src/overlays/model_variant/variant320_ribbon.c",
            "src/overlays/model_variant/variant320_ribbon_slot1.c",
            "src/overlays/model_variant/variant320_rings.c",
            "src/overlays/model_variant/variant320_rings_slot1.c",
            "src/overlays/model_variant/variant320_streamers.c",
            "src/overlays/model_variant/variant320_streamers_slot1.c",
            "src/overlays/model_variant/variant321_entry.c",
            "src/overlays/model_variant/variant321_entry_slot1.c",
            "src/overlays/model_variant/variant321_ribbon.c",
            "src/overlays/model_variant/variant321_ribbon_slot1.c",
            "src/overlays/model_variant/variant321_rings.c",
            "src/overlays/model_variant/variant321_rings_slot1.c",
            "src/overlays/model_variant/variant321_strand.c",
            "src/overlays/model_variant/variant321_strand_slot1.c",
            "src/overlays/model_variant/variant321_streamers.c",
            "src/overlays/model_variant/variant321_streamers_slot1.c",
            "src/overlays/model_variant/variant324_bands.c",
            "src/overlays/model_variant/variant324_bands_slot1.c",
            "src/overlays/model_variant/variant324_draw.c",
            "src/overlays/model_variant/variant324_draw_slot1.c",
            "src/overlays/model_variant/variant324_entry.c",
            "src/overlays/model_variant/variant324_entry_slot1.c",
            "src/overlays/model_variant/variant324_fan.c",
            "src/overlays/model_variant/variant324_fan_slot1.c",
            "src/overlays/model_variant/variant324_rings.c",
            "src/overlays/model_variant/variant324_rings_slot1.c",
            "src/overlays/model_variant/variant324_spokes.c",
            "src/overlays/model_variant/variant324_spokes_slot1.c",
            "src/overlays/model_variant/variant324_webs.c",
            "src/overlays/model_variant/variant324_webs_slot1.c",
            "src/overlays/model_variant/variant376_entry.c",
            "src/overlays/model_variant/variant376_entry_slot1.c",
            "src/overlays/model_variant/variant376_ribbon.c",
            "src/overlays/model_variant/variant376_ribbon_slot1.c",
            "src/overlays/model_variant/variant376_rings.c",
            "src/overlays/model_variant/variant376_rings_slot1.c",
            "src/overlays/model_variant/variant376_streamers.c",
            "src/overlays/model_variant/variant376_streamers_slot1.c",
            "src/overlays/model_variant/variant391_draw.c",
            "src/overlays/model_variant/variant391_draw_slot1.c",
            "src/overlays/model_variant/variant391_entry.c",
            "src/overlays/model_variant/variant391_entry_slot1.c",
            "src/overlays/model_variant/variant391_update.c",
            "src/overlays/model_variant/variant391_update_slot1.c",
            "src/overlays/model_variant/variant397_bands.c",
            "src/overlays/model_variant/variant397_bands_slot1.c",
            "src/overlays/model_variant/variant397_entry.c",
            "src/overlays/model_variant/variant397_entry_slot1.c",
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
            "src/overlays/model_variant/variant398_bands.c",
            "src/overlays/model_variant/variant398_bands_slot1.c",
            "src/overlays/model_variant/variant398_curtains.c",
            "src/overlays/model_variant/variant398_curtains_slot1.c",
            "src/overlays/model_variant/variant398_entry.c",
            "src/overlays/model_variant/variant398_entry_slot1.c",
            "src/overlays/model_variant/variant398_rings.c",
            "src/overlays/model_variant/variant398_rings_slot1.c",
            "src/overlays/model_variant/variant398_sheets.c",
            "src/overlays/model_variant/variant398_sheets_slot1.c",
            "src/overlays/model_variant/variant398_webs.c",
            "src/overlays/model_variant/variant398_webs_slot1.c",
            "src/overlays/model_variant/variant401_bands.c",
            "src/overlays/model_variant/variant401_bands_slot1.c",
            "src/overlays/model_variant/variant401_entry.c",
            "src/overlays/model_variant/variant401_entry_slot1.c",
            "src/overlays/model_variant/variant401_quad.c",
            "src/overlays/model_variant/variant401_quad_slot1.c",
            "src/overlays/model_variant/variant401_rays.c",
            "src/overlays/model_variant/variant401_rays_slot1.c",
            "src/overlays/model_variant/variant401_rings.c",
            "src/overlays/model_variant/variant401_rings_slot1.c",
            "src/overlays/model_variant/variant401_sheet.c",
            "src/overlays/model_variant/variant401_sheet_slot1.c",
            "src/overlays/model_variant/variant401_spokes.c",
            "src/overlays/model_variant/variant401_spokes_slot1.c",
            "src/overlays/model_variant/variant401_webs.c",
            "src/overlays/model_variant/variant401_webs_slot1.c",
            "src/overlays/model_variant/variant404_bands.c",
            "src/overlays/model_variant/variant404_bands_slot1.c",
            "src/overlays/model_variant/variant404_entry.c",
            "src/overlays/model_variant/variant404_entry_slot1.c",
            "src/overlays/model_variant/variant404_quad.c",
            "src/overlays/model_variant/variant404_quad_slot1.c",
            "src/overlays/model_variant/variant404_ribbons.c",
            "src/overlays/model_variant/variant404_ribbons_slot1.c",
            "src/overlays/model_variant/variant404_rings.c",
            "src/overlays/model_variant/variant404_rings_slot1.c",
            "src/overlays/model_variant/variant404_sheets.c",
            "src/overlays/model_variant/variant404_sheets_slot1.c",
            "src/overlays/model_variant/variant404_spiral.c",
            "src/overlays/model_variant/variant404_spiral_slot1.c",
            "src/overlays/model_variant/variant404_spokes.c",
            "src/overlays/model_variant/variant404_spokes_slot1.c",
            "src/overlays/model_variant/variant404_webs.c",
            "src/overlays/model_variant/variant404_webs_slot1.c",
            "src/overlays/model_variant/variant405_bands.c",
            "src/overlays/model_variant/variant405_bands_slot1.c",
            "src/overlays/model_variant/variant405_entry.c",
            "src/overlays/model_variant/variant405_entry_slot1.c",
            "src/overlays/model_variant/variant405_halo.c",
            "src/overlays/model_variant/variant405_halo_slot1.c",
            "src/overlays/model_variant/variant405_quad.c",
            "src/overlays/model_variant/variant405_quad_slot1.c",
            "src/overlays/model_variant/variant405_rings.c",
            "src/overlays/model_variant/variant405_rings_slot1.c",
            "src/overlays/model_variant/variant405_sheets.c",
            "src/overlays/model_variant/variant405_sheets_slot1.c",
            "src/overlays/model_variant/variant405_spokes.c",
            "src/overlays/model_variant/variant405_spokes_slot1.c",
            "src/overlays/model_variant/variant405_veils.c",
            "src/overlays/model_variant/variant405_veils_slot1.c",
            "src/overlays/model_variant/variant405_webs.c",
            "src/overlays/model_variant/variant405_webs_slot1.c",
            "src/overlays/model_variant/variant407_entry.c",
            "src/overlays/model_variant/variant407_entry_slot1.c",
            "src/overlays/model_variant/variant407_petals.c",
            "src/overlays/model_variant/variant407_petals_slot1.c",
            "src/overlays/model_variant/variant414_bands.c",
            "src/overlays/model_variant/variant414_bands_slot1.c",
            "src/overlays/model_variant/variant414_entry.c",
            "src/overlays/model_variant/variant414_entry_slot1.c",
            "src/overlays/model_variant/variant414_fan.c",
            "src/overlays/model_variant/variant414_fan_slot1.c",
            "src/overlays/model_variant/variant414_rings.c",
            "src/overlays/model_variant/variant414_rings_slot1.c",
            "src/overlays/model_variant/variant414_sheets.c",
            "src/overlays/model_variant/variant414_sheets_slot1.c",
            "src/overlays/model_variant/variant414_spokes.c",
            "src/overlays/model_variant/variant414_spokes_slot1.c",
            "src/overlays/model_variant/variant414_webs.c",
            "src/overlays/model_variant/variant414_webs_slot1.c",
            "src/overlays/model_variant/variant415_bands.c",
            "src/overlays/model_variant/variant415_bands_slot1.c",
            "src/overlays/spanish_model_variant/variant432_draw.c",
            "src/overlays/model_variant/variant415_draw_slot1.c",
            "src/overlays/model_variant/variant415_entry.c",
            "src/overlays/model_variant/variant415_entry_slot1.c",
            "src/overlays/model_variant/variant415_layers.c",
            "src/overlays/model_variant/variant415_layers_slot1.c",
            "src/overlays/model_variant/variant415_ribbons.c",
            "src/overlays/model_variant/variant415_ribbons_slot1.c",
            "src/overlays/model_variant/variant416_bands.c",
            "src/overlays/model_variant/variant416_bands_slot1.c",
            "src/overlays/model_variant/variant416_entry.c",
            "src/overlays/model_variant/variant416_entry_slot1.c",
            "src/overlays/model_variant/variant416_quad.c",
            "src/overlays/model_variant/variant416_quad_slot1.c",
            "src/overlays/model_variant/variant416_rays.c",
            "src/overlays/model_variant/variant416_rays_slot1.c",
            "src/overlays/model_variant/variant416_rings.c",
            "src/overlays/model_variant/variant416_rings_slot1.c",
            "src/overlays/model_variant/variant416_sheets.c",
            "src/overlays/model_variant/variant416_sheets_slot1.c",
            "src/overlays/model_variant/variant416_spokes.c",
            "src/overlays/model_variant/variant416_spokes_slot1.c",
            "src/overlays/model_variant/variant416_webs.c",
            "src/overlays/model_variant/variant416_webs_slot1.c",
            "src/overlays/model_variant/variant418_bands.c",
            "src/overlays/model_variant/variant418_bands_slot1.c",
            "src/overlays/model_variant/variant418_entry.c",
            "src/overlays/model_variant/variant418_entry_slot1.c",
            "src/overlays/model_variant/variant418_quad.c",
            "src/overlays/model_variant/variant418_quad_slot1.c",
            "src/overlays/model_variant/variant418_ribbons.c",
            "src/overlays/model_variant/variant418_ribbons_slot1.c",
            "src/overlays/model_variant/variant418_rings.c",
            "src/overlays/model_variant/variant418_rings_slot1.c",
            "src/overlays/model_variant/variant418_sheet.c",
            "src/overlays/model_variant/variant418_sheet_slot1.c",
            "src/overlays/model_variant/variant418_spiral.c",
            "src/overlays/model_variant/variant418_spiral_slot1.c",
            "src/overlays/model_variant/variant418_spokes.c",
            "src/overlays/model_variant/variant418_spokes_slot1.c",
            "src/overlays/model_variant/variant418_webs.c",
            "src/overlays/model_variant/variant418_webs_slot1.c",
            "src/overlays/model_variant/variant422_bands.c",
            "src/overlays/model_variant/variant422_bands_slot1.c",
            "src/overlays/model_variant/variant422_curtains.c",
            "src/overlays/model_variant/variant422_curtains_slot1.c",
            "src/overlays/model_variant/variant422_entry.c",
            "src/overlays/model_variant/variant422_entry_slot1.c",
            "src/overlays/model_variant/variant422_screen_rings.c",
            "src/overlays/model_variant/variant422_screen_rings_slot1.c",
            "src/overlays/model_variant/variant422_sheets.c",
            "src/overlays/model_variant/variant422_sheets_slot1.c",
            "src/overlays/model_variant/variant422_webs.c",
            "src/overlays/model_variant/variant422_webs_slot1.c",
            "src/overlays/model_variant/variant423_bands.c",
            "src/overlays/model_variant/variant423_bands_slot1.c",
            "src/overlays/model_variant/variant423_entry.c",
            "src/overlays/model_variant/variant423_entry_slot1.c",
            "src/overlays/model_variant/variant423_quad.c",
            "src/overlays/model_variant/variant423_quad_slot1.c",
            "src/overlays/model_variant/variant423_rings.c",
            "src/overlays/model_variant/variant423_rings_slot1.c",
            "src/overlays/model_variant/variant423_sheets.c",
            "src/overlays/model_variant/variant423_sheets_slot1.c",
            "src/overlays/model_variant/variant423_spokes.c",
            "src/overlays/model_variant/variant423_spokes_slot1.c",
            "src/overlays/model_variant/variant423_webs.c",
            "src/overlays/model_variant/variant423_webs_slot1.c",
            "src/overlays/model_variant/variant425_bands.c",
            "src/overlays/model_variant/variant425_bands_slot1.c",
            "src/overlays/model_variant/variant425_entry.c",
            "src/overlays/model_variant/variant425_entry_slot1.c",
            "src/overlays/model_variant/variant425_quad.c",
            "src/overlays/model_variant/variant425_quad_slot1.c",
            "src/overlays/model_variant/variant425_rays.c",
            "src/overlays/model_variant/variant425_rays_slot1.c",
            "src/overlays/model_variant/variant425_ribbons.c",
            "src/overlays/model_variant/variant425_ribbons_slot1.c",
            "src/overlays/model_variant/variant425_rings.c",
            "src/overlays/model_variant/variant425_rings_slot1.c",
            "src/overlays/model_variant/variant425_sheet.c",
            "src/overlays/model_variant/variant425_sheet_slot1.c",
            "src/overlays/model_variant/variant425_spiral.c",
            "src/overlays/model_variant/variant425_spiral_slot1.c",
            "src/overlays/model_variant/variant425_spokes.c",
            "src/overlays/model_variant/variant425_spokes_slot1.c",
            "src/overlays/model_variant/variant425_webs.c",
            "src/overlays/model_variant/variant425_webs_slot1.c",
            "src/overlays/model_variant/variant428_bands.c",
            "src/overlays/model_variant/variant428_bands_slot1.c",
            "src/overlays/model_variant/variant428_entry.c",
            "src/overlays/model_variant/variant428_entry_slot1.c",
            "src/overlays/model_variant/variant428_quad.c",
            "src/overlays/model_variant/variant428_quad_slot1.c",
            "src/overlays/model_variant/variant428_rings.c",
            "src/overlays/model_variant/variant428_rings_slot1.c",
            "src/overlays/model_variant/variant428_sheets.c",
            "src/overlays/model_variant/variant428_sheets_slot1.c",
            "src/overlays/model_variant/variant428_spiral.c",
            "src/overlays/model_variant/variant428_spiral_slot1.c",
            "src/overlays/model_variant/variant428_spokes.c",
            "src/overlays/model_variant/variant428_spokes_slot1.c",
            "src/overlays/model_variant/variant428_webs.c",
            "src/overlays/model_variant/variant428_webs_slot1.c",
            "src/overlays/model_variant/variant433_bands.c",
            "src/overlays/model_variant/variant433_bands_slot1.c",
            "src/overlays/model_variant/variant433_coils.c",
            "src/overlays/model_variant/variant433_coils_slot1.c",
            "src/overlays/model_variant/variant433_entry.c",
            "src/overlays/model_variant/variant433_entry_slot1.c",
            "src/overlays/model_variant/variant433_ribbons.c",
            "src/overlays/model_variant/variant433_ribbons_slot1.c",
            "src/overlays/model_variant/variant433_rings.c",
            "src/overlays/model_variant/variant433_rings_slot1.c",
            "src/overlays/model_variant/variant443_entry.c",
            "src/overlays/model_variant/variant443_entry_slot1.c",
            "src/overlays/model_variant/variant443_ribbons.c",
            "src/overlays/model_variant/variant443_ribbons_slot1.c",
            "src/overlays/model_variant/variant443_sheets.c",
            "src/overlays/model_variant/variant443_sheets_slot1.c",
            "src/overlays/model_variant/variant443_strand.c",
            "src/overlays/model_variant/variant443_strand_slot1.c",
            "src/overlays/model_variant/variant443_streamers.c",
            "src/overlays/model_variant/variant443_streamers_slot1.c",
            "src/overlays/model_variant/variant448_entry.c",
            "src/overlays/model_variant/variant448_entry_slot1.c",
            "src/overlays/model_variant/variant448_fan.c",
            "src/overlays/model_variant/variant448_fan_slot1.c",
            "src/overlays/model_variant/variant448_orbit.c",
            "src/overlays/model_variant/variant448_orbit_slot1.c",
            "src/overlays/model_variant/variant448_quad.c",
            "src/overlays/model_variant/variant448_quad_slot1.c",
            "src/overlays/model_variant/variant448_radial.c",
            "src/overlays/model_variant/variant448_radial_slot1.c",
            "src/overlays/model_variant/variant448_rings.c",
            "src/overlays/model_variant/variant448_rings_slot1.c",
            "src/overlays/model_variant/variant448_spiral.c",
            "src/overlays/model_variant/variant448_spiral_slot1.c",
            "src/overlays/model_variant/variant448_spokes.c",
            "src/overlays/model_variant/variant448_spokes_slot1.c",
            "src/overlays/model_variant/variant448_webs.c",
            "src/overlays/model_variant/variant448_webs_slot1.c",
            "src/overlays/model_variant/variant450_ntsc_lines.c",
            "src/overlays/model_variant/variant450_ntsc_lines_slot1.c",
            "src/overlays/model_variant/variant450_ntsc_quads.c",
            "src/overlays/model_variant/variant450_ntsc_quads_slot1.c",
            "src/overlays/model_variant/variant458_entry.c",
            "src/overlays/model_variant/variant458_entry_slot1.c",
            "src/overlays/model_variant/variant458_quads.c",
            "src/overlays/model_variant/variant458_quads_slot1.c",
            "src/overlays/model_variant/variant458_ribbons.c",
            "src/overlays/model_variant/variant458_ribbons_slot1.c",
            "src/overlays/model_variant/variant458_sheets.c",
            "src/overlays/model_variant/variant458_sheets_slot1.c",
            "src/overlays/model_variant/variant458_strand.c",
            "src/overlays/model_variant/variant458_strand_slot1.c",
            "src/overlays/model_variant/variant458_streamers.c",
            "src/overlays/model_variant/variant458_streamers_slot1.c",
            "src/overlays/model_variant/variant459_curtains.c",
            "src/overlays/model_variant/variant459_curtains_slot1.c",
            "src/overlays/model_variant/variant459_entry.c",
            "src/overlays/model_variant/variant459_entry_slot1.c",
            "src/overlays/model_variant/variant459_grid.c",
            "src/overlays/model_variant/variant459_grid_slot1.c",
            "src/overlays/model_variant/variant459_screen_grid.c",
            "src/overlays/model_variant/variant459_screen_grid_slot1.c",
            "src/overlays/model_variant/variant459_sheet.c",
            "src/overlays/model_variant/variant459_sheet_slot1.c",
            "src/overlays/model_variant/variant459_spiral.c",
            "src/overlays/model_variant/variant459_spiral_slot1.c",
            "src/overlays/model_variant/variant459_webs.c",
            "src/overlays/model_variant/variant459_webs_slot1.c",
        })

    def test_variant_sources_read_guest_pointers_through_g32_views(self):
        # A stored descriptor pointer is a 4-byte guest field; a native u8 **
        # dereference would load 8 bytes on 64-bit hosts.
        for path in sorted((ROOT / "src/overlays/model_variant").glob("*.c")):
            self.assertNotIn("*(u8 **)", path.read_text(), path.name)


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
            [("0x8013B004", "matching_c"), ("0x8013C12C", "matching_c"), ("0x8013C620", "matching_c"),
             ("0x8013CD04", "matching_c"),
             ("0x8013D410", "matching_c"), ("0x8013D8FC", "matching_c"), ("0x8013DE54", "matching_c"),
             ("0x8013E168", "matching_c"), ("0x8013E4E8", "matching_c")],
        )
        counts = load_overlay_inventories(ROOT)["model_variant_1_pos0_slot0"]
        self.assertEqual(counts["function_count"], 9)
        self.assertEqual(counts["matching_c_function_count"], 9)
        self.assertEqual(counts["matching_c_bytes"], 0x1128 + 0x4F4 + 0x6E4 + 0x70C + 0x4EC + 0x558 + 0x314 + 0x380 + 0x368)

    def test_image_data_symbols_fit_their_images(self):
        # A byte-identical build does not check symbol extents, so a raw-data
        # symbol that runs past the image end would go unnoticed.
        manifest = json.loads((self.config / "overlays.json").read_text())
        checked = 0
        for module in manifest["modules"]:
            name = module["name"]
            if not name.startswith("model_variant_"):
                continue
            path = self.config / "overlays" / f"{name}_symbols.txt"
            if not path.exists():
                continue
            start = int(module["load_address"], 16)
            end = start + module["sector_count"] * 2048
            for line in path.read_text().splitlines():
                if not line.startswith("D_") or " size:0x" not in line:
                    continue
                address = int(line.split("=", 1)[1].split(";", 1)[0], 16)
                size = int(line.split(" size:0x", 1)[1].split()[0], 16)
                self.assertGreaterEqual(address, start, (name, line))
                self.assertLessEqual(address + size, end, (name, line))
                checked += 1
        self.assertGreater(checked, 0)

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
            71: (names(set(), {31, 408, 531}, stage7={290, 295, 501, 518}), 0x954, 1),
            201: (names(set(), stage10_slot1={31, 408, 531}, stage8_slot1={290, 295, 501, 518}), 0x954, 1),
            397: (names({2, 20, 87, 108, 138, 193, 573}, {152, 168, 170, 388, 427}), 0x2DE4, 7),
            404: (names({84, 162}, {88, 114, 184, 369}), 0x40FC, 9),
            405: (names({1, 550}), 0x3850, 9),
            418: (names({34, 71, 124, 182, 279, 361, 491, 580, 640}, {166, 275, 469, 590}), 0x396C, 9),
            428: (names({187, 596}, {239, 361, 368, 478}), 0x359C, 8),
            401: (names(set(), {410}), 0x3124, 8),
            414: (names({401}), 0x2F58, 7),
            416: (names({180, 440}), 0x30D0, 8),
            423: (names(set(), {262, 631}), 0x2B38, 7),
            425: (names({259, 630}), 0x43FC, 10),
            448: (names(set(), {108, 573}), 0x43E4, 9),
            443: (names({70, 460, 469, 704}, {44, 98, 161, 370, 400, 458, 462, 558}), 0x2888, 5),
            (443, "model 125"): (names({125}), 0x4D0C, 5),
            # The Spanish header-408 source, built for North America.
            391: (names(set(), {54}), 0x146C, 3),
            541: (names(set(), stage10_slot1={54}), 0x146C, 3),
            # The Spanish header-337 and header-432 sources, built for North America.
            320: (names(set(), {110, 159}, stage7={410}), 0x1F44, 4),
            470: (names(set(), stage10_slot1={110, 159}, stage8_slot1={410}), 0x1F44, 4),
            415: (names(set(), {401}), 0x1F7C, 5),
            565: (names(set(), stage10_slot1={401}), 0x1F7C, 5),
            # The North American MODEL450 CDK pair.
            433: (names(set(), {174}), 0x38DC, 7),
            583: (names(set(), stage10_slot1={174}), 0x38DC, 7),
            # Slot-1 images at 0x8017B000: each header is its slot-0 header plus 150,
            # built from the same C.
            547: (names(set(), stage8_slot1={2, 20, 87, 108, 138, 193, 573}, stage10_slot1={152, 168, 170, 388, 427}), 0x2DE4, 7),
            551: (names(set(), stage8_slot1=set(), stage10_slot1={410}), 0x3124, 8),
            554: (names(set(), stage8_slot1={84, 162}, stage10_slot1={88, 114, 184, 369}), 0x40FC, 9),
            555: (names(set(), stage8_slot1={1, 550}, stage10_slot1=set()), 0x3850, 9),
            564: (names(set(), stage8_slot1={401}, stage10_slot1=set()), 0x2F58, 7),
            566: (names(set(), stage8_slot1={180, 440}, stage10_slot1=set()), 0x30D0, 8),
            568: (names(set(), stage8_slot1={34, 71, 124, 182, 279, 361, 491, 580, 640}, stage10_slot1={166, 275, 469, 590}), 0x396C, 9),
            573: (names(set(), stage8_slot1=set(), stage10_slot1={262, 631}), 0x2B38, 7),
            575: (names(set(), stage8_slot1={259, 630}, stage10_slot1=set()), 0x43FC, 10),
            578: (names(set(), stage8_slot1={187, 596}, stage10_slot1={239, 361, 368, 478}), 0x359C, 8),
            593: (names(set(), stage8_slot1={70, 460, 469, 704}, stage10_slot1={44, 98, 161, 370, 400, 458, 462, 558}), 0x2888, 5),
            (593, "model 125"): (names(set(), stage8_slot1={125}), 0x4D0C, 5),
            # Header 459: the header-418 webs with header-425 constants.
            459: (names(set(), {712}, stage10_slot1=set(), stage8_slot1=set()), 0x3338, 7),
            609: (names(set(), set(), stage10_slot1={712}, stage8_slot1=set()), 0x3338, 7),
            # Header 407: the petals helper, decompiled here.
            407: (names({68, 96, 186, 297, 376, 595}, {165, 242, 294, 352, 358, 399, 465, 520, 621}, stage10_slot1=set(), stage8_slot1=set()), 0x18C8, 2),
            557: (names(set(), set(), stage10_slot1={165, 242, 294, 352, 358, 399, 465, 520, 621}, stage8_slot1={68, 96, 186, 297, 376, 595}), 0x18C8, 2),
            # Header 324: the header-397 rings, header-405 spokes and header-432 draw as siblings.
            324: (names({7, 552}, set(), stage10_slot1=set(), stage8_slot1=set()), 0x2D64, 7),
            474: (names(set(), set(), stage10_slot1=set(), stage8_slot1={7, 552}), 0x2D64, 7),
            # The header-443 strand and header-337 rings as sibling bodies.
            321: (names({164, 165, 210, 424, 609}, {34, 443, 459}, stage10_slot1=set(), stage8_slot1=set()), 0x2690, 5),
            376: (names({427, 458, 459}, {190, 217, 221, 296, 457, 598, 612, 647}, stage10_slot1=set(), stage8_slot1=set()), 0x2338, 4),
            471: (names(set(), set(), stage10_slot1={34, 443, 459}, stage8_slot1={164, 165, 210, 424, 609}), 0x2690, 5),
            526: (names(set(), set(), stage10_slot1={190, 217, 221, 296, 457, 598, 612, 647}, stage8_slot1={427, 458, 459}), 0x2338, 4),
            # Sibling bodies ported from the header-397, 405 and 443 helpers.
            398: (names(set(), {102, 282, 288, 642, 645}, stage10_slot1=set(), stage8_slot1=set()), 0x2FE0, 6),
            422: (names({185, 391, 436, 504, 594}, {367, 395}, stage10_slot1=set(), stage8_slot1=set()), 0x2CA8, 6),
            458: (names({116, 576}, set(), stage10_slot1=set(), stage8_slot1=set()), 0x2CD4, 6),
            548: (names(set(), set(), stage10_slot1={102, 282, 288, 642, 645}, stage8_slot1=set()), 0x2FE0, 6),
            572: (names(set(), set(), stage10_slot1={367, 395}, stage8_slot1={185, 391, 436, 504, 594}), 0x2CA8, 6),
            608: (names(set(), set(), stage10_slot1=set(), stage8_slot1={116, 576}), 0x2CD4, 6),
            598: (names(set(), stage8_slot1=set(), stage10_slot1={108, 573}), 0x43E4, 9),
            # North American images whose entry is the French C of the same body (relocations differ only by binding).
            59: (names({57, 447, 562}, {489}, stage10_slot1=set(), stage8_slot1=set()), 0xB98, 1),
            60: (names({386}, {53, 84, 547}, stage10_slot1=set(), stage8_slot1=set()), 0xC18, 1),
            65: (names({66}, set(), stage10_slot1=set(), stage8_slot1=set()), 0xDE4, 1),
            70: (names(set(), {140, 584}, stage10_slot1=set(), stage8_slot1=set()), 0xA1C, 1),
            80: (names({100}, {150}, stage10_slot1=set(), stage8_slot1=set()), 0x1158, 1),
            84: (names({130}, set(), stage10_slot1=set(), stage8_slot1=set()), 0xAE4, 1),
            90: (names({402}, {28, 85}, stage10_slot1=set(), stage8_slot1=set()), 0x1188, 1),
            99: (names({422, 556}, {22, 73, 127, 454, 484, 504, 581, 701}, stage10_slot1=set(), stage8_slot1=set()), 0x115C, 1),
            100: (names({139, 146}, {15}, stage10_slot1=set(), stage8_slot1=set()), 0x108C, 1),
            104: (names({281, 502, 509, 514, 641}, {39, 227, 477, 514, 557, 708}, stage10_slot1=set(), stage8_slot1=set()), 0x10A8, 1),
            112: (names({219}, {30, 355, 435, 490}, stage10_slot1=set(), stage8_slot1=set()), 0xFE8, 1),
            114: (names({183, 273, 486, 490, 540, 637}, set(), stage10_slot1=set(), stage8_slot1=set()), 0x15BC, 1),
            115: (names(set(), {416}, stage10_slot1=set(), stage8_slot1=set()), 0xEB8, 1),
            124: (names({428, 445}, {20}, stage10_slot1=set(), stage8_slot1=set()), 0xF60, 1),
            126: (names({481, 496}, {234, 546}, stage10_slot1=set(), stage8_slot1=set()), 0xFF4, 1),
            189: (names(set(), set(), stage10_slot1={489}, stage8_slot1={57, 447, 562}), 0xB98, 1),
            190: (names(set(), set(), stage10_slot1={53, 84, 547}, stage8_slot1={386}), 0xC18, 1),
            195: (names(set(), set(), stage10_slot1=set(), stage8_slot1={66}), 0xDE4, 1),
            200: (names(set(), set(), stage10_slot1={140, 584}, stage8_slot1=set()), 0xA1C, 1),
            210: (names(set(), set(), stage10_slot1={150}, stage8_slot1={100}), 0x1158, 1),
            214: (names(set(), set(), stage10_slot1=set(), stage8_slot1={130}), 0xAE4, 1),
            220: (names(set(), set(), stage10_slot1={28, 85}, stage8_slot1={402}), 0x1188, 1),
            229: (names(set(), set(), stage10_slot1={22, 73, 127, 454, 484, 504, 581, 701}, stage8_slot1={422, 556}), 0x115C, 1),
            230: (names(set(), set(), stage10_slot1={15}, stage8_slot1={139}), 0x108C, 1),
            234: (names(set(), set(), stage10_slot1={39, 227, 477, 514, 557, 708}, stage8_slot1={281, 502, 509, 514, 641}), 0x10A8, 1),
            242: (names(set(), set(), stage10_slot1={30, 355, 435, 490}, stage8_slot1={219}), 0xFE8, 1),
            244: (names(set(), set(), stage10_slot1=set(), stage8_slot1={183, 273, 486, 490, 540, 637}), 0x15BC, 1),
            245: (names(set(), set(), stage10_slot1={416}, stage8_slot1=set()), 0xEB8, 1),
            254: (names(set(), set(), stage10_slot1={20}, stage8_slot1={428, 445}), 0xF60, 1),
            256: (names(set(), set(), stage10_slot1={234, 546}, stage8_slot1={481, 496}), 0xFF4, 1),
            765: (names({58, 217, 453, 563, 612}, {78}, stage10_slot1=set(), stage8_slot1=set()), 0x14B0, 1),
            812: (names({485}, set(), stage10_slot1=set(), stage8_slot1=set()), 0xF88, 1),
            862: (names(set(), set(), stage10_slot1={78}, stage8_slot1={58, 217, 453, 563, 612}), 0x14B0, 1),
            909: (names(set(), set(), stage10_slot1=set(), stage8_slot1={485}), 0xF88, 1),
            # North American images whose entry is a French entry with its regional constants (burst CLUT, screen height).
            79: (names(set(), {192, 196}, stage10_slot1=set(), stage8_slot1=set()), 0xC4C, 1),
            127: (names(set(), {492}, stage10_slot1=set(), stage8_slot1=set()), 0xEB0, 1),
            181: (names(set(), {508}, stage10_slot1=set(), stage8_slot1=set()), 0xD74, 1),
            182: (names(set(), {243, 622}, stage10_slot1=set(), stage8_slot1=set()), 0x1594, 1),
            209: (names(set(), set(), stage10_slot1={192, 196}, stage8_slot1=set()), 0xC4C, 1),
            257: (names(set(), set(), stage10_slot1={492}, stage8_slot1=set()), 0xEB0, 1),
            311: (names(set(), set(), stage10_slot1={508}, stage8_slot1=set()), 0xD74, 1),
            312: (names(set(), set(), stage10_slot1={243, 622}, stage8_slot1=set()), 0x1594, 1),
            753: (names({23, 85, 103, 239, 480, 499, 555}, {187, 596}, stage10_slot1=set(), stage8_slot1=set()), 0x16E8, 1),
            850: (names(set(), set(), stage10_slot1={187, 596}, stage8_slot1={23, 85, 103, 239, 480, 499, 555}), 0x16E8, 1),
            # North American images whose entry is a French entry plus the North American work-size printf.
            136: (names({38, 271, 635}, {356}, stage10_slot1=set(), stage8_slot1=set()), 0xE18, 1),
            156: (names({392}, {8, 113, 350, 386, 448}, stage10_slot1=set(), stage8_slot1=set()), 0xEC4, 1),
            266: (names(set(), set(), stage10_slot1={167, 356}, stage8_slot1={38, 271, 635}), 0xE18, 1),
            286: (names(set(), set(), stage10_slot1={8, 113, 350, 386, 448}, stage8_slot1={392}), 0xEC4, 1),
            167: (names(set(), {169, 524, 591}, stage10_slot1=set(), stage8_slot1=set()), 0xF08, 1),
            297: (names(set(), set(), stage10_slot1={169, 524, 591}, stage8_slot1=set()), 0xF08, 1),
            140: (names(set(), {131, 145}, stage10_slot1=set(), stage8_slot1=set()), 0x106C, 1),
            270: (names(set(), set(), stage10_slot1={131, 145}, stage8_slot1=set()), 0x106C, 1),
            160: (names(set(), {164, 191, 494, 521, 599}, stage10_slot1=set(), stage8_slot1=set()), 0x11B0, 1),
            290: (names(set(), set(), stage10_slot1={164, 191, 494, 521, 599}, stage8_slot1=set()), 0x11B0, 1),
            145: (names({37}, set(), stage10_slot1=set(), stage8_slot1=set()), 0xDC0, 1),
            275: (names(set(), set(), stage10_slot1=set(), stage8_slot1={37}), 0xDC0, 1),
            146: (names(set(), {37}, stage10_slot1=set(), stage8_slot1=set()), 0xEBC, 1),
            276: (names(set(), set(), stage10_slot1={37}, stage8_slot1=set()), 0xEBC, 1),
            161: (names(set(), {141}, stage10_slot1=set(), stage8_slot1=set()), 0xD48, 1),
            291: (names(set(), set(), stage10_slot1={141}, stage8_slot1=set()), 0xD48, 1),
            150: (names(set(), {71}, stage10_slot1=set(), stage8_slot1=set()), 0xF18, 1),
            280: (names(set(), set(), stage10_slot1={71}, stage8_slot1=set()), 0xF18, 1),
            # North American images whose entry is North American C (headers 111, 118, 119, 164, 172, 176, 627, 647, 752, 754, 756, 766, 779, 827 and 828).
            111: (names({466, 483, 548}, {247, 486}, stage10_slot1=set(), stage8_slot1=set()), 0xCF8, 1),
            241: (names(set(), set(), stage10_slot1={247, 486}, stage8_slot1={466, 483, 548}), 0xCF8, 1),
            627: (names({489, 505}, {13, 286, 421, 482, 541}, stage10_slot1=set(), stage8_slot1=set()), 0x136C, 1),
            694: (names(set(), set(), stage10_slot1={13, 286, 421, 482, 541}, stage8_slot1={489, 505}), 0x136C, 1),
            647: (names({67, 115, 142, 289, 435, 439, 477, 575}, {123, 206, 271, 299, 381, 382, 393, 397, 415, 579, 606, 635, 649}, stage10_slot1=set(), stage8_slot1=set()), 0xE58, 1),
            714: (names(set(), set(), stage10_slot1={123, 206, 299, 381, 382, 393, 397, 415, 579, 606, 635, 649}, stage8_slot1={67, 115, 142, 289, 435, 439, 477, 575}), 0xE58, 1),
            752: (names({31, 80, 99, 104, 117, 570, 572, 616}, {5, 24, 46, 59, 359, 564, 610}, stage10_slot1=set(), stage8_slot1=set()), 0x1980, 1),
            849: (names(set(), set(), stage10_slot1={5, 24, 46, 59, 359, 564, 610}, stage8_slot1={31, 80, 99, 104, 247, 570, 572, 616}), 0x1980, 1),
            754: (names({45, 46, 78, 192, 198, 199, 254, 286, 574, 601, 602, 603, 628}, {10, 11, 158, 160, 193, 281, 396, 481, 553, 589, 641}, stage10_slot1=set(), stage8_slot1=set()), 0x195C, 1),
            851: (names(set(), set(), stage10_slot1={10, 11, 158, 160, 193, 281, 396, 481, 553, 589, 641}, stage8_slot1={45, 78, 112, 192, 198, 199, 209, 254, 286, 574, 601, 602, 603, 628}), 0x195C, 1),
            756: (names({11, 128, 132, 172, 179, 235, 251, 255, 285, 380, 415, 553, 593, 626, 644}, {65, 99, 132, 244, 245, 256, 284, 540, 629, 643}, stage10_slot1=set(), stage8_slot1=set()), 0x2000, 1),
            853: (names(set(), set(), stage10_slot1={65, 99, 132, 244, 245, 256, 284, 540, 629, 643}, stage8_slot1={11, 128, 132, 179, 235, 255, 285, 380, 415, 553, 593, 626, 644}), 0x2000, 1),
            827: (names({4, 91, 150, 200, 294, 383, 403, 516, 521, 527}, {147, 171, 449, 463, 534}, stage10_slot1=set(), stage8_slot1=set()), 0x1A30, 1),
            924: (names(set(), set(), stage10_slot1={147, 171, 260, 449, 463, 534}, stage8_slot1={4, 91, 150, 200, 294, 383, 403, 516, 521, 527, 546}), 0x1A30, 1),
            828: (names({50, 79, 122, 396, 397, 414, 513, 536, 560, 569, 578}, {64, 75, 225, 353, 374, 495, 567}, stage10_slot1=set(), stage8_slot1=set()), 0x1A38, 1),
            925: (names(set(), set(), stage10_slot1={64, 75, 225, 353, 374, 495, 567}, stage8_slot1={50, 79, 396, 397, 414, 513, 536, 560, 569, 578}), 0x1A38, 1),
            654: (names({204, 503, 515, 526, 532, 547, 566, 589, 604}, {122, 487, 509, 538, 578, 585, 613}, stage10_slot1=set(), stage8_slot1=set()), 0x2370, 1),
            721: (names(set(), set(), stage10_slot1={122, 151, 487, 509, 538, 578, 585, 613}, stage8_slot1={204, 503, 515, 526, 532, 547, 566, 589, 604}), 0x2370, 1),
            634: (names({256, 409, 605, 629}, {47, 476}, stage10_slot1=set(), stage8_slot1=set()), 0x1360, 1),
            701: (names(set(), set(), stage10_slot1={47, 476}, stage8_slot1={48, 256, 409, 605, 629}), 0x1360, 1),
            72: (names({30, 137, 233, 234, 617}, {9, 45, 104, 176, 186, 572, 592, 595}, stage10_slot1=set(), stage8_slot1=set()), 0x1D68, 1),
            202: (names(set(), set(), stage10_slot1={45, 104, 176, 186, 572, 592, 595}, stage8_slot1={30, 137, 233, 234, 617}), 0x1D68, 1),
            110: (names({28, 191, 393, 467, 479, 599}, {112, 250, 445, 533, 574, 625}, stage10_slot1=set(), stage8_slot1=set()), 0x1918, 1),
            240: (names(set(), set(), stage10_slot1={112, 250, 445, 533, 574, 625}, stage8_slot1={28, 191, 393, 467, 479, 599}), 0x1918, 1),
            779: (names({5, 24, 49, 53, 98, 120, 154, 157, 177, 196, 221, 473, 559, 577, 586, 588}, {49, 80, 86, 124, 210, 265, 273, 376, 387, 402, 474, 559, 570, 580, 609, 620, 634, 637}, stage10_slot1=set(), stage8_slot1=set()), 0x20E0, 1),
            876: (names(set(), set(), stage10_slot1={49, 80, 86, 124, 210, 238, 265, 273, 376, 387, 402, 474, 559, 570, 580, 609, 620, 634, 637}, stage8_slot1={5, 24, 49, 53, 92, 98, 120, 136, 154, 157, 177, 196, 221, 473, 559, 577, 586, 588}), 0x20E0, 1),
            766: (names({65, 83, 90, 94, 109, 110, 114, 126, 160, 208, 230, 245, 298, 381, 404, 444, 468, 470, 478, 608, 633, 648}, {224, 226, 233, 450, 614, 617}, stage10_slot1=set(), stage8_slot1=set()), 0x1838, 1),
            863: (names(set(), set(), stage10_slot1={97, 224, 226, 233, 450, 614, 617}, stage8_slot1={65, 83, 90, 94, 109, 114, 126, 160, 208, 230, 245, 298, 381, 404, 444, 468, 470, 478, 608, 633, 648}), 0x1838, 1),
            118: (names({16, 17, 59, 358, 378, 452, 484, 564}, {51, 390, 510, 511}, stage10_slot1=set(), stage8_slot1=set()), 0x1044, 1),
            248: (names(set(), set(), stage10_slot1={51, 390, 510, 511}, stage8_slot1={16, 17, 59, 224, 358, 378, 452, 484, 564}), 0x1044, 1),
            119: (names({3, 25, 369, 705}, {16, 17, 96, 155, 436, 717}, stage10_slot1=set(), stage8_slot1=set()), 0xA1C, 1),
            249: (names(set(), set(), stage10_slot1={16, 17, 96, 155, 436, 717}, stage8_slot1={3, 25, 369, 705}), 0xA1C, 1),
            164: (names({169, 178, 272, 494, 591, 636}, {280}, stage10_slot1=set(), stage8_slot1=set()), 0x1E60, 1),
            294: (names(set(), set(), stage10_slot1={280}, stage8_slot1={169, 178, 272, 494, 591, 636}), 0x1E60, 1),
            172: (names(set(), {7, 56, 157, 179, 228, 505, 552, 588, 593, 615}, stage10_slot1=set(), stage8_slot1=set()), 0xCF8, 1),
            302: (names(set(), set(), stage10_slot1={7, 56, 157, 179, 228, 505, 552, 588, 593, 615}, stage8_slot1=set()), 0xCF8, 1),
            176: (names({13, 52, 127, 388, 462, 506, 581}, {177, 213, 467, 479, 544, 608}, stage10_slot1=set(), stage8_slot1=set()), 0x13A4, 1),
            306: (names(set(), set(), stage10_slot1={177, 208, 213, 467, 479, 544, 608}, stage8_slot1={13, 52, 127, 388, 462, 506, 581}), 0x13A4, 1),
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
        self.assertEqual(len(registered), 1051)


if __name__ == "__main__":
    unittest.main()
