#pragma once
#include <map>
#include "base.h"

using namespace std;
using std::vector;
using std::iterator;

//---calculate the cost according to given sequence and allocated machines
vector<JobInQueue> EvaluationSingleMachine(const vector<JobData> jData, vector<int> pSequence, VMData vm);
double EvaluationSolution(const vector<JobData> jData, const vector<VMData> VMsdata, PopInfo p);

//---allocate jobs by given sequence 
bool SortComTime(pair<int, int> s1, pair<int, int> s2);
void GeneralSort(const vector<JobData> jData, const vector<VMData> VMsdata, PopInfo &p);

//---preemptive allocation
bool QueueCompareJobsByRunningTime(JobInQueue u, JobInQueue v);
void GreedySortV1(const vector<JobData> jData, vector<VMData> VMsdata, PopInfo &p); 
void GreedySortV2(const vector<JobData> jData, vector<VMData> VMsdata, PopInfo &p);  
void GreedySortV3(const vector<JobData> jData, vector<VMData> VMsdata, PopInfo &p);
void GreedySortV4(const vector<JobData> jData, vector<VMData> VMsdata, PopInfo &p);
void GreedySortV5(const vector<JobData> jData, vector<VMData> VMsdata, PopInfo &p);
bool CompareJobsByStartTime(JobInQueue u, JobInQueue v);
bool CompareJobsByRunningTime(JobInQueue u, JobInQueue v);
vector<JobInQueue> singleMachineSort(const vector<JobData> jData, vector<int> pSequence, double MaxCPU, double MaxMEM);

//---allocate jobs by resource gap
bool sortResGap(pair<int, double> ReGap1, pair<int, double> ReGap2);
int findSmallGapJob(vector<JobData> leftJ, double MaxCPU, double MaxMEM);
void resGapSort(const vector<JobData> jData, const vector<VMData> VMsdata, PopInfo &p);


