#!/bin/sh

#PBS -N WBMsed3.0_Qbfn0.5
#PBS -o job_out.txt            
#PBS -e job_err.txt            
#PBS -m ae                    
#PBS -M sagy.cohen@colorado.edu
         
cd ${PBS_O_WORKDIR}
pwd
./WBMsed3.0_n0.5_dTS.sh Global 06min dist
