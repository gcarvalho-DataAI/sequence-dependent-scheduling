#ifndef SOLVER_HPP
#define SOLVER_HPP
#define DEBUG 0
#include <sys/stat.h>
#include <random>
#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <climits>
#include <vector>
#include <chrono>
#include <cstdio>
#include <numeric>
#include <tuple>
#include <functional>
#include <set>
#include <chrono>
 
#include "ortools/base/logging.h"
#include "ortools/sat/cp_model.h"
#include "ortools/sat/cp_model.pb.h"
#include "ortools/sat/cp_model_solver.h"
      // std::chrono::system_clock

using namespace std;
using namespace std::chrono;
using namespace operations_research::sat;

#define PRINT_MATRIX true

//Configuration of the datasets n, m, c, Tmin, Tmaz
int nJobs, nTools, magazineCapacity, minTools, maxTools;

int totalTime, bestTime, nSwaps, indexJob, value;

vector<int> bestSolution; /* array with the best jobs assignements*/
vector<int> currentSolution = {35 ,20 ,11 ,47 ,36 ,39 ,6 ,33 ,10 ,43 ,2 ,45 ,37 ,44 ,5 ,15 ,0 ,17 ,23 ,40 ,3 ,21 ,22 ,7 ,27 ,25 ,12 ,30 ,24 ,29 ,26 ,31 ,32 ,13 ,34 ,8 ,19 ,18 ,28 ,16 ,14 ,41 ,42 ,38 ,1 ,4 ,46 ,49 ,9 ,48}; /* array with the best jobs assignements*/
vector<set<int>> jobsAssignements; /* array with the jobs assignements and it's tools needed*/
vector<vector<int>> toolsTimesSwap; /* array with the tool swap times */
set<int> magazine;
vector<int> jobs;
vector<int> initialSetupTimes;
vector<vector<int>> jobsAssignementsBinary; /* array with the jobs assignements and it's tools needed*/
vector<set<pair<int, int>>> jobsAssignementsMatrix;
vector<pair<int, int>> localSearch;

//local-search
bool swap();
bool insertion();
bool oneBlock();
bool twoOpt();

void updateBestSolution(vector<int> jobs);
void printStart();
int runSolution(vector<int> solution);
int populateMagazine(set<int> tools);
vector<int> unsetZeros(vector<int> vector);
void checkToolOnMagazine(vector<int> tools);
int insertOnMagazine();
int getMaxElement(vector<int> vector);
int evaluation();
int contOneBlock(vector<int> solution);
int GPCA();
int getCost(vector<int> dif1, vector<int> dif2);
int searchCost(vector<int> costs, int startNodes, int endNodes);
int LAP(vector<vector<int>> costs, vector<int> uninstall, vector<int> install);

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& arr);
template<typename T>
ostream& operator<<(ostream& out, const vector<vector<T>>& matrix);
ostream& operator<<(ostream& output, const set<int>& set);
ostream& operator<<(ostream& output, const vector<vector<int>>& matrix);
ostream& operator<<(ostream& output, const vector<set<int>>& matrix);
ostream& operator<<(ostream& output, const vector<set<pair<int, int>>>& matrix);

//Node class for use in graph search
class Node{
public:	
	Node()
	{
		degree = 0;
		visited = false;
		id = 0;
	}
	
	void setDegree(int i)
	{
		degree = i;
	}
	
	void setVisited(bool b)
	{
		visited = b;
	}
		
	void setId(int i)
	{
		id = i;
	}

	int getDegree()
	{
		return degree;
	}
	
	bool wasVisited()
	{
		return visited;
	}
		
	int getId()
	{
		return id;
	}

private:
	int degree;	 //stores the degree of the node
	int id;		 //stores the index of the node
	bool visited; //indicates whether the node has been reached by the BFS
};


/*
Initializes the structures, vectors and matrices used
*/
void initialization(vector<int> searchs)
{
	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	localSearch.resize(4);

	for(int i = 0; i < 4; i++){
		localSearch[i].first = i;
		localSearch[i].second = 0; 
	}
	for(int i = 0; i < searchs.size(); i++){
		localSearch[i].first = searchs[i];
		localSearch[searchs[i]].first = i;
		localSearch[i].second = 1; 
	}

	
	totalTime = 0;
	bestTime = INT_MAX;
	nSwaps = 0;
	indexJob = 0;
    initialSetupTimes.resize(nTools);
	toolsTimesSwap.resize(nTools);
	bestSolution.resize(nJobs);
	//currentSolution.resize(nJobs);

	jobsAssignementsBinary.resize(nTools);
	jobsAssignements.resize(nJobs);
	jobs.resize(nJobs);

	for(int i = 0 ; i < nTools ; i++) {
		toolsTimesSwap[i].resize(nTools);
		jobsAssignementsBinary[i].resize(nJobs);

    }

	
	
}

/*
Reads the problem from a file specified by fileName
*/
void readProblem(string fileName)
{
	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	ifstream fpIn(fileName);
	int get;

	if(!fpIn)
        throw invalid_argument("ERROR : Instance " + fileName + "doesn't exist");


	fpIn >> nJobs >> nTools >> magazineCapacity >> minTools >> maxTools;

	cout << nJobs << " " << nTools << " " << magazineCapacity << " " << minTools << " " << maxTools << endl;

	initialization({0,1,3});//initializes all structures, vectors and matrices
	for(int i = 0 ; i < nTools ; i++) {
        fpIn >> get;
        initialSetupTimes[i] = get;
    }

	for(int i = 0 ; i < nTools ; i++) {
        for(int j = 0 ; j < nTools ; j++) {
            fpIn >> get;
            toolsTimesSwap[i][j] = get;
        }
    }

	for(int i = 0 ; i < nJobs ; i++) {
        currentSolution[i] = i;
    }




	//Deixar o inicio randomico
	// obtain a time-based seed:

	/*
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

	shuffle (currentSolution.begin(), currentSolution.end(), std::default_random_engine(seed));*/

	for(int i = 0 ; i < maxTools ; i++) {
        for(int j = 0 ; j < nJobs ; j++) {
            fpIn >> get;

			if(get > 0){
				jobsAssignements[j].insert(get);
				jobsAssignementsBinary[get - 1][j] = 1;
			}
				
        }
    }

	cout << jobsAssignements;
	cout << jobsAssignementsBinary;	
	
	
	updateBestSolution(currentSolution);
	//cout << jobsAssignementsBinary;

	//printStart();



}
/*
void printStart(){
	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	cout << initialSetupTimes;
	cout << endl << endl;

	
	cout << toolsTimesSwap;
	cout << endl << endl;

	
	cout << jobs;
	cout << endl << endl;

	
	cout << jobsAssignements;
	cout << endl << endl;


	cout << bestSolution;
	cout << endl << endl;
}
*/
void updateBestSolution(vector<int> jobs){
	if(DEBUG)
		cout <<__FUNCTION__ << endl;
    copy(jobs.begin(), jobs.end(), bestSolution.begin());

}

int runSolution(vector<int> solution){

	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	int cost = 0;
	pair<int, int> tool;
    set<pair<int, int>>::iterator it;
	vector<vector<int>> auxJobsAssignementsMatrix;

	indexJob = 0;

	if(DEBUG){
		cout << jobsAssignements; 
		cout << "job -> " << currentSolution[indexJob] << endl;
		cout << magazine;
		cout << "cost: " << cost << endl;

	} 
   
    jobsAssignementsMatrix.resize(nJobs);

    auxJobsAssignementsMatrix.resize(nTools);
    for(int i = 0; i < nTools; i++){
        auxJobsAssignementsMatrix[i].resize(nJobs);

    }

    for(int i = 0; i < nTools; i++){
        if(jobsAssignementsBinary[i][solution[nJobs - 1]] == 1){

            auxJobsAssignementsMatrix[i][nJobs - 1] = nJobs - 1;

        }
        else{
            auxJobsAssignementsMatrix[i][nJobs - 1] = INT_MAX;

        }
    }

    for(int i = 0; i < nTools; i++){
        for(int j = nJobs - 2; j >= 0;j--){
            if(jobsAssignementsBinary[i][solution[j]] == 1){
                
                auxJobsAssignementsMatrix[i][j] = j;
            }
            else{
                auxJobsAssignementsMatrix[i][j] = auxJobsAssignementsMatrix[i][j + 1];

            }
        }
    }

    for(int i = 0; i < nTools; i++){
        for(int j = nJobs - 1; j >= 0;j--){

            tool.first = auxJobsAssignementsMatrix[i][j];
            tool.second = i + 1;
            jobsAssignementsMatrix[j].insert(tool);

        }
    }
	if(DEBUG)
		cout << "Jobs assignements matrix: " << jobsAssignementsMatrix;


    for(auto& v : auxJobsAssignementsMatrix) {
        v.clear();
    }
    auxJobsAssignementsMatrix.clear();
    magazine.clear();

    indexJob = 0;
    cost = insertOnMagazine(); 

	for(indexJob = 1; indexJob < nJobs; indexJob++){

		cost += GPCA();

		if(DEBUG){
			cout << "job -> " << currentSolution[indexJob] << endl;
			cout << magazine;
			cout << "cost: " << cost << endl;
			cout << "fim job" << endl;

		} 

	}

	magazine.clear();

	for(int i = 0; i < nJobs; i++){
		jobsAssignementsMatrix[i].clear();

	}
	jobsAssignementsMatrix.clear();

	return cost;
	
}

int GPCA(){

	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	int cost = 0;
	set<int> updateMagazine;
	set<int> uninstall;
	vector<int> vectorUninstall;
	vector<int> vectorInstall;
	set<int> install;
	set<int>::iterator it1, it2;
	vector<vector<int>> all_costs;

	
	for(auto& v : jobsAssignementsMatrix[indexJob]){
        if(updateMagazine.size() < magazineCapacity  && v.second < INT_MAX){

                updateMagazine.insert(v.second);
        }
        else{
            break;
        }
    }

	if(updateMagazine.size() < magazineCapacity){
		for(auto& v : magazine){
			updateMagazine.insert(v);

			if(updateMagazine.size() >= magazineCapacity){
				break;
			}
		}
	}


	set_difference(updateMagazine.begin(), updateMagazine.end(), magazine.begin(), magazine.end(),  inserter(install, install.begin()));
	set_difference(magazine.begin(), magazine.end(), updateMagazine.begin(), updateMagazine.end(), inserter(uninstall, uninstall.begin()));

	if(DEBUG){

		cout << "Current magazine: " << magazine;
		cout << "Update magazine: " << updateMagazine;
		cout << "Unstalling tools: " << uninstall;
		cout << "Installing tools: " << install; 

	}

	it1 = uninstall.begin();
	it2 = install.begin();

	for(int i = 0; i < uninstall.size(); i++){

		cost += toolsTimesSwap[*it1 - 1][*it2- 1];

		it1++;
		it2++;

	}

	/*
	int indexCost = 0;
	all_costs.resize(uninstall.size());
	vectorUninstall.push_back(*it1);


	for(int j = 0; j < install.size(); j++){

		vectorInstall.push_back(*it2);
		if(DEBUG)
			cout << *it1 << " -> " << *it2 << " = " << toolsTimesSwap[*it1 - 1][*it2- 1] << endl;

		all_costs[indexCost].push_back(toolsTimesSwap[*it1 - 1][*it2- 1]);
		it2++;
	}
	it1++;
	indexCost++;

	for(int i = 1; i < uninstall.size(); i++){

		vectorUninstall.push_back(*it1);
		it2 = install.begin();
		
		for(int j = 0; j < install.size(); j++){
			if(DEBUG)
				cout << *it1 << " -> " << *it2 << " = " << toolsTimesSwap[*it1 - 1][*it2- 1] << endl;
			all_costs[indexCost].push_back(toolsTimesSwap[*it1 - 1][*it2- 1]);
			it2++;
		}

		it1++;
		indexCost++;

	}

	cost = LAP(all_costs, vectorUninstall, vectorInstall);*/

	if(DEBUG)
		cout << "Job cost: " << cost << endl;

	swap(updateMagazine, magazine);

	updateMagazine.clear();
	for(auto& v : all_costs)
        v.clear();
    
	all_costs.clear();
	//exit(1);



	return cost;
}


int LAP(vector<vector<int>> costs, vector<int> uninstall, vector<int> install) {
	// Data
	
	const int num_workers = static_cast<int>(costs.size());
	const int num_tasks = static_cast<int>(costs[0].size());

	// Model
	CpModelBuilder cp_model;

	// Variables
	// x[i][j] is an array of Boolean variables. x[i][j] is true
	// if worker i is assigned to task j.
	std::vector<std::vector<BoolVar>> x(num_workers, std::vector<BoolVar>(num_tasks));

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

	const CpSolverResponse response = Solve(cp_model.Build());

	if (response.status() == CpSolverStatus::INFEASIBLE) {
		cout << "LAP failure, no solution found." << endl;
		exit(1);
	}

	if(DEBUG){
		cout << "Total cost: " << response.objective_value() << endl;
		
		for (int i = 0; i < num_workers; ++i) {
			for (int j = 0; j < num_tasks; ++j) {
				if (SolutionBooleanValue(response, x[i][j])) {
					cout << "Uninstalling " << uninstall[i] << " installing " << install[j]
							<< ".  Cost: " << costs[i][j] << endl;
				}
			}
		}
	}

	return response.objective_value();
	
}


int getCost(vector<int> dif1, vector<int> dif2){
	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	int cost = 0;

	for(int i = 0; i < dif1.size(); i++){

		cost += toolsTimesSwap[dif1[i] - 1][dif2[i] - 1];

	}

	return cost;
}

vector<int> unsetZeros(vector<int> vector){
	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	vector.erase( remove( vector.begin(), vector.end(), 0 ), vector.end() ) ;

	return vector;

}

int insertOnMagazine(){

	if(DEBUG)
		cout <<__FUNCTION__ << endl;
	
	int cost = 0;
	int i = 1, n;

	for(auto& v : jobsAssignementsMatrix[indexJob]){

        if(magazine.size() < magazineCapacity){
            magazine.insert(v.second);

			if(DEBUG){

				cout << "Total cost: " << cost << endl;
				cout << "Installing tool: " << v.second << endl;
				cout << "Install cost: " << initialSetupTimes[v.second - 1] << endl;
			}
            
            cost += initialSetupTimes[v.second - 1];
			if(DEBUG)
				cout << "Updated total cost: " << cost << endl;

        }
        else{
            break;
        }
    }
    
    indexJob++;
	
	return cost;

}

bool oneBlock(){
	if(DEBUG)
		cout <<__FUNCTION__ << endl;
	int bestCont = runSolution(currentSolution);
	int aux, tempCont = 0;

	vector<int> index;

	index.resize(nJobs);

	for(int i = 0; i < nJobs; i++){
		index[i] = i;
	}

	
	for(int i = 0; i < nTools; i++){

		for(int j = 0; j < nJobs; j++){

			if(jobsAssignementsBinary[i][currentSolution[index[j]]] == 1){
				for(int k = j + 1; k  < nJobs; k++){

					if(jobsAssignementsBinary[i][currentSolution[index[j]]] == 1 && k != j){

						//inserir uma posição anterior ao bloco de 1 encontrado

						if(k - 1 >= 0){
							aux = currentSolution[index[k - 1]];
							currentSolution[index[k - 1]] = currentSolution[index[j]];
							currentSolution[index[j]] = aux;
							tempCont = runSolution(currentSolution);

							if(tempCont < bestCont){

								copy(currentSolution.begin(), currentSolution.end(), bestSolution.begin());
								totalTime = tempCont;

								cout << "Improvement, best cont: " << tempCont << endl << bestSolution;

								index.clear();
								return true;

							}
							else{
								aux = currentSolution[index[k - 1]];
								currentSolution[index[k - 1]] = currentSolution[index[j]];
								currentSolution[index[j]] = aux;
							}

						}

						int l;
						for(l = k + 1; l < nJobs && jobsAssignementsBinary[i][currentSolution[index[l]]] != 0; l++){
							
							//entre o bloco
							aux = currentSolution[index[l]];
							currentSolution[index[l]] = currentSolution[index[j]];
							currentSolution[index[j]] = aux;
							tempCont = runSolution(currentSolution);

							if(tempCont < bestCont){

								copy(currentSolution.begin(), currentSolution.end(), bestSolution.begin());

								totalTime = tempCont;

								cout << "Improvement, best cont: " << tempCont << endl << bestSolution;

								index.clear();
								return true;

							}
							else{
								aux = currentSolution[index[l]];
								currentSolution[index[l]] = currentSolution[index[j]];
								currentSolution[index[j]] = aux;
							}
						}
						//inserir depois do bloco de 1

						if(jobsAssignementsBinary[i][currentSolution[index[l]]] == 0 && jobsAssignementsBinary[i][currentSolution[index[l - 1]]] == 1){
							aux = currentSolution[index[l]];
							currentSolution[index[l]] = currentSolution[index[j]];
							currentSolution[index[j]] = aux;
							tempCont = runSolution(currentSolution);

							if(tempCont < bestCont){

								copy(currentSolution.begin(), currentSolution.end(), bestSolution.begin());
								totalTime = tempCont;

								//cout << currentSolution;

								cout << "Improvement, best cont: " << tempCont << endl << bestSolution;

								index.clear();
								return true;

							}
							else{
								aux = currentSolution[index[l]];
								currentSolution[index[l]] = currentSolution[index[j]];
								currentSolution[index[j]] = aux;
							}
						}

					}
				}
			}
		}
	}
	

	cout << "No improvement, best cont: " << bestCont << endl;


	return false;

}

bool twoOpt(){
	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	int bestCont = runSolution(currentSolution);
	int aux, tempCont = 0;

	vector<int> index;
	index.resize(nJobs);

	for(int i = 0; i < nJobs; i++){
		index[i] = i;
	}

	
	for(int i = 1; i < nJobs; i++){

		for(int j = 0; (j + i) < nJobs; j++){


			reverse(&currentSolution[index[j]], &currentSolution[index[j + i]]);

			tempCont = runSolution(currentSolution);

			if(tempCont < bestCont){

				copy(currentSolution.begin(), currentSolution.end(), bestSolution.begin());

				totalTime = tempCont;

				cout << "Improvement, best cont: " << tempCont << endl << bestSolution;

				index.clear();

				return true;

			}
			else{
				reverse(&currentSolution[index[j]], &currentSolution[index[j + i]]);
			}

			
		}
	}
	//cout << bestSolution;
	cout << "No improvement, best cont: " << bestCont << endl;

	return false;

}

bool insertion(){
	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	int bestCont = runSolution(currentSolution);
	int aux, tempCont = 0;

	vector<int> index;
	vector<int> insertions;

	index.resize(nJobs);
	insertions.resize(nJobs);

	for(int i = 0; i < nJobs; i++){
		index[i] = i;
		insertions[i] = i;
	}

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle (index.begin(), index.end(), std::default_random_engine(seed));
	
	for(int i = 0; i < nJobs; i++){

		shuffle (insertions.begin(), insertions.end(), std::default_random_engine(seed));

		for(int j = 0; (j + i) < nJobs; j++){

			aux = currentSolution[index[i]];
			currentSolution[index[i]] = currentSolution[insertions[j]];
			currentSolution[insertions[j]] = aux;

			tempCont = runSolution(currentSolution);

			if(tempCont < bestCont){

				copy(currentSolution.begin(), currentSolution.end(), bestSolution.begin());

				totalTime = tempCont;

				cout << "Improvement, best cont: " << tempCont << endl << bestSolution;

				index.clear();

				return true;

			}
			else{

				aux = currentSolution[index[i]];
				currentSolution[index[i]] = currentSolution[insertions[j]];
				currentSolution[insertions[j]] = aux;

			}

			
		}
	}

	cout << "No improvement, best cont: " << bestCont << endl;

	return false;

}

bool swap(){
	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	int bestCont = runSolution(currentSolution);
	int aux, tempCont = 0;

	vector<int> index;
	index.resize(nJobs);

	for(int i = 0; i < nJobs; i++){
		index[i] = i;
	}

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle (index.begin(), index.end(), std::default_random_engine(seed));

	for(int i = 0; i < nJobs; i++){

		for(int j = 0; j < nJobs; j++){

			aux = currentSolution[index[i]];
			currentSolution[index[i]] = currentSolution[index[j]];
			currentSolution[index[j]] = aux;
			tempCont = runSolution(currentSolution);


			if(tempCont < bestCont){

				copy(currentSolution.begin(), currentSolution.end(), bestSolution.begin());
				totalTime = tempCont;

				cout << "Improvement, best cont: " << tempCont << endl << bestSolution;

				index.clear();
				return true;

			}
			else{
				aux = currentSolution[index[i]];
				currentSolution[index[i]] = currentSolution[index[j]];
				currentSolution[index[j]] = aux;
			}
		}
	}

	cout << "No improvement, best cont: " << bestCont << endl << bestSolution;
	index.clear();	

	return false;

}

void vnd(){

	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	int k = 0;
	int bestCont = runSolution(currentSolution);
	int cont = INT_MAX;

	cout << "Result: " << bestCont << endl;

	while(k != 4){

		switch (localSearch[k].first)
		{
			case 0: {

				if(localSearch[k].second){
					if(!swap())
						k = 1;
					else
						k = 0;
				}
				
			}
			break;

			case 1: {
				if(localSearch[k].second){
					if(!twoOpt())
						k = 2;
					else
						k = 0;
				}
			}
			break;

			case 2: {
				if(localSearch[k].second){
					if(!oneBlock())
						k = 3;
					else
						k = 0;
				}
			}
			break;

			case 3: {
				if(localSearch[k].second){
					if(!insertion())
						k = 4;
					else
						k = 0;
				}
			}
			break;
		
		}
		

	}
	cout << "Result: " << bestCont << endl;

	cout << bestSolution;

}


template <typename T>
ostream& operator<<(ostream& output, const vector<T>& vector) {
    output << "[";

	for(int element : vector){
		output << element << " ,";
	}
    
    output << "]\n";
    return output;
}

ostream& operator<<(ostream& output, const set<int>& set) {
    output << "[";

	for(int element : set){
		output << element << " ";
	}
    
    output << "]\n";
    return output;
}

ostream& operator<<(ostream& output, const vector<vector<int>>& matrix) {

	for(vector<int> line : matrix){
		output << line;
	}
    
    return output;
}

ostream& operator<<(ostream& output, const vector<set<pair<int, int>>>& matrix) {

	for(set<pair<int, int>> line : matrix){
		output << "[";
		for(pair<int, int> elem : line)
			output << "( " << elem.first << ", " << elem.second << " ) ";

	    output << "]\n";

	}
    
    return output;
}

ostream& operator<<(ostream& output, const vector<set<int>>& matrix) {

	for(set<int> line : matrix){
		output << line;
	}
    
    return output;
}

bool fileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}


/*
Builds the graph 
*/
void buildGraph()
{
}


/*
Evaluates the current solution
*/
int evaluation()
{
	cout <<__FUNCTION__ << endl;

	int value = 0;
	return value;						//returns the maximum number of open stacks
}

/*
 Pre-processing procedure
 */
void preProcessing()
{
}

/*
Terminates all data structures. 
*/
void termination()
{

	for(int i = 0 ; i < nJobs ; i++) {
        jobsAssignements[i].clear();
		toolsTimesSwap[i].clear();
    }

	for(int i = 0; i < nTools; i++){
		jobsAssignementsBinary[i].clear();

	}

	for(auto& v : jobsAssignements) {
        v.clear();
    }
    for(auto& v : toolsTimesSwap) {
        v.clear();
    }


    magazine.clear();
    initialSetupTimes.clear();
    toolsTimesSwap.clear();
    jobsAssignements.clear();
    jobs.clear();
	bestSolution.clear();
	currentSolution.clear();

	jobsAssignementsBinary.clear();
	jobsAssignements.clear();
	jobs.clear();

	nJobs = 0;
	nTools = 0;
	magazineCapacity = 0;
	minTools = 0;
	maxTools= 0;
	nSwaps = 0;
	totalTime = 0;
	//.clear in all data structures
	//=0 in all numeric variables
}

/*
Prints the solution information to the output file
*/
void printSolution(string inputFileName, int solutionValue, double time, int run)
{
    string outputFileName = "solutions/" + inputFileName;
	ofstream fpSolution(outputFileName);
	fpSolution << "Run = " << run << " -> time: " << time << endl; 
	fpSolution << "Total Time = " << solutionValue << endl; 
	fpSolution << "Best Solution = " << bestSolution; 

	
					//file that contains the information about the solution of a problem instance
	
	//print input file name/characteristics	
	//print algorithm parameters and settings
	//print solution value
	//print solution time
	//print preprocessing result
	//print solution structure
}

/*
Sets the lowerbound 
*/
void setLowerBound()
{
}

/*
Main procedure - Multi-Run version
Parameters:
solutionValue			stores the solution value obtained by the method
runningTime				stores the exeution time of the method
inputFileName			stores the name of the file containing the instance
run 					stores the run #
*/

void multiRun(int *solutionValue, double *runningTime, string inputFileName, int run)
{
	int FinalSolutionValue = INT_MAX;
	duration<double> time_span2;

	readProblem(inputFileName);
	vnd();													//reads the problem data

	high_resolution_clock::time_point t1 = high_resolution_clock::now();		//time taking
	
	setLowerBound();															//determines the largest pattern size
   	preProcessing();															//preprocessing procedure, removes dominated patterns
   	buildGraph();																//builds the mosp graph


	high_resolution_clock::time_point t2 = high_resolution_clock::now(); 		//time taking
  	duration<double> time_span = duration_cast<duration<double> >(t2 - t1);

	*solutionValue = totalTime;                             			//stores the solution value
	*runningTime =  time_span.count();											//stores the execution time

	printSolution(inputFileName, *solutionValue, *runningTime, run);			//prints the solution to the file

	termination();																//terminates all data structures
}

/*
Main procedure - single run
Parameters:
solutionValue			stores the solution value obtained by the method
runningTime				stores the exeution time of the method
inputFileName			stores the name of the file containing the instance
*/
void singleRun(int *solutionValue, double *runningTime, string inputFileName)
{
	int i, j;
	int FinalSolutionValue = INT_MAX;											//considering minimization problems		

	readProblem(inputFileName);													//reads the problem data
	
	high_resolution_clock::time_point t1 = high_resolution_clock::now();		//time taking
	
	setLowerBound();															//determines the lower bound
   	preProcessing();															//preprocessing procedure
   	buildGraph();																//builds the graph
    
    //solve the problem

	high_resolution_clock::time_point t2 = high_resolution_clock::now(); 		//time taking
  	duration<double> time_span = duration_cast<duration<double> >(t2 - t1);

	*solutionValue = totalTime;                             			//stores the solution value
	*runningTime =  time_span.count();											//stores the execution time

	printSolution(inputFileName, *solutionValue, *runningTime, 1);					//prints the solution to the file

	termination();																//terminates all data structures
}
#endif
