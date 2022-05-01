#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "functions.h"
#define SEGMENTPERM 0666

int main (int argc, char* argv[]){
  typedef struct vegetables{
    int tomatoes;
    int onions;
    int peppers;
  }vegetables;
  	int id,err,*numofsalads,offset,i=1,n,t,num,pnum,total;
    long int size;
    int pe=0,to=0,on=0;
    //one semaphore for each saladmaker, one for the chef
    sem_t *pepper,*tomato,*onion,*chef;
    vegetables *v;
    FILE *fpchef;

    if(argc !=5){
      fprintf(stderr, "Not enough arguments.\n" );
      exit(1);
    }

    fpchef=fopen("chef.txt","a");
    if(fpchef==NULL){
      fprintf(stderr, "Failed to open file\n");
      exit(2);
    }

    while (i<argc )
    {
      if(strcmp(argv[i],"-n")==0){
          i++;
          n=atoi(argv[i]);
          total=n;
      }
      if(strcmp(argv[i],"-m")==0){
          i++;
          t=atoi(argv[i]);
      }
      i++;
    }

    size = sizeof(vegetables)+4*sizeof(sem_t)+sizeof(int)+sizeof(sem_t);  //size of the shared memory
    id = shmget(IPC_PRIVATE, size, SEGMENTPERM); //create shared memory
    if (id == -1) {
      fprintf(stderr, "Error creating shared memory.\n" );
      exit(2);}
    else printf("Allocated memory with id= %d\n", id);


    void *sharedmemory =  shmat(id,(void*) 0, 0);  //attach the shared memory
	  if ( sharedmemory == NULL) {
      fprintf(stderr, "Error attching the shared memory\n");
      exit(2);}

      err = shmctl(id, IPC_RMID, 0); //remove the shared memory
   	 if (err == -1) {
       fprintf(stderr, "Error removing the shared memory.\n");}
       
    v = (vegetables*)sharedmemory;   //the vegetables in the shared memory
    offset = sizeof(vegetables);
    v->tomatoes= 0;
    v->onions = 0;
    v->peppers = 0;

    pepper =(sem_t*)v+offset;  //the semaphore in the shared memory for the saladmaker who has peppers
    offset = offset+sizeof(sem_t);
    initializeSem(pepper,0);

    tomato = (sem_t*)v +offset;//the semaphore in the shared memory for the saladmaker who has tomatoes
    offset = offset+sizeof(sem_t);
    initializeSem(tomato,0);

    onion = (sem_t*)v +offset; //the semaphore in the shared memory for the saladmaker who has onions
    offset = offset+sizeof(sem_t);
    initializeSem(onion,0);

    chef = (sem_t*)v +offset; //the semaphore in the shared memory for the chef
    offset = offset+sizeof(sem_t);
    initializeSem(chef,0);

    numofsalads = (int*)v+offset;  //the number of salads
    *numofsalads = n;

   srand(time(0));
   pnum=12;

 while(n>0) { //haven't made all the salads

    num = (rand() % (3 - 1 + 1)) + 1;
    while(num==pnum) {//not putting the same ingredients on the table
      num = (rand() % (3 - 1 + 1)) + 1;  }

    if(num==1){
      writeinfile(fpchef,"Chef","Selecting ingredients onions,peppers");
    }else if(num==2){
      writeinfile(fpchef,"Chef","Selecting ingredients tomatoes,peppers");
    }else{
      writeinfile(fpchef,"Chef","Selecting ingredients onions,tomatoes");
    }

   if(num==1){//put onions and peppers on the table
     n= *numofsalads;
     if(n==0){
       break;
     }
     to++;
     v->onions ++;
     v->peppers++;
     writeinfile(fpchef,"Chef","Notify saladmaker1");
     sem_post(tomato);
     sem_wait(chef); //wait for the salad maker to take the vegetables

   }else if(num==2){ //put tomatoes and peppers on the table
     n= *numofsalads;
     if(n==0){
       break;
     }
     on++;
     v->peppers++;
     v->tomatoes++;
    writeinfile(fpchef,"Chef","Notify saladmaker2");
    sem_post(onion);
    sem_wait(chef); //wait for the salad maker to take the vegetables

   }else if(num==3){ //put tomatoes and onions on the table
     n= *numofsalads;
     if(n==0){
       break;
     }
     pe++;
     v->tomatoes++;
     v->onions ++;
    writeinfile(fpchef,"Chef","Notify saladmaker3");
    sem_post(pepper);
    sem_wait(chef); //wait for the salad maker to take the vegetables
   }
   writeinfile(fpchef,"Chef","Man time for resting");
   sleep(t);
   pnum = num;
 }
fclose(fpchef);
 sem_post(tomato); //to exit
 sem_post(onion);
 sem_post(pepper);


 sem_wait(chef);
 sem_wait(chef);
 sem_wait(chef);
 sem_wait(chef);

printf( "Total # of %d\n",total-(*numofsalads));
printf("# of saladmaker1 %d\n",to );
printf("# of saladmaker2 %d\n",on );
printf("# of saladmaker3 %d\n",pe);
 if(printfiles()==-1){
   fprintf(stderr, "Cannot open files.\n");
 }


   sem_destroy(pepper); //destroy the semaphores
   sem_destroy(onion);
   sem_destroy(tomato);
   sem_destroy(chef);

   err = shmdt((void *) sharedmemory); //detaching the shared memory
 	if (err == -1) fprintf(stderr, "Error detaching the shared memory\n");


	return 0;
}
