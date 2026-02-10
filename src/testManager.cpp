#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

#include "solver.hpp"

using namespace std;
namespace fs = std::filesystem;

#define RUNS 1						//how many runs for each instance?

int main(int argc, char** argv)
{
	int solutionValue;					//stores the result
	int i = 0, j;

	double runningTime;					//stores the running time

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
		ofstream fpOut(outPath);

		if (!fpOut) {
			fprintf(stderr, "ERROR: cannot write to %s\n", outPath.c_str());
			return 1;
		}

		for (j = 0; j < RUNS; j++)
		{
			printf("Run %d\n", j + 1);

			multiRun(&solutionValue, &runningTime, inputFileName, j + 1);
			printf("PROBLEM %d: %s %d\n", i, inputFileName.c_str(), solutionValue);
			fpOut << inputFileName << " " << j + 1 << " " << solutionValue << " " << runningTime << endl;
		}
	}
}
