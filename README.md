# RTRRIP_Cache_Replacement
Cache replacement techniques help in achieving this by optimizing cache utilization, reducing access rates, and boosting overall system performance. This project presents a novel approach, 'Recency-Time Re-Reference Interval Prediction (RT-RRIP),' which is a 2-stage model with a recency-time pre-filter in the first stage, coupled with a Re-Reference Interval Prediction (RRIP) model in the second.

# Project Outline
In RTRRIP replacement policy, the accuracy of the RRIP predictor is enhanced by initially subjecting cache blocks to a pre-filter. This pre-filter creates a subset based on a threshold value (where average recency time is considered). Subsequently, this subset is forwarded to the RRIP predictor, which determines the eviction of a data block based on Re-reference Prediction Values (RRPV).

# Simulator Setup,Compiling and Configuration
```bash
Simulator Used: zsim (version 2)
Zsim Dependencies: gcc >= 4.6, pin, scons, libconfig, libhdf5, libelfg0
```
Process of Compilation and Execution:
```bash
•	git apply cse_server.patch
•	zip -F benchmarks.zip --out single-benchmark.zip && unzip single-benchmark.zip && mkdir benchmarks/parsec-2.1/inputs/streamcluster tar -zxvf polybench-c-3.2.tar.gz
•	source setup_env
•	cd zsim
•	scons -j4
•	./rtrrip_script  <Benchmark> <Replacement policy> , where Benchmark:[SPEC,RTRRIP] and Replacement policy:[LRU,LFU,SRRIP,RTRRIP]
•	Once the script has successfully run, the output can be viewed at zsim/outputs/<Replacement_policy>/<Benchmark_workload>/zsim.out
```
Project Structure:
RTRRIP_Cache_Replacement
```bash    
|_ README.md
|_ benchmarks.zip
|_ zsim
    |_ build
    |_ configs
       |_ LRU
       |_ LFU
       |_ SRRIP
       |_ RTRRIP
    |_ misc 
    |_ outputs
       |_ benchmark
       	  |_ LRU
       	  |_ LFU
       	  |_ SRRIP
           |_ RTRRIP
    |_ src
       |_ rtrrip_repl.h
       |_ init.cpp
    |_ tests
```
# Project Reference Paper
https://ieeexplore.ieee.org/document/9824298

#Acknowledgement
<br/>Dr.EJ KIM - Professor, CSE Department, Texas A & M University
<br/>Sabuj Laskar - Teaching Assistant, CSCE-614 
