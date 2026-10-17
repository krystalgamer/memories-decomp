"""Check resident camera and metric interfaces with the supported compiler."""

import os
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"


@unittest.skipUnless(COMPILER.is_file(), "requires supported GCC 2.8.1")
class CameraBufferGetterContractTests(unittest.TestCase):
    def test_typed_returns_and_rejected_opaque_returns(self):
        contracts = (
            ("Model_GetCameraViewBuffer", "GsRVIEW2 *"),
            ("Model_GetCameraCoordinateUnit", "GsCOORDUNIT *"),
            ("Model_GetViewMetricsBuffer", "ModelViewMetrics *"),
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
                            '#include "../../src/game/model_update_view_metrics.h"\n'
                            f"{result_type} (*checked)(void) = {symbol};\n"
                            "u16 *distance(ModelViewMetrics *p) { return &p->distance; }\n"
                            "u16 *yaw(ModelViewMetrics *p) { return &p->yaw; }\n"
                            "u16 *pitch(ModelViewMetrics *p) { return &p->pitch; }\n"
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

    def test_metric_update_requires_the_sdk_view_input(self):
        with tempfile.TemporaryDirectory(
            prefix="camera-metric-input-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            for argument, accepted in (
                ("GsRVIEW2 *", True), ("u8 *", False), ("void *", False),
            ):
                with self.subTest(argument=argument):
                    source = directory / "probe.c"
                    source.write_text(
                        '#include "../../src/game/model_update_view_metrics.h"\n'
                        f"void (*checked)({argument}) = Model_UpdateViewMetrics;\n"
                        "void update(GsRVIEW2 *view) {\n"
                        "    Model_UpdateViewMetrics(view);\n"
                        "    Model_UpdateViewMetrics(0);\n"
                        "}\n"
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
