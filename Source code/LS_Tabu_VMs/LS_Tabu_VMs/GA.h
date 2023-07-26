#pragma once
#include "base.h"
#include "Evaluation.h"
#include "TabuLS.h"

GAPopInfo BasicGA(const vector<JobData> Jdata, const vector<VMData> VMsdata, vector<GAPopInfo> Pops);
int GaChoose(vector <GAPopInfo> pops);
GAPopInfo GaCross(GAPopInfo p1, GAPopInfo p2);
GAPopInfo GaMutation(GAPopInfo p);