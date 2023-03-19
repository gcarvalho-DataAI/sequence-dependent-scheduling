#include <stdlib.h>

#include <vector>

#include "ortools/base/logging.h"
#include "ortools/sat/cp_model.h"
#include "ortools/sat/cp_model.pb.h"
#include "ortools/sat/cp_model_solver.h"

namespace operations_research {
namespace sat {

void IntegerProgrammingExample() {
  // Data
  const std::vector<std::vector<int>> costs{
      { 5, 4, 2, 2, 2, 3, 2},{ 5, 4, 2, 2, 2, 3, 2},{ 4, 3, 2, 4, 2, 3, 1}, { 3, 4, 4, 3, 2, 1, 3},{ 5, 1, 4, 5, 5, 3, 3},{ 3, 3, 1, 1, 2, 3, 4},{ 2, 5, 4, 4, 2, 1, 2},
  };
  
  const int num_workers = static_cast<int>(costs.size());
  const int num_tasks = static_cast<int>(costs[0].size());

  // Model
  CpModelBuilder cp_model;

  // Variables
  // x[i][j] is an array of Boolean variables. x[i][j] is true
  // if worker i is assigned to task j.
  std::vector<std::vector<BoolVar>> x(num_workers,
                                      std::vector<BoolVar>(num_tasks));
  for (int i = 0; i < num_workers; ++i) {
    for (int j = 0; j < num_tasks; ++j) {
      x[i][j] = cp_model.NewBoolVar();
    }
  }

  // Constraints
  // Each worker is assigned to at most one task.
  for (int i = 0; i < num_workers; ++i) {
    cp_model.AddAtMostOne(x[i]);
  }
  // Each task is assigned to exactly one worker.
  for (int j = 0; j < num_tasks; ++j) {
    std::vector<BoolVar> tasks;
    for (int i = 0; i < num_workers; ++i) {
      tasks.push_back(x[i][j]);
    }
    cp_model.AddExactlyOne(tasks);
  }

  // Objective
  LinearExpr total_cost;
  for (int i = 0; i < num_workers; ++i) {
    for (int j = 0; j < num_tasks; ++j) {
      total_cost += x[i][j] * costs[i][j];
    }
  }
  cp_model.Minimize(total_cost);

  // Solve
  const CpSolverResponse response = Solve(cp_model.Build());

  // Print solution.
  if (response.status() == CpSolverStatus::INFEASIBLE) {
    LOG(FATAL) << "No solution found.";
  }

  LOG(INFO) << "Total cost: " << response.objective_value();
  LOG(INFO);
  for (int i = 0; i < num_workers; ++i) {
    for (int j = 0; j < num_tasks; ++j) {
      if (SolutionBooleanValue(response, x[i][j])) {
        LOG(INFO) << "Task " << i << " assigned to worker " << j
                  << ".  Cost: " << costs[i][j];
      }
    }
  }
}
}  // namespace sat
}  // namespace operations_research

int main(int argc, char** argv) {
  operations_research::sat::IntegerProgrammingExample();
  return EXIT_SUCCESS;
}