# MPI Contention Benchmark
Author: Patrick Lavin

## Purpose
This code measures the slowdown due to network contention from job running near each other on large clusters. It is currently configured for a BG/Q cluster, Vulcan.

Also included is the `bgqshared.py` file. 

## Method
Two communicators are created and assigned to some ranks in your job. A single communicator is run, to get a baseline for performance. Then both are run to determine the slowdown. This program does not currently have a comm phase. Instead, an All-to-All is run a user-specified number of times. 

## Example
Look in `moab-vulcan.sh` for an example of how to run it. Check the Files section below for the meaning of the output.
A mapping file for the rank placement should be specified. This should be somehwere globally-writable, such as tmp2. 

## Map Files
A collection of map files is included in the `maps` directory. They are in a format that can be supplied to the included moab command. If you would like to run them through the code bgqsharedlinks.py code, you can suse the maps in `maps/split_maps` directory. These are in a format such that comm-{x}-0 and comm-{x}-1 form a full communicator. This is the format that bgqsharedlinks.py expects. 

## Options
-c set to 1 to use a custom map file (ALWAYS USE THIS)
-s messsage size (in bytes) 
-l specifies number of all to all loops
-r specifies number of ranks in inner commnicator
-i index of the job (will be used for names of output)                                                                                                                            
-a assignment (DEPRECATED: Please use a custom map file instead, and ALWAYS use the -c option.) (values found in assignments.h) (NOTE: Not all of these have been implemented. Please stick to using the first of each 'type' of assignement, not all are supported)

## Compile
`$make vulcan` 
(catalyst, non-bgq  is not supported)

##Running
msub moab-vulcan.sh

## Output
-net counters from each run will be placed in net/
-timing info for each run is in timings.out (Output format is {
-configuration info is placed in config-$i.out
-squeue-*.out contains info on what other jobs are running at the time you start the job
-slurm-*.out is not used but will probably be filled with errors
	

## Files:
bench.c:     contains the benchmark
get_dims.c:  utility for reading a 5-tuple into a c-array
process.sh:  a script to take output from runnign the benchmark 
		     and place configurations in config-processed/
		     (useful for understanding random placements)

