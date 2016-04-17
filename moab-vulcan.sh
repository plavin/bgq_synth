#!/bin/bash

#MSUB -l partition=vulcan
#MSUB -l nodes=512
#MSUB -l walltime=2:00:00
#MSUB -l gres=ignore
#MSUB -q psmall
#MSUB -j oe
#MSUB -N bench

## -s specifies messsage size, 
## -l specifies number of all to all loops,
## -r specifies number of ranks in inner commnicator
## -i index of the job (will be used for names of output)
## -a assignment (values found in assignments.h)

mkdir net
squeue >> squeue-$SLURM_JOB_ID.out
((count=1))

for idx in `seq 1 10`
do
    for map in 4D-2 4D-comm
    do
	export BGQ_COUNTER_FILE=net/net-$map-$idx.out
	cp /nfs/tmp2/lavin2/manual_maps/$map /nfs/tmp2/lavin2/map
	srun -N512 --runjob-opts='--mapping /nfs/tmp2/lavin2/map' ./bench -s 16384 -l 5000 -r 256 -i $count -a 0 -c 1
	((count=count+1))
    done
done