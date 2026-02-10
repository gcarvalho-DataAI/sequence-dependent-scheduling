# Sequence-Dependent Scheduling (SSP) Solver

C++17 solver for the Sequence-Dependent Scheduling Problem, focusing on minimizing tool switches and setup times. The project integrates OR-Tools for optimization and provides benchmark instances and reproducible experiments.

## Repository Layout

- `src/` – main entry point (`testManager.cpp`)
- `include/` – solver headers (`solver.hpp`, `pl.hpp`)
- `instances/` – problem instances (txt/xlsx)
- `index.txt` – list of instance files used by the solver
- `tests/` – small tests and examples
- `tools/` – utilities (e.g., XLSX → TXT converter)
- `results/` – experiment outputs (ignored by Git)
- `third_party/or-tools/` – OR-Tools source (not tracked; see setup)

## Setup

### 1) Dependencies (Ubuntu)

```bash
sudo apt update
sudo apt install -y cmake g++ make pkg-config libprotobuf-dev protobuf-compiler libgoogle-glog-dev libeigen3-dev zlib1g-dev
```

### 2) OR-Tools (source, local build)

Clone OR-Tools into `third_party/or-tools`:

```bash
git clone https://github.com/google/or-tools.git third_party/or-tools
cd third_party/or-tools
make cpp -j4
```

### 3) Build the solver

```bash
cd /path/to/sequence-dependent-scheduling

g++ -std=c++17 -O2 src/testManager.cpp -o bin/solver \
  -I include \
  -I third_party/or-tools/install_make/include \
  -L third_party/or-tools/install_make/lib \
  -Wl,-rpath,$PWD/third_party/or-tools/install_make/lib \
  -lortools -lprotobuf -lglog -pthread
```

### 4) Run

```bash
./bin/solver
```

The solver reads instances from `index.txt` and writes one summary file per instance in `results/` (ignored by Git).

### Parallel execution

Run all instances in parallel (24 jobs example):

```bash
find instances/instance_txt -type f -name "*.txt" | sort | parallel -j24 ./bin/solver {}
```

Each instance writes to `results/\<instance\>.summary.txt`.

Or run:

```bash
JOBS=24 ./scripts/run_all.sh
```


## Notes

- OR-Tools is **not** committed to this repository. Always clone it into `third_party/or-tools`.
- If you prefer not to set rpath, export:
  ```bash
  export LD_LIBRARY_PATH=$PWD/third_party/or-tools/install_make/lib:$LD_LIBRARY_PATH
  ```

## Results (Sample)

A typical run writes a summary in `RESULTS_SUMMARY.txt`. Example format:

```
<instance_path> <run_id> <objective_value> <runtime_seconds>
```

Add a short table or plot here once you select the benchmarks you want to showcase.

## License

MIT License. See `LICENSE`.
