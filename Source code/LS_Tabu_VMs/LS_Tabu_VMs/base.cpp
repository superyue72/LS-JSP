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
using std::vector;

//Random Numbers Generation
int RandomInt(int low, int high) {
	if (low > high) {
		return high;
	}
	return low + (rand() % (high - low + 1));
}

double RandomDouble(double low, double high) {
	//double f = rand() % 10000 / 10000;
	double f = (double)rand() / RAND_MAX;
	return low + f * (high - low);
}

//------------------------------sort by Priority-------------------------------
bool SortPri(pair<int, int> s1, pair<int, int> s2)
{
	return s1.second < s2.second;
}
vector<int> SortByPri(PopInfo p, vector<JobData> jData)
{
	vector<int> newSequence;
	vector<pair<int, int>> jPri;
	for (int i = 0; i < p.pSequence.size(); i++)
	{
		jPri.push_back(pair<int, int>({p.pSequence[i], jData[p.pSequence[i]].jPriority}));
	}
	stable_sort(jPri.begin(), jPri.end(), SortPri);
	for (std::vector<pair<int, int>>::iterator it = jPri.begin(); it != jPri.end(); ++it)
	{
		newSequence.push_back(it->first);
	}
	return newSequence;
}

//----------------------------------GA decoding----------------------------------
bool MyCmp(pair<int, double> s1, pair<int, double> s2)
{
	return s1.second < s2.second;
}
vector<int> GiveSort(const vector<double> sp)
{
	vector<int> s;
	vector<pair<int, double>> m;
	for (int i = 0; i < sp.size(); i++)
	{
		m.push_back(pair<int, double>(i, sp[i]));
	}
	sort(m.begin(), m.end(), MyCmp);

	for (vector<pair<int, double>>::iterator it = m.begin(); it != m.end(); it++)
	{
		s.push_back(it->first);
	}

	return s;
}

//--------------------Print and save information of pops-----------------------
void SavePops(vector<PopInfo> pops, string fname)
{
	//save the information
	ofstream Pops;
	Pops.open("Pops_" + fname + ".csv", ios::out | ios::trunc);

	for (std::vector<PopInfo>::iterator it = pops.begin(); it != pops.end(); ++it)
	{
		Pops << it->pID << ",";
		for (std::vector<int>::iterator it1 = it->pSequence.begin(); it1 != it->pSequence.end(); ++it1)
			Pops << *it1 << ":";
		Pops << ",";
		Pops << it->pCost << ",";
		Pops << endl;
	}

	Pops.close();
}

void PrintPop(PopInfo p)
{
	cout << "PopInfoAsFollows:" << endl;
	cout << "PID:" << p.pID << endl;
	cout << "Makespan" << p.pCost << endl;

	std::cout << "Sequence:";
	for (std::vector<int>::iterator it = p.pSequence.begin(); it != p.pSequence.end(); ++it)
		std::cout << ' ' << *it;
	cout << endl;

	std::cout << "True Order:";
	for (std::vector<int>::iterator it = p.trueSequence.begin(); it != p.trueSequence.end(); ++it)
		std::cout << ' ' << *it;
	cout << endl;

	std::cout << "machine placement:";
	for (std::vector<int>::iterator it = p.mPlacement.begin(); it != p.mPlacement.end(); ++it)
		std::cout << ' ' << *it;
	cout << endl;
}

void SaveRecord(vector<pair<long long, double>> SolutionRecord, string fname)
{
	ofstream Records;
	Records.open("Records_" + fname + ".csv", ios::out | ios::trunc);

	for (std::vector<pair<long long, double>>::iterator it = SolutionRecord.begin(); it != SolutionRecord.end(); ++it)
	{
		Records << it->first << ",";
		Records << it->second << ",";
		Records << endl;
	}
	Records.close();
}

bool jGapCom(JobData j1, JobData j2)
{
	if (j1.jPriority != j2.jPriority)
	{
		return j1.jPriority < j2.jPriority;
	}
	else
	{
		return abs(j1.jCPU - j1.jMEM) < abs(j2.jCPU - j2.jMEM);
	}
}
bool ExeTimeCom(JobData j1, JobData j2)
{
	//if (j1.jPriority != j2.jPriority)
	//{
	//	return j1.jPriority < j2.jPriority;
	//}
	//else
	//{
	//    return j1.jTime < j2.jTime;
	//}
	return j1.jTime < j2.jTime;
}
bool CMCom(JobData j1, JobData j2)
{
	if (j1.jCPU < j2.jCPU) {
		return true;
	}
	else {
		if (j2.jCPU < j1.jCPU) {
			return false;
		}
		else {
			return j1.jMEM < j2.jMEM;
		}
	}
}

vector<int> returnPriLoc(vector<JobData> jData)
{
	vector<int> IDNums;                               //the number of jobs in each priority group
	vector <pair<int, int>> PriID;                    //the ID of jobs in each priority group
	vector<int> IDSeq;                                // the position

	for (int i = 0; i < jData.size(); i++)
	{
		PriID.push_back(pair<int, int>({ jData[i].jID, jData[i].jPriority }));
	}

	sort(PriID.begin(), PriID.end(), SortPri);

	int maxPri = PriID.back().second;
	int minPri = PriID.front().second;

	while (minPri <= maxPri)
	{
		int num = 0;
		for (int i = 0; i < PriID.size(); i++)
		{
			if (PriID[i].second == minPri)
			{
				num++;
			}
		}
		IDNums.push_back(num);
		minPri++;
	}

	IDSeq.push_back(-1);
	for (int i = 0; i < IDNums.size(); i++)
	{
		IDSeq.push_back(accumulate(IDNums.begin(), IDNums.begin() + i + 1, 0) - 1);
	}
	return IDSeq;
}
vector<pair<int, vector<int>>> GroupByPriorityIndex(vector<JobData> jData)
{
	vector<pair<int, vector<int>>> res;
	vector <pair<int, int>> PriID;
	for (int i = 0; i < jData.size(); i++)
	{
		PriID.push_back(pair<int, int>({ jData[i].jID, jData[i].jPriority }));
	}

	sort(PriID.begin(), PriID.end(), SortPri);

	int maxPri = PriID.back().second;
	int minPri = PriID.front().second;

	while (minPri <= maxPri)
	{
		vector<int> JobsID;
		for (int i = 0; i < PriID.size(); i++)
		{
			if (PriID[i].second == minPri)
			{
				JobsID.push_back(PriID[i].first);
			}
		}
		res.push_back(pair<int, vector<int>>({ minPri, JobsID }) );
		minPri++;
	}
	return res;
}

int RouletteIndex(vector<int> reward_value) {
	vector<double> reward_probability;
	int totalReward = accumulate(reward_value.begin(), reward_value.end(), 0);
	if (totalReward == 0)
	{
		return RandomInt(0, reward_value.size() - 1);
	}

	for (int i = 0; i < reward_value.size(); i++)
	{
		reward_probability.push_back(double(reward_value[i]) / double(totalReward));
	}

	double rndNumber = rand() / (double)RAND_MAX;
	double offset = 0.0;
	int pick_num = 0;
	for (int i = 0; i < reward_probability.size(); i++) {
		offset += reward_probability[i];
		if (rndNumber < offset) {
			pick_num = i;
			if (pick_num == reward_probability.size())
			{
				cout << "???" << endl;
			}
			break;
		}
	}
	return pick_num;
}

bool isNotEqual(double a, double b) {
	return fabs(a - b) > eps;
}

bool isEqual(double a, double b) {
	return fabs(a - b) <= eps;
}

bool isGreaterThan(double a, double b) {
	return a + eps > b;
}

bool isGreaterThanOrEqual(double a, double b) {
	return isGreaterThan(a, b) || isEqual(a, b);
}

bool isLessThan(double a, double b) {
	return a + eps < b;
}

bool isLessThanOrEqual(double a, double b) {
	return isLessThan(a, b) || isEqual(a, b);
}