#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++17 -O2 src/legacyManager.cpp -o bin/solver_legacy \
  -I backup/backup \
  -I third_party/or-tools/install_make/include \
  -L third_party/or-tools/install_make/lib \
  -Wl,-rpath,$PWD/third_party/or-tools/install_make/lib \
  -lortools -lprotobuf -lglog -pthread
