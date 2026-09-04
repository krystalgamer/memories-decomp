ROOT := $(CURDIR)
LOCAL_PYTHON := $(ROOT)/tools/environments/python/bin/python
SPLAT := $(ROOT)/tools/environments/python/bin/splat
PYTHON ?= $(if $(wildcard $(LOCAL_PYTHON)),$(LOCAL_PYTHON),python3)
BOOTSTRAP_PYTHON ?= python3
USE_SYSTEM_MIPS_BINUTILS ?= 0

ifeq ($(USE_SYSTEM_MIPS_BINUTILS),1)
BUILD_BINUTILS_CHECK := tools/bootstrap/binutils_system.py --check
else
BUILD_BINUTILS_CHECK := tools/bootstrap/binutils.py --check
endif

export HOME := $(ROOT)/tmp/home
export TMPDIR := $(ROOT)/tmp
export XDG_CACHE_HOME := $(ROOT)/tmp/cache
export PIP_CACHE_DIR := $(ROOT)/tmp/pip-cache
export PYTHONPYCACHEPREFIX := $(ROOT)/tmp/pycache
export NPM_CONFIG_CACHE := $(ROOT)/tmp/npm-cache
export CARGO_HOME := $(ROOT)/tools/environments/cargo
export RUSTUP_HOME := $(ROOT)/tools/environments/rustup
export GOPATH := $(ROOT)/tools/environments/go
export GOMODCACHE := $(ROOT)/tools/environments/go/pkg/mod

.DEFAULT_GOAL := help

.PHONY: help workspace verify-target verify-inputs tools python-tools toolchain toolchain-system compiler compiler-281 compiler-281-prebuilt compiler-272 check-tools check-build-tools info extract map split build match overlays verify-overlays build-overlays match-overlays inventory classify-functions candidates siblings external-attempts basic-types global-usage check-global-usage progress check-progress disc-layout verify-disc runtime-files verify-runtime-files audit clean

help:
	@printf '%s\n' \
		'Available targets:' \
		'  tools          Install pinned project tools beneath tools/' \
		'  check-tools    Verify pinned local project tools' \
		'  check-build-tools  Verify only tools required for a clean build' \
		'  info           Show the verified PS-X executable header' \
		'  extract        Extract the verified header and loaded payload' \
		'  map            Validate the top-level executable region map' \
		'  split          Split the executable into temporary analysis output' \
		'  build          Build the assembly/data PS-X executable baseline' \
		'  match          Build and compare the complete target executable' \
		'  overlays       Extract verified runtime overlay module images' \
		'  verify-overlays  Verify extracted overlay images and metadata' \
		'  build-overlays Build verified runtime overlay module images' \
		'  match-overlays Build and compare all configured overlay modules' \
		'  inventory      Update the tracked resident-function inventory' \
		'  classify-functions  Apply verified ownership classifications' \
		'  candidates     List smallest zero-attempt game functions' \
		'  siblings       Find exact-C functions with similar instruction shapes' \
		'  external-attempts  Validate external-reference/refinement attempts' \
		'  basic-types    Verify all C sources use src/types.h' \
		'  global-usage   Regenerate tracked game-global usage reports' \
		'  check-global-usage  Verify tracked game-global usage reports' \
		'  progress       Update README and generate current progress metrics' \
		'  check-progress Verify that the README progress snapshot is current' \
		'  disc-layout    Regenerate the tracked ISO9660 LBA manifest' \
		'  verify-disc    Verify BIN/CUE layout and extracted file contents' \
		'  runtime-files  Regenerate executable file-index/LBA metadata' \
		'  verify-runtime-files  Verify runtime file order against disc LBAs' \
		'  audit          Verify exact output, metadata, and repository policy' \
		'  clean          Remove known generated project output under tmp/' \
		'  verify-target  Validate only the SLUS executable needed to build' \
		'  verify-inputs  Validate the SLUS-01411 executable and DATA files' \
		'  workspace      Validate that commands are running from the project root'

workspace:
	@$(PYTHON) tools/project/session_memory_guard.py
	@$(PYTHON) tools/project/workspace.py

verify-target: workspace
	@$(PYTHON) tools/project/verify_inputs.py --executable-only

verify-inputs: workspace
	@$(PYTHON) tools/project/verify_inputs.py

tools: python-tools toolchain compiler

python-tools: verify-target
	@$(BOOTSTRAP_PYTHON) tools/bootstrap/bootstrap.py

toolchain: verify-target
	@$(BOOTSTRAP_PYTHON) tools/bootstrap/binutils.py

toolchain-system: verify-target
	@$(BOOTSTRAP_PYTHON) tools/bootstrap/binutils_system.py

compiler: compiler-281 compiler-272

compiler-281: verify-target
	@$(BOOTSTRAP_PYTHON) tools/bootstrap/old_gcc.py

compiler-281-prebuilt: verify-target
	@$(BOOTSTRAP_PYTHON) tools/bootstrap/old_gcc_prebuilt.py

compiler-272: verify-target
	@$(BOOTSTRAP_PYTHON) tools/bootstrap/old_gcc_272.py

check-tools: workspace
	@$(PYTHON) tools/bootstrap/bootstrap.py --check
	@$(PYTHON) tools/bootstrap/binutils.py --check
	@$(PYTHON) tools/bootstrap/old_gcc.py --check
	@$(PYTHON) tools/bootstrap/old_gcc_272.py --check

check-build-tools: workspace
	@$(PYTHON) tools/bootstrap/bootstrap.py --check
	@$(PYTHON) $(BUILD_BINUTILS_CHECK)
	@$(PYTHON) tools/bootstrap/old_gcc_prebuilt.py --check

info: verify-target
	@$(PYTHON) tools/project/psx_exe.py info

extract: verify-target
	@$(PYTHON) tools/project/psx_exe.py extract

map: verify-target
	@$(PYTHON) tools/project/validate_image_map.py

split: map check-build-tools
	@$(PYTHON) tools/project/clean.py generated splat
	@$(PYTHON) tools/project/generate_build_config.py
	@$(SPLAT) split tmp/generated/slus_01411.split.yaml

build: split
	@$(PYTHON) tools/project/clean.py project-build
	@$(PYTHON) tools/project/build_baseline.py

match: build
	@$(PYTHON) tools/project/match.py

overlays: workspace
	@$(PYTHON) tools/project/overlay_extract.py extract

verify-overlays: workspace
	@$(PYTHON) tools/project/overlay_extract.py verify

build-overlays: overlays check-build-tools
	@$(PYTHON) tools/project/overlay_build.py build

match-overlays: build-overlays
	@$(PYTHON) tools/project/overlay_build.py verify

build-incremental: split
	@$(PYTHON) tools/project/build_incremental.py

match-incremental: build-incremental
	@$(PYTHON) tools/project/match.py

inventory: split
	@$(PYTHON) tools/project/function_inventory.py

classify-functions: inventory
	@$(PYTHON) tools/project/classify_functions.py

candidates: workspace
	@$(PYTHON) tools/project/select_candidates.py $(CANDIDATE_ARGS)

siblings: verify-inputs
	@$(PYTHON) tools/project/find_siblings.py $(SIBLING_ARGS)

external-attempts: workspace
	@$(PYTHON) tools/project/record_external_attempt.py --check

basic-types: workspace
	@$(PYTHON) tools/project/centralize_basic_types.py --check

global-usage: split
	@$(PYTHON) tools/project/global_usage.py

check-global-usage: split
	@$(PYTHON) tools/project/global_usage.py --check

progress: split
	@$(PYTHON) tools/project/progress.py

check-progress: split
	@$(PYTHON) tools/project/progress.py --check

disc-layout: verify-inputs
	@$(PYTHON) tools/project/disc_image.py write

verify-disc: verify-inputs
	@$(PYTHON) tools/project/disc_image.py verify

runtime-files: verify-disc
	@$(PYTHON) tools/project/runtime_files.py write

verify-runtime-files: verify-disc
	@$(PYTHON) tools/project/runtime_files.py verify

audit: match verify-runtime-files
	@$(PYTHON) tools/project/function_inventory.py
	@$(PYTHON) tools/project/classify_functions.py
	@$(PYTHON) tools/project/centralize_basic_types.py --check
	@$(PYTHON) tools/project/progress.py --check
	@$(PYTHON) tools/project/audit_repository.py

clean: workspace
	@$(PYTHON) tools/project/clean.py extract generated splat project-build overlays reports
