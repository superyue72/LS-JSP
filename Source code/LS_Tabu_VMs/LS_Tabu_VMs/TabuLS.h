#pragma once
#include <chrono> 
#include <numeric>
#include <algorithm>
#include "tabu.h"
#include "base.h"
#include "Evaluation.h"

TLSPopInfo GATabuLS(const vector<JobData> jData, const vector<VMData> VMsdata, vector<TLSPopInfo> pops);
TLSPopInfo TabuLS(const vector<JobData> jData, const vector<VMData> VMsdata, vector<TLSPopInfo> pops);
TLSPopInfo GreedySearch(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int tabuStamp);
TLSPopInfo IntensificationSearch(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p,int tabuStamp);
TLSPopInfo VNSSearch(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int tabuStamp);
TLSPopInfo DiversificationSearch(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int timeStamp, int bestStamp);
//void evaluationChoose(int i, vector<JobData> Jdata, const vector<VMData> VMsdata, TLSPopInfo& p);

TLSPopInfo VNSChoose(int i, vector<JobData> Jdata, const vector<VMData> VMsdata, TLSPopInfo p, int PriNum, vector<int> IDSeq, int tabuStamp);
TLSPopInfo ExeTimeSearch(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int tabuStamp);
TLSPopInfo RS0(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int PriNum, vector<int> IDSeq);
TLSPopInfo RS1(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int PriNum, vector<int> IDSeq);
TLSPopInfo RS2(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int PriNum, vector<int> IDSeq);
TLSPopInfo RS3(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int PriNum, vector<int> IDSeq);


TLSPopInfo LS(const vector<JobData> jData, const vector<VMData> VMsdata, vector<TLSPopInfo> pops);
TLSPopInfo TabuLSNoTabu(const vector<JobData> jData, const vector<VMData> VMsdata, vector<TLSPopInfo> pops);
TLSPopInfo TabuLSNoEva(const vector<JobData> jData, const vector<VMData> VMsdata, vector<TLSPopInfo> pops);
TLSPopInfo DSearchNoTabu(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int iterationStamp, int bestStamp);
TLSPopInfo ISearchNoEva(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int tabuStamp);