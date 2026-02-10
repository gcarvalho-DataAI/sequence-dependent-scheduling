#include <cstdio>
#include <filesystem>
#include <numeric>
#include <string>
#include <vector>

#include "solver.hpp"

using namespace std;
namespace fs = std::filesystem;

#define RUNS 10						// how many runs for each instance?

static string jsonEscape(const string& s) {
	string out;
	out.reserve(s.size() + 8);
	for (char c : s) {
		switch (c) {
			case '"': out += "\\\""; break;
			case '\\': out += "\\\\"; break;
			case '\b': out += "\\b"; break;
			case '\f': out += "\\f"; break;
			case '\n': out += "\\n"; break;
			case '\r': out += "\\r"; break;
			case '\t': out += "\\t"; break;
			default:
				if (static_cast<unsigned char>(c) < 0x20) {
					char buf[7];
					snprintf(buf, sizeof(buf), "\\u%04x", c);
					out += buf;
				} else {
					out += c;
				}
		}
	}
	return out;
}

int main(int argc, char** argv)
{
	int solutionValue;					// stores the result
	int i = 0, j;

	double runningTime;					// stores the running time

	string indexPath = "index.txt";
	string outDir = "results";
	vector<string> instances;

	for (int a = 1; a < argc; ++a) {
		string arg = argv[a];
		if (arg == "--index" && a + 1 < argc) {
			indexPath = argv[++a];
		} else if (arg == "--out" && a + 1 < argc) {
			outDir = argv[++a];
		} else if (arg == "--help" || arg == "-h") {
			printf("Usage: %s [--index FILE] [--out DIR] [instance1 instance2 ...]\n", argv[0]);
			return 0;
		} else {
			instances.push_back(arg);
		}
	}

	if (instances.empty()) {
		ifstream fpIndex(indexPath);
		if (!fpIndex) {
			fprintf(stderr, "ERROR: cannot open index file: %s\n", indexPath.c_str());
			return 1;
		}
		string inputFileName;
		while (fpIndex >> inputFileName) {
			instances.push_back(inputFileName);
		}
	}

	fs::create_directories(outDir);

	for (const auto& inputFileName : instances)
	{
		i++;

		fs::path inPath(inputFileName);
		string baseName = inPath.filename().string();
		string outPath = (fs::path(outDir) / (baseName + ".summary.txt")).string();
		string jsonPath = (fs::path(outDir) / (baseName + ".summary.json")).string();
		ofstream fpOut(outPath);
		ofstream fpJson(jsonPath);

		if (!fpOut) {
			fprintf(stderr, "ERROR: cannot write to %s\n", outPath.c_str());
			return 1;
		}
		if (!fpJson) {
			fprintf(stderr, "ERROR: cannot write to %s\n", jsonPath.c_str());
			return 1;
		}

		vector<int> values;
		vector<double> times;
		values.reserve(RUNS);
		times.reserve(RUNS);

		fpOut << "# instance: " << inputFileName << "\n";
		fpOut << "# runs: " << RUNS << "\n";
		fpOut << "# run_id value time_seconds\n";

		fpJson << "{\n";
		fpJson << "  \"instance\": \"" << jsonEscape(inputFileName) << "\",\n";
		fpJson << "  \"runs\": " << RUNS << ",\n";
		fpJson << "  \"items\": [\n";

		for (j = 0; j < RUNS; j++)
		{
			printf("Run %d\n", j + 1);

			multiRun(&solutionValue, &runningTime, inputFileName, j + 1);
			printf("PROBLEM %d: %s %d\n", i, inputFileName.c_str(), solutionValue);

			values.push_back(solutionValue);
			times.push_back(runningTime);

			fpOut << (j + 1) << " " << solutionValue << " " << runningTime << "\n";
			fpJson << "    {\"run_id\": " << (j + 1)
					<< ", \"value\": " << solutionValue
					<< ", \"time_seconds\": " << runningTime << "}";
			if (j + 1 < RUNS) fpJson << ",";
			fpJson << "\n";
		}

		double avgValue = values.empty() ? 0.0
			: static_cast<double>(accumulate(values.begin(), values.end(), 0LL)) / values.size();
		double avgTime = times.empty() ? 0.0
			: accumulate(times.begin(), times.end(), 0.0) / times.size();

		int bestValue = values.empty() ? 0 : values.front();
		double bestTime = times.empty() ? 0.0 : times.front();
		for (size_t k = 1; k < values.size(); ++k) {
			if (values[k] < bestValue) {
				bestValue = values[k];
				bestTime = times[k];
			}
		}

		fpOut << "# avg_value " << avgValue << "\n";
		fpOut << "# avg_time " << avgTime << "\n";
		fpOut << "# best_value " << bestValue << "\n";
		fpOut << "# best_time " << bestTime << "\n";

		// Convergence curve as best-so-far across runs.
		fpOut << "# convergence_best_so_far ";
		int bestSoFar = values.empty() ? 0 : values.front();
		fpJson << "  ],\n";
		fpJson << "  \"avg_value\": " << avgValue << ",\n";
		fpJson << "  \"avg_time\": " << avgTime << ",\n";
		fpJson << "  \"best_value\": " << bestValue << ",\n";
		fpJson << "  \"best_time\": " << bestTime << ",\n";
		fpJson << "  \"convergence_best_so_far\": [";
		for (size_t k = 0; k < values.size(); ++k) {
			if (values[k] < bestSoFar) {
				bestSoFar = values[k];
			}
			fpOut << bestSoFar;
			fpJson << bestSoFar;
			if (k + 1 < values.size()) {
				fpOut << ",";
				fpJson << ",";
			}
		}
		fpOut << "\n";
		fpJson << "]\n";
		fpJson << "}\n";
	}
}
