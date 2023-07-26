#pragma once
#include "base.h"
#include "tabu.h"

//----------------------------tabu list of solutions-------------------------

bool SolutionTabu(queue<vector<int>>& QList, map<vector<int>, bool>& MList, vector <int> e)
{
	bool is_exist = false;

	for (auto itr = MList.find(e); itr != MList.end(); itr++)
	{
		is_exist = true;
	}

	if (!is_exist)
	{
		QList.push(e);
		MList.insert(pair<vector<int>, bool>({ e, is_exist }));
	}

	if (QList.size() > tabuLen) {
		vector <int> a = QList.front();
		QList.pop();
		MList.erase(a);
	}

	return is_exist;
}

//----------------------------tabu list of 2opt neighborhood-------------------------

bool opt2Tabu(queue<vector<int>>& QList, map<vector<int>, bool>& MList, vector <int> e)
{
	bool is_exist = false;

	for (auto itr = MList.find(e); itr != MList.end(); itr++)
	{
		is_exist = true;
	}

	if (!is_exist)
	{
		QList.push(e);
		MList.insert(pair<vector<int>, bool>({ e, is_exist }));
	}

	if (QList.size() > tabuLen) {
		vector <int> a = QList.front();
		QList.pop();
		MList.erase(a);
	}

	return is_exist;
}


//----------------------------tabu list of neighborhood-------------------------

bool NeighTabu(queue<vector<int>>& QList, map<vector<int>, bool>& MList, vector <int> e)
{
	bool is_exist = false;

	for (auto itr = MList.find(e); itr != MList.end(); itr++)
	{
		is_exist = true;
	}

	if (!is_exist)
	{
		QList.push(e);
		MList.insert(pair<vector<int>, bool>({ e, is_exist }));
	}

	if (QList.size() > tabuLen) {
		vector <int> a = QList.front();
		QList.pop();
		MList.erase(a);
	}

	return is_exist;
}