#pragma once

#include "base.h"
#include "Evaluation.h"
#include "TabuLS.h"
#include "GA.h"

GAPopInfo BasicGA(const vector<JobData> Jdata, const vector<VMData> VMsdata, vector <GAPopInfo> Pops)
{
	GAPopInfo bestp;
	GAPopInfo newpop;
	int slocation1;
	vector <GAPopInfo> oldPops = Pops;             //parents generation
	vector <GAPopInfo> totalPops;

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
				slocation1 = GaChoose(oldPops);
				while (slocation1 == j)
				{
					slocation1 = GaChoose(oldPops);
				}
				newpop = GaCross(oldPops[slocation1], oldPops[j]);
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
				totalPops[j] = GaMutation(totalPops[j]);
			}
			totalPops[j].pSequence = SortByPri(totalPops[j], Jdata);	
			GreedySortV1(Jdata, VMsdata, totalPops[j]);
		}

		//----------------new generation selection-------------
		for (int i = 0; i < oldPops.size(); i++)
		{
			totalPops.push_back(oldPops[i]);
		}

		sort(totalPops.begin(), totalPops.end());
		
		if (totalPops.front() < bestp)
		{
			bestp = totalPops.front();
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
	string reName = "GA";
	//SaveRecord(SolutionRecord, reName);
	cout << "The runTime of basic GA  = " << runTime << endl;
	cout << "The loops of basic GA = " << iniTime << endl;
	cout << "The maxmum repeat times of GA = " << inibest << endl;
	return bestp;
}

int GaChoose(vector <GAPopInfo> pops)
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

GAPopInfo GaCross(GAPopInfo p1, GAPopInfo p2)
{
	GAPopInfo newP = p2;
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
	return newP;
}

GAPopInfo GaMutation(GAPopInfo p)
{
	GAPopInfo newP = p;
	int a = RandomInt(0, newP.pSequence.size() - 1);
	newP.pChromosome[a] = rand() / double(RAND_MAX);
	newP.pSequence = GiveSort(newP.pChromosome);
	return newP;
}
