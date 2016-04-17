#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void get_dim(const char * s, int * a){
  a[0] = dimA(s);
  a[1] = dimD(s);
  a[2] = dimC(s);
  a[3] = dimB(s);
  a[4] = dimE(s);
}

int dimA(const char * s){
  char sc[100];
  strcpy(sc, s);
  int i = 0;
  while(sc[i] != '(') i++;
  i+=1;
  sc[i+1] = 0;
  return atoi(sc+i);
}

int dimB(const char * s){
  char sc[100];
  strcpy(sc, s);
  int i = 0;
  while(sc[i] != '(') i++;
  i+=3;
  sc[i+1] = 0;
  return atoi(sc+i);
}

int dimC(const char * s){
  char sc[100];
  strcpy(sc, s);
  int i = 0;
  while(sc[i] != '(') i++;
  i+=5;
  sc[i+1] = 0;
  return atoi(sc+i);
}

int dimD(const char * s){
  char sc[100];
  strcpy(sc, s);
  int i = 0;
  while(sc[i] != '(') i++;
  i+=7;
  sc[i+1] = 0;
  return atoi(sc+i);
}

int dimE(const char * s){
  char sc[100];
  strcpy(sc, s);
  int i = 0;
  while(sc[i] != '(') i++;
  i+=9;
  sc[i+1] = 0;
  return atoi(sc+i);
}
