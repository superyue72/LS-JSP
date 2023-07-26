#pragma once

#include "base.h"
#include "Evaluation.h"

PSOPopInfo BasicPSO(const vector<JobData> Jdata, const vector<VMData> VMsdata, vector <PSOPopInfo> Pops);
PSOPopInfo updateP(const vector<JobData> Jdata, const vector<VMData> VMsdata, PSOPopInfo pop, PSOPopInfo& gbest, PSOPopInfo& pbest);