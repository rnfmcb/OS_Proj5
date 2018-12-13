#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <sys/shm.h> 
#include <sys/stat.h>
#include <string.h>  
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h> 
#include <time.h> 
#define BUFFSIZE  2 * sizeof(int)
#define PERM 0666
#define CKEY 10271984

int randomNum(int, int); 
  
// structure for message queue

struct clockStruct{ 
   long ctype; 
   unsigned int sec; 
   unsigned int mil; 
   int request; 
   int release;
   int allocate;  
}; 
 
           
  int main(void) 
 {    
    int msgid;
    struct clockStruct clock;
    int option;
    int request = 0;     
    int resourceArray[3]; 

    int i; 
    for( i = 0; i < 3; i++) 
       resourceArray[i] = 0;    
    
    //process will terminate  
    FILE *fp = fopen("child.log", "a");
        if (fp == NULL)
          perror("failed to create logfile");
       fprintf(fp, "Opened file in child\n");

     printf("Successful exec\n"); 
       
      // msgget creates a message queue  and returns identifier
  
     msgid = msgget(CKEY,0666);  /* connect to the queue */
       if (msgid == -1)
        perror("msgget in child ");
     
     printf("number for msgget %d\n", msgid); 
     printf("ready to receive message\n"); 
 
                       
    int length  =  sizeof(struct clockStruct) - sizeof(long);
    printf("%d length in child\n", length); 
    //Loops through to get and release resources 
    int done = 0; 
    while (done == 0){ 
      int get = msgrcv(msgid, &clock,length,0,0);
      printf("receive was this number %d\n", get);
      fprintf(fp,"Clock is %d : %d in child \n", clock.sec, clock.mil);  
      //Check if a resource was given 
      int k; int getResource = 0;
      if (clock.allocate != 0){  
      while(getResource == 0){ 
          for(k = 0; k < 3; k++) {
            if(resourceArray[k]== 0){
              resourceArray[k] = clock.allocate; 
              getResource = 1; 
            }
          }
        }
       }
      //Print resources allocated to child 
      fprintf(fp, "Resources allocated to child are: "); 
      for( k = 0; k < 3; k++)
         fprintf(fp, "%d,", resourceArray[k]); 
      // fprintf(fp, "Data Received is : %s \n", message.cpuTime);  
      clock.mil = clock.mil + 100;                                        
      fprintf(fp, "Adding 100 milliseconds to clock\n"); 
      length  =  sizeof(struct clockStruct) - sizeof(long);   
      fprintf(fp,"Clock is %d : %d in child \n", clock.sec, clock.mil);
  
      //pick an option 
      option = randomNum(0,2); 
      fprintf(fp, "option %d was picked by child process\n");        

      //If option 0, terminate process 
       if(option == 0){
         fprintf(fp, "Ending child process\n");
         int send = msgsnd(msgid, &clock, length, 0);  //Send messages
            if (send == -1)
              perror("msgsnd in option 0 ");
          return 0;  
        } 

       if(option == 1){ //If option 1, request process 
         clock.request = randomNum(0,3); 
         fprintf(fp, "requesting resource %d  from OSS\n", clock.request);   
       }       
 
       if(option == 2){ 
         int j; int finish = 0; 
         while(finish == 0){ //Search for a process in array 
           for( j = 0; j < 3; j++){
            if (resourceArray[j] != 0){
                clock.release = resourceArray[j]; 
                finish = 1;
            } 
          }
          finish = 1; //Incase no resources in array 
          fprintf(fp, "No resources to release\n"); 
         }                                                                               fprintf(fp, "Releasing resource %d\n", clock.release);                
       } 

 int send = msgsnd(msgid, &clock, length, 0);  //Send messages
   if (send == -1)
      perror("msgsnd ");
   request++; 
   fprintf(fp,"This is request number %d\n", request); 
   if (request == 3) 
      done = 1;
 }
 fprintf(fp,"Terminating child process\n");  
 return 0;
 }
 

  int randomNum(int min, int max){
    srand(time(NULL));
    int num = (rand() % (max - min + 1)) + min;
    return num;
   }
                       

