#!/usr/bin/env python3
"""Move MODEL images' leading build-list word to the shared C source.

For every `config/<region>/overlays/model_*.yaml` whose `module_header`
segment still holds the raw `[0x0, data, overlays/<name>/module_header]`
subsegment, read the word from the extracted image (the layout's
`target_path`), record it in `config/<region>/overlays/model_module_words.csv`
and point the subsegment at `overlays/model_variant/module_header` as `.data`.
The overlay build then compiles src/overlays/model_variant/module_header.c for
that image with the recorded word (tools/project/overlay_sources.py).

Run from the repository root after extracting the region's overlays.
"""

from __future__ import annotations

import argparse
import csv
from pathlib import Path
import re
import struct
import sys

from overlay_sources import (
    MODEL_MODULE_WORDS,
    MODEL_MODULE_WORDS_HEADER,
    SEGMENT_NAME,
    OverlaySourceError,
    model_module_words,
)
from workspace import WorkspaceError, require_workspace_root, resolve_within


RAW_HEADER = re.compile(
    r"^(\s*-\s*\[\s*)(0x0|0)(\s*,\s*)data(\s*,\s*)"
    r"overlays/[A-Za-z0-9_]+/module_header(\s*\]\s*)$"
)
SUBSEGMENT = re.compile(r"^\s*-\s*\[")
TARGET_PATH = re.compile(r"^\s*target_path:\s*(\S+)\s*$", re.MULTILINE)
SHARED_HEADER = "overlays/model_variant/module_header"


class OwnError(RuntimeError):
    pass


def own_header(text: str, label: str) -> str | None:
    """Rewrite the raw header subsegment; None if the layout has none."""
    lines = text.split("\n")
    found = [index for index, line in enumerate(lines) if RAW_HEADER.match(line)]
    if not found:
        return None
    if len(found) > 1:
        raise OwnError(f"{label}: several raw module_header subsegments")
    index = found[0]
    owner = None
    for back in range(index - 1, -1, -1):
        match = SEGMENT_NAME.match(lines[back])
        if match is not None:
            owner = (back, match)
            break
    if owner is None or owner[1].group(2) != "module_header":
        raise OwnError(f"{label}: raw header is not in a module_header segment")
    indent = len(owner[1].group(1))
    subsegments = 0
    for line in lines[owner[0] + 1 :]:
        match = SEGMENT_NAME.match(line)
        if match is not None and len(match.group(1)) <= indent:
            break
        if SUBSEGMENT.match(line):
            subsegments += 1
    if subsegments != 1:
        raise OwnError(
            f"{label}: the module_header segment must hold only the header word"
        )
    lines[index] = RAW_HEADER.sub(
        lambda m: f"{m.group(1)}{m.group(2)}{m.group(3)}.data{m.group(4)}"
        f"{SHARED_HEADER}{m.group(5)}",
        lines[index],
    )
    return "\n".join(lines)


def write_words(path: Path, words: dict[str, int]) -> None:
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle, lineterminator="\n")
        writer.writerow(MODEL_MODULE_WORDS_HEADER)
        for stem in sorted(words):
            writer.writerow([stem, words[stem]])


def own_region(root: Path, region: str) -> tuple[int, int]:
    overlays = resolve_within(root, f"config/{region}/overlays", must_exist=True)
    words = dict(model_module_words(root, f"config/{region}"))
    rewrites: list[tuple[Path, str]] = []
    for layout in sorted(overlays.glob("model_*.yaml")):
        label = layout.relative_to(root).as_posix()
        text = layout.read_text(encoding="utf-8")
        owned = own_header(text, label)
        if owned is None:
            continue
        if layout.stem in words:
            raise OwnError(f"{label}: already has a {MODEL_MODULE_WORDS} row")
        target = TARGET_PATH.search(text)
        if target is None:
            raise OwnError(f"{label}: no target_path")
        image = resolve_within(root, target.group(1))
        if not image.is_file():
            raise OwnError(f"{label}: extract the overlays first ({target.group(1)})")
        with image.open("rb") as handle:
            head = handle.read(4)
        if len(head) != 4:
            raise OwnError(f"{label}: {target.group(1)} is shorter than one word")
        (word,) = struct.unpack("<I", head)
        if word == 0:
            raise OwnError(f"{label}: a zero word would compile to .bss")
        words[layout.stem] = word
        rewrites.append((layout, owned))
    for layout, owned in rewrites:
        layout.write_text(owned, encoding="utf-8")
    if rewrites:
        write_words(resolve_within(root, f"config/{region}/{MODEL_MODULE_WORDS}"), words)
    return len(rewrites), len(words)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument(
        "region", help="config directory name, for example slus_01411"
    )
    args = parser.parse_args()
    if re.fullmatch(r"[a-z]{4}_[0-9]{5}", args.region) is None:
        parser.error("region must be a config directory name such as slus_01411")
    try:
        root = require_workspace_root()
        owned, total = own_region(root, args.region)
    except (OwnError, OverlaySourceError, WorkspaceError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    print(f"{args.region}: owned {owned} module words ({total} rows)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
