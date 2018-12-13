//OS Project 5, impliments a resource management module for operating system
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h> 
#include <sys/stat.h> 
#include <signal.h> 
#include <sys/wait.h>
#include <stdbool.h>  
#define BUFFSIZE  2 * sizeof( int)
#define PERMS 0666
#define CKEY 10271984
#define MSKEY 5032011
#define MAX 5 //Max processes is 18 

const int P = 6; 
const int R = 4; 

int randomNum(int, int);
bool safe(int processes[], int available[], int max[][R], int allocate[][R]);  
                       
    // display the message 
void processNeed(int [P][R], int[P][R],int [P][R]);  

  
static volatile sig_atomic_t doneflag = 0; //For ctrl C signal handler

static void setdoneflag(int signo){ //Set function for flag for C signal 
   doneflag = 1;  
}


// control block 
struct clockStructure { 
   long ctype;
   unsigned int sec;
   unsigned int mil;
   int request;
   int release;
   int allocate;
 
};  



int  main(void) {
   struct sigaction act; 
   int msqid; 
   struct clockStructure clock;
   int shmid; 
   key_t key = 10271984; 
    
   //Setup processes and resources for bankers algorithm
   int proc[] = {0,1,2,3,4};//Processes 
   int avail[] = {4,4,2};  //Available resources 
   int max[P][R]; //Max resources a process can have  
   int allot[P][R]; //Allocated resources 
   int p, r, num1, num2; 
   
   //fill random values in max and allocated arrays 
   for(p = 0; p < P; p++){ 
     for(r = 0; r < R; r++){ 
        num1 = randomNum(0,7); 
        num2 = randomNum(0,5); 
        max[p][r] = num1; 
        allot[p][r] = num2; 
     }
   } 


  
   //system("touch msgq.txt"); 
  // int sizeMem = sizeof(*clock)*count; 
   act.sa_handler = setdoneflag; //Set up signal handler
   act.sa_flags = 0; 

  // pid_t queueArray[MAX];  
  
   // printf("key for message queue", mkey);   
   if ((sigemptyset(&act.sa_mask) == -1) ||
	(sigaction(SIGINT, &act, NULL) == -1))   {
      perror("Failed to set up signal handler"); 
      exit(1); 
   }
  // while(!doneflag) {
   //Create and open a log file 
   FILE *fp = fopen("x.log", "a");
   if (fp == NULL) 
     perror("failed to create logfile\n");  
   fprintf(fp, "OSS: Opened logfile in parent \n"); 
    
   //Create message queue
   if ((msqid = msgget(CKEY, PERMS | IPC_CREAT)) == -1) {
      perror("msgget in oss");
      exit(1);
   }
    printf("msgget in oss is %d \n", msqid); 
    clock.sec = 1;
    clock.mil = 1;  
    clock.ctype = 1;
    clock.allocate = 0; 
    clock.request = 0; 
    clock.release = 0;  
    int length = sizeof(struct clockStructure) - sizeof(long);  
    printf("%d length\n", length); 
    printf("messages initialized\n");

      int send = msgsnd(msqid, &clock,length,0);  //Send messages
      printf("send number %d\n", send); 
       if (send == -1)
          perror("msgsnd in parent");
     //print the clock */
     fprintf(fp, "OSS: Time initalized at %d seconds\n", clock.sec); 
     fprintf(fp,"OSS: Time initalized at %d nanoseconds\n", clock.mil);
     printf("Starting oss process\n"); 
    
        //fork 5 children 
       // pid_t pid[5] = {0,1,2,3,4};  
        pid_t pid;
        pid = fork();  
        int i; 
        int status; 
       // for ( i = 0; i < 2; i++){  
           if(pid  == -1) { 
             perror("Fork:"); 
             exit(EXIT_FAILURE); 
           }            
          else if(pid  == 0) { //Child process
           printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid()); 
           char *args[]={"./user",NULL}; 
           execv(args[0], args); 
           fprintf(fp,"OSS: child %d was successfully returned"); 
           exit(0); 
           } 
           else{
             //pid[i] = wait(&status); 
             pid = wait(&status); //Wait for child to come back  
             fprintf(fp,"OSS: parent detected child process returns\n"); 
             fprintf(fp, "OSS: Time in parent before accessing clock  %d seconds\n", clock.sec);
             fprintf(fp,"OSS: Time in parent  %d nanoseconds\n", clock.mil);
            // if ((msqid = msgget(CKEY, PERMS | IPC_CREAT)) == -1)                               perror("msgget in oss");
             int get = msgrcv(msqid,&clock, length,0,0); 
               if(get == -1)
                  perror("msgrcv receive in parent");
             fprintf(fp, "Oss: Time in clock from child in milliseconds %d\n", clock.mil);  
	     clock.mil  = clock.mil +  500;
             fprintf(fp, "Oss: process returned with request %d\n", clock.request);  
             fprintf(fp,"Oss: process %d allocated \n", clock.release); 
             
             fprintf(fp, "OSS: Time in parent after clock %d seconds\n", clock.sec);
             fprintf(fp,"OSS: Time in parent  %d nanoseconds\n", clock.mil);
             //Running bankers algrothm 
              if(clock.request != 0){ 
                 int num = clock.request; 
                bool isSafe = safe(proc,avail,max,allot); 
                 if (isSafe == true){ 
                   //  allot[num] = allot[num] - 1; //subtract one resource 
                     clock.allocate = clock.request; //give request
                     clock.request = 0; //reset resource
                  } 
                } 
               }
              if(clock.release != 0){ 
               int num = clock.release;
              // int sum = allot[num];  
              // allot[num] = sum  + 1; //Add another resource 
              }  
                     
                 
     // }//end of for loop    
           printf("End of oss process\n");
   //  doneflag = 1; 
     
    // }//Bracket for flag


     // fprintf(fp,"End program"); 
     fprintf(fp, "Oss: Remove the message queue\n"); 
    if (msgctl(msqid, IPC_RMID, NULL) == -1) 
       perror("msgctl");
       
   return 0;  
 } 
    
 int randomNum(int min, int max){
    srand(time(NULL));
    int num = (rand() % (max - min + 1)) + min;

    return num;
  }
//Bakers algorithm functions 
//Calculates need for processes 
void processNeed(int need[P][R], int max[P][R],int allocate[P][R]){ 
//  P = algorithm.processes; 
//  R = algorithm.resources;
 FILE *fp = fopen("x.log", "a");
   if (fp == NULL)
     perror("failed to create logfile\n");
   fprintf(fp, "OSS: Opened logfile in processNeed \n");
 
 need[P][R];  max[P][R];  allocate[P][R]; 
 //Calculate what is needed by each process 
 fprintf(fp, "printing need\n"); 
 int i,j; 
 for(i = 0; i < P; i++){ 
   for(j = 0; j < R; j++){ 
     //max - allocatted is the need 
     need[i][j] = max[i][j] - allocate[i][j];
    fprintf(fp, "need %d = max %d - allocate %d\n",need[i][j],max[i][j],allocate[i][j]); 
    }
  } 
}

//See if function is safe 
bool safe(int processes[], int available[], int max[][R], int allocate[][R]){ 
  int need[P][R]; 
  FILE *fp = fopen("x.log", "a");
   if (fp == NULL)
     perror("failed to open logfile\n");
   fprintf(fp, "OSS: Opened logfile in safe \n");


  //Calculate process need 
  processNeed(need, max, allocate);
  bool finished[P];  //To determine if the processes have finished
  int y; 
   fprintf(fp, "oss: See if processes have finished\n"); 
   for(y = 0; y < P; y++){ //Initialize array 
      finished[y] = 0; 
      fprintf(fp, "%d ",finished[y]); 
   }
   
  int storeSafe[P]; //Store processes that are safe
  int copy[R]; //Copies resources that are available 
  int i; 
    for (i = 0; i < R; i++)  
       copy[i] = available[i]; 
     
  //while loop for if system is not safe or if processes not finished
  int count = 0;  
    while( count < P){ 
      //find process not finished and needs can be satisfied 
       bool found = false; 
       int j; 
        for( j = 0; j < P; j++){ 
          if (finished[j] == 0){ //check if process finished
           int x; 
           for (x = 0; x < R; x++) { 
              if (need[j][x] >  copy[j]) 
                 break; 
            }
          }       
          //if needs are satisfied 
           if (j == R) {
           int k;  
            for ( k = 0; k < R; k++) 
               copy[k] = allocate[j][k]; //free resources 
           //Add process to safe sequence 
           storeSafe[count++] = j; 
           finished[j] = 1; //make process as finished 
           found = true; 
           } 
        }
 
    if (found == false){ //couldn't find a safe sequence 
     fprintf(fp,"Oss:System not in a safe state\n"); 
     return false; 
    } 
  }//End of while loop 
   //Safe state 
      fprintf(fp, "Oss:Safe process array\n"); 
      for ( i = 0; i < P ; i++) 
        fprintf(fp, "%d ", storeSafe[i]);
      return true;  
   
}//End of function bracket


  
