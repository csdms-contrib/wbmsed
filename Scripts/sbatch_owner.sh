#!/bin/bash
#SBATCH --job-name=4.4.1
#SBATCH -n 8
#SBATCH --mem=32G
#SBATCH -p owners
#SBATCH --qos scohen2
#SBATCH --error=error.%J.txt
#SBATCH --output=output.%J.txt
./WBMsed4.4.1.sh Global dist 06min
#./WBMsed4.3.7_India.sh India dist 30sec