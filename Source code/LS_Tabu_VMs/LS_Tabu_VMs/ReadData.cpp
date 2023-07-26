#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "base.h"

using namespace std;
using std::vector;
using std::iterator;

//--------------------------Read data of jobs----------------------------------
void ReadJobs(vector<JobData>& Jdata)
{
	JobData j;
	ifstream ifs("instance_job.csv", ios::in);

	if (!ifs.is_open())
	{
		cout << "It's fail to open the JobDatas file." << endl;
		return;
	}
	string buff;
	int n = 0;
	while (getline(ifs, buff))
	{
		vector<double> nums;
		char* s_input = (char*)buff.c_str();
		const char* split = ", ";
		char* p = NULL;
		char* next_token = NULL;
		p = strtok_s(s_input, split, &next_token);
		//char* p = strtok(s_input, split);
		double a;
		while (p != NULL)
		{
			a = atof(p);
			nums.push_back(a);
			p = strtok_s(NULL, split, &next_token);
		}

		j.jID = nums[0];
		j.jTime = nums[1];
		j.jPriority = nums[2];
		j.jCPU = nums[3];
		j.jMEM = nums[4];

		Jdata.push_back(j);

	}
	ifs.close();
}


//--------------------------Read data of VMs----------------------------------
void ReadVMs(vector<VMData>& VMsdata)
{
	VMData Vm;
	ifstream ifs("instance_vm.csv", ios::in);

	if (!ifs.is_open())
	{
		cout << "It's fail to open the VMsDatas file." << endl;
		return;
	}
	string buff;
	int n = 0;
	while (getline(ifs, buff))
	{
		vector<float> nums;
		stringstream ss(buff);
		string word;
		char sep = ',';
		while (!ss.eof()) {
			getline(ss, word, sep);
			nums.push_back(atof(word.c_str()));
		}

		Vm.vmID = (int) nums[0];
		Vm.vmCPU = nums[1];
		Vm.vmMEM = nums[2];
		Vm.vmPrice = nums[3];
		VMsdata.push_back(Vm);
	}

	ifs.close();
}

//-----------------------Read data of Initial Solution-----------------------------
void split(const string& w, const char spliter, vector<string>& res) {
	stringstream s(w);
	string token;
	while (getline(s, token, spliter)) {
		res.push_back(token);
	}
	char* next_token = NULL;
	/*char* token = strtok_s((char*)spliter.c_str(), (char*)w.c_str(), &next_token);
	while (token != NULL) {
		res.push_back(token);
		token = strtok_s(NULL, (char*)spliter.c_str(), &next_token);
	}*/
}
void ReadIniSolution(vector<PopInfo>& Pops)
{
	fstream fin;
	try
	{
		fin.open("IniPops.csv", ios::in);
	}
	catch (const std::exception& e)
	{
		cout << "It's fail to open the file." << endl;
		return;
	}

	vector<string> row;
	string line, word, temp;

	while (fin >> temp) {
		row.clear();
		split(temp, ',', row);

		PopInfo p;
		p.pID = stoi(row[0].c_str());

		vector<string> spS;
		split(row[1], ':', spS);
		for (int i = 0; i < spS.size(); i++) {
			p.pSequence.push_back(stoi(spS[i]));
		}

		p.pCost = stoi(row[2].c_str());
		//p.pTardiness = stoi(row[3].c_str());
		Pops.push_back(p);
	}

}