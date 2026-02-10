#!/usr/bin/env bash
set -euo pipefail

JOBS=${JOBS:-24}
INDEX_FILE=""
OUT_DIR=""

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
    --jobs)
      JOBS="${2:-}"
      shift 2
      ;;
    -h|--help)
      echo "Usage: $0 [--index FILE] [--out DIR] [--jobs N]"
      exit 0
      ;;
    *)
      echo "Unknown argumento: $1"
      exit 1
      ;;
  esac
done

if [[ -n "${INDEX_FILE}" && -f "${INDEX_FILE}" ]]; then
  mapfile -t files < "${INDEX_FILE}"
else
  mapfile -t files < <(find instances/instance_txt -type f -name '*.txt' | sort)
fi

SOLVER_ARGS=()
if [[ -n "${OUT_DIR}" ]]; then
  SOLVER_ARGS+=(--out "${OUT_DIR}")
fi

if command -v parallel >/dev/null 2>&1; then
  printf "%s\n" "${files[@]}" | parallel -j"${JOBS}" ./bin/solver "${SOLVER_ARGS[@]}" {}
else
  printf "%s\n" "${files[@]}" | xargs -P "${JOBS}" -n 1 ./bin/solver "${SOLVER_ARGS[@]}"
fi
