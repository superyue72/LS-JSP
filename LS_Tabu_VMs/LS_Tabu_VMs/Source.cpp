#pragma once
#include "base.h"
#include "ReadData.h"
#include "Evaluation.h"
#include "GA.h"
#include "PSO.h"
#include "MVO.h"
#include "TabuLS.h"

// the parameters can be changed in base.h

int main()
{
	srand((unsigned int)time(nullptr));
	//----------initial input and settings----------
	vector<VMData> VMsdata;
	ReadVMs(VMsdata);
	vector<JobData> jData;
	ReadJobs(jData);

	int JOBNum = jData.size();       
	int VMNum = VMsdata.size();
	vector<int> allPriority;
	for (int i = 0; i < jData.size(); i++)
	{
		allPriority.push_back(jData[i].jPriority);

	}
	auto maxPosition = max_element(allPriority.begin(), allPriority.end());
	int PriNum = *maxPosition;
	
	//-----------------------Initialization-------------------------
	vector<GAPopInfo> GAPops;
	vector<PSOPopInfo> PSOPops;
	vector<MVOPopInfo> MVOPops;
	vector<TLSPopInfo> TLSPops;
	vector<double> sLocation;
	vector<double> sVelocity;
	vector<double> upBound;
	vector<double> lowBound;
	vector<int> tabu;
	vector<int> reward;
	for (size_t i = 0; i < jData.size(); i++)
	{
		tabu.push_back(0);
		reward.push_back(0);
	}

	for (int i = 0; i < Popsize; i++)
	{
		GAPopInfo GAp;
		PSOPopInfo PSOp;
		MVOPopInfo MVOp;
		TLSPopInfo TLSp;
		GAp.pID = i;
		PSOp.pID = i;
		MVOp.pID = i;
		TLSp.pID = i;
		sLocation.clear();
		sVelocity.clear();
		lowBound.clear();
		upBound.clear();

		for (int j = 0; j < JOBNum; j++)
		{
			sLocation.push_back(rand() / double(RAND_MAX));
			sVelocity.push_back(RandomDouble(0, v_max));
			upBound.push_back(RandomDouble(1, upB));
			lowBound.push_back(RandomDouble(-1, lowB));
		}

		PSOp.pVelocity = sVelocity;
		PSOp.pLocation = sLocation;
		PSOp.pSequence = GiveSort(sLocation);
		PSOp.pSequence = SortByPri(PSOp, jData);
		GreedySortV1(jData, VMsdata, PSOp);
		PSOPops.push_back(PSOp);

		GAp.pChromosome = sLocation;
		GAp.pSequence = GiveSort(sLocation);
		GAp.pSequence = SortByPri(GAp, jData);
		GreedySortV1(jData, VMsdata, GAp);
		GAPops.push_back(GAp);

		MVOp.lowBound = lowBound;
		MVOp.upBound = upBound;
		MVOp.pObjects = sLocation;
		MVOp.pSequence = GiveSort(sLocation);
		MVOp.pSequence = SortByPri(MVOp, jData);
		GreedySortV1(jData, VMsdata, MVOp);
		MVOPops.push_back(MVOp);

		TLSp.pChromosome = sLocation;
		TLSp.pSequence = GAp.pSequence;
		TLSp.p_TabuList = tabu;
		TLSp.p_RewardList = reward;
		GreedySortV2(jData, VMsdata, TLSp);              // GreedySortV2 is the one with furthur optimization
		TLSPops.push_back(TLSp);
	}
	
	
	//---------------------------------------------
	vector<pair<long long, double>> res_cost_TLS;
	vector<pair<long long, double>> res_cost_GA;
	vector<pair<long long, double>> res_cost_PSO;
	vector<pair<long long, double>> res_cost_MVO;
	vector<pair<long long, double>> res_cost_EMVO;

	for (int i = 0; i < 1; i++)								// for each instance, run 10 times
	{
		TLSPopInfo pTLS;										// LS-JSP algorithm
		pTLS = TabuLS(jData, VMsdata, TLSPops);
		res_cost_TLS.push_back(pair<int, double>({ i, pTLS.pCost }));
		PrintPop(pTLS);

		GAPopInfo pGA;											//baseline: GA algorithm	
		pGA = BasicGA(jData, VMsdata, GAPops);
		res_cost_GA.push_back(pair<int, double>({ i, pGA.pCost }));
		PrintPop(pGA);
	
		PSOPopInfo pPSO;										//baseline: PSO algorithm
		pPSO = BasicPSO(jData, VMsdata, PSOPops);
		res_cost_PSO.push_back(pair<int, double>({ i, pPSO.pCost }));
		PrintPop(pPSO);

		MVOPopInfo pEMVO;										//baseline: EMVO algorithm
		pEMVO = EMVO(jData, VMsdata, MVOPops);
		res_cost_EMVO.push_back(pair<int, double>({ i, pEMVO.pCost }));
		PrintPop(pEMVO);		

		MVOPopInfo pMVO;										//baseline: MVO algorithm 
		pMVO = MVO(jData, VMsdata, MVOPops);
		res_cost_MVO.push_back(pair<int, double>({ i, pMVO.pCost }));
		PrintPop(pMVO);
	}

	//--------------save results-----------------

	string reNameTLS = "Res_TLS";
	string reNameGA = "Res_GA";
	string reNamePSO = "Res_PSO";
	string reNameEMVO = "Res_EMVO";
	string reNameMVO = "Res_MVO";

	SaveRecord(res_cost_TLS, reNameTLS);
	SaveRecord(res_cost_GA, reNameGA);
	SaveRecord(res_cost_PSO, reNamePSO);
	SaveRecord(res_cost_EMVO, reNameEMVO);
	SaveRecord(res_cost_MVO, reNameMVO);

	return 0;
}