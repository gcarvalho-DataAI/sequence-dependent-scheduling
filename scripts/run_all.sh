#!/usr/bin/env bash
set -euo pipefail

JOBS=${JOBS:-24}
INDEX_FILE=${1:-}

if [[ -n "${INDEX_FILE}" && -f "${INDEX_FILE}" ]]; then
  mapfile -t files < "${INDEX_FILE}"
else
  mapfile -t files < <(find instances/instance_txt -type f -name '*.txt' | sort)
fi

if command -v parallel >/dev/null 2>&1; then
  printf "%s\n" "${files[@]}" | parallel -j"${JOBS}" ./bin/solver {}
else
  printf "%s\n" "${files[@]}" | xargs -P "${JOBS}" -n 1 ./bin/solver
fi
