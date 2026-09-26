#!/usr/bin/env python3

from __future__ import annotations

import sys

from build_baseline import BuildError
from build_regional_baseline import RegionalBuildConfiguration, build
from workspace import WorkspaceError, require_workspace_root


EUROPEAN_BUILD = RegionalBuildConfiguration(
    splat_directory="tmp/splat/sles_03947",
    output_directory="tmp/project-build",
    matching_config="config/sles_03947/matching_c.json",
    output_name="SLES_039.47",
    linker_script="sles_03947.ld",
    asm_directory="tmp/project-build/european-asm",
    expected_size=0x1D0800,
    link_symbols="config/sles_03947/link_symbols.ld",
)


def main() -> int:
    try:
        root = require_workspace_root()
        output = build(root, EUROPEAN_BUILD)
    except (BuildError, WorkspaceError, OSError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    print(f"built: {output.relative_to(root)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
