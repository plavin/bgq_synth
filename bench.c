#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>
#include "assignments.h"

#define INNER_COMM 0
#define OUTER_COMM 1

double Alltoall(MPI_Comm comm, int num_ranks, int rank, int msg_size, int  num_time_steps){

  //declare, initialize message space
  char send[msg_size*num_ranks];
  char recv[msg_size*num_ranks];
  for(int i = 0; i < msg_size*num_ranks; i++) send[i] = rand() % 256;

  //perform all-to-all 
  double start = MPI_Wtime();
  for(int i = 0; i < num_time_steps; i++){
    MPI_Alltoall(send, msg_size, MPI_CHAR, recv, msg_size, MPI_CHAR, comm);
  }
  double end = MPI_Wtime();
  
  return end - start;
}

int main(int argc, char**argv){

  int num_ranks, rank, split_num_ranks, split_rank;
  int outer_ranks, inner_ranks;
  int new_comm_id;
  int msg_size, loops;
  int slurm_id, run_index;
  MPI_Comm split_comm;
  FILE * timings, * configs;
  int assignment;
  int custom;

  char c;
  while ((c = getopt (argc, argv, "s:r:l:i:ac:")) != -1){
    switch (c)
      {
      case 's':
	sscanf(optarg, "%d", &msg_size);
	break;
      case 'r':
	sscanf(optarg, "%d", &inner_ranks);
	break;
      case 'l':
	sscanf(optarg, "%d", &loops);
	break;
      case 'i':
	sscanf(optarg, "%d", &run_index);
	break;
      case 'a':
	sscanf(optarg, "%d", &assignment);
	assignment = 0;
	break;
      case 'c':
	sscanf(optarg, "%d", &custom);
	break;
      default:
	printf("Unrecognized option: %c\n", optopt);
	break;
      }
    if(c != 's' && c != 'i' && c != 'l' && c != 'r' ){break;}
  }

  timings = fopen("timings.out", "a");
  char configs_buf[128] = {0};
  sprintf(configs_buf, "config-%d.out", run_index);
  configs = fopen(configs_buf, "a");

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  if(num_ranks == 0){
    printf("MPI_Comm_size failure\n");
    exit(1);
  }
  outer_ranks = num_ranks - inner_ranks;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  //get node names
  char name[MPI_MAX_PROCESSOR_NAME] = {0};
  char * recv = (char*)calloc(MPI_MAX_PROCESSOR_NAME*num_ranks, sizeof(char));
  int proc_len;
  MPI_Get_processor_name(name, &proc_len);
  name[proc_len] = 0;
  MPI_Gather(name, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, recv, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, 0, MPI_COMM_WORLD);

  int * splitter = (int*)malloc(sizeof(int)*num_ranks);
  for(int i = 0; i < num_ranks; i++) splitter[i] = OUTER_COMM;
  if(!custom){
    if(rank == 0){
      if(assignment == RANDOM){
	int num_assigned = 0;
	while(num_assigned < inner_ranks){
	  int val = rand() % num_ranks;
	  if(splitter[val] == INNER_COMM){
	    continue;
	  }else{
	    splitter[val] = INNER_COMM;
	    num_assigned += 1;
	  }
	}
      }else if(assignment == APLANES){
	for(int i = 0; i < num_ranks; i++){
	  int dims[5] = {0};
	  get_dim(recv + i*MPI_MAX_PROCESSOR_NAME, dims);
	  if (dims[0] == 0 || dims[0] == 2){
	    splitter[i] = INNER_COMM;
	  }
	}
      }else if(assignment == APLANES_COARSE){
	for(int i = 0; i < num_ranks; i++){
	  int dims[5] = {0};
	  get_dim(recv + i*MPI_MAX_PROCESSOR_NAME, dims);
	  if (dims[0] == 0 || dims[0] == 1){
	    splitter[i] = INNER_COMM;
	  }
	}
      }else if(assignment == BPLANES){
	for(int i = 0; i < num_ranks; i++){
	  int dims[5] = {0};
	  get_dim(recv + i*MPI_MAX_PROCESSOR_NAME, dims);
	  if (dims[1] == 0 || dims[1] == 2){
	    splitter[i] = INNER_COMM;
	  }
	}
      }else if(assignment == CPLANES){
	for(int i = 0; i < num_ranks; i++){
	  int dims[5] = {0};
	  get_dim(recv + i*MPI_MAX_PROCESSOR_NAME, dims);
	  if (dims[2] == 0 || dims[2] == 2){
	    splitter[i] = INNER_COMM;
	  }
	}
      }else if(assignment == DPLANES){
	for(int i = 0; i < num_ranks; i++){
	  int dims[5] = {0};
	  get_dim(recv + i*MPI_MAX_PROCESSOR_NAME, dims);
	  if (dims[3] == 0 || dims[3] == 2){
	    splitter[i] = INNER_COMM;
	  }
	}
      }else if(assignment == EPLANES){
	for(int i = 0; i < num_ranks; i++){
	  int dims[5] = {0};
	  get_dim(recv + i*MPI_MAX_PROCESSOR_NAME, dims);
	  if (dims[4] == 0){
	    splitter[i] = INNER_COMM;
	  }
	}
      }else if(assignment == SQUAREAB1){
	for(int i = 0; i < num_ranks; i++){
	  int dims[5] = {0};
	  get_dim(recv + i*MPI_MAX_PROCESSOR_NAME, dims);
	  if (((dims[0] == 0 || dims[0] == 1) && (dims[1] == 0 || dims[1] == 1)) || 
	      ((dims[0] == 2 || dims[0] == 3) && (dims[1] == 2 || dims[1] == 3))){
	    splitter[i] = INNER_COMM;
	  }
	}
      }else if(assignment == SQUAREAB2){
	for(int i = 0; i < num_ranks; i++){
	  int dims[5] = {0};
	  get_dim(recv + i*MPI_MAX_PROCESSOR_NAME, dims);
	  if (((dims[0] == 0 || dims[0] == 2) && (dims[1] == 0 || dims[1] == 2)) || 
	      ((dims[0] == 1 || dims[0] == 3) && (dims[1] == 1 || dims[1] == 3))){
	    splitter[i] = INNER_COMM;
	  }
	}
      }else if(assignment == ALTERABC_NONE){
	for(int i = 0; i < num_ranks; i++){
	  int dims[5] = {0};
	  get_dim(recv + i*MPI_MAX_PROCESSOR_NAME, dims);
	  if (((dims[0] == 0 || dims[0] == 1) && (dims[1] == 0 || dims[1] == 1) && (dims[2] == 0 || dims[2] == 1)) || 
	      ((dims[0] == 2 || dims[0] == 3) && (dims[1] == 2 || dims[1] == 3) && (dims[2] == 0 || dims[2] == 1)) || 
	      ((dims[0] == 0 || dims[0] == 1) && (dims[1] == 2 || dims[1] == 3) && (dims[2] == 2 || dims[2] == 3)) || 
	      ((dims[0] == 2 || dims[0] == 3) && (dims[1] == 0 || dims[1] == 1) && (dims[2] == 2 || dims[2] == 3))) {
	    splitter[i] = INNER_COMM;
	  }
	}
      }else if(assignment == ALTERABC_ALL){
	for(int i = 0; i < num_ranks; i++){
	  int dims[5] = {0};
	  get_dim(recv + i*MPI_MAX_PROCESSOR_NAME, dims);
	  if (((dims[0] == 0 || dims[0] == 2) && (dims[1] == 0 || dims[1] == 2) && (dims[2] == 0 || dims[2] == 2)) || 
	      ((dims[0] == 1 || dims[0] == 3) && (dims[1] == 1 || dims[1] == 3) && (dims[2] == 0 || dims[2] == 2)) || 
	      ((dims[0] == 0 || dims[0] == 2) && (dims[1] == 1 || dims[1] == 3) && (dims[2] == 1 || dims[2] == 3)) || 
	      ((dims[0] == 1 || dims[0] == 3) && (dims[1] == 0 || dims[1] == 2) && (dims[2] == 1 || dims[2] == 3))) {
	    splitter[i] = INNER_COMM;
	  }
	}
      }
    }
  }else{ //using custon mapping in map.out
    for(int i = 0; i < num_ranks/2; i++){
      splitter[i] = INNER_COMM;
    }
  }
    
  MPI_Bcast(splitter, num_ranks, MPI_INT, 0, MPI_COMM_WORLD);
  
  //split communicator
  MPI_Comm_split(MPI_COMM_WORLD, splitter[rank], 1, &split_comm);
  MPI_Comm_size(split_comm, &split_num_ranks);
  MPI_Comm_rank(split_comm, &split_rank);
  MPI_Barrier(MPI_COMM_WORLD);
    
  
  //print names to file
  if(rank == 0){
    fprintf(configs,"rank,comm,node\n");
    for(int i = 0; i < num_ranks; i++){
      fprintf(configs,"%d,%d,%s\n", i, splitter[i], recv + i*MPI_MAX_PROCESSOR_NAME);
    }   
  }
  
  //run the inner communicator as a warm-up, seems to reduce variance
  if(splitter[rank] == INNER_COMM){
    Alltoall(split_comm, split_num_ranks, split_rank, msg_size, loops);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  //run the inside alone, as a baseline

  //start network counters region 1
  MPI_Pcontrol(1);

  float run1;
  if(splitter[rank] == INNER_COMM){
    run1 = Alltoall(split_comm, split_num_ranks, split_rank, msg_size, loops);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  //start network counters region 2
  MPI_Pcontrol(2);

  //run both communicators
  float run2;
  if(splitter[rank] == INNER_COMM){
    run2 = Alltoall(split_comm, split_num_ranks, split_rank, msg_size, loops); 
  }else{
    Alltoall(split_comm, split_num_ranks, split_rank, msg_size, loops);
  }

  //stop network counters
  MPI_Pcontrol(0);

  //print timings
  if(splitter[rank] == INNER_COMM && split_rank==0) fprintf(timings, "%d,%f,%f\n", run_index, run1, run2);

  //free(recv);
  free(splitter);
  MPI_Finalize();
  exit(0);
}
  
