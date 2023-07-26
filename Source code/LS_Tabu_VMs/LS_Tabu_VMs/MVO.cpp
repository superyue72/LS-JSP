#pragma once
#include <cmath>
#include "base.h"
#include "Evaluation.h"
#include "tabu.h"
#include "MVO.h"


MVOPopInfo EMVO(const vector<JobData> Jdata, const vector<VMData> VMsdata, vector<MVOPopInfo> Pops)
{
	MVOPopInfo bestp;
	vector<MVOPopInfo> BestFiveSolutions;
	vector<MVOPopInfo> newGeneration;
	sort(Pops.begin(), Pops.end());
	bestp = Pops.front();
	queue<vector<int>> tabuMap;
	map<vector<int>, bool> MList;
	int bestLength = 5;

	//-----------------------start searching-------------------------
	auto start = high_resolution_clock::now();
	long long runTime = 0;
	int iniTime = 0;
	int inibest = 0;
	vector<pair<long long, double>> SolutionRecord;

	while ((runTime < RunTimes) && (iniTime < IniTimes) && (inibest < Inibest))
	{
		if (BestFiveSolutions.size() == Pops.size())
		{
			Pops = BestFiveSolutions;
			BestFiveSolutions.clear();
			//cout << iniTime;
		}
		double wormhole_existence_probability = minWep + iniTime * ((maxWep - minWep) / IniTimes);
		double travelling_distance_rate = 1 - (pow(iniTime, 1 / mDegree) / pow(IniTimes, 1 / mDegree));
		newGeneration = UpdateUniverse(Jdata, VMsdata, Pops, bestp, wormhole_existence_probability, travelling_distance_rate);
		sort(newGeneration.begin(), newGeneration.end());
		SolutionRecord.push_back(pair<long long, double>({ runTime, newGeneration.front().pCost }));
		if (newGeneration.front() < bestp)
		{
			bestp = newGeneration.front();
			inibest = 0;
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(stop - start);
			runTime = duration.count();
			SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));
		}
		else
		{
			inibest++;
			/*auto stop = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(stop - start);
			runTime = duration.count();
			SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));*/
		}

		int a = 0;
		for (int i = 0; i < newGeneration.size(); i++)
		{
			if ( a < bestLength)
			{
				//if (!SolutionTabu(tabuMap, MList,newGeneration[i].pSequence))
				//{
				//	BestFiveSolutions.push_back(newGeneration[i]);
				//	a++;
				//}
				BestFiveSolutions.push_back(newGeneration[i]);
				a++;
			}
			else
			{
				break;
			}
		}
		//BestFiveSolutions.insert(BestFiveSolutions.end(), newGeneration.begin(), newGeneration.begin() + 5);
		Pops = newGeneration;
		newGeneration.clear();
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
		runTime = duration.count();
		iniTime++;
	}
	string reName = "EMVO";
	//SaveRecord(SolutionRecord, reName);
	cout << "The runTime of basic EMVO  = " << runTime << endl;
	cout << "The loops of basic EMVO = " << iniTime << endl;
	cout << "The maxmum repeat times of EMVO = " << inibest << endl;
	return bestp;
}

MVOPopInfo MVO(const vector<JobData> Jdata, const vector<VMData> VMsdata, vector<MVOPopInfo> Pops)
{
	MVOPopInfo bestp;
	vector<MVOPopInfo> BestFiveSolutions;
	vector<MVOPopInfo> newGeneration;
	sort(Pops.begin(), Pops.end());
	bestp = Pops.front();
	queue<vector<int>> tabuMap;
	map<vector<int>, bool> MList;
	int bestLength = 5;

	//-----------------------start searching-------------------------
	auto start = high_resolution_clock::now();
	long long runTime = 0;
	int iniTime = 0;
	int inibest = 0;
	vector<pair<long long, double>> SolutionRecord;

	while ((runTime < RunTimes) && (iniTime < IniTimes) && (inibest < Inibest))
	{
		if (BestFiveSolutions.size() == Pops.size())
		{
			Pops = BestFiveSolutions;
			BestFiveSolutions.clear();
			//cout << iniTime;
		}
		double wormhole_existence_probability = minWep + iniTime * ((maxWep - minWep) / IniTimes);
		double travelling_distance_rate = 1 - (pow(iniTime, 1 / mDegree) / pow(IniTimes, 1 / mDegree));
		newGeneration = UpdateUniverse(Jdata, VMsdata, Pops, bestp, wormhole_existence_probability, travelling_distance_rate);
		sort(newGeneration.begin(), newGeneration.end());
		SolutionRecord.push_back(pair<long long, double>({ runTime, newGeneration.front().pCost }));
		if (newGeneration.front() < bestp)
		{
			bestp = newGeneration.front();
			inibest = 0;
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(stop - start);
			runTime = duration.count();
			SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));
		}
		else
		{
			inibest++;
			/*auto stop = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(stop - start);
			runTime = duration.count();
			SolutionRecord.push_back(pair<long long, double>({ runTime, bestp.pCost }));*/
		}

		int a = 0;
		for (int i = 0; i < newGeneration.size(); i++)
		{
			if (a < bestLength)
			{
				if (!SolutionTabu(tabuMap, MList,newGeneration[i].pSequence))
				{
					BestFiveSolutions.push_back(newGeneration[i]);
					a++;
				}
				BestFiveSolutions.push_back(newGeneration[i]);
				a++;
			}
			else
			{
				break;
			}
		}
		//BestFiveSolutions.insert(BestFiveSolutions.end(), newGeneration.begin(), newGeneration.begin() + 5);
		Pops = newGeneration;
		newGeneration.clear();
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
		runTime = duration.count();
		iniTime++;
	}
	string reName = "MVO";
	//SaveRecord(SolutionRecord, reName);
	cout << "The runTime of basic MVO  = " << runTime << endl;
	cout << "The loops of basic MVO = " << iniTime << endl;
	cout << "The maxmum repeat times of MVO = " << inibest << endl;
	return bestp;
}

vector<MVOPopInfo> UpdateUniverse(const vector<JobData> Jdata, const vector<VMData> VMsdata, vector<MVOPopInfo> Pops, MVOPopInfo bestP, double wep, double tdr)
{
	vector<MVOPopInfo> newPops;
	MVOPopInfo newP;
	//calculate inflation rate
	int InfRate = 0;
	for (int i = 0; i < Pops.size(); i++)
	{
		InfRate = InfRate + Pops[i].pCost;
	}
	for (int i = 0; i < Pops.size(); i++)
	{
		Pops[i].InflationRate = Pops[i].pCost / InfRate;
	}

	for (int i = 0; i < Pops.size(); i++)
	{
		newP = Pops[i];
		for (int j = 0; j < Pops[i].pObjects.size(); j++)
		{
			double r1 = RandomDouble(0, 1);
			double r2 = RandomDouble(0, 1);
			double r3 = RandomDouble(0, 1);
			if (r1 < Pops[i].InflationRate)
			{
				int k = RouletteChoice(Pops);
				newP.pObjects[j] = Pops[k].pObjects[j];
			}
			if (r2 < wep)
			{
				if (r3 <= 0.5) 
				{
					newP.pObjects[j] = bestP.pObjects[j] + tdr * ((newP.upBound[j] - newP.lowBound[j]) * RandomDouble(0, 1) + newP.lowBound[j]);
				}
				else
				{
					newP.pObjects[j] = bestP.pObjects[j] - tdr * ((newP.upBound[j] - newP.lowBound[j]) * RandomDouble(0, 1) + newP.lowBound[j]);
				}
			}
			
		}
		newP.pSequence = GiveSort(newP.pObjects);
		newP.pSequence = SortByPri(newP, Jdata);
		GreedySortV1(Jdata, VMsdata, newP);
		newPops.push_back(newP);
	}
	return newPops;
}


int RouletteChoice(vector<MVOPopInfo> pops)
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

