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

int main(int argc, char **argv) {
  typedef struct vegetables{
    int tomatoes;
    int onions;
    int peppers;
  }vegetables;
    FILE *fpe,*fto,*fon;
  	int id,err,*numofsalads,offset,i=1,n,lb,ub,num,k;
    //one semaphore for each saladmaker, one for the chef
    sem_t *pepper,*tomato,*onion,*chef;
    vegetables *v;

   if(argc !=9){
      fprintf(stderr, "Not enough arguments.\n" );
      exit(1);
    }

    while (i<argc )
    {
      if(strcmp(argv[i],"-t1")==0){
          i++;
          lb=atoi(argv[i]);
      }
      else if(strcmp(argv[i],"-t2")==0){
          i++;
          ub=atoi(argv[i]);
      }else if(strcmp(argv[i],"-s")==0){
          i++;
          id=atoi(argv[i]);
      }else if(strcmp(argv[i],"-v")==0){
            i++;
            n=atoi(argv[i]); //if n=1 the saladmaker has tomatoes, if n=2 he has onions, if n=3 he has peppers
      }
      i++;
    }
   if(n==1){
     fto=fopen("tomato.txt","a");
     if(fto==NULL){
       fprintf(stderr, "Failed to open file\n");
       exit(2);
     }
   }else if(n==2){
     fon=fopen("onion.txt","a");
     if(fon==NULL){
       fprintf(stderr, "Failed to open file\n");
       exit(2);
     }
   }else if (n==3){
     fpe=fopen("pepper.txt","a");
     if(fpe==NULL){
       fprintf(stderr, "Failed to open file\n");
       exit(2);
     }
   }


  void *sharedmemory =  shmat(id,(void*) 0, 0); //attach shared memory
  if ( sharedmemory == NULL) {
    fprintf(stderr, "Error attching the shared memory\n");
     exit(2);}

    v = (vegetables*)sharedmemory;   //the vegetables in the shared memory
    offset = sizeof(vegetables);

    pepper =(sem_t*)v+offset;  //the semaphore in the shared memory for the saladmaker who has peppers
    offset = offset+sizeof(sem_t);

    tomato = (sem_t*)v +offset;//the semaphore in the shared memory for the saladmaker who has tomatoes
    offset = offset+sizeof(sem_t);

    onion = (sem_t*)v +offset; //the semaphore in the shared memory for the saladmaker who has onions
    offset = offset+sizeof(sem_t);

    chef = (sem_t*)v +offset; //the semaphore in the shared memory for the chef
    offset = offset+sizeof(sem_t);

    numofsalads = (int*)v+offset;  //the number of salads
    k=*numofsalads;

    srand(time(0));

  while(k>0) {
      if(n==1){
        writeinfile(fto,"Saladmaker1","Waiting for ingredients");
        sem_wait(tomato); //wait for the chef to put the vegetables in the table
        if(*numofsalads<=0){ //if all the salads have been made
          sem_post(chef);
          break;}
        writeinfile(fto,"Saladmaker1","Get ingredients");
        v->peppers--;
        v->onions--;
        (*numofsalads)--;
        k=*numofsalads;
        sem_post(chef); //signal chef that it has taken the vegetables
        num = (rand() % (ub - lb + 1)) + lb;
        writeinfile(fto,"Saladmaker1","Start making salad");
        sleep(num);
        writeinfile(fto,"Saladmaker1","End making salad");

        }else if(n==2){
        writeinfile(fon,"Saladmaker2","Waiting for ingredients");
        sem_wait(onion); //wait for the chef to put the vegetables in the table
        if(*numofsalads<=0){//if all the salads have been made
          sem_post(chef);
          break;}
        writeinfile(fon,"Saladmaker2","Get ingredients");
        v->tomatoes--;
        v->peppers--;
        (*numofsalads)--;
        k=*numofsalads;
        sem_post(chef);//signal chef that it has taken the vegetables
        num = (rand() % (ub - lb + 1)) + lb;
        writeinfile(fon,"Saladmaker2","Start making salad");
        sleep(num);
        writeinfile(fon,"Saladmaker2","End making salad");

      }else if(n==3){
        writeinfile(fpe,"Saladmaker3","Waiting for ingredients");
        sem_wait(pepper); //wait for the chef to put the vegetables in the table
        if(*numofsalads<=0){//if all the salads have been made
          sem_post(chef);
          break;}
        writeinfile(fpe,"Saladmaker3","Get ingredients");
        v->tomatoes--;
        v->onions--;
        (*numofsalads)--;
        k=*numofsalads;
        sem_post(chef);//signal chef that it has taken the vegetables
        num = (rand() % (ub - lb + 1)) + lb;
        writeinfile(fpe,"Saladmaker3","Start making salad");
        sleep(num);
        writeinfile(fpe,"Saladmaker3","End making salad");
      }

   }

   if(n==1){
     fclose(fto);
     sem_post(chef);
   }else if(n==2){
     fclose(fon);
     sem_post(chef);
   }else if(n==3){
     fclose(fpe);
     sem_post(chef);
   }

	err = shmdt((void *) sharedmemory); //detaching the shared memory
	if (err == -1) fprintf(stderr, "Error detaching the shared memory\n");

	return 0;
}
