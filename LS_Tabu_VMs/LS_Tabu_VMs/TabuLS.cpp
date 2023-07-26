#pragma once
#include <chrono> 
#include <numeric>
#include <algorithm>
#include "tabu.h"
#include "base.h"
#include "Evaluation.h"
#include "TabuLS.h"

using namespace std;
using namespace std::chrono;

int PopChoose(vector<TLSPopInfo> pops)
{
	vector<double> allFitness;
	vector<double> allPro;
	vector<double> roulette;

	for (int i = 0; i < pops.size(); i++)
	{
		allFitness.push_back(pops[i].pCost);
	}
	double sumFitness = accumulate(allFitness.begin(), allFitness.end(), 0.0000);

	for (int i = 0; i < allFitness.size(); i++)
	{
		allPro.push_back(allFitness[i] / sumFitness);
	}

	for (int i = 1; i < allPro.size() + 1; i++)
	{
		roulette.push_back(accumulate(allPro.begin(), allPro.begin() + i, 0.0000));
	}

	double a = RandomDouble(0, 1);

	std::vector<double>::iterator low;
	low = std::lower_bound(roulette.begin(), roulette.end(), a);

	int slocation = low - roulette.begin();

	if (slocation == pops.size())
	{
		slocation = slocation - 1;
	}

	return slocation;
}

TLSPopInfo PopCross(TLSPopInfo p1, TLSPopInfo p2, int tabuStamp)
{
	TLSPopInfo newP = p2;
	vector<double> newSeq;
	int jNum = p2.pChromosome.size();
	//generate two position randomly
	int c = RandomInt(1, ceil(jNum * 0.2));
	int a = RandomInt(0, jNum - 1 - c);
	int b = RandomInt(0, jNum - 1 - c);

	newSeq.insert(newSeq.end(), p1.pChromosome.begin(), p1.pChromosome.begin() + a);
	newSeq.insert(newSeq.end(), p2.pChromosome.begin() + b, p2.pChromosome.begin() + b + c);
	newSeq.insert(newSeq.end(), p1.pChromosome.begin() + a + c, p1.pChromosome.end());

	newP.pChromosome = newSeq;
	newP.pSequence = GiveSort(newP.pChromosome);

	for (int i = 0; i < newP.pSequence.size(); i++)
	{
		if (newP.pSequence[i] != p2.pSequence[i]) {
			newP.p_TabuList[i]++;
		}
	}
	return newP;
}

TLSPopInfo PopMutation(const vector<JobData> Jdata, const vector<VMData> VMsdata, TLSPopInfo p, int iterationStamp)
{
	vector<int> IDList_tabu;
	auto minPosition = min_element(p.p_TabuList.begin(), p.p_TabuList.end());
	int minSearch = *minPosition;
	//cout << minSearch << endl;
	for (int i = 0; i < p.p_TabuList.size(); i++)
	{
		if (p.p_TabuList[i] - iterationStamp + tabuLen >= 0) {
			IDList_tabu.push_back(i);
		}
		//if (p.p_TabuList[i] <= tabuLen ) {
		//	IDList_tabu.push_back(i);
		//}
	}
	if (IDList_tabu.size() > lIteNum)
	{
		random_shuffle(IDList_tabu.begin(), IDList_tabu.end());
	}

	if (IDList_tabu.size() == 0)
	{
		for (int i = 0; i < p.p_TabuList.size(); i++) {
			if (p.p_TabuList[i] == minSearch) {
				IDList_tabu.push_back(i);
			}
		}
	}
	int n = min(lIteNum, int(IDList_tabu.size()));
	vector<TLSPopInfo> pops;
	for (int i = 0; i < n; i++)
	{
		TLSPopInfo newP = p;
		newP.pChromosome[IDList_tabu[i]] = rand() / double(RAND_MAX);
		newP.pSequence = GiveSort(newP.pChromosome);
		newP.pSequence = SortByPri(newP, Jdata);
		GreedySortV2(Jdata, VMsdata, newP);
		pops.push_back(newP);
	}

	sort(pops.begin(), pops.end());
	for (int i = 0; i < pops.front().pSequence.size(); i++)
	{
		if (pops.front().pSequence[i] != p.pSequence[i]) {
			pops.front().p_TabuList[i]++;
			//pops.front().p_TabuList[i] = iterationStamp;
		}
	}
	return pops.front();
}

TLSPopInfo GATabuLS(const vector<JobData> Jdata, const vector<VMData> VMsdata, vector<TLSPopInfo> Pops)
{
	TLSPopInfo p;
	TLSPopInfo bestp;
	TLSPopInfo newpop;
	int slocation1;
	vector <TLSPopInfo> oldPops = Pops;             //parents generation
	vector <TLSPopInfo> totalPops;

	sort(Pops.begin(), Pops.end());
	bestp = Pops.front();
	//PrintPop(bestp);
	//-----------------------start searching-------------------------
	auto start = high_resolution_clock::now();
	long long runTime = 0;
	int iniTime = 0;
	int inibest = 0;
	vector<pair<long long, double>> SolutionRecord;
	while ((runTime < RunTimes) && (iniTime < IniTimes) && (inibest < Inibest))
	{
		//------------new generation by cross operation------
		for (int j = 0; j < oldPops.size(); j++)
		{
			double Pcross = RandomDouble(0, 1);
			if (Pcross < p_cross)
			{
				slocation1 = PopChoose(oldPops);
				while (slocation1 == j)
				{
					slocation1 = PopChoose(oldPops);
				}
				newpop = PopCross(oldPops[slocation1], oldPops[j], iniTime);
				newpop.pID = j;
				totalPops.push_back(newpop);
			}
		}
		//------------------mutation-------------------------
		for (int j = 0; j < totalPops.size(); j++)
		{
			double Pmutation = RandomDouble(0, 1);
			if (Pmutation < p_mutation)
			{
				//totalPops[j] = GreedySearch(Jdata, VMsdata, totalPops[j], iniTime);
				//totalPops[j] = DiversificationSearch(Jdata, VMsdata, totalPops[j],iniTime, inibest);
				totalPops[j] = PopMutation(Jdata, VMsdata, totalPops[j], iniTime);
				//cout << inibest << endl;
			}
			totalPops[j].pSequence = SortByPri(totalPops[j], Jdata);
			GreedySortV2(Jdata, VMsdata, totalPops[j]);
			//evaluationChoose(RouletteIndex(sort_probability), Jdata, VMsdata, totalPops[j]);
		}


		//----------------new generation selection-------------
		for (int i = 0; i < oldPops.size(); i++)
		{
			totalPops.push_back(oldPops[i]);
		}

		sort(totalPops.begin(), totalPops.end());
		
		//int pIndex = PopChoose(totalPops);
		//totalPops[pIndex] = ExeTimeSearch(Jdata, VMsdata, totalPops[pIndex], sort_probability);
		//totalPops[pIndex] = LS(Jdata, VMsdata, totalPops[pIndex], SortScore);

		if (totalPops.front() < bestp)
		{
			bestp = GreedySearch(Jdata, VMsdata, totalPops.front(), iniTime);
			//bestp = ExeTimeSearch(Jdata, VMsdata, totalPops.front(), sort_probability, iniTime);
			//bestp = totalPops.front();
			p = bestp;
			inibest = 0;
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(stop - start);
			runTime = duration.count();
			SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));
		}
		else
		{
			inibest++;
		}

		while (totalPops.size() > Popsize)
		{
			totalPops.pop_back();
		}
		oldPops = totalPops;
		totalPops.clear();

		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
		runTime = duration.count();
		iniTime++;
	}
	string reName = "GATabuLS";
	//SaveRecord(SolutionRecord, reName);
	cout << "The runTime of basic GATabuLS  = " << runTime << endl;
	cout << "The loops of basic GATabuLS = " << iniTime << endl;
	cout << "The maxmum repeat times of GATabuLS = " << inibest << endl;
	return bestp;
}

TLSPopInfo TabuLS(const vector<JobData> jData, const vector<VMData> VMsdata, vector<TLSPopInfo> pops)
{
	sort(pops.begin(), pops.end());
	TLSPopInfo bestp = pops.front();
	TLSPopInfo p = pops.front();
	//PrintPop(bestp);
	//-----------------------start searching-------------------------
	auto start = high_resolution_clock::now();
	long long runTime = 0;
	int iniTime = 0;
	int inibest = 0;
	vector<pair<long long, double>> SolutionRecord;
	while ((runTime < RunTimes) && (iniTime < IniTimes) && (inibest < Inibest))
	{
		double a = RandomDouble(0, 1);
		if (a > p_ls){
			p = IntensificationSearch(jData, VMsdata, p, iniTime);
			//p = VNSSearch(jData, VMsdata, p, iniTime);
			//p = GreedySearch(jData, VMsdata, p, iniTime); 
			if (p < bestp){
				//p = VNSSearch(jData, VMsdata, p, iniTime);
				p = GreedySearch(jData, VMsdata, p, iniTime);
				bestp = p;
				inibest = 0;
				auto stop = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>(stop - start);
				runTime = duration.count();
				SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));
			}
			else
			{
				p = DiversificationSearch(jData, VMsdata, p, iniTime, inibest);
				if (p < bestp) {
					//p = VNSSearch(jData, VMsdata, p, iniTime);
					p = GreedySearch(jData, VMsdata, p, iniTime);
					bestp = p;
					inibest = 0;
					auto stop = high_resolution_clock::now();
					auto duration = duration_cast<milliseconds>(stop - start);
					runTime = duration.count();
					SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));
				}
				else {
					inibest++;
				}
			}
		}
		else{
			p = DiversificationSearch(jData, VMsdata, p, iniTime, inibest);
			if (p < bestp){
				//p = VNSSearch(jData, VMsdata, p, iniTime);
				p = GreedySearch(jData, VMsdata, p, iniTime);
				bestp = p;
				inibest = 0;
				auto stop = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>(stop - start);
				runTime = duration.count();
				SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));
			}
			else{
				inibest++;
			}
		}
				
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
		runTime = duration.count();
		iniTime++;
	}
	string reName = "TabuLS";
	//SaveRecord(SolutionRecord, reName);
	cout << "The runTime of basic TabuLS  = " << runTime << endl;
	cout << "The loops of basic TabuLS = " << iniTime << endl;
	cout << "The maxmum repeat times of TabuLS = " << inibest << endl;
	return bestp; 
}


//without further search 
TLSPopInfo LS(const vector<JobData> jData, const vector<VMData> VMsdata, vector<TLSPopInfo> pops)
{
	sort(pops.begin(), pops.end());
	TLSPopInfo bestp = pops.front();
	TLSPopInfo p = pops.front();
	//PrintPop(bestp);
	//-----------------------start searching-------------------------
	auto start = high_resolution_clock::now();
	long long runTime = 0;
	int iniTime = 0;
	int inibest = 0;
	vector<pair<long long, double>> SolutionRecord;
	while ((runTime < RunTimes) && (iniTime < IniTimes) && (inibest < Inibest))
	{
		double a = RandomDouble(0, 1);
		if (a > p_ls) {
			p = IntensificationSearch(jData, VMsdata, p, iniTime);
			if (p < bestp) {
				bestp = p;
				inibest = 0;
				auto stop = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>(stop - start);
				runTime = duration.count();
				SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));
			}
			else
			{
				p = DiversificationSearch(jData, VMsdata, p, iniTime, inibest);
				if (p < bestp) {
					bestp = p;
					inibest = 0;
					auto stop = high_resolution_clock::now();
					auto duration = duration_cast<milliseconds>(stop - start);
					runTime = duration.count();
					SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));
				}
				else {
					inibest++;
				}
			}
		}
		else {
			p = DiversificationSearch(jData, VMsdata, p, iniTime, inibest);
			if (p < bestp) {
				bestp = p;
				inibest = 0;
				auto stop = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>(stop - start);
				runTime = duration.count();
				SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));
			}
			else {
				inibest++;
			}
		}

		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
		runTime = duration.count();
		iniTime++;
	}
	string reName = "LS";
	//SaveRecord(SolutionRecord, reName);
	cout << "The runTime of basic LS  = " << runTime << endl;
	cout << "The loops of basic LS = " << iniTime << endl;
	cout << "The maxmum repeat times of LS = " << inibest << endl;
	return bestp;
}


TLSPopInfo IntensificationSearch(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int tabuStamp) {
	
	vector<TLSPopInfo> pops;
	int iLoop = 0;
	int jIndex1 = RouletteIndex(p.p_RewardList);
	int jIndex2 = RouletteIndex(p.p_RewardList);
	while (jIndex1 == jIndex2)
	{
	    jIndex2 = RouletteIndex(p.p_RewardList);
	}

	for (int i = 0; i < p.pSequence.size(); i++)
	{
		if (jData[p.pSequence[jIndex1]].jPriority == jData[p.pSequence[i]].jPriority)
		{
			swap(p.pSequence[jIndex1], p.pSequence[i]);
			pops.push_back(p);
		}
	}
	for (int i = 0; i < p.pSequence.size(); i++)
	{
		if (jData[p.pSequence[jIndex2]].jPriority == jData[p.pSequence[i]].jPriority)
		{
			swap(p.pSequence[jIndex2], p.pSequence[i]);
		}
		pops.push_back(p);
	}

	random_shuffle(pops.begin(), pops.end());
	vector<TLSPopInfo> newpops;
	int n = min(int(pops.size()), lIteNum);
	for (int i = 0; i < n; i++)
	{
		GreedySortV2(jData, VMsdata, pops[i]);
		newpops.push_back(pops[i]);
	}
	sort(newpops.begin(), newpops.end());

	if (newpops.front() < p)
	{
		for (int i = 0; i < pops.front().pSequence.size(); i++)
		{
			if (pops.front().pSequence[i] != p.pSequence[i]) {
				pops.front().p_TabuList[i] = tabuStamp;
				pops.front().p_RewardList[i]++;
			}
		}
		return newpops.front();
	}
	else
	{
		return p;
		
	}
}

TLSPopInfo DiversificationSearch(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int iterationStamp, int bestStamp)
{
	vector<int> IDList_tabu;
	auto minPosition = min_element(p.p_TabuList.begin(), p.p_TabuList.end());
	int minSearch = *minPosition;
	
	double p_a = RandomDouble(0, 1) + p_div;
	double p_best = double(bestStamp) / double(Inibest);

	if (isLessThan(p_a, p_best))
	{
		TLSPopInfo newP = p;
		TLSPopInfo p1 = p;
		TLSPopInfo p2;
		for (int j = 0; j < p.pChromosome.size(); j++)
		{
			p2.pChromosome.push_back(rand() / double(RAND_MAX));
		}
		vector<double> newSeq;
		int jNum = p2.pChromosome.size();
		//generate two position randomly
		int c = RandomInt(1, ceil(jNum * 0.2));
		int a = RandomInt(0, jNum - 1 - c);
		int b = RandomInt(0, jNum - 1 - c);

		newSeq.insert(newSeq.end(), p1.pChromosome.begin(), p1.pChromosome.begin() + a);
		newSeq.insert(newSeq.end(), p2.pChromosome.begin() + b, p2.pChromosome.begin() + b + c);
		newSeq.insert(newSeq.end(), p1.pChromosome.begin() + a + c, p1.pChromosome.end());

		newP.pChromosome = newSeq;
		newP.pSequence = GiveSort(newP.pChromosome);
		newP.pSequence = SortByPri(newP, jData);
		GreedySortV2(jData, VMsdata, newP);
		if (newP < p)
		{
			for (int i = 0; i < newP.pSequence.size(); i++)
			{
				if (newP.pSequence[i] != p.pSequence[i]) {
					newP.p_TabuList[i] = iterationStamp;
					newP.p_RewardList[i]++;
				}
			}
			return newP;
		}
		else
		{
			for (int i = 0; i < newP.pSequence.size(); i++)
			{
				if (newP.pSequence[i] != p.pSequence[i]) {
					newP.p_TabuList[i] = iterationStamp;
				}
			}
			return newP;
		}
	}
	else
	{
		//cout << minSearch << endl;
		for (int i = 0; i < p.p_TabuList.size(); i++)
		{
			if (p.p_TabuList[i] <= tabuLen) {
				IDList_tabu.push_back(i);
			}
		}
		if (IDList_tabu.size() > lIteNum)
		{
			random_shuffle(IDList_tabu.begin(), IDList_tabu.end());
		}

		if (IDList_tabu.size() == 0)
		{
			for (int i = 0; i < p.p_TabuList.size(); i++) {
				if (p.p_TabuList[i] == minSearch) {
					IDList_tabu.push_back(i);
				}
			}
		}
		int n = min(lIteNum, int(IDList_tabu.size()));
		vector<TLSPopInfo> pops;
		TLSPopInfo newP = p;
		for (int i = 0; i < n; i++)
		{
			newP = p;
			newP.pChromosome[IDList_tabu[i]] = rand() / double(RAND_MAX);
			newP.pSequence = GiveSort(newP.pChromosome);
			newP.pSequence = SortByPri(newP, jData);
			GreedySortV2(jData, VMsdata, newP);
			if (newP != p)
			{
				pops.push_back(newP);
			}
		}
		if (pops.size() == 0)
		{
			for (int i = 0; i < n; i++)
			{
				newP.pChromosome[IDList_tabu[i]] = rand() / double(RAND_MAX);
			}
			newP.pSequence = GiveSort(newP.pChromosome);
			newP.pSequence = SortByPri(newP, jData);
			GreedySortV2(jData, VMsdata, newP);
			if (newP < p){
				for (int i = 0; i < newP.pSequence.size(); i++)
				{
					if (newP.pSequence[i] != p.pSequence[i]) {
						newP.p_TabuList[i] = iterationStamp;
						newP.p_RewardList[i]++;
					}
				}
				return newP;
			}
			else {
				for (int i = 0; i < newP.pSequence.size(); i++)
				{
					if (newP.pSequence[i] != p.pSequence[i]) {
						newP.p_TabuList[i] = iterationStamp;
					}
				}
				return newP;
			}
		}
		sort(pops.begin(), pops.end());
		if (pops.front() < p )
		{
			for (int i = 0; i < pops.front().pSequence.size(); i++)
			{
				if (pops.front().pSequence[i] != p.pSequence[i]) {
					pops.front().p_TabuList[i] = iterationStamp;
					pops.front().p_RewardList[i]++;
				
				}
			}
			return pops.front();
		}
		else
		{
			for (int i = 0; i < pops.front().pSequence.size(); i++)
			{
				if (pops.front().pSequence[i] != p.pSequence[i]) {
					pops.front().p_TabuList[i] = iterationStamp;
				}
			}
			return pops.front();
		}
	}
	
}

vector<JobInQueue> SingleMachineExeTimeRule(const vector<JobData> jData, vector<int> pSequence, VMData vm, TLSPopInfo& newP, int tabuStamp)
{
	vector<int> bestSequence = pSequence;
	vector<int> newSequence = pSequence;
	//the original result
	vector<JobInQueue> res;
	res = EvaluationSingleMachine(jData, pSequence, vm);
	double bestCost = res.back().e_time;
	//cout << "original " << "cost" << res.back().e_time << endl;

	vector<JobInQueue> newres;
	vector<JobInQueue> bestres = res;

	//BNS
	if (newSequence.size() - 1 > lIteNum)
	{
		vector<int> randomIndex;
		for (int i = 0; i < newSequence.size() - 1; i++)
		{
			randomIndex.push_back(i);
		}
		random_shuffle(randomIndex.begin(), randomIndex.end());
		for (int j = 0; j < lIteNum; j++)
		{
			int a = randomIndex[lIteNum];
			if ((jData[newSequence[a + 1]].jPriority == jData[newSequence[a]].jPriority) && (ExeTimeCom(jData[newSequence[a + 1]], jData[newSequence[a]])))
			{
				newSequence = pSequence;
				swap(newSequence[a], newSequence[a + 1]);
				newP.p_TabuList[a + 1] = tabuStamp;
				newP.p_TabuList[a] = tabuStamp;
				newP.p_RewardList[a + 1]++;
				newP.p_RewardList[a]++;
				newres = EvaluationSingleMachine(jData, newSequence, vm);
				//cout << "i = " << i << "cost" << newres.back().e_time << endl;
				if (isLessThan(newres.back().e_time, bestCost))
				{
					bestCost = newres.back().e_time;
					bestres = newres;
				}
				newres.clear();
			}

		}
	}
	else
	{
		for (int i = 0; i < newSequence.size() - 1; i++)
		{
			if ((jData[newSequence[i + 1]].jPriority == jData[newSequence[i]].jPriority) && (ExeTimeCom(jData[newSequence[i + 1]], jData[newSequence[i]])))
			{
				newSequence = pSequence;
				swap(newSequence[i], newSequence[i + 1]);
				newP.p_TabuList[i + 1] = tabuStamp;
				newP.p_TabuList[i] = tabuStamp;
				newres = EvaluationSingleMachine(jData, newSequence, vm);
				//cout << "i = " << i << "cost" << newres.back().e_time << endl;
				if (isLessThan(newres.back().e_time, bestCost))
				{
					bestCost = newres.back().e_time;
					bestres = newres;
				}
				newres.clear();
			}
		}
	}

	//for (int i = 0; i < newSequence.size() - 1; i++)
	//{
	//	if ((ExeTimeCom(jData[newSequence[i + 1]], jData[newSequence[i]])) && (jData[newSequence[i + 1]].jPriority == jData[newSequence[i]].jPriority))
	//	{
	//		newSequence = pSequence;
	//		swap(newSequence[i], newSequence[i + 1]);
	//		newP.p_TabuList[i + 1] = tabuStamp;
	//		newP.p_TabuList[i] = tabuStamp;
	//		newres = EvaluationSingleMachine(jData, newSequence, vm);
	//		//cout << "i = " << i << "cost" << newres.back().e_time << endl;
	//		if (isLessThan(newres.back().e_time, bestCost))
	//		{
	//			bestCost = newres.back().e_time;
	//			bestres = newres;
	//		}
	//		newres.clear();
	//	}
	//}

	newP.pSequence.insert(newP.pSequence.end(), bestSequence.begin(), bestSequence.end());
	newP.pCost = newP.pCost + bestCost * vm.vmPrice;
	return bestres;
}

TLSPopInfo GreedySearch(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int tabuStamp)  //execution time rule
{
	TLSPopInfo bestp = p;
	TLSPopInfo newP;
	newP.p_TabuList = p.p_TabuList;
	newP.p_RewardList = p.p_RewardList;
	newP.pChromosome = p.pChromosome;
	//seperate by machines
	vector<vector<int>> vmList;
	vector<int> vm;
	for (int i = 0; i < VMsdata.size(); i++)
	{
		vm.clear();
		for (int j = 0; j < p.mPlacement.size(); j++)
		{
			if (VMsdata[i].vmID == p.mPlacement[j])
			{
				vm.push_back(p.trueSequence[j]);
				//cout << "vmID:" << VMsdata[i].vmID << "jID:" << p.trueSequence[j] << "jpriority:" << p.trueSequence[j] << endl;
			}
		}
		vmList.push_back(vm);
		vm.clear();
	}
	//optimize each machine sequence
	vector<JobInQueue> res;
	for (size_t i = 0; i < vmList.size(); i++)
	{
		vector<JobInQueue> part_res = SingleMachineExeTimeRule(jData, vmList[i], VMsdata[i], newP, tabuStamp);
		res.insert(res.end(), part_res.begin(), part_res.end());
	}
	sort(res.begin(), res.end(), CompareJobsByStartTime);

	//fix the solution
	//update information of pop
	newP.trueSequence.clear();
	newP.mPlacement.clear();
	for (int i = 0; i < res.size(); i++)
	{
		newP.trueSequence.push_back(res[i].jID);
		newP.mPlacement.push_back(res[i].mID);
	}

	if (newP < bestp)
	{
		//cout << "1" << endl;
		return newP;
	}
	else
	{
		return bestp;
	}

}
//void evaluationChoose(int i, vector<JobData> Jdata, const vector<VMData> VMsdata, TLSPopInfo &p) {
//	switch (i)
//	{
//	//case 0:
//	//	GeneralSort(Jdata, VMsdata, p);
//	//	break;
//	//case 1:
//	//	GreedySortV1(Jdata, VMsdata, p);
//	//	break;
//	case 0:
//		GreedySortV2(Jdata, VMsdata, p);
//		break;
//	case 1:
//		GreedySortV2(Jdata, VMsdata, p);
//		break;
//	case 2:
//		GreedySortV2(Jdata, VMsdata, p);
//		break;
//	case 3:
//		GreedySortV2(Jdata, VMsdata, p);
//		break;
//	}
//}

TLSPopInfo VNSSearch(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int tabuStamp)
{
	//---------get the number of priority and the ID of jobs-------
	vector<int> allPriority;
	for (int i = 0; i < jData.size(); i++)
	{
		allPriority.push_back(jData[i].jPriority);
	}
	auto maxPosition = max_element(allPriority.begin(), allPriority.end());
	int PriNum = *maxPosition;
	vector<int> IDSeq;
	IDSeq = returnPriLoc(jData);

	vector<pair<int, vector<int>>> JobsByPri = GroupByPriorityIndex(jData);

	//-------------------------------
	int IDneighborhood = 0;
	TLSPopInfo  resPop;
	TLSPopInfo  prePop = p;
	while (IDneighborhood < NeighborNum)
	{
		bool isUpdate = false;
		resPop = VNSChoose(IDneighborhood, jData, VMsdata, prePop, PriNum, IDSeq, tabuStamp);
		if (resPop < prePop)                   //research in all neighborhood
		{
			prePop = resPop;
			isUpdate = true;
			IDneighborhood = 0;
			continue;
		}
		else
		{
			IDneighborhood++;
		}

		if ((IDneighborhood == NeighborNum) && (!isUpdate))
		{
			for (int i = 0; i < prePop.pSequence.size(); i++)
			{
				if (prePop.pSequence[i] != p.pSequence[i]) {
					//prePop.p_TabuList[i]++;
					prePop.p_TabuList[i] = tabuStamp;
					prePop.p_RewardList[i]++;
				}
			}
			return prePop;
		}
	}
}
TLSPopInfo  VNSChoose(int i, vector<JobData> Jdata, const vector<VMData> VMsdata, TLSPopInfo p, int PriNum, vector<int> IDSeq, int tabuStamp)
{
	switch (i)
	{
	case 0:
		return RS0(Jdata, VMsdata, p, PriNum, IDSeq);
		break;
	case 1:
		return RS1(Jdata, VMsdata, p, PriNum, IDSeq);
		break;
	case 2:
		return RS2(Jdata, VMsdata, p, PriNum, IDSeq);
		break;	
	case 3:
		return RS3(Jdata, VMsdata, p, PriNum, IDSeq);
		//return ExeTimeSearch(Jdata, VMsdata, p, tabuStamp);
		break;
	}
}



TLSPopInfo ExeTimeSearch(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p,int tabuStamp)  //execution time rule
{
	TLSPopInfo bestP = p;
	//Execution time rule
	for (int i = 0; i < p.pSequence.size() - 1; i++)
	{
		if ((ExeTimeCom(jData[p.pSequence[i + 1]], jData[p.pSequence[i]])) && (jData[p.pSequence[i + 1]].jPriority == jData[p.pSequence[i]].jPriority))
		{
			int a = p.pSequence[i + 1];
			p.pSequence[i + 1] = p.pSequence[i];
			p.pSequence[i] = a;
			//p.p_TabuList[i + 1] = tabuStamp;
			//p.p_TabuList[i] = tabuStamp;
			GreedySortV2(jData, VMsdata, p);
			if (p < bestP)
			{
				//p.p_RewardList[i + 1]++;
				//p.p_RewardList[i]++;
				bestP = p;
			}
		}
	}
	return bestP;
}

//-------------------------neighbor 0: 2*opt -----------------------------
TLSPopInfo RS0(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int PriNum, vector<int> IDSeq)
{
	TLSPopInfo Newp = p;
	vector<TLSPopInfo> BetterPops;
	vector<int> PartSeq;
	vector<int> NewPartSeq;
	int Inum = 0;
	vector<int> SeqIndex;
	for (int i = 0; i < PriNum; i++)
	{
		int a = IDSeq[i] + 1;
		int b = IDSeq[i + 1] + 1;
		PartSeq.insert(PartSeq.end(), p.pSequence.begin() + a, p.pSequence.begin() + b);

		for (int j = 0; j < PartSeq.size(); j++)
		{
			SeqIndex.push_back(j);
		}
		random_shuffle(SeqIndex.begin(), SeqIndex.end());
		Inum = min((int)SeqIndex.size(), lIteNum);

		for (int l = 0; l < Inum; l++){
			int j = SeqIndex[l];
			NewPartSeq.insert(NewPartSeq.end(), PartSeq.begin() + j, PartSeq.end()); //exchagne two units
			NewPartSeq.insert(NewPartSeq.end(), PartSeq.begin(), PartSeq.begin() + j);

			//----------change the part-----------
			for (int k = 0; k < NewPartSeq.size(); k++){
				Newp.pSequence[a + k] = NewPartSeq[k];
				Newp.p_TabuList[a + k]++;
			}
			GreedySortV2(jData, VMsdata, Newp);

			//---save all the pops better the p----
			if (Newp < p){
				BetterPops.push_back(Newp);
			}
			Newp = p;
			NewPartSeq.clear();
		}
		PartSeq.clear();
		SeqIndex.clear();
	}

	if (BetterPops.size() > 0)
	{
		sort(BetterPops.begin(), BetterPops.end());
		return BetterPops.front();
	}
	else
	{
		return p;
	}
}

//-------------------------neighbor 1: reverse(small) -----------------------------
TLSPopInfo RS1(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int PriNum, vector<int> IDSeq)
{
	TLSPopInfo Newp = p;
	vector<TLSPopInfo> BetterPops;
	vector<int> PartSeq;
	vector<int> NewPartSeq;
	vector<int> arrA;
	vector<int> arrB;
	vector<int> SeqIndex;
	int Inum = 0;
	for (int i = 0; i < PriNum; i++)
	{
		int a = IDSeq[i] + 1;
		int b = IDSeq[i + 1] + 1;
		PartSeq.insert(PartSeq.end(), p.pSequence.begin() + a, p.pSequence.begin() + b);

		for (int j = 0; j < PartSeq.size(); j++)
		{
			SeqIndex.push_back(j);
		}
		random_shuffle(SeqIndex.begin(), SeqIndex.end());
		Inum = min((int)SeqIndex.size(), lIteNum);

		for (int l = 0; l < Inum; l++)
		{
			int j = SeqIndex[l];
			arrA.insert(arrA.end(), PartSeq.begin(), PartSeq.begin() + j);
			reverse(arrA.begin(), arrA.end());

			arrB.insert(arrB.end(), PartSeq.begin() + j, PartSeq.end());
			reverse(arrB.begin(), arrB.end());

			NewPartSeq.insert(NewPartSeq.end(), arrA.begin(), arrA.end());
			NewPartSeq.insert(NewPartSeq.end(), arrB.begin(), arrB.end());
			//----------change the part-----------
			for (int k = 0; k < NewPartSeq.size(); k++)
			{
				Newp.pSequence[a + k] = NewPartSeq[k];
				Newp.p_TabuList[a + k]++;
			}
			GreedySortV2(jData, VMsdata, Newp);

			//---save all the pops better the p----
			if (Newp < p)
			{
				BetterPops.push_back(Newp);
			}
			Newp = p;
			NewPartSeq.clear();
			arrA.clear();
			arrB.clear();

		}
		PartSeq.clear();
		SeqIndex.clear();
	}

	if (BetterPops.size() > 0)
	{
		sort(BetterPops.begin(), BetterPops.end());
		return BetterPops.front();
	}
	else
	{
		return p;
	}
}
//-------------------------neighbor 2: 2opt (big)-----------------------------
TLSPopInfo RS2(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int PriNum, vector<int> IDSeq)
{
	TLSPopInfo Newp = p;
	vector<TLSPopInfo> BetterPops;
	vector<int> PartSeq;
	vector<int> NewPartSeq;
	vector<pair<int, int>> ComSeq;
	int Inum = 0;
	for (int i = 0; i < PriNum; i++)
	{
		int a = IDSeq[i] + 1;
		int b = IDSeq[i + 1] + 1;
		PartSeq.insert(PartSeq.end(), p.pSequence.begin() + a, p.pSequence.begin() + b);

		for (int j = 0; j < PartSeq.size(); j++)
		{
			for (int q = j + 1; q < PartSeq.size(); q++)
			{
				ComSeq.push_back(pair<int, int>{j, q});
			}
		}

		random_shuffle(ComSeq.begin(), ComSeq.end());
		Inum = min((int)ComSeq.size(), lIteNum);

		for (int l = 0; l < Inum; l++)
		{
			int c = ComSeq[l].first;
			int d = ComSeq[l].second;
			NewPartSeq = PartSeq;
			swap(NewPartSeq[c], NewPartSeq[d]);

			//----------change the part-----------
			for (int k = 0; k < NewPartSeq.size(); k++)
			{
				Newp.pSequence[a + k] = NewPartSeq[k];
				//Newp.p_TabuList[a + k]++;
			}
			GreedySortV2(jData, VMsdata, Newp);

			//---save all the pops better the p----
			if (Newp < p)
			{
				BetterPops.push_back(Newp);
			}
			Newp = p;
			NewPartSeq.clear();
		}
		ComSeq.clear();
		PartSeq.clear();
	}

	if (BetterPops.size() > 0)
	{
		sort(BetterPops.begin(), BetterPops.end());
		return BetterPops.front();
	}
	else
	{
		return p;
	}
}

TLSPopInfo RS3(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int PriNum, vector<int> IDSeq)
{
	TLSPopInfo Newp = p;
	vector<TLSPopInfo> BetterPops;
	vector<int> PartSeq;
	vector<int> NewPartSeq;
	vector<pair<int, int>> ComSeq;
	int Inum = 0;
	for (int i = 0; i < PriNum; i++)
	{
		int a = IDSeq[i] + 1;
		int b = IDSeq[i + 1] + 1;
		PartSeq.insert(PartSeq.end(), p.pSequence.begin() + a, p.pSequence.begin() + b);

		for (int j = 0; j < PartSeq.size(); j++)
		{
			for (int q = j + 1; q < PartSeq.size(); q++)
			{
				ComSeq.push_back(pair<int, int>{j, q});
			}
		}

		random_shuffle(ComSeq.begin(), ComSeq.end());
		Inum = min((int)ComSeq.size(), lIteNum);

		for (int l = 0; l < Inum; l++)
		{
			int c = ComSeq[l].first;
			int d = ComSeq[l].second;
			NewPartSeq = PartSeq;
			swap(NewPartSeq[c], NewPartSeq[d]);

			//----------change the part-----------
			for (int k = 0; k < NewPartSeq.size(); k++)
			{
				Newp.pSequence[a + k] = NewPartSeq[k];
				//Newp.p_TabuList[a + k]++;
			}
			
			GreedySortV2(jData, VMsdata, Newp);

			//---save all the pops better the p----
			if (Newp < p)
			{
				BetterPops.push_back(Newp);
			}
			Newp = p;
			NewPartSeq.clear();
		}
		ComSeq.clear();
		PartSeq.clear();
	}

	if (BetterPops.size() > 0)
	{
		sort(BetterPops.begin(), BetterPops.end());
		return BetterPops.front();
	}
	else
	{
		return p;
	}
}



TLSPopInfo TabuLSNoEva(const vector<JobData> jData, const vector<VMData> VMsdata, vector<TLSPopInfo> pops)
{
	sort(pops.begin(), pops.end());
	TLSPopInfo bestp = pops.front();
	TLSPopInfo p = pops.front();
	//PrintPop(bestp);
	//-----------------------start searching-------------------------
	auto start = high_resolution_clock::now();
	long long runTime = 0;
	int iniTime = 0;
	int inibest = 0;
	vector<pair<long long, double>> SolutionRecord;
	while ((runTime < RunTimes) && (iniTime < IniTimes) && (inibest < Inibest))
	{
		double a = RandomDouble(0, 1);
		if (a > p_ls) {
			p = ISearchNoEva(jData, VMsdata, p, iniTime);
			if (p < bestp) {
				p = GreedySearch(jData, VMsdata, p, iniTime);
				bestp = p;
				inibest = 0;
				auto stop = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>(stop - start);
				runTime = duration.count();
				SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));
			}
			else
			{
				p = DiversificationSearch(jData, VMsdata, p, iniTime, inibest);
				if (p < bestp) {
					p = GreedySearch(jData, VMsdata, p, iniTime);
					bestp = p;
					inibest = 0;
					auto stop = high_resolution_clock::now();
					auto duration = duration_cast<milliseconds>(stop - start);
					runTime = duration.count();
					SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));
				}
				else {
					inibest++;
				}
			}
		}
		else {
			p = DiversificationSearch(jData, VMsdata, p, iniTime, inibest);
			if (p < bestp) {
				p = GreedySearch(jData, VMsdata, p, iniTime);
				bestp = p;
				inibest = 0;
				auto stop = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>(stop - start);
				runTime = duration.count();
				SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));
			}
			else {
				inibest++;
			}
		}

		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
		runTime = duration.count();
		iniTime++;
	}
	string reName = "TabuLSNoEva";
	//SaveRecord(SolutionRecord, reName);
	cout << "The runTime of basic TabuLSNoEva  = " << runTime << endl;
	cout << "The loops of basic TabuLSNoEva = " << iniTime << endl;
	cout << "The maxmum repeat times of TabuLSNoEva = " << inibest << endl;
	return bestp;
}

TLSPopInfo TabuLSNoTabu(const vector<JobData> jData, const vector<VMData> VMsdata, vector<TLSPopInfo> pops)
{
	sort(pops.begin(), pops.end());
	TLSPopInfo bestp = pops.front();
	TLSPopInfo p = pops.front();
	//PrintPop(bestp);
	//-----------------------start searching-------------------------
	auto start = high_resolution_clock::now();
	long long runTime = 0;
	int iniTime = 0;
	int inibest = 0;
	vector<pair<long long, double>> SolutionRecord;
	while ((runTime < RunTimes) && (iniTime < IniTimes) && (inibest < Inibest))
	{
		double a = RandomDouble(0, 1);
		if (a > p_ls) {
			p = IntensificationSearch(jData, VMsdata, p, iniTime);
			//p = VNSSearch(jData, VMsdata, p, iniTime);
			//p = GreedySearch(jData, VMsdata, p, iniTime); 
			if (p < bestp) {
				//p = VNSSearch(jData, VMsdata, p, iniTime);
				p = GreedySearch(jData, VMsdata, p, iniTime);
				bestp = p;
				inibest = 0;
				auto stop = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>(stop - start);
				runTime = duration.count();
				SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));
			}
			else
			{
				p = DSearchNoTabu(jData, VMsdata, p, iniTime, inibest);
				if (p < bestp) {
					//p = VNSSearch(jData, VMsdata, p, iniTime);
					p = GreedySearch(jData, VMsdata, p, iniTime);
					bestp = p;
					inibest = 0;
					auto stop = high_resolution_clock::now();
					auto duration = duration_cast<milliseconds>(stop - start);
					runTime = duration.count();
					SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));
				}
				else {
					inibest++;
				}
			}
		}
		else {
			p = DSearchNoTabu(jData, VMsdata, p, iniTime, inibest);
			if (p < bestp) {
				//p = VNSSearch(jData, VMsdata, p, iniTime);
				p = GreedySearch(jData, VMsdata, p, iniTime);
				bestp = p;
				inibest = 0;
				auto stop = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>(stop - start);
				runTime = duration.count();
				SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));
			}
			else {
				inibest++;
			}
		}

		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
		runTime = duration.count();
		iniTime++;
	}
	string reName = "TabuLSNoTabu";
	//SaveRecord(SolutionRecord, reName);
	cout << "The runTime of basic TabuLSNoTabu  = " << runTime << endl;
	cout << "The loops of basic TabuLSNoTabu = " << iniTime << endl;
	cout << "The maxmum repeat times of TabuLSNoTabu = " << inibest << endl;
	return bestp;
}

TLSPopInfo ISearchNoEva(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int tabuStamp) {

	vector<TLSPopInfo> pops;
	int iLoop = 0;
	vector<int> randomIndex = p.pSequence;
	random_shuffle(randomIndex.begin(), randomIndex.end());
	int jIndex1 = randomIndex[0];
	int jIndex2 = randomIndex[1];

	for (int i = 0; i < p.pSequence.size(); i++)
	{
		if (jData[p.pSequence[jIndex1]].jPriority == jData[p.pSequence[i]].jPriority)
		{
			swap(p.pSequence[jIndex1], p.pSequence[i]);
			pops.push_back(p);
		}
	}
	for (int i = 0; i < p.pSequence.size(); i++)
	{
		if (jData[p.pSequence[jIndex2]].jPriority == jData[p.pSequence[i]].jPriority)
		{
			swap(p.pSequence[jIndex2], p.pSequence[i]);
		}
		pops.push_back(p);
	}

	random_shuffle(pops.begin(), pops.end());
	vector<TLSPopInfo> newpops;
	int n = min(int(pops.size()), lIteNum);
	for (int i = 0; i < n; i++)
	{
		GreedySortV2(jData, VMsdata, pops[i]);
		newpops.push_back(pops[i]);
	}
	sort(newpops.begin(), newpops.end());

	if (newpops.front() < p)
	{
		for (int i = 0; i < pops.front().pSequence.size(); i++)
		{
			if (pops.front().pSequence[i] != p.pSequence[i]) {
				pops.front().p_TabuList[i] = tabuStamp;
				pops.front().p_RewardList[i]++;
			}
		}
		return newpops.front();
	}
	else
	{
		return p;

	}
}

TLSPopInfo DSearchNoTabu(const vector<JobData> jData, const vector<VMData> VMsdata, TLSPopInfo p, int iterationStamp, int bestStamp)
{
	vector<int> IDList_tabu;
	auto minPosition = min_element(p.p_TabuList.begin(), p.p_TabuList.end());
	int minSearch = *minPosition;

	double p_a = RandomDouble(0, 1) + p_div;
	double p_best = double(bestStamp) / double(Inibest);

	if (isLessThan(p_a, p_best))
	{
		TLSPopInfo newP = p;
		TLSPopInfo p1 = p;
		TLSPopInfo p2;
		for (int j = 0; j < p.pChromosome.size(); j++)
		{
			p2.pChromosome.push_back(rand() / double(RAND_MAX));
		}
		vector<double> newSeq;
		int jNum = p2.pChromosome.size();
		//generate two position randomly
		int c = RandomInt(1, ceil(jNum * 0.2));
		int a = RandomInt(0, jNum - 1 - c);
		int b = RandomInt(0, jNum - 1 - c);

		newSeq.insert(newSeq.end(), p1.pChromosome.begin(), p1.pChromosome.begin() + a);
		newSeq.insert(newSeq.end(), p2.pChromosome.begin() + b, p2.pChromosome.begin() + b + c);
		newSeq.insert(newSeq.end(), p1.pChromosome.begin() + a + c, p1.pChromosome.end());

		newP.pChromosome = newSeq;
		newP.pSequence = GiveSort(newP.pChromosome);
		newP.pSequence = SortByPri(newP, jData);
		GreedySortV2(jData, VMsdata, newP);
		if (newP < p)
		{
			for (int i = 0; i < newP.pSequence.size(); i++)
			{
				if (newP.pSequence[i] != p.pSequence[i]) {
					newP.p_TabuList[i] = iterationStamp;
					newP.p_RewardList[i]++;
				}
			}
			return newP;
		}
		else
		{
			for (int i = 0; i < newP.pSequence.size(); i++)
			{
				if (newP.pSequence[i] != p.pSequence[i]) {
					newP.p_TabuList[i] = iterationStamp;
				}
			}
			return newP;
		}
	}
	else
	{
		vector<int> randomIndex;
		randomIndex = p.pSequence;
		random_shuffle(randomIndex.begin(), randomIndex.end());
		int N = min(int(p.pSequence.size()), lIteNum);

		for (int i = 0; i < N; i++)
		{
			IDList_tabu.push_back(randomIndex[i]);
		}
		
		vector<TLSPopInfo> pops;
		TLSPopInfo newP = p;
		for (int i = 0; i < N; i++)
		{
			newP = p;
			newP.pChromosome[IDList_tabu[i]] = rand() / double(RAND_MAX);
			newP.pSequence = GiveSort(newP.pChromosome);
			newP.pSequence = SortByPri(newP, jData);
			GreedySortV2(jData, VMsdata, newP);
			if (newP != p)
			{
				pops.push_back(newP);
			}
		}
		sort(pops.begin(), pops.end());
		if (pops.front() < p)
		{
			for (int i = 0; i < pops.front().pSequence.size(); i++)
			{
				if (pops.front().pSequence[i] != p.pSequence[i]) {
					pops.front().p_TabuList[i] = iterationStamp;
					pops.front().p_RewardList[i]++;

				}
			}
			return pops.front();
		}
		else
		{
			for (int i = 0; i < pops.front().pSequence.size(); i++)
			{
				if (pops.front().pSequence[i] != p.pSequence[i]) {
					pops.front().p_TabuList[i] = iterationStamp;
				}
			}
			return pops.front();
		}
	}
}