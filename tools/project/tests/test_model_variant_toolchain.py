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
            "src/overlays/model_variant/variant397_quad.c",
            "src/overlays/model_variant/variant397_rings.c",
            "src/overlays/model_variant/variant397_sheets.c",
            "src/overlays/model_variant/variant397_spokes.c",
            "src/overlays/model_variant/variant405_quad.c",
            "src/overlays/model_variant/variant405_rings.c",
        })

    def test_first_variant_image(self):
        manifest = json.loads((self.config / "overlays.json").read_text())
        (module,) = [m for m in manifest["modules"] if m["name"] == "model_variant_1_pos0_slot0"]
        checksums = load_checksum_manifest(self.config / "files.sha256")
        self.assertEqual(module["archive"], "game/DATA/MODEL.MRG")
        self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
        # Stage 7 of Model_LoadMonsterMerge: sectors 180-189 of record 1.
        self.assertEqual(module["sector_offset"], 1 * 276 + 180)
        self.assertEqual(module["sector_count"], 10)
        self.assertEqual(module["load_address"], "0x8013B000")
        layout = ROOT / module["layout"]
        with layout.with_name(layout.stem + "_functions.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 9)
        self.assertEqual(
            [(r["address"], r["status"]) for r in rows if r["status"] == "matching_c"],
            [("0x8013E168", "matching_c"), ("0x8013E4E8", "matching_c")],
        )
        counts = load_overlay_inventories(ROOT)["model_variant_1_pos0_slot0"]
        self.assertEqual(counts["function_count"], 9)
        self.assertEqual(counts["matching_c_function_count"], 2)
        self.assertEqual(counts["matching_c_bytes"], 0x380 + 0x368)

    def test_header_families_share_text(self):
        manifest = json.loads((self.config / "overlays.json").read_text())
        modules = {m["name"]: m for m in manifest["modules"] if m["name"].startswith("model_variant_")}
        families = {
            397: ({2, 20, 87, 108, 138, 193, 573, 152, 168, 170, 388, 427}, 0x2DE4, 4),
            405: ({1, 550}, 0x3850, 2),
        }
        archive = (ROOT / "game/DATA/MODEL.MRG").read_bytes()
        registered = set()
        for header, (models, text_end, c_count) in families.items():
            texts = set()
            for name, module in modules.items():
                model = int(name.split("_")[2])
                if model not in models:
                    continue
                registered.add(name)
                start = module["sector_offset"] * 2048
                image = archive[start:start + module["sector_count"] * 2048]
                self.assertEqual(int.from_bytes(image[:4], "little"), header, name)
                texts.add(image[4:text_end])
                entries = json.loads((self.config / "overlays" / f"{name}_matching_c.json").read_text())
                self.assertEqual(len(entries["functions"]), c_count, name)
            self.assertEqual(len(texts), 1, header)
        self.assertEqual(registered, set(modules))
        self.assertEqual(len(registered), 14)


if __name__ == "__main__":
    unittest.main()
