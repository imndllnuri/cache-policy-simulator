#!/usr/bin/env bash
set -euo pipefail

# Resolve paths robustly
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
cd "${REPO_ROOT}"

# ---- config ----
SDE_VERSION="10.5.0"
SDE_URL="https://downloadmirror.intel.com/873619/sde-external-10.5.0-2026-01-13-lin.tar.xz"
SDE_DIR="${REPO_ROOT}/tools/sde-kit"
TARBALL="sde.tar.xz"

# ---- sanity checks ----
command -v tar >/dev/null
command -v make >/dev/null
command -v g++ >/dev/null
command -v wget >/dev/null || command -v curl >/dev/null

mkdir -p "${SDE_DIR}"

# ---- download + extract SDE ----
if [[ -x "${SDE_DIR}/sde64" ]]; then
  echo "[INFO] Intel SDE already installed"
else
  echo "[INFO] Downloading Intel SDE ${SDE_VERSION}"

  if command -v wget >/dev/null; then
    wget -O "${TARBALL}" "${SDE_URL}"
  else
    curl -L -o "${TARBALL}" "${SDE_URL}"
  fi

  echo "[INFO] Extracting SDE..."
  tar -xf "${TARBALL}" -C "${SDE_DIR}" --strip-components=1
  rm -f "${TARBALL}"
fi

# ---- environment ----
source "${SCRIPT_DIR}/setvars.sh"

# ---- sanity check Pin ----
[[ -x "${PIN_ROOT}/pin" ]] || {
  echo "[ERROR] Pin not found under pinkit"
  exit 1
}

# ---- build tool ----
echo "[INFO] Building Pintool..."
make -C tools/pintool obj-intel64/cache_trace.so

echo "[OK] Installation completed successfully"
