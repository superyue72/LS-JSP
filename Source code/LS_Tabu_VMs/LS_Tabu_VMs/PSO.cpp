#pragma once
#include "base.h"
#include "Evaluation.h"
#include "PSO.h"


PSOPopInfo BasicPSO(const vector<JobData> Jdata, const vector<VMData> VMsdata, vector <PSOPopInfo> Pops)
{
	PSOPopInfo gbest;
	vector<PSOPopInfo> pbest = Pops ;
	int slocation1;
	vector <PSOPopInfo> oldPops = Pops;             //parents generation
	vector <PSOPopInfo> totalPops;

	sort(Pops.begin(), Pops.end());
	gbest = Pops.front();
	//PrintPop(gbest);
	
	//-----------------------start searching-------------------------
	auto start = high_resolution_clock::now();
	long long runTime = 0;
	int iniTime = 0;
	int inibest = 0;
	vector<pair<long long, double>> SolutionRecord;
	while ((runTime < RunTimes) && (iniTime < IniTimes) && (inibest < Inibest))
	{
		for (int j = 0; j < oldPops.size(); j++)
		{
			totalPops.push_back(updateP(Jdata, VMsdata, oldPops[j], gbest, pbest[j]));
		}
		sort(totalPops.begin(), totalPops.end());
		if (totalPops.front() < gbest)
		{
			gbest = totalPops.front();
			inibest = 0;
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(stop - start);
			runTime = duration.count();
			SolutionRecord.push_back(pair<long long, double>({ runTime, gbest.pCost }));

		}
		else
		{
			inibest++;
			/*auto stop = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(stop - start);
			runTime = duration.count();
			SolutionRecord.push_back(pair<long long, double>({ runTime, gbest.pCost }));*/
		}
		oldPops = totalPops;
		totalPops.clear();
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
		runTime = duration.count();
		iniTime++;
	}

	cout << "The runTime of basic PSO  = " << runTime << endl;
	cout << "The loops of basic PSO = " << iniTime << endl;
	cout << "The maxmum repeat times of PSO = " << inibest << endl;
	string reName = "PSO";
	//SaveRecord(SolutionRecord, reName);
	return gbest;
}


PSOPopInfo updateP(const vector<JobData> Jdata, const vector<VMData> VMsdata, PSOPopInfo pop, PSOPopInfo& gbest, PSOPopInfo& pbest)
{
	PSOPopInfo newPop;
	vector<double> pv;
	vector<double> pl;
	double v;

	newPop = pop;

	//update each pop
	for (int i = 0; i < pop.pVelocity.size(); i++)
	{
		//cout << RandomDouble(0.0, 1.0) << " " << pbest.pLocation[i] - pop.pLocation[i] << " " << (gbest.pLocation[i] - pop.pLocation[i]) << endl;
		v = popW * pop.pVelocity[i] + c1 * RandomDouble(0.0, 1.0) * (pbest.pLocation[i] - pop.pLocation[i])
			+ c2 * RandomDouble(0.0, 1.0) * (gbest.pLocation[i] - pop.pLocation[i]);
		pv.push_back(v);
		pl.push_back(pop.pLocation[i] + v);
	}
	newPop.pVelocity = pv;
	newPop.pLocation = pl;
	newPop.pSequence = GiveSort(newPop.pLocation);   
	newPop.pSequence = SortByPri(newPop, Jdata);
	GreedySortV1(Jdata, VMsdata, newPop);

	if (newPop < pbest)
	{
		pbest = newPop;
	}
	return newPop;
}