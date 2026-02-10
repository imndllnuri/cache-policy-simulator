#!/usr/bin/env bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"

export INSPECTOR_HOME="${REPO_ROOT}"
export SDE_BUILD_KIT="${REPO_ROOT}/tools/sde-kit"

# Pin-compatible kit root inside SDE
export PIN_ROOT="${SDE_BUILD_KIT}/pinkit"

export PATH="${PATH}:${INSPECTOR_HOME}"
