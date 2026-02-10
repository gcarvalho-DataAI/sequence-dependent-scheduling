#!/usr/bin/env bash
set -euo pipefail

JOBS=${JOBS:-8}
INDEX_FILE=""
OUT_DIR="results_legacy"
SOL_DIR="solutions_legacy"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --index)
      INDEX_FILE="${2:-}"
      shift 2
      ;;
    --out)
      OUT_DIR="${2:-}"
      shift 2
      ;;
    --solutions)
      SOL_DIR="${2:-}"
      shift 2
      ;;
    --jobs)
      JOBS="${2:-}"
      shift 2
      ;;
    -h|--help)
      echo "Usage: $0 [--index FILE] [--out DIR] [--solutions DIR] [--jobs N]"
      exit 0
      ;;
    *)
      echo "Unknown argumento: $1"
      exit 1
      ;;
  esac
done

ORTOOLS_LIB_DIR="third_party/or-tools/install_make/lib"
if [[ -d "${ORTOOLS_LIB_DIR}" ]]; then
  export LD_LIBRARY_PATH="${PWD}/${ORTOOLS_LIB_DIR}:${LD_LIBRARY_PATH:-}"
fi

if [[ -n "${INDEX_FILE}" && -f "${INDEX_FILE}" ]]; then
  mapfile -t files < "${INDEX_FILE}"
else
  mapfile -t files < <(find instances/instance_txt -type f -name '*.txt' | sort)
fi

if command -v parallel >/dev/null 2>&1; then
  printf "%s\n" "${files[@]}" | parallel -j"${JOBS}" ./bin/solver_legacy --out "${OUT_DIR}" --solutions "${SOL_DIR}" {}
else
  printf "%s\n" "${files[@]}" | xargs -P "${JOBS}" -n 1 ./bin/solver_legacy --out "${OUT_DIR}" --solutions "${SOL_DIR}"
fi
