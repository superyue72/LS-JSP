#pragma once

#include <algorithm>
#include <numeric>
#include "base.h"
#include "Evaluation.h"

using namespace std;
using std::vector;
using std::iterator;

//Description: calculate the makespan by the given allocation and sequence for a machine
vector<JobInQueue> EvaluationSingleMachine(const vector<JobData> jData, vector<int> pSequence, VMData vm) {
	double MaxCPU = vm.vmCPU;
	double MaxMEM = vm.vmMEM;
	vector<JobInQueue> res;
	vector<JobInQueue> job_queue;
	int m_time = 0;
	for (int i = 0; i < pSequence.size(); i++) {
		JobData crr_job = jData[pSequence[i]];
		if (isGreaterThanOrEqual(MaxCPU, crr_job.jCPU) && isGreaterThanOrEqual(MaxMEM, crr_job.jMEM)) {
			MaxCPU -= crr_job.jCPU;
			MaxMEM -= crr_job.jMEM;
			JobInQueue qjob;
			qjob.jID = pSequence[i];
			qjob.mID = vm.vmID;
			qjob.priority = jData[pSequence[i]].jPriority;
			qjob.s_time = m_time;
			qjob.r_time = crr_job.jTime;
			qjob.e_time = qjob.s_time + qjob.r_time;
			job_queue.push_back(qjob);
			continue;
		}
		while (isLessThan(MaxCPU, crr_job.jCPU) || isLessThan(MaxMEM, crr_job.jMEM)) {
			//cout << MaxCPU << "," << crr_job.jCPU << "," << (MaxCPU < crr_job.jCPU) << endl;
			//cout << MaxMEM << "," << crr_job.jMEM << "," << (MaxMEM < crr_job.jMEM) << endl;
			sort(job_queue.begin(), job_queue.end(), QueueCompareJobsByRunningTime);
			JobInQueue front_job = job_queue.back();
			res.push_back(front_job);
			job_queue.pop_back();
			m_time = max(m_time, front_job.e_time);
			MaxCPU += jData[front_job.jID].jCPU;
			MaxMEM += jData[front_job.jID].jMEM;
		}
		MaxCPU -= crr_job.jCPU;
		MaxMEM -= crr_job.jMEM;
		JobInQueue qjob;
		qjob.jID = pSequence[i];
		qjob.mID = vm.vmID;
		qjob.priority = jData[pSequence[i]].jPriority;
		qjob.s_time = m_time;
		qjob.r_time = crr_job.jTime;
		qjob.e_time = qjob.s_time + qjob.r_time;
		job_queue.push_back(qjob);
		//cout << jData[pSequence[i]].jID << "," << qjob.s_time << "," << qjob.e_time << endl;
	}
	while (!job_queue.empty()) {
		JobInQueue front_job = job_queue.back();
		res.push_back(front_job);
		job_queue.pop_back();
		m_time = max(m_time, front_job.e_time);
	}
	//sort(res.begin(), res.end(), CompareJobsByStartTime);
	//for (int i = 0; i < res.size(); i++)
	//{
	//	cout << res[i].priority << "  ";
	//}
	//cout << endl;
	sort(res.begin(), res.end(), CompareJobsByRunningTime);

	return res;
}

double EvaluationSolution(const vector<JobData> jData, const vector<VMData> VMsdata, PopInfo p)
{
	double pCost = 0;
	vector<JobInQueue> res;
	//------------------divide jobs by machines---------------
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
			}
		}
		res = EvaluationSingleMachine(jData, vm, VMsdata[i]);
		int time = res.back().e_time;
		pCost = pCost + time * VMsdata[i].vmPrice;
		cout << "res: " << "machine" << i << ":  time: " << time << endl;
	}
	return pCost;
}


bool SortComTime(pair<int, int> s1, pair<int, int> s2)
{
	return s1.second > s2.second;
}

void GeneralSort(const vector<JobData> jData, const vector<VMData> VMsdata, PopInfo &p)
{
	//---------reset placement and negative means hasn't been allocated--------
	p.mPlacement.clear();
	for (int i = 0; i < p.pSequence.size(); i++)
	{
		p.mPlacement.push_back(-1);
	}

	//---------------------machine information---------------------------------
	vector<double> MaxCPU;
	vector<double> MaxMEM;
	vector<double> usedCPU;
	vector<double> usedMEM;
	for (int i = 0; i < VMsdata.size(); i++)
	{
		MaxCPU.push_back(VMsdata[i].vmCPU);
		MaxMEM.push_back(VMsdata[i].vmMEM);
		usedCPU.push_back(0);
		usedMEM.push_back(0);
	}

	//---------------------job information---------------------------------
	vector<int> jStartTime;                  //start execution time of each job
	vector<int> jComTime;                    //completion time of each job
	vector<pair<int, int>> InsComTime;       //processing job list
	map<int, int> jVMs;                      //placement of each job on each machine
	vector<pair<int, int>> jobStartTime;       

	// allocate jobs at time 0
	int CurTime = 0;
	int mID = 0;
	int candidateVM;
	int curIndex = 0;
	for (int i = 0; i < p.pSequence.size(); i++)
	{
		bool paused = true;
		for (int j = 0; j < VMsdata.size(); j++)
		{
			candidateVM = (mID + j) % VMsdata.size();
			if ((VMsdata[candidateVM].vmCPU - jData[p.pSequence[i]].jCPU - usedCPU[candidateVM] >= 0)&& (VMsdata[candidateVM].vmMEM - jData[p.pSequence[i]].jMEM - usedMEM[candidateVM] >= 0))
			{
				InsComTime.push_back(pair<int, int>({ p.pSequence[i],CurTime + jData[p.pSequence[i]].jTime }));
				jVMs.insert(pair<int, int>({ p.pSequence[i], candidateVM }));
				p.mPlacement[i] = candidateVM;
				usedCPU[candidateVM] += jData[p.pSequence[i]].jCPU;
				usedMEM[candidateVM] += jData[p.pSequence[i]].jMEM;
				jStartTime.push_back(CurTime);
				jobStartTime.push_back(pair<int, int>({ p.pSequence[i], CurTime }));
				jComTime.push_back(CurTime + jData[p.pSequence[i]].jTime);
				mID = (candidateVM + 1) % VMsdata.size();
				paused = false;
				break;
			}
		}
		if (paused)
		{
			curIndex = i;
			//cout << "curIndex is " << curIndex << endl;
			break;
		}
	}

	//allocate the left jobs
	int jID;
	int jmID;
	mID = 0;
	candidateVM = 0;
	
	for (int i = curIndex; i < p.pSequence.size(); i++)
	{
		// update resource when a job on the instance is finished
		sort(InsComTime.begin(), InsComTime.end(), SortComTime);

		jID = InsComTime.back().first;

		if (jVMs.count(jID) > 0)
		{
			candidateVM = jVMs.at(jID);
		}
	
		usedCPU[candidateVM] -= jData[jID].jCPU;
		usedMEM[candidateVM] -= jData[jID].jMEM;
		CurTime = InsComTime.back().second;
		InsComTime.pop_back();

		//if the available resource satisfy the demand of next job, add it
		bool paused = true;
		for (int j = 0; j < VMsdata.size(); j++)
		{
			candidateVM = (mID + j) % VMsdata.size();
			if ((VMsdata[candidateVM].vmCPU - jData[p.pSequence[i]].jCPU - usedCPU[candidateVM] >= 0) && (VMsdata[candidateVM].vmMEM - jData[p.pSequence[i]].jMEM - usedMEM[candidateVM] >= 0))
			{
				InsComTime.push_back(pair<int, int>({ p.pSequence[i],CurTime + jData[p.pSequence[i]].jTime }));
				jVMs.insert(pair<int, int>({ p.pSequence[i], candidateVM }));
				p.mPlacement[i] = candidateVM;
				usedCPU[candidateVM] += jData[p.pSequence[i]].jCPU;
				usedMEM[candidateVM] += jData[p.pSequence[i]].jMEM;
				jStartTime.push_back(CurTime);
				jobStartTime.push_back(pair<int, int>({ p.pSequence[i], CurTime }));
				jComTime.push_back(CurTime + jData[p.pSequence[i]].jTime);
				mID = (candidateVM + 1) % VMsdata.size();
				paused = false;
				break;
			}
		}
		if (paused)
		{
			i = i - 1;
		}
	}
	// calculate the total cost
	vector<vector<int>> vmList;
	vector<int> vm;
	double pCost = 0;
	for (int i = 0; i < VMsdata.size(); i++)
	{
		for (int j = 0; j < p.mPlacement.size(); j++)
		{
			if (VMsdata[i].vmID == p.mPlacement[j])
			{
				vm.push_back(jComTime[j]);
			}

		}
		vmList.push_back(vm);
		auto maxPosition = max_element(vm.begin(), vm.end());
		pCost = pCost + (*maxPosition) * VMsdata[i].vmPrice;
		vm.clear();
	}
	p.pCost = pCost;
	sort(jobStartTime.begin(), jobStartTime.end(), SortComTime);
}


bool QueueCompareJobsByRunningTime(JobInQueue u, JobInQueue v)
{
	if (u.e_time > v.e_time) {
		return true;
	}
	else {
		if (u.e_time < v.e_time) {
			return false;
		}
		return u.priority > v.priority;
	}
}

bool CompareJobsByStartTime(JobInQueue u, JobInQueue v)
{
	if (u.s_time < v.s_time) {
		return true;
	}
	else {
		if (u.s_time > v.s_time) {
			return false;
		}
		return u.priority < v.priority;
	}
}

bool CompareJobsByRunningTime(JobInQueue u, JobInQueue v)
{
	if (u.e_time < v.e_time) {
		return true;
	}
	else {
		if (u.e_time > v.e_time) {
			return false;
		}
		return u.priority < v.priority;
	}
}
/*FIFO*/
void GreedySortV1(const vector<JobData> jData, vector<VMData> VMsdata, PopInfo &p) {
	int p_starter = 0;

	vector<JobInQueue> job_queue; // {machine id, job id, finished time}
	vector<JobInQueue> res;

	vector<int> machine_exe_time;
	for (int i = 0; i < VMsdata.size(); i++) {
		machine_exe_time.push_back(0);
	}

	int max_s_time = 0;

	while (p_starter < p.pSequence.size()) {
		for (int i = 0; i < VMsdata.size(); i++) {
			//cout << machine_exe_time[i] << " ";
			machine_exe_time[i] = max_s_time;
		}

		vector<pair<int, JobData>> jobs_with_current_priority;
		// get job with the current priority
		for (int i = p_starter; i < p.pSequence.size(); i++) {
			if (jData[p.pSequence[i]].jPriority == jData[p.pSequence[p_starter]].jPriority) {
				jobs_with_current_priority.push_back(pair<int, JobData>{p.pSequence[i], jData[p.pSequence[i]]});
				p_starter = i;
			}
			else {
				break;
			}
		}
		p_starter += 1;
		// schedule
		while (!jobs_with_current_priority.empty()) {
			bool can_exe_one_job = false;
			int jid_tobe_exe = -1;
			for (int jid = 0; jid < jobs_with_current_priority.size(); jid++) {
				pair<int, JobData> crr_job = jobs_with_current_priority[jid];
				int real_jid = crr_job.first;
				JobData jdata = crr_job.second;
				for (int mid = 0; mid < VMsdata.size(); mid++) {
					VMData vm = VMsdata[mid];
					if ((vm.vmCPU - jdata.jCPU >= 0) && (vm.vmMEM - jdata.jMEM >= 0)) {
						VMsdata[mid].vmCPU -= jdata.jCPU;
						VMsdata[mid].vmMEM -= jdata.jMEM;
						JobInQueue job;
						job.jID = real_jid;
						job.mID = mid;
						job.e_time = machine_exe_time[mid] + jdata.jTime;
						job.s_time = machine_exe_time[mid];
						job.r_time = jdata.jTime;
						job.priority = jdata.jPriority;
						max_s_time = max(max_s_time, job.s_time);
						job_queue.push_back(job);
						can_exe_one_job = true;
						jid_tobe_exe = jid;
						break;
					}
				}
				if (can_exe_one_job) {
					break;
				}
			}
			if (can_exe_one_job) {
				// remove jid from jobs_with_current_priority
				jobs_with_current_priority.erase(jobs_with_current_priority.begin() + jid_tobe_exe);
				sort(job_queue.begin(), job_queue.end(), QueueCompareJobsByRunningTime);
				continue;
			}

			while (true && !job_queue.empty()) {
				int first_job_to_finish_etime = job_queue[job_queue.size() - 1].e_time;
				while (true && !job_queue.empty()) {
					if (abs(job_queue[job_queue.size() - 1].e_time - first_job_to_finish_etime) <= Diff) {
						res.push_back(job_queue[job_queue.size() - 1]);
						int mid = job_queue[job_queue.size() - 1].mID;
						int jid = job_queue[job_queue.size() - 1].jID;
						job_queue.pop_back();
						machine_exe_time[mid] = first_job_to_finish_etime;
						VMsdata[mid].vmCPU += jData[jid].jCPU;
						VMsdata[mid].vmMEM += jData[jid].jMEM;
					}
					else {
						break;
					}
				}

				bool can_exe_one_job = false;
				int jid_tobe_exe = -1;

				for (int jid = 0; jid < jobs_with_current_priority.size(); jid++) {
					pair<int, JobData> crr_job = jobs_with_current_priority[jid];
					int real_jid = crr_job.first;
					JobData jdata = crr_job.second;
					for (int mid = 0; mid < VMsdata.size(); mid++) {
						VMData vm = VMsdata[mid];
						if (vm.vmCPU - jdata.jCPU >= 0 && vm.vmMEM - jdata.jMEM >= 0) {
							VMsdata[mid].vmCPU -= jdata.jCPU;
							VMsdata[mid].vmMEM -= jdata.jMEM;
							JobInQueue job;
							job.jID = real_jid;
							job.mID = mid;
							job.e_time = machine_exe_time[mid] + jdata.jTime;
							job.s_time = machine_exe_time[mid];
							job.r_time = jdata.jTime;
							job.priority = jdata.jPriority;
							max_s_time = max(max_s_time, job.s_time);
							job_queue.push_back(job);
							can_exe_one_job = true;
							jid_tobe_exe = jid;
							break;
						}
					}
					if (can_exe_one_job) {
						break;
					}
				}
				if (can_exe_one_job) {
					// remove jid from jobs_with_current_priority
					jobs_with_current_priority.erase(jobs_with_current_priority.begin() + jid_tobe_exe);
					sort(job_queue.begin(), job_queue.end(), QueueCompareJobsByRunningTime);
					break;
				}
			}
		}
	}

	while (!job_queue.empty()) {
		res.push_back(job_queue[job_queue.size() - 1]);
		job_queue.pop_back();
	}

	//update pop information
	sort(res.begin(), res.end(), CompareJobsByStartTime);

	double TotalCost = 0;
	int end_time_in_machine = 0;
	for (int i = 0; i < VMsdata.size(); i++)
	{
		for (int j = 0; j < res.size(); j++)
		{
			if (VMsdata[i].vmID == res[j].mID)
			{
				end_time_in_machine = max(end_time_in_machine, res[j].e_time);
			}
		}
		//auto maxPosition = max_element(end_time_in_machine.begin(), end_time_in_machine.end());
		//cout << "GreedySortV1: " << "machine" << i << ":  time: " << end_time_in_machine << endl;
		TotalCost = TotalCost + end_time_in_machine * VMsdata[i].vmPrice;
	}

	stable_sort(res.begin(), res.end(), CompareJobsByStartTime);
	vector<int> jobs_placment;
	vector<int> true_sequence;
	for (int i = 0; i < res.size(); i++)
	{
		jobs_placment.push_back(res[i].mID);
		true_sequence.push_back(res[i].jID);
	}

	p.pCost = TotalCost;
	p.trueSequence = true_sequence;
	p.mPlacement = jobs_placment;
}


/*
Description:the jobs with lower priority could be executed when it doesn't influent the start time of the jobs with higher priority
when there are multiple machines are availble, choose random one
*/
void GreedySortV2(const vector<JobData> jData, vector<VMData> VMsdata, PopInfo& p) {
	int p_starter = 0;
	vector<JobInQueue> job_queue; // {machine id, job id, finished time}
	vector<JobInQueue> res;

	vector<int> machine_exe_time;
	for (int i = 0; i < VMsdata.size(); i++) {
		machine_exe_time.push_back(0);
	}

	while (p_starter < p.pSequence.size()) {
		vector<pair<int, JobData>> jobs_with_current_priority;
		// get job with the current priority
		for (int i = p_starter; i < p.pSequence.size(); i++) {
			if (jData[p.pSequence[i]].jPriority == jData[p.pSequence[p_starter]].jPriority) {
				jobs_with_current_priority.push_back(pair<int, JobData>{p.pSequence[i], jData[p.pSequence[i]]});
				p_starter = i;
			}
			else {
				break;
			}
		}
		p_starter += 1;
		// schedule
		while (!jobs_with_current_priority.empty()) {
			bool can_exe_one_job = false;
			int jid_tobe_exe = -1;
			for (int jid = 0; jid < jobs_with_current_priority.size(); jid++) {
				pair<int, JobData> crr_job = jobs_with_current_priority[jid];
				int real_jid = crr_job.first;
				JobData jdata = crr_job.second;
				for (int mid = 0; mid < VMsdata.size(); mid++) {
					VMData vm = VMsdata[mid];
					if (isGreaterThanOrEqual(vm.vmCPU, jdata.jCPU) && isGreaterThanOrEqual(vm.vmMEM, jdata.jMEM)) {
						VMsdata[mid].vmCPU -= jdata.jCPU;
						VMsdata[mid].vmMEM -= jdata.jMEM;
						JobInQueue job;
						job.jID = real_jid;
						job.mID = vm.vmID;
						job.e_time = machine_exe_time[mid] + jdata.jTime;
						job.s_time = machine_exe_time[mid];
						job.r_time = jdata.jTime;
						job.priority = jdata.jPriority;
						job_queue.push_back(job);
						can_exe_one_job = true;
						jid_tobe_exe = jid;
						break;
					}
				}
				if (can_exe_one_job) {
					break;
				}
			}
			if (can_exe_one_job) {
				// remove jid from jobs_with_current_priority
				jobs_with_current_priority.erase(jobs_with_current_priority.begin() + jid_tobe_exe);
				sort(job_queue.begin(), job_queue.end(), QueueCompareJobsByRunningTime);
				continue;
			}
			if (job_queue.empty()) {
				cout << "?" << endl;
			}
			while (true && !job_queue.empty()) {
				JobInQueue first_job_to_finish = job_queue[job_queue.size() - 1];
				while (true && !job_queue.empty()) {
					if (job_queue[job_queue.size() - 1].e_time - first_job_to_finish.e_time == 0) {
						res.push_back(job_queue[job_queue.size() - 1]);
						int mid = job_queue[job_queue.size() - 1].mID;
						int jid = job_queue[job_queue.size() - 1].jID;
						job_queue.pop_back();
						machine_exe_time[mid] = max(machine_exe_time[mid], first_job_to_finish.e_time);
						VMsdata[mid].vmCPU += jData[jid].jCPU;
						VMsdata[mid].vmMEM += jData[jid].jMEM;
					}
					else {
						break;
					}
				}

				bool can_exe_one_job = false;
				int jid_tobe_exe = -1;

				for (int jid = 0; jid < jobs_with_current_priority.size(); jid++) {
					pair<int, JobData> crr_job = jobs_with_current_priority[jid];
					int real_jid = crr_job.first;
					JobData jdata = crr_job.second;
					for (int mid = 0; mid < VMsdata.size(); mid++) {
						VMData vm = VMsdata[mid];
						if (isGreaterThanOrEqual(vm.vmCPU, jdata.jCPU) && isGreaterThanOrEqual(vm.vmMEM, jdata.jMEM)) {
							VMsdata[mid].vmCPU -= jdata.jCPU;
							VMsdata[mid].vmMEM -= jdata.jMEM;
							JobInQueue job;
							job.jID = real_jid;
							job.mID = vm.vmID;
							job.e_time = machine_exe_time[mid] + jdata.jTime;
							job.s_time = machine_exe_time[mid];
							job.r_time = jdata.jTime;
							job.priority = jdata.jPriority;
							job_queue.push_back(job);
							can_exe_one_job = true;
							jid_tobe_exe = jid;
							break;
						}
					}
					if (can_exe_one_job) {
						break;
					}
				}
				if (can_exe_one_job) {
					// remove jid from jobs_with_current_priority
					jobs_with_current_priority.erase(jobs_with_current_priority.begin() + jid_tobe_exe);
					sort(job_queue.begin(), job_queue.end(), QueueCompareJobsByRunningTime);
					break;
				}
			}
		}
	}

	while (!job_queue.empty()) {
		res.push_back(job_queue[job_queue.size() - 1]);
		job_queue.pop_back();
	}

	//update pop information
	//sort(res.begin(), res.end(), CompareJobsByRunningTime);

	stable_sort(res.begin(), res.end(), CompareJobsByStartTime);
	//vector<int> stime;
	//vector<int> etime;
	//vector<int> exetime;

	double TotalCost = 0;

	for (int i = 0; i < VMsdata.size(); i++)
	{
		int end_time_in_machine = 0;
		for (int j = 0; j < res.size(); j++)
		{
			if (VMsdata[i].vmID == res[j].mID)
			{
				end_time_in_machine = max(end_time_in_machine, res[j].e_time);
				//stime.push_back(res[j].s_time);
				//etime.push_back(res[j].e_time);
				//exetime.push_back(res[j].r_time);
			}
		}
		TotalCost = TotalCost + end_time_in_machine * VMsdata[i].vmPrice;
		//cout << "GreedySortV2: " << "machine" << i << ":  time: " << end_time_in_machine << endl;
		//stime.clear();
		//etime.clear();
		//exetime.clear();
	}

	//stable_sort(res.begin(), res.end(), CompareJobsByStartTime);

	vector<int> jobs_placment;
	vector<int> true_sequence;
	for (int i = 0; i < res.size(); i++)
	{
		jobs_placment.push_back(res[i].mID);
		true_sequence.push_back(res[i].jID);
	}

	p.pCost = TotalCost;
	p.trueSequence = true_sequence;
	p.mPlacement = jobs_placment;
}

vector<VMData> SortMachineByPriority(vector<VMData> VMsdata, int type) {
	if (type == -1) {
		// Sort by ID
		sort(VMsdata.begin(), VMsdata.end(), [](VMData u, VMData v) {
			return u.vmID < v.vmID;
		});
	}
	if (type == 0) {
		// Sort by Price
		sort(VMsdata.begin(), VMsdata.end(), [](VMData u, VMData v) {
			return u.vmPrice < v.vmPrice;
		});
	}
	else {
		if (type == 1) {
			// Sort by CPU
			sort(VMsdata.begin(), VMsdata.end(), [](VMData u, VMData v) {
				if (u.vmCPU < v.vmCPU) {
					return true;
				}
				else {
					if (u.vmCPU > v.vmCPU) {
						return false;
					}
					else {
						return u.vmMEM < v.vmMEM;
					}
				}
			});
		}
		else {
			if (type == 2) {
				// Sort by Mem
				sort(VMsdata.begin(), VMsdata.end(), [](VMData u, VMData v) {
					if (u.vmMEM < v.vmMEM) {
						return true;
					}
					else {
						if (u.vmMEM > v.vmMEM) {
							return false;
						}
						else {
							return u.vmCPU < v.vmCPU;
						}
					}
				});
			}
		}
	}
	return VMsdata;
}

vector<pair<VMData, int>> SortMachineByPriorityWithIndex(vector<pair<VMData, int>> VMsdata, int type) {
	if (type == -1) {
		// Sort by ID
		sort(VMsdata.begin(), VMsdata.end(), [](pair<VMData, int> u, pair<VMData, int> v) {
			return u.first.vmID < v.first.vmID;
		});
	}
	if (type == 0) {
		// Sort by Price
		sort(VMsdata.begin(), VMsdata.end(), [](pair<VMData, int> u, pair<VMData, int> v) {
			return u.first.vmPrice < v.first.vmPrice;
		});
	}
	else {
		if (type == 1) {
			// Sort by CPU
			sort(VMsdata.begin(), VMsdata.end(), [](pair<VMData, int> u, pair<VMData, int> v) {
				if (u.first.vmCPU < v.first.vmCPU) {
					return true;
				}
				else {
					if (u.first.vmCPU > v.first.vmCPU) {
						return false;
					}
					else {
						return u.first.vmMEM < v.first.vmMEM;
					}
				}
			});
		}
		else {
			if (type == 2) {
				// Sort by Mem
				sort(VMsdata.begin(), VMsdata.end(), [](pair<VMData, int> u, pair<VMData, int> v) {
					if (u.first.vmMEM < v.first.vmMEM) {
						return true;
					}
					else {
						if (u.first.vmMEM > v.first.vmMEM) {
							return false;
						}
						else {
							return u.first.vmCPU < v.first.vmCPU;
						}
					}
				});
			}
		}
	}
	return VMsdata;
}
/*
Description: when there are multiple machines are availble, choose cheapest one
*/
void GreedySortV3(const vector<JobData> jData, vector<VMData> VMsdata, PopInfo& p) {
	// Price-oriented Machine Selection
	int type = 0;
	VMsdata = SortMachineByPriority(VMsdata, type);

	int p_starter = 0;
	vector<JobInQueue> job_queue; // {machine id, job id, finished time}
	vector<JobInQueue> res;
	vector<int> machine_exe_time;
	for (int i = 0; i < VMsdata.size(); i++) {
		machine_exe_time.push_back(0);
	}

	while (p_starter < p.pSequence.size()) {
		vector<pair<int, JobData>> jobs_with_current_priority;
		// get job with the current priority
		for (int i = p_starter; i < p.pSequence.size(); i++) {
			if (jData[p.pSequence[i]].jPriority == jData[p.pSequence[p_starter]].jPriority) {
				jobs_with_current_priority.push_back(pair<int, JobData>{p.pSequence[i], jData[p.pSequence[i]]});
				p_starter = i;
			}
			else {
				break;
			}
		}
		p_starter += 1;
		// schedule
		while (!jobs_with_current_priority.empty()) {
			bool can_exe_one_job = false;
			int jid_tobe_exe = -1;
			for (int jid = 0; jid < jobs_with_current_priority.size(); jid++) {
				pair<int, JobData> crr_job = jobs_with_current_priority[jid];
				int real_jid = crr_job.first;
				JobData jdata = crr_job.second;
				for (int mid = 0; mid < VMsdata.size(); mid++) {
					VMData vm = VMsdata[mid];
					if ((vm.vmCPU - jdata.jCPU >= 0) && (vm.vmMEM - jdata.jMEM >= 0)) {
						VMsdata[mid].vmCPU -= jdata.jCPU;
						VMsdata[mid].vmMEM -= jdata.jMEM;
						JobInQueue job;
						job.jID = real_jid;
						job.mID = mid;
						job.e_time = machine_exe_time[mid] + jdata.jTime;
						job.s_time = machine_exe_time[mid];
						job.r_time = jdata.jTime;
						job.priority = jdata.jPriority;
						job_queue.push_back(job);
						can_exe_one_job = true;
						jid_tobe_exe = jid;
						break;
					}
				}
				if (can_exe_one_job) {
					break;
				}
			}
			if (can_exe_one_job) {
				// remove jid from jobs_with_current_priority
				jobs_with_current_priority.erase(jobs_with_current_priority.begin() + jid_tobe_exe);
				sort(job_queue.begin(), job_queue.end(), QueueCompareJobsByRunningTime);
				continue;
			}
			if (job_queue.empty()) {
				cout << "?" << endl;
			}
			while (true && !job_queue.empty()) {
				JobInQueue first_job_to_finish = job_queue[job_queue.size() - 1];
				while (true && !job_queue.empty()) {
					if (abs(job_queue[job_queue.size() - 1].e_time - first_job_to_finish.e_time) <= Diff) {
						res.push_back(job_queue[job_queue.size() - 1]);
						int mid = job_queue[job_queue.size() - 1].mID;
						int jid = job_queue[job_queue.size() - 1].jID;
						job_queue.pop_back();
						machine_exe_time[mid] = max(machine_exe_time[mid], first_job_to_finish.e_time);
						VMsdata[mid].vmCPU += jData[jid].jCPU;
						VMsdata[mid].vmMEM += jData[jid].jMEM;
					}
					else {
						break;
					}
				}

				bool can_exe_one_job = false;
				int jid_tobe_exe = -1;

				for (int jid = 0; jid < jobs_with_current_priority.size(); jid++) {
					pair<int, JobData> crr_job = jobs_with_current_priority[jid];
					int real_jid = crr_job.first;
					JobData jdata = crr_job.second;
					for (int mid = 0; mid < VMsdata.size(); mid++) {
						VMData vm = VMsdata[mid];
						if ((vm.vmCPU - jdata.jCPU >= 0) && (vm.vmMEM - jdata.jMEM >= 0)) {
							VMsdata[mid].vmCPU -= jdata.jCPU;
							VMsdata[mid].vmMEM -= jdata.jMEM;
							JobInQueue job;
							job.jID = real_jid;
							job.mID = mid;
							job.e_time = machine_exe_time[mid] + jdata.jTime;
							job.s_time = machine_exe_time[mid];
							job.r_time = jdata.jTime;
							job.priority = jdata.jPriority;
							job_queue.push_back(job);
							can_exe_one_job = true;
							jid_tobe_exe = jid;
							break;
						}
					}
					if (can_exe_one_job) {
						break;
					}
				}
				if (can_exe_one_job) {
					// remove jid from jobs_with_current_priority
					jobs_with_current_priority.erase(jobs_with_current_priority.begin() + jid_tobe_exe);
					sort(job_queue.begin(), job_queue.end(), QueueCompareJobsByRunningTime);
					break;
				}
			}
		}
	}

	while (!job_queue.empty()) {
		res.push_back(job_queue[job_queue.size() - 1]);
		job_queue.pop_back();
	}

	//update pop information
	sort(res.begin(), res.end(), CompareJobsByRunningTime);
	//stable_sort(res.begin(), res.end(), CompareJobsByStartTime);
	double TotalCost = 0;
	vector<int> end_time_in_machine;

	for (int i = 0; i < VMsdata.size(); i++)
	{
		for (int j = 0; j < res.size(); j++)
		{
			if (i == res[j].mID)
			{
				end_time_in_machine.push_back(res[j].e_time);
			}
		}
		auto maxPosition = max_element(end_time_in_machine.begin(), end_time_in_machine.end());
		TotalCost = TotalCost + (*maxPosition) * VMsdata[i].vmPrice;
		//cout << "GreedySortV3: " << "machine" << VMsdata[i].vmID << ":  time: " << *maxPosition << endl;
		end_time_in_machine.clear();
	}

	stable_sort(res.begin(), res.end(), CompareJobsByStartTime);
	vector<int> jobs_placment;
	vector<int> true_sequence;
	for (int i = 0; i < res.size(); i++)
	{
		jobs_placment.push_back(VMsdata[res[i].mID].vmID);
		true_sequence.push_back(res[i].jID);
	}

	p.pCost = TotalCost;
	p.trueSequence = true_sequence;
	p.mPlacement = jobs_placment;
}

void GreedySortV4(const vector<JobData> jData, vector<VMData> VMsdata, PopInfo& p) {
	/*
	Resource - oriented Machine Selection
	type 1: CPU
	type 2: MEM
	*/
	int type = 1;
	int p_starter = 0;
	vector<JobInQueue> job_queue; // {machine id, job id, finished time}
	vector<JobInQueue> res;
	vector<int> machine_exe_time;
	vector<pair<VMData, int>> vm_idx;
	for (int i = 0; i < VMsdata.size(); i++) {
		machine_exe_time.push_back(0);
		vm_idx.push_back(pair<VMData, int> {VMsdata[i], i});
	}
	vm_idx = SortMachineByPriorityWithIndex(vm_idx, type);

	while (p_starter < p.pSequence.size()) {
		vector<pair<int, JobData>> jobs_with_current_priority;
		// get job with the current priority
		for (int i = p_starter; i < p.pSequence.size(); i++) {
			if (jData[p.pSequence[i]].jPriority == jData[p.pSequence[p_starter]].jPriority) {
				jobs_with_current_priority.push_back(pair<int, JobData>{p.pSequence[i], jData[p.pSequence[i]]});
				p_starter = i;
			}
			else {
				break;
			}
		}
		p_starter += 1;
		// schedule
		while (!jobs_with_current_priority.empty()) {
			bool can_exe_one_job = false;
			int jid_tobe_exe = -1;
			for (int jid = 0; jid < jobs_with_current_priority.size(); jid++) {
				pair<int, JobData> crr_job = jobs_with_current_priority[jid];
				int real_jid = crr_job.first;
				JobData jdata = crr_job.second;
				for (int mid = 0; mid < vm_idx.size(); mid++) {
					int real_mid = vm_idx[mid].second;
					VMData vm = VMsdata[real_mid];
					if ((vm.vmCPU - jdata.jCPU >= 0) && (vm.vmMEM - jdata.jMEM >= 0)) {
						VMsdata[real_mid].vmCPU -= jdata.jCPU;
						VMsdata[real_mid].vmMEM -= jdata.jMEM;
						vm_idx[mid].first.vmCPU -= jdata.jCPU;
						vm_idx[mid].first.vmMEM -= jdata.jMEM;
						JobInQueue job;
						job.jID = real_jid;
						job.mID = vm.vmID;
						job.e_time = machine_exe_time[real_mid] + jdata.jTime;
						job.s_time = machine_exe_time[real_mid];
						job.r_time = jdata.jTime;
						job.priority = jdata.jPriority;
						job_queue.push_back(job);
						can_exe_one_job = true;
						jid_tobe_exe = jid;
						break;
					}
				}
				if (can_exe_one_job) {
					break;
				}
			}
			if (can_exe_one_job) {
				// remove jid from jobs_with_current_priority
				jobs_with_current_priority.erase(jobs_with_current_priority.begin() + jid_tobe_exe);
				sort(job_queue.begin(), job_queue.end(), QueueCompareJobsByRunningTime);
				vm_idx = SortMachineByPriorityWithIndex(vm_idx, type);
				continue;
			}
			if (job_queue.empty()) {
				cout << "?" << endl;
			}
			while (true && !job_queue.empty()) {
				JobInQueue first_job_to_finish = job_queue[job_queue.size() - 1];
				while (true && !job_queue.empty()) {
					if (abs(job_queue[job_queue.size() - 1].e_time - first_job_to_finish.e_time) <= Diff) {
						res.push_back(job_queue[job_queue.size() - 1]);
						int mid = job_queue[job_queue.size() - 1].mID;
						int jid = job_queue[job_queue.size() - 1].jID;
						job_queue.pop_back();
						machine_exe_time[mid] = max(machine_exe_time[mid], first_job_to_finish.e_time);
						VMsdata[mid].vmCPU += jData[jid].jCPU;
						VMsdata[mid].vmMEM += jData[jid].jMEM;
						for (int i = 0; i < vm_idx.size(); i++) {
							if (vm_idx[i].second == mid) {
								vm_idx[i].first.vmCPU += jData[jid].jCPU;
								vm_idx[i].first.vmMEM += jData[jid].jMEM;
								break;
							}
						}
					}
					else {
						break;
					}
				}

				bool can_exe_one_job = false;
				int jid_tobe_exe = -1;

				for (int jid = 0; jid < jobs_with_current_priority.size(); jid++) {
					pair<int, JobData> crr_job = jobs_with_current_priority[jid];
					int real_jid = crr_job.first;
					JobData jdata = crr_job.second;
					for (int mid = 0; mid < vm_idx.size(); mid++) {
						int real_mid = vm_idx[mid].second;
						VMData vm = VMsdata[real_mid];
						if ((vm.vmCPU - jdata.jCPU >= 0) && (vm.vmMEM - jdata.jMEM >= 0)) {
							VMsdata[real_mid].vmCPU -= jdata.jCPU;
							VMsdata[real_mid].vmMEM -= jdata.jMEM;
							vm_idx[mid].first.vmCPU -= jdata.jCPU;
							vm_idx[mid].first.vmMEM -= jdata.jMEM;
							JobInQueue job;
							job.jID = real_jid;
							job.mID = vm.vmID;
							job.e_time = machine_exe_time[real_mid] + jdata.jTime;
							job.s_time = machine_exe_time[real_mid];
							job.r_time = jdata.jTime;
							job.priority = jdata.jPriority;
							job_queue.push_back(job);
							can_exe_one_job = true;
							jid_tobe_exe = jid;
							break;
						}
					}
					if (can_exe_one_job) {
						break;
					}
				}
				if (can_exe_one_job) {
					// remove jid from jobs_with_current_priority
					jobs_with_current_priority.erase(jobs_with_current_priority.begin() + jid_tobe_exe);
					sort(job_queue.begin(), job_queue.end(), QueueCompareJobsByRunningTime);
					vm_idx = SortMachineByPriorityWithIndex(vm_idx, type);
					break;
				}
			}
		}
	}

	while (!job_queue.empty()) {
		res.push_back(job_queue[job_queue.size() - 1]);
		job_queue.pop_back();
	}

	//update pop information
	//sort(res.begin(), res.end(), CompareJobsByRunningTime);
	stable_sort(res.begin(), res.end(), CompareJobsByStartTime);
	double TotalCost = 0;
	vector<int> end_time_in_machine;

	vector<int> stime;
	vector<int> jid;


	VMsdata = SortMachineByPriority(VMsdata, -1);
	for (int i = 0; i < VMsdata.size(); i++)
	{
		for (int j = 0; j < res.size(); j++)
		{
			if (VMsdata[i].vmID == res[j].mID)
			{
				end_time_in_machine.push_back(res[j].e_time);
				stime.push_back(res[j].s_time);
				jid.push_back(res[j].jID);
			}
		}
		auto maxPosition = max_element(end_time_in_machine.begin(), end_time_in_machine.end());
		TotalCost = TotalCost + (*maxPosition) * VMsdata[i].vmPrice;
		//cout << "GreedySortV4: " << "machine" << i << ":  time: " << *maxPosition << endl;
		end_time_in_machine.clear();
		stime.clear();
		jid.clear();
	}

	stable_sort(res.begin(), res.end(), CompareJobsByStartTime);
	/*for (int i = 0; i < res.size(); i++)
	{
		cout << res[i].priority << "," << endl;
	}*/
	vector<int> jobs_placment;
	vector<int> true_sequence;
	for (int i = 0; i < res.size(); i++)
	{
		jobs_placment.push_back(res[i].mID);
		true_sequence.push_back(res[i].jID);
	}

	p.pCost = TotalCost;
	p.trueSequence = true_sequence;
	p.mPlacement = jobs_placment;
}

void GreedySortV5(const vector<JobData> jData, vector<VMData> VMsdata, PopInfo& p) {
	/*
	Resource - oriented Machine Selection
	type 1: CPU
	type 2: MEM
	*/
	int type = 2;
	int p_starter = 0;
	vector<JobInQueue> job_queue; // {machine id, job id, finished time}
	vector<JobInQueue> res;
	vector<int> machine_exe_time;
	vector<pair<VMData, int>> vm_idx;
	for (int i = 0; i < VMsdata.size(); i++) {
		machine_exe_time.push_back(0);
		vm_idx.push_back(pair<VMData, int> {VMsdata[i], i});
	}
	vm_idx = SortMachineByPriorityWithIndex(vm_idx, type);

	while (p_starter < p.pSequence.size()) {
		vector<pair<int, JobData>> jobs_with_current_priority;
		// get job with the current priority
		for (int i = p_starter; i < p.pSequence.size(); i++) {
			if (jData[p.pSequence[i]].jPriority == jData[p.pSequence[p_starter]].jPriority) {
				jobs_with_current_priority.push_back(pair<int, JobData>{p.pSequence[i], jData[p.pSequence[i]]});
				p_starter = i;
			}
			else {
				break;
			}
		}
		p_starter += 1;
		// schedule
		while (!jobs_with_current_priority.empty()) {
			bool can_exe_one_job = false;
			int jid_tobe_exe = -1;
			for (int jid = 0; jid < jobs_with_current_priority.size(); jid++) {
				pair<int, JobData> crr_job = jobs_with_current_priority[jid];
				int real_jid = crr_job.first;
				JobData jdata = crr_job.second;
				for (int mid = 0; mid < vm_idx.size(); mid++) {
					int real_mid = vm_idx[mid].second;
					VMData vm = VMsdata[real_mid];
					if ((vm.vmCPU - jdata.jCPU >= 0) && (vm.vmMEM - jdata.jMEM >= 0)) {
						VMsdata[real_mid].vmCPU -= jdata.jCPU;
						VMsdata[real_mid].vmMEM -= jdata.jMEM;
						vm_idx[mid].first.vmCPU -= jdata.jCPU;
						vm_idx[mid].first.vmMEM -= jdata.jMEM;
						JobInQueue job;
						job.jID = real_jid;
						job.mID = vm.vmID;
						job.e_time = machine_exe_time[real_mid] + jdata.jTime;
						job.s_time = machine_exe_time[real_mid];
						job.r_time = jdata.jTime;
						job.priority = jdata.jPriority;
						job_queue.push_back(job);
						can_exe_one_job = true;
						jid_tobe_exe = jid;
						break;
					}
				}
				if (can_exe_one_job) {
					break;
				}
			}
			if (can_exe_one_job) {
				// remove jid from jobs_with_current_priority
				jobs_with_current_priority.erase(jobs_with_current_priority.begin() + jid_tobe_exe);
				sort(job_queue.begin(), job_queue.end(), QueueCompareJobsByRunningTime);
				vm_idx = SortMachineByPriorityWithIndex(vm_idx, type);
				continue;
			}
			if (job_queue.empty()) {
				cout << "?" << endl;
			}
			while (true && !job_queue.empty()) {
				JobInQueue first_job_to_finish = job_queue[job_queue.size() - 1];
				while (true && !job_queue.empty()) {
					if (abs(job_queue[job_queue.size() - 1].e_time - first_job_to_finish.e_time) <= Diff) {
						res.push_back(job_queue[job_queue.size() - 1]);
						int mid = job_queue[job_queue.size() - 1].mID;
						int jid = job_queue[job_queue.size() - 1].jID;
						job_queue.pop_back();
						machine_exe_time[mid] = max(machine_exe_time[mid], first_job_to_finish.e_time);
						VMsdata[mid].vmCPU += jData[jid].jCPU;
						VMsdata[mid].vmMEM += jData[jid].jMEM;
						for (int i = 0; i < vm_idx.size(); i++) {
							if (vm_idx[i].second == mid) {
								vm_idx[i].first.vmCPU += jData[jid].jCPU;
								vm_idx[i].first.vmMEM += jData[jid].jMEM;
								break;
							}
						}
					}
					else {
						break;
					}
				}

				bool can_exe_one_job = false;
				int jid_tobe_exe = -1;

				for (int jid = 0; jid < jobs_with_current_priority.size(); jid++) {
					pair<int, JobData> crr_job = jobs_with_current_priority[jid];
					int real_jid = crr_job.first;
					JobData jdata = crr_job.second;
					for (int mid = 0; mid < vm_idx.size(); mid++) {
						int real_mid = vm_idx[mid].second;
						VMData vm = VMsdata[real_mid];
						if ((vm.vmCPU - jdata.jCPU >= 0) && (vm.vmMEM - jdata.jMEM >= 0)) {
							VMsdata[real_mid].vmCPU -= jdata.jCPU;
							VMsdata[real_mid].vmMEM -= jdata.jMEM;
							vm_idx[mid].first.vmCPU -= jdata.jCPU;
							vm_idx[mid].first.vmMEM -= jdata.jMEM;
							JobInQueue job;
							job.jID = real_jid;
							job.mID = vm.vmID;
							job.e_time = machine_exe_time[real_mid] + jdata.jTime;
							job.s_time = machine_exe_time[real_mid];
							job.r_time = jdata.jTime;
							job.priority = jdata.jPriority;
							job_queue.push_back(job);
							can_exe_one_job = true;
							jid_tobe_exe = jid;
							break;
						}
					}
					if (can_exe_one_job) {
						break;
					}
				}
				if (can_exe_one_job) {
					// remove jid from jobs_with_current_priority
					jobs_with_current_priority.erase(jobs_with_current_priority.begin() + jid_tobe_exe);
					sort(job_queue.begin(), job_queue.end(), QueueCompareJobsByRunningTime);
					vm_idx = SortMachineByPriorityWithIndex(vm_idx, type);
					break;
				}
			}
		}
	}

	while (!job_queue.empty()) {
		res.push_back(job_queue[job_queue.size() - 1]);
		job_queue.pop_back();
	}

	//update pop information
	//sort(res.begin(), res.end(), CompareJobsByRunningTime);
	stable_sort(res.begin(), res.end(), CompareJobsByStartTime);
	double TotalCost = 0;
	vector<int> end_time_in_machine;

	vector<int> stime;
	vector<int> jid;


	VMsdata = SortMachineByPriority(VMsdata, -1);
	for (int i = 0; i < VMsdata.size(); i++)
	{
		for (int j = 0; j < res.size(); j++)
		{
			if (VMsdata[i].vmID == res[j].mID)
			{
				end_time_in_machine.push_back(res[j].e_time);
				stime.push_back(res[j].s_time);
				jid.push_back(res[j].jID);
			}
		}
		auto maxPosition = max_element(end_time_in_machine.begin(), end_time_in_machine.end());
		TotalCost = TotalCost + (*maxPosition) * VMsdata[i].vmPrice;
		//cout << "GreedySortV5: " << "machine" << i << ":  time: " << *maxPosition << endl;
		end_time_in_machine.clear();
		stime.clear();
		jid.clear();
	}

	stable_sort(res.begin(), res.end(), CompareJobsByStartTime);
	/*for (int i = 0; i < res.size(); i++)
	{
		cout << res[i].priority << "," << endl;
	}*/
	vector<int> jobs_placment;
	vector<int> true_sequence;
	for (int i = 0; i < res.size(); i++)
	{
		jobs_placment.push_back(res[i].mID);
		true_sequence.push_back(res[i].jID);
	}

	p.pCost = TotalCost;
	p.trueSequence = true_sequence;
	p.mPlacement = jobs_placment;
}

//-----------------------------
bool sortResGap(pair<int, double> ReGap1, pair<int, double> ReGap2)
{
	return ReGap1.second < ReGap2.second;
}

int findSmallGapJob(vector<JobData> leftJ, double MaxCPU, double MaxMEM)
{
	vector<pair<int, double>> ReGapJ;
	vector<int> priSet;
	//double diff;
	for (int i = 0; i < leftJ.size(); i++)
	{
		priSet.push_back(leftJ[i].jPriority);
	}
	auto minPosition = min_element(priSet.begin(), priSet.end());

	for (int i = 0; i < leftJ.size(); i++)
	{
		if (leftJ[i].jPriority == *minPosition)
		{
			ReGapJ.push_back(pair<int, double>({ i, (MaxCPU - leftJ[i].jCPU) + (MaxMEM - leftJ[i].jMEM) }));
		}
	}
	return ReGapJ.begin()->first;	
}

void resGapSort(const vector<JobData> jData, const vector<VMData> VMsdata, PopInfo &p)
{
	//---------------------job information---------------------------------
	vector<int> newProSqe;                  //true processing sequence of each job
	vector<int> newPlacement;
	vector<int> jStartTime;                 //start execution time of each job
	vector<int> jComTime;                   //completion time of each job
	vector<pair<int, int>> InsComTime;      //processing job list
	map<int, int> jVMs;                     //placement of each job on each machine

	//---------reset placement and negative means hasn't been allocated--------
	p.mPlacement.clear();
	for (int i = 0; i < p.pSequence.size(); i++)
	{
		p.mPlacement.push_back(-1);
		//jStartTime.push_back(0);
		//jComTime.push_back(0);
	}
	//---------------------machine information---------------------------------
	vector<double> MaxCPU;
	vector<double> MaxMEM;
	vector<double> usedCPU;
	vector<double> usedMEM;
	for (int i = 0; i < VMsdata.size(); i++)
	{
		MaxCPU.push_back(VMsdata[i].vmCPU);
		MaxMEM.push_back(VMsdata[i].vmMEM);
		usedCPU.push_back(0);
		usedMEM.push_back(0);
	}
	
	// allocate jobs at time 0 
	int CurTime = 0;
	int mID = 0;
	int candidateVM;
	int curIndex = 0;

	vector<JobData> leftJ;
	for (int i = 0; i < p.pSequence.size(); i++)
	{
		bool paused = true;
		for (int j = 0; j < VMsdata.size(); j++)
		{
			candidateVM = (mID + j) % VMsdata.size();
			if (abs(VMsdata[candidateVM].vmCPU - jData[p.pSequence[i]].jCPU - usedCPU[candidateVM]) <= Diff)
			{
				newProSqe.push_back(p.pSequence[i]);
				newPlacement.push_back(candidateVM);
				InsComTime.push_back(pair<int, int>({ p.pSequence[i],CurTime + jData[p.pSequence[i]].jTime }));
				jVMs.insert(pair<int, int>({ p.pSequence[i], candidateVM }));
				p.mPlacement[i] = candidateVM;
				usedCPU[candidateVM] += jData[p.pSequence[i]].jCPU;
				usedMEM[candidateVM] += jData[p.pSequence[i]].jMEM;
				MaxCPU[candidateVM] -= jData[p.pSequence[i]].jCPU;
				MaxMEM[candidateVM] -= jData[p.pSequence[i]].jMEM;
				jStartTime.push_back(CurTime);
				jComTime.push_back(CurTime + jData[p.pSequence[i]].jTime);
				//jStartTime[i] = CurTime;
				//jComTime[i] = CurTime + jData[p.pSequence[i]].jTime;
				mID = (candidateVM + 1) % VMsdata.size();
				paused = false;
				break;
			}
		}
		if (paused)
		{
			leftJ.push_back(jData[p.pSequence[i]]);
		}
	}

	//allocate the left jobs by gap
	int jID;
	int jmID;
	mID = 0;
	candidateVM = 0;

	while (leftJ.size() > 0)
	{
		// update resource when a job on the instance is finished
		sort(InsComTime.begin(), InsComTime.end(), SortComTime);
		jID = InsComTime.back().first;
		if (jVMs.count(jID) > 0)
		{
			candidateVM = jVMs.at(jID);
		}
		usedCPU[candidateVM] -= jData[jID].jCPU;
		usedMEM[candidateVM] -= jData[jID].jMEM;
		CurTime = InsComTime.back().second;
		InsComTime.pop_back();
		//if the available resource satisfy the demand of next job, add it
		bool paused = true;
		//candidateVM = (mID + j) % VMsdata.size();
		//find the suitable job in the waiting list 
		int index = findSmallGapJob(leftJ, MaxCPU[candidateVM], MaxMEM[candidateVM]);
		if (abs(VMsdata[candidateVM].vmCPU - leftJ[index].jCPU - usedCPU[candidateVM]) <= Diff)
		{
			newProSqe.push_back(leftJ[index].jID);
			newPlacement.push_back(candidateVM);
			InsComTime.push_back(pair<int, int>({ leftJ[index].jID,CurTime + leftJ[index].jTime }));
			jVMs.insert(pair<int, int>({ leftJ[index].jID, candidateVM }));
			//p.mPlacement[i] = candidateVM;
			usedCPU[candidateVM] += leftJ[index].jCPU;
			usedMEM[candidateVM] += leftJ[index].jMEM;
			MaxCPU[candidateVM] -= leftJ[index].jCPU;
			MaxMEM[candidateVM] -= leftJ[index].jMEM;
			jStartTime.push_back(CurTime);
			jComTime.push_back(CurTime + leftJ[index].jTime);
			//jStartTime[index] = CurTime;
			//jComTime[index] = CurTime + leftJ[index].jTime;
			//mID = (candidateVM + 1) % VMsdata.size();
			leftJ.erase(std::begin(leftJ) + index);
		}
		if (leftJ.size() == 0)
		{
			break;
		}
	}

	// calculate the total cost
	vector<vector<int>> vmList;
	vector<int> vm;
	double pCost = 0;
	for (int i = 0; i < VMsdata.size(); i++)
	{
		for (int j = 0; j < newPlacement.size(); j++)
		{
			if (VMsdata[i].vmID == newPlacement[j])
			{
				vm.push_back(jComTime[j]);
			}

		}
		vmList.push_back(vm);
		auto maxPosition = max_element(vm.begin(), vm.end());
		pCost = pCost + (*maxPosition) * VMsdata[i].vmPrice;
		vm.clear();
	}

	//update true sequence and VM placement
	p.pCost = pCost;
	p.pSequence = newProSqe;
	p.mPlacement = newPlacement;
	
}

vector<JobInQueue> singleMachineSort(const vector<JobData> jData, vector<int> pSequence, double MaxCPU, double MaxMEM) {
	double leftCPU = MaxCPU;
	double leftMEM = MaxMEM;
	// find better sequence
	int p_starter = 0;
	vector<JobInQueue> job_queue; // {machine id, job id, finished time}
	vector<JobInQueue> res;
	int machine_exe_time = 0;
	while (p_starter < pSequence.size()) {
		vector<pair<int, JobData>> jobs_with_current_priority;
		// get job with the current priority
		for (int i = p_starter; i < pSequence.size(); i++) {
			if (jData[pSequence[i]].jPriority == jData[pSequence[p_starter]].jPriority) {
				jobs_with_current_priority.push_back(pair<int, JobData>{pSequence[i], jData[pSequence[i]]});
				p_starter = i;
			}
			else {
				break;
			}
		}
		p_starter += 1;
		// schedule
		while (!jobs_with_current_priority.empty()) {
			bool can_exe_one_job = false;
			int jid_tobe_exe = -1;
			for (int jid = 0; jid < jobs_with_current_priority.size(); jid++) {
				pair<int, JobData> crr_job = jobs_with_current_priority[jid];
				int real_jid = crr_job.first;
				JobData jdata = crr_job.second;
				if ((leftCPU - jdata.jCPU >= 0) && (leftMEM - jdata.jMEM >= 0)) {
					leftCPU -= jdata.jCPU;
					leftMEM -= jdata.jMEM;
					JobInQueue job;
					job.jID = real_jid;
					job.mID = -1;
					job.e_time = machine_exe_time + jdata.jTime;
					job.s_time = machine_exe_time;
					job.r_time = jdata.jTime;
					job.priority = jdata.jPriority;
					job_queue.push_back(job);
					can_exe_one_job = true;
					jid_tobe_exe = jid;
					break;
				}
				if (can_exe_one_job) {
					break;
				}
			}
			if (can_exe_one_job) {
				// remove jid from jobs_with_current_priority
				jobs_with_current_priority.erase(jobs_with_current_priority.begin() + jid_tobe_exe);
				sort(job_queue.begin(), job_queue.end(), QueueCompareJobsByRunningTime);
				continue;
			}
			while (true && !job_queue.empty()) {
				JobInQueue first_job_to_finish = job_queue[job_queue.size() - 1];
				while (true && !job_queue.empty()) {
					if (abs(job_queue[job_queue.size() - 1].e_time - first_job_to_finish.e_time) <= Diff) {
						res.push_back(job_queue[job_queue.size() - 1]);
						int jid = job_queue[job_queue.size() - 1].jID;
						job_queue.pop_back();
						machine_exe_time = max(machine_exe_time, first_job_to_finish.e_time);
						leftCPU += jData[jid].jCPU;
						leftMEM += jData[jid].jMEM;
					}
					else {
						break;
					}
				}

				bool can_exe_one_job = false;
				int jid_tobe_exe = -1;

				for (int jid = 0; jid < jobs_with_current_priority.size(); jid++) {
					pair<int, JobData> crr_job = jobs_with_current_priority[jid];
					int real_jid = crr_job.first;
					JobData jdata = crr_job.second;
					if ((leftCPU - jdata.jCPU >= 0) && (leftMEM - jdata.jMEM >= 0)) {
						leftCPU -= jdata.jCPU;
						leftMEM -= jdata.jMEM;
						JobInQueue job;
						job.jID = real_jid;
						job.mID = -1;
						job.e_time = machine_exe_time + jdata.jTime;
						job.s_time = machine_exe_time;
						job.r_time = jdata.jTime;
						job.priority = jdata.jPriority;
						job_queue.push_back(job);
						can_exe_one_job = true;
						jid_tobe_exe = jid;
						break;
					}
					if (can_exe_one_job) {
						break;
					}
				}
				if (can_exe_one_job) {
					// remove jid from jobs_with_current_priority
					jobs_with_current_priority.erase(jobs_with_current_priority.begin() + jid_tobe_exe);
					sort(job_queue.begin(), job_queue.end(), QueueCompareJobsByRunningTime);
					break;
				}
			}
		}
	}
	while (!job_queue.empty()) {
		res.push_back(job_queue[job_queue.size() - 1]);
		job_queue.pop_back();
	}
	sort(res.begin(), res.end(), CompareJobsByRunningTime);
	return res;
}

