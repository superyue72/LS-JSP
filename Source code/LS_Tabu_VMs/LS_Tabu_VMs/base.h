#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <queue>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <chrono> 
#include <numeric>
#include <iomanip>
#include "base.h"

using namespace std;
using namespace std::chrono;
using std::vector;
using std::iterator;

extern char** argv;

//termination  
const long long RunTimes = 60000;                             
const int IniTimes = 10000;                     
const int Inibest = 1000;

const float eps = 1e-3; // rounded to (4 - 2) decimal digits
const float Diff = 1e-9; // rounded to (3 - 1) decimal digits
/*
//Parameters of GA 
extern double p_cross;
extern double p_mutation;

//Parameters of PSO
extern double c1;
extern double c2;
extern double v_max;
extern double popW;

//Parameters of MVO
extern double minWep;                   // min wormhole existence probability 
extern double maxWep;                     // max wormhole existence probability
extern int mDegree;
*/

//Parameters of GA 
const double p_cross = 0.9;
const double p_mutation = 0.2;

//Parameters of PSO
const double c1 = 0.2;
const double c2 = 0.8;
const double v_max = 1;
const double popW = 0.6;

//Parameters of MVO
const double minWep = 0.2;                   // min wormhole existence probability 
const double maxWep = 1;                     // max wormhole existence probability
const int mDegree = 4;
const int upB = 5;
const int lowB = -5;

//Parameters of local search
const int Popsize = 50;                       //number of solutions in each iteration
const double p_Bsharking = 0.1;               //probability of big sharking
const double p_Ssharking = 0.1;               //probability of small sharking
const int NeighborNum = 3;                    //number of neighborhood
const int gIteNum = 50;                      //maximum total loop times of local search
const int lIteNum = 150;                      //maximum search times of each neighborhood

//Parameters of Tabu
const int tabuLen = 500;
const double p_ls = 0.1;
const double p_div = 0.4;
const class JobData
{
public:
	int jTime;
	int jPriority;
	double jCPU;
	double jMEM;
	int jID;

	//bool operator==(const JobData& j) {
	//	return abs((*this).jTime - j.jTime) < Diff;
	//}

	//bool operator<(const JobData& j) {
	//	return (*this).jTime < j.jTime;
	//}

	//bool operator>(const JobData& j) {
	//	return (*this).jTime > j.jTime;
	//}
};

const class JobInQueue {
public:
	int jID;
	int mID;
	int s_time;
	int r_time;
	int e_time;
	/*double cpu;
	double mem;*/
	int priority;
};

const class VMData
{
public:
	int vmID;
	double vmCPU;
	double vmMEM;
	double vmPrice;
};

bool isGreaterThan(double a, double b);
bool isGreaterThanOrEqual(double a, double b);
bool isEqual(double a, double b);
bool isLessThan(double a, double b);
bool isLessThanOrEqual(double a, double b);
bool isNotEqual(double a, double b);

const class PopInfo
{
public:
	int pID;
	vector<int> pSequence;
	vector<int> trueSequence;
	vector<int> mPlacement;
	double pCost;
	bool operator==(const PopInfo& p) {
		return isEqual((*this).pCost, p.pCost);
	}

	bool operator<(const PopInfo& p) {
		return isLessThan((*this).pCost, p.pCost);
	}

	bool operator>(const PopInfo& p) {
		return isGreaterThan((*this).pCost, p.pCost);
	}

	bool operator!=(const PopInfo& p) {
		return isNotEqual((*this).pCost, p.pCost);
	}
};

const class GAPopInfo : public PopInfo
{
public:
	vector<double> pChromosome;
};

const class PSOPopInfo : public PopInfo
{
public:
	vector<double> pLocation;
	vector<double> pVelocity;
};

const class MVOPopInfo : public PopInfo
{
public:
	vector<double> pObjects;
	vector<double> upBound;
	vector<double> lowBound;
	double InflationRate;
};

const class TLSPopInfo : public GAPopInfo
{
public:
	vector<int> p_TabuList;
	vector<int> p_RewardList;
};

class AllSequence {
public:
	vector<vector<int>> result;
	vector<int> path;
	void backtracking(vector<int>& nums, vector<bool>& used)
	{
		if (path.size() == nums.size())
		{
			result.push_back(path);
			return;
		}
		for (int i = 0; i < nums.size(); i++)
		{
			if (used[i] == true) continue;
			used[i] = true;
			path.push_back(nums[i]);
			backtracking(nums, used);
			path.pop_back();
			used[i] = false;
		}
	}
	vector<vector<int>> permute(vector<int>& nums)
	{
		result.clear();
		path.clear();
		vector<bool> used(nums.size(), false);
		backtracking(nums, used);
		return result;
	}
};
int RandomInt(int low, int high);
double RandomDouble(double low, double high);

//vector <vector<int>> PriorityNum(vector<JobData> Jdata);
//bool GroupCmp(pair<int, int> s1, pair<int, int> s2);
//vector<int> MyGroup(vector<int> pSeq, vector<JobData> Jdata);

bool SortPri(pair<int, int> s1, pair<int, int> s2);
vector<int> SortByPri(PopInfo p, vector<JobData> jData);

bool MyCmp(pair<int, double> s1, pair<int, double> s2);
vector<int> GiveSort(const vector<double> sp);

void SavePops(vector<PopInfo> pops, string fname);
void PrintPop(PopInfo p);

void SaveRecord(vector<pair<long long, double>> SolutionRecord, string fname);
vector<int> returnPriLoc(vector<JobData> jData);

bool jGapCom(JobData j1, JobData j2);
bool ExeTimeCom(JobData j1, JobData j2);
bool CMCom(JobData j1, JobData j2);


vector<pair<int, vector<int>>> GroupByPriorityIndex(vector<JobData> jData);
int RouletteIndex(vector<int> reward_value);
