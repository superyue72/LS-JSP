# Effective Local Search for Priority-Constrained Job Scheduling in Cloud

This repository includes the implementation of LS-JSP and the experimental results.

## 1. Introduction
We propose a novel local search-based job scheduling method, namely LS-JSP, for the job scheduling problem with recourse and priority constraints. To validate the performance of our LS-JSP algorithm, we compare it with four well-known algorithms, i.e., Genetic Algorithm (GA), Particle Swarm Optimisation Algorithm(PSO), Multi-verse Optimiser Algorithm (MVO) and Enhanced Multi-verse Optimiser Algorithm (EMVO). 

The completed results are available in <em>Results</em>

## 2. Benchmark
To evaluate the performance of the proposed algorithm, we extract the real-world trace data as one benchmark (Google benchmark) and generate another based on it (synthetic benchmark).

We extract the information of jobs scheduled on the cloud from the first-day trace. The failed jobs and killed jobs are not considered in current research. The preprocessed data is provided in Dataset. Raw and preprocessed data are available at: <https://github.com/google/cluster-data>

## 3. Reproducibility
### 3.1 Environment
We implement the proposed LS-JSP and all baselines in C++. All algorithms have the same coding mechanism, initial populations, and stopping criterion for fair comparisons. The experiments were conducted in the Microsoft Windows 11 OS with an Intel(R) Core(TM) i7-11700 CPU @2.50 GHz.

### 3.2 Parameter Setting
The Taguchi method of design of experiment (DOE) is conducted to set the parameters of all algorithms. Based on the experiments, the appropriate parameter combination of the LS-JSP is set as follows: γ = 0.1, α∗d = 0.4, δ = 200 and σ = 500. In GA, the cross probability is set as 0.5 and the mutation probability is 0.15. In PSO, four key parameters, including the weight, two learning factors, and the maximum velocity are set as 0.6, 0.2, 0.8, and 1, respectively. In EMVO and MVO, the minimal wormhole existence probability is 0.2, the maximum wormhole existence probability is 1 and the exploitation accuracy is 4.

Parameters can be changed in <em>base.h</em>

### 3.2 Source code
The source code of LS-JSP and baseline algorithms are provided in <em>LS_Tabu_VMs</em>.

For all the algorithms, there are three termination conditions: 1) the maximum computation time, 2) the maximum number of iterations, and 3) the maximum
repeat times of the best solution. In experiments, the maximum number of generations is set as 10000, and the maximum number of repeat times of the best solution is set as 1000. The maximum computation time is related to the number of jobs. The specific experiment settings for all instances is given in <em>ExperimentDesign.xlsx</em>

The termination conditions can be changed in <em>base.h</em>
