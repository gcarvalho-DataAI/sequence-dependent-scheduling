#ifndef SOLVER_HPP
#define SOLVER_HPP
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

using namespace std;
using namespace std::chrono;

#define PRINT_MATRIX true

//Configuration of the datasets n, m, c, Tmin, Tmaz
int nJobs, nTools, magazineCapacity, minTools, maxTools;

int totalTime, bestTime, nSwaps, indexJob;

vector<int> bestSolution; /* array with the best jobs assignements*/
vector<int> currentSolution; /* array with the best jobs assignements*/
vector<vector<int>> jobsAssignements; /* array with the jobs assignements and it's tools needed*/
vector<vector<int>> toolSwaps; /* array with the jobs assignements and it's tools needed*/
vector<vector<int>> toolsTimesSwap; /* array with the tool swap times */
vector<int> magazine;
vector<int> jobs;
vector<int> dif;
vector<int> initialSetupTimes;


void updateBestSolution(vector<int> jobs);
void printStart();
void runSolution();
void trait(vector<int> tools);
void populateMagazine(vector<int> tools);
vector<int> unsetZeros(vector<int> vector);
void ktns();
void checkToolOnMagazine(vector<int> tools);
void insertOnMagazine();
int getMaxElement(vector<int> vector);


template <typename T>
ostream& operator<<(ostream& os, const vector<T>& arr);
template<typename T>
ostream& operator<< (ostream& out, const vector<vector<T>>& matrix);

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
void initialization()
{

	totalTime = 0;
	bestTime = INT_MAX;
	nSwaps = 0;
	indexJob = 0;
	magazine.resize(magazineCapacity);
    initialSetupTimes.resize(nTools);
	toolsTimesSwap.resize(nTools);
	toolSwaps.resize(nTools);
	bestSolution.resize(nJobs);
	currentSolution.resize(nJobs);


	jobsAssignements.resize(nJobs);
	jobs.resize(nJobs);

	for(int i = 0 ; i < nTools ; i++) {
		toolsTimesSwap[i].resize(nTools);
		toolSwaps[i].resize(nTools);

    }

	for(int i = 0 ; i < maxTools ; i++) {
        jobsAssignements[i].resize(nJobs);
    }
	
}

/*
Reads the problem from a file specified by fileName
*/
void readProblem(string fileName)
{
	ifstream fpIn(fileName);
	int get;

	if(!fpIn)
        throw invalid_argument("ERROR : Instance " + fileName + "doesn't exist");


	fpIn >> nJobs >> nTools >> magazineCapacity >> minTools >> maxTools;

	cout << nJobs << " " << nTools << " " << magazineCapacity << " " << minTools << " " << maxTools << endl;

	initialization();//initializes all structures, vectors and matrices
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
        jobs[i] = i;
    }

	for(int i = 0 ; i < maxTools ; i++) {
        for(int j = 0 ; j < nJobs ; j++) {
            fpIn >> get;
            jobsAssignements[i][j] = get;
        }
    }

	
	updateBestSolution(jobs);

	//printStart();

	runSolution();


}

void printStart(){

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

void updateBestSolution(vector<int> jobs){

    copy(jobs.begin(), jobs.end(), bestSolution.begin());

}

void runSolution(){

	vector<int> tools;
	tools.resize(maxTools);

	for(int i = 0; i < nJobs; i++){
		for(int j = 0; j < maxTools; j++){

			tools[j] = jobsAssignements[j][jobs[i]];			
		}

		if(i == 0){
			populateMagazine(tools);		
		}
		else{
			trait(tools);
		}

		indexJob++;
	}

	tools.clear();
	
}

void trait(vector<int> tools){

	checkToolOnMagazine(tools);

	vector<int> axMagazine;

	axMagazine = unsetZeros(magazine);

	int n = magazineCapacity - (axMagazine.size() + dif.size());

	if(n < 0){
		ktns();
	}
	else{
		insertOnMagazine();
	}

	axMagazine.clear();


}

void populateMagazine(vector<int> tools){

	for(int i = 0; i < tools.size(); i++){

		totalTime += initialSetupTimes[tools[i]];
		nSwaps ++;

		magazine[i] = tools[i];
		
	}

}

vector<int> unsetZeros(vector<int> vector){

	vector.erase( remove( vector.begin(), vector.end(), 0 ), vector.end() ) ;

	return vector;

}

void checkToolOnMagazine(vector<int> tools){

	vector<int> updateMagazine;

	dif.clear();

	dif.resize(magazineCapacity);

	sort(tools.begin(), tools.end());
	sort(magazine.begin(), magazine.end());

	set_difference(tools.begin(), tools.end(), magazine.begin(), magazine.end(), dif.begin());

	dif = unsetZeros(dif);

}

void insertOnMagazine(){
	
	int count = 0;
	for(int i = 0; i < maxTools; i++){
		if(magazine[i] == 0){
			totalTime += initialSetupTimes[dif[count]];
			nSwaps++;
			magazine[i] = dif[count++]; 
		}
	}
	

}

void ktns(){

	vector<int> tools;
	vector<int> time;
	vector<int>::iterator it;
	int timeCount = 0, n = 0;

	time.resize(magazineCapacity);
	tools.resize(maxTools);

	for(int j = 0; j < magazineCapacity; j++){
		time[j] = INT_MAX;
	}


	for(int i = indexJob; i < nJobs; i++){

		for(int j = 0; j < maxTools; j++){

			tools[j] = jobsAssignements[j][jobs[i]];

		}

		for(int j = 0; j < magazineCapacity; j++){

			if(magazine[j] != 0){

				it = find(tools.begin(), tools.end(), magazine[j]);

				if (it != tools.end()){
					if(time[j] == INT_MAX){
						time[j] = i - indexJob;
					}
				}
			}
			
		}
		
	}

	for(int i = 0; i < dif.size(); i++){

		n = getMaxElement(time);
		time[n] = INT_MIN;

		if(magazine[n] == 0){
			totalTime += initialSetupTimes[dif[i] - 1];
		}
		else{
			totalTime += toolsTimesSwap[magazine[n] - 1][dif[i] - 1];

		}

		magazine[n] = dif[i];

		nSwaps++;

	}

	tools.clear();
	time.clear();



	
}

int getMaxElement(vector<int> vector){

	int elem, i;

	elem = *max_element(vector.begin(), vector.end());

	for(i = 0; i < vector.size(); i++){
		if(vector[i] == elem){
			break;
		}
	}

	return i;

}


template <typename T>
ostream& operator<<(ostream& output, const vector<T>& vector) {
    output << "[";

	for(int element : vector){
		output << element << " ";
	}
    
    output << "]\n";
    return output;
}

template<typename T>
ostream& operator<<(ostream& output, const vector<vector<T>>& matrix) {

	for(vector<T> line : matrix){
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
	int value;

	if(totalTime < bestTime){
		bestTime = totalTime;

		copy(currentSolution.begin(), currentSolution.end(), bestSolution.end());


	}

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
	toolSwaps.clear();
	currentSolution.clear();
	dif.clear();

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

	readProblem(inputFileName);													//reads the problem data

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
