#!/usr/bin/env python3

from __future__ import annotations

import json
import sys
from dataclasses import replace
from pathlib import Path
from typing import Any

from function_inventory import (
    InventoryError,
    STATUSES,
    Function,
    load_inventory,
    write_inventory,
)
from workspace import WorkspaceError, require_workspace_root, resolve_within


class ClassificationError(RuntimeError):
    pass


def parse_integer(value: Any, description: str) -> int:
    if isinstance(value, int) and not isinstance(value, bool):
        return value
    if isinstance(value, str):
        try:
            return int(value, 0)
        except ValueError as error:
            raise ClassificationError(
                f"{description} is not a valid integer: {value}"
            ) from error
    raise ClassificationError(
        f"{description} must be an integer or integer string"
    )


def load_regions(
    root: Path, configuration_path: str = "config/slus_01411/function_regions.json"
) -> list[dict[str, Any]]:
    path = resolve_within(
        root,
        configuration_path,
        must_exist=True,
    )
    with path.open("r", encoding="utf-8") as handle:
        configuration = json.load(handle)
    if configuration.get("schema") != 1:
        raise ClassificationError(f"{path}: unsupported classification schema")

    raw_regions = configuration.get("regions")
    if not isinstance(raw_regions, list) or not raw_regions:
        raise ClassificationError(f"{path}: regions must be a non-empty list")

    regions: list[dict[str, Any]] = []
    previous_end: int | None = None
    for index, raw_region in enumerate(raw_regions):
        if not isinstance(raw_region, dict):
            raise ClassificationError(f"{path}: region {index} must be an object")
        name = raw_region.get("name")
        module = raw_region.get("module")
        if not isinstance(name, str) or not name:
            raise ClassificationError(f"{path}: region {index} has no name")
        if not isinstance(module, str) or not module:
            raise ClassificationError(f"{path}: region {name} has no module")

        start = parse_integer(raw_region.get("start"), f"{name}.start")
        end = parse_integer(raw_region.get("end"), f"{name}.end")
        if end <= start:
            raise ClassificationError(f"{path}: region {name} has an empty range")
        if previous_end is not None and start != previous_end:
            raise ClassificationError(
                f"{path}: region {name} starts at {start:#010x}; "
                f"expected {previous_end:#010x}"
            )

        status = raw_region.get("status")
        if status is not None and status not in STATUSES:
            raise ClassificationError(
                f"{path}: region {name} has unsupported status {status}"
            )

        regions.append(
            {
                "name": name,
                "start": start,
                "end": end,
                "module": module,
                "status": status,
            }
        )
        previous_end = end
    return regions


def classify_function(
    function: Function, regions: list[dict[str, Any]]
) -> Function:
    for region in regions:
        if region["start"] <= function.address < region["end"]:
            if function.address + function.size > region["end"]:
                raise ClassificationError(
                    f"{function.name} at {function.address:#010x} crosses "
                    f"region {region['name']}"
                )
            status = function.status
            if region["status"] is not None and status != "matching_c":
                status = region["status"]
            return replace(
                function,
                status=status,
                module=region["module"],
            )
    raise ClassificationError(
        f"{function.name} at {function.address:#010x} is outside all regions"
    )


def validate_coverage(
    functions: list[Function], regions: list[dict[str, Any]]
) -> None:
    if not functions:
        raise ClassificationError("function inventory is empty")
    if functions[0].address != regions[0]["start"]:
        raise ClassificationError(
            "function inventory does not start at the first classification region"
        )
    final_function = functions[-1]
    if final_function.address + final_function.size != regions[-1]["end"]:
        raise ClassificationError(
            "function inventory does not end at the last classification region"
        )


def main() -> int:
    try:
        root = require_workspace_root()
        inventory_path = resolve_within(
            root,
            "config/slus_01411/functions.csv",
            must_exist=True,
        )
        functions = load_inventory(inventory_path)
        regions = load_regions(root)
        validate_coverage(functions, regions)
        classified = [
            classify_function(function, regions) for function in functions
        ]
        write_inventory(inventory_path, classified)
    except (
        ClassificationError,
        InventoryError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
        json.JSONDecodeError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1

    counts: dict[str, int] = {}
    for function in classified:
        counts[function.module] = counts.get(function.module, 0) + 1
    for module in sorted(counts):
        print(f"{module}: {counts[module]} functions")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
