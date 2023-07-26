# Effective Local Search for Priority-Constrained Job Scheduling in Cloud

This repository includes the implementation of LS-JSP and the experimental results.

## 1. Introduction
We propose a novel local search-based job scheduling method, namely LS-JSP, for the job scheduling problem with recourse and priority constraints.

## 2. Benchmark
To evaluate the performance of the proposed algorithm, we extract the real-world trace data as one benchmark (Google benchmark) and generate another based on it (synthetic benchmark).

We extract the information of jobs scheduled on the cloud from the first-day trace. The failed jobs and killed jobs are not considered in current research. The preprocessed data is provided in Dataset. Raw and preprocessed data are available at: https://github.com/google/cluster-data


