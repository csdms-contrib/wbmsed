#!/bin/sh

#PBS -N WBMsed_bankfull       
#PBS -o job_out.txt            
#PBS -e job_err.txt            
#PBS -m ae                    
#PBS -M sagy.cohen@colorado.edu
 
#PBS -l mem=31gb             
#PBS -l nodes=1:ppn=3           
cd ${PBS_O_WORKDIR}
pwd
./BQARTdaily.sh Global 06min dist
