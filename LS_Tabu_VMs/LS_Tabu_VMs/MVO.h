#pragma once

#include <cmath>
#include "base.h"
#include "Evaluation.h"
#include "tabu.h"

MVOPopInfo EMVO(const vector<JobData> Jdata, const vector<VMData> VMsdata, vector <MVOPopInfo> Pops);
MVOPopInfo MVO(const vector<JobData> Jdata, const vector<VMData> VMsdata, vector<MVOPopInfo> Pops);
vector<MVOPopInfo> UpdateUniverse(const vector<JobData> Jdata, const vector<VMData> VMsdata, vector<MVOPopInfo> Pops, MVOPopInfo bestP, double wep, double tdr);
int RouletteChoice(vector<MVOPopInfo> pops);