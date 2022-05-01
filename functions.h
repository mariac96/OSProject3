#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
#include <stdio.h>
#include <semaphore.h>

void initializeSem(sem_t *sp,int value);
void writeinfile(FILE*fp,char* name,char*message);
int printfiles();
#endif
