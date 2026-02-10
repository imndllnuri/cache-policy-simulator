#!/usr/bin/env bash
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"

export INSPECTOR_HOME="${REPO_ROOT}"
export SDE_BUILD_KIT="${REPO_ROOT}/tools/sde-kit"
export PIN_ROOT="${SDE_BUILD_KIT}/pinkit"

# Pin-compatible kit root inside SDE
export PIN_ROOT="${SDE_BUILD_KIT}/pinkit"

export PATH="${PATH}:${INSPECTOR_HOME}"
