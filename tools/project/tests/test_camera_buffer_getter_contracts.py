"""Check resident camera getter return types with the supported compiler."""

import os
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"


@unittest.skipUnless(COMPILER.is_file(), "requires supported GCC 2.8.1")
class CameraBufferGetterContractTests(unittest.TestCase):
    def test_sdk_returns_and_rejected_opaque_returns(self):
        contracts = (
            ("Model_GetCameraViewBuffer", "GsRVIEW2 *"),
            ("Model_GetCameraCoordinateUnit", "GsCOORDUNIT *"),
        )
        with tempfile.TemporaryDirectory(
            prefix="camera-getter-contract-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            for symbol, canonical in contracts:
                for result_type, accepted in ((canonical, True), ("void *", False)):
                    with self.subTest(symbol=symbol, result_type=result_type):
                        source = directory / "probe.c"
                        source.write_text(
                            '#include "../../src/types.h"\n'
                            '#include "../../src/game/camera_view.h"\n'
                            f"{result_type} (*checked)(void) = {symbol};\n"
                        )
                        result = subprocess.run(
                            [
                                str(COMPILER), "-S", "-O2", "-G0", "-Werror",
                                "-o", str(directory / "probe.s"), str(source),
                            ],
                            cwd=ROOT, env=environment, text=True,
                            capture_output=True, timeout=60,
                        )
                        if accepted:
                            self.assertEqual(result.returncode, 0, result.stderr)
                        else:
                            self.assertNotEqual(result.returncode, 0, result.stderr)
                            self.assertIn("incompatible pointer type", result.stderr)


if __name__ == "__main__":
    unittest.main()
