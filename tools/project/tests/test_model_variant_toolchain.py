import json
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[3]



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

    def test_gcc_2_8_0_release_is_pinned(self):
        release = json.loads((ROOT / "tools/bootstrap/old_gcc_280_prebuilt.json").read_text())
        self.assertEqual(release["release"], "0.17")
        self.assertTrue(release["url"].endswith("/0.17/gcc-2.8.0-psx.tar.gz"))
        self.assertEqual(release["lock"]["prefix"], "tools/toolchains/gcc-2.8.0-psx")
        self.assertEqual(release["dumpversion"], "2.8.0")
        self.assertEqual(set(release["files"]), {"gcc", "cc1", "cpp"})

    def test_gcc_2_8_0_profile_differs_from_gcc_2_8_1_g0_split_only_in_compiler(self):
        profiles = self.profiles()
        old = dict(profiles["gcc_2_8_0_g0_split"])
        base = dict(profiles["gcc_2_8_1_g0_split"])
        self.assertEqual(old.pop("compiler"), "tools/toolchains/gcc-2.8.0-psx/bin/mips-sony-psx-gcc")
        base.pop("compiler")
        self.assertEqual(old, base)

    def test_variant_sources_read_guest_pointers_through_g32_views(self):
        # A stored descriptor pointer is a 4-byte guest field; a native u8 **
        # dereference would load 8 bytes on 64-bit hosts.
        for path in sorted((ROOT / "src/overlays/model_variant").glob("*.c")):
            self.assertNotIn("*(u8 **)", path.read_text(), path.name)



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



if __name__ == "__main__":
    unittest.main()
