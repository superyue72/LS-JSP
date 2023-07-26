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

void ReadVMs(vector<VMData>& VMsdata);
void ReadJobs(vector<JobData>& Jdata);
void split(const string& w, const char spliter, vector<string>& res);
void ReadIniSolution(vector<PopInfo>& Pops);