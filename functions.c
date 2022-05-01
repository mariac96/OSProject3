#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include "functions.h"

void initializeSem(sem_t *sp,int value){
  int val;
  val = sem_init(sp,1,value); //initialize the semaphores
  if(val!=0){
    fprintf(stderr, "Couldn't initialize the semaphore.\n" );
    exit(3);
  }
}

void writeinfile(FILE*fp,char* name,char*message){

  char buffer[26];
  int millisec;
  struct tm* tm_info;
  struct timeval tv;

  gettimeofday(&tv, NULL);

  millisec = (tv.tv_usec/1000.0); // round to nearest millisec
  if (millisec==1000) { // allow for rounding up to nearest second
    millisec -=1000;
    tv.tv_sec++;
  }

  tm_info = localtime(&tv.tv_sec);

  strftime(buffer, 26, "%d:%H:%M:%S", tm_info);
  fprintf(fp, "%s:%03d %d %s %s\n",buffer,millisec,getpid(),name,message );
}


int printfiles(){
  FILE *fpe,*fon,*fto;
  char *p,*o,*t;
  char buffer1[70],time1[15],message1[20];
  char buffer2[70],time2[15],message2[20];
  char buffer3[70],time3[15],message3[20];
  char buffer4[70],time4[15],message4[20];
  char buffer5[70],time5[15],message5[20];
  char buffer6[70],time6[15],message6[20];
  fpe = fopen("pepper.txt","r+");
  fon = fopen("onion.txt","r+");
  fto = fopen("tomato.txt","r+");

  if(fpe==NULL || fon==NULL || fto==NULL){
    return -1;
  }


  p=fgets(buffer1,sizeof(buffer1),fpe);
  sscanf(buffer1,"%s %*d %*s %s",time1,message1);
  o=fgets(buffer2,sizeof(buffer2),fon);
  sscanf(buffer2,"%s %*d %*s %s",time2,message2);
  t=fgets(buffer3,sizeof(buffer3),fto);
  sscanf(buffer3,"%s %*d %*s %s",time3,message3);
  while(p!=NULL || o!=NULL || t!=NULL){
    while(p!=NULL && strcmp(message1,"Start")!=0){ //find when it started making the salad
      p=fgets(buffer1,sizeof(buffer1),fpe);
      sscanf(buffer1,"%s %*d %*s %s",time1,message1);
    }
   if(strcmp(message1,"Start")==0){
      p=fgets(buffer4,sizeof(buffer4),fpe);
      sscanf(buffer4,"%s %*d %*s %s",time4,message4);
      while(p!=NULL && strcmp(message4,"End")!=0){ //find when it finished making the salad
        p=fgets(buffer4,sizeof(buffer4),fpe);
        sscanf(buffer4,"%s %*d %*s %s",time4,message4);
      }
     }
    while(o!=NULL && strcmp(message2,"Start")!=0){
      o=fgets(buffer2,sizeof(buffer2),fon);
      sscanf(buffer2,"%s %*d %*s %s",time2,message2);
    }
      if(strcmp(message2,"Start")==0){
        o=fgets(buffer5,sizeof(buffer5),fon);
        sscanf(buffer5,"%s %*d %*s %s",time5,message5);
        while(p!=NULL && strcmp(message5,"End")!=0){
          o=fgets(buffer5,sizeof(buffer5),fon);
          sscanf(buffer5,"%s %*d %*s %s",time5,message5);
      }
    }
    while(t!=NULL && strcmp(message3,"Start")!=0){
      t=fgets(buffer3,sizeof(buffer3),fto);
      sscanf(buffer3,"%s %*d %*s %s",time3,message3);
    }
    if(strcmp(message3,"Start")==0){
      t=fgets(buffer6,sizeof(buffer6),fto);
      sscanf(buffer6,"%s %*d %*s %s",time6,message6);
      while(t!=NULL && strcmp(message6,"End")!=0){
        t=fgets(buffer6,sizeof(buffer6),fto);
        sscanf(buffer6,"%s %*d %*s %s",time6,message6);
      }
      }
    if(strcmp(message1,"Start")==0){
      if(strcmp(message2,"Start")==0){
        if(strcmp(time1,time2)>0){ //print the bigger time
          printf("%s- ",time1 );
        }else{
          printf("%s- ", time2);
        }
        if(strcmp(time4,time5)<0){ //print the smaller time
          printf("%s\n",time4 );
        }else{
          printf("%s\n",time5 );
        }
      }else if(strcmp(message3,"Start")==0){
       if(strcmp(time1,time3)>0){
          printf("%s- ",time1 );
        }else{
          printf("%s- ",time3 );
        }
        if(strcmp(time4,time6)<0){
          printf("%s\n",time4 );
        }else{
          printf("%s\n",time6 );
        }
      }
    }else if(strcmp(message2,"Start")==0){
      if(strcmp(message3,"Start")==0){
       if(strcmp(time2,time3)>0){
         printf("%s- ",time2 );
       }else{
         printf("%s- ",time3 );
       }
       if(strcmp(time5,time6)<0){
         printf("%s\n",time5 );
       }else{
         printf("%s\n",time6 );
       }
     }
    }

    p=fgets(buffer1,sizeof(buffer1),fpe);
    sscanf(buffer1,"%s %*d %*s %s",time1,message1);

    o=fgets(buffer2,sizeof(buffer2),fon);
    sscanf(buffer2,"%s %*d %*s %s",time2,message2);

    t=fgets(buffer3,sizeof(buffer3),fto);
    sscanf(buffer3,"%s %*d %*s %s",time3,message3);

   }

  fclose(fpe);
  fclose(fon);
  fclose(fto);
  return 0;
}
