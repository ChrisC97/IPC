#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

int random_int(int min, int max);
void  DepositMoney(int SharedMem[]);
void  ParentProcess(int SharedMem[]);
void  ChildProcess(int SharedMem[]);

int  main(int  argc, char *argv[])
{
	int    ShmID;
	int    *ShmPTR;
	pid_t  pid;
  
	// Allocating the shared memory, getting it's ID back.
  ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
  if (ShmID < 0) {
      printf("*** shmget error (server) ***\n");
      exit(1);
  }
  printf("Process has received a shared memory of two integers...\n");

  // Attach the memory segment to our address space.
  ShmPTR = (int *) shmat(ShmID, NULL, 0);
  if (*ShmPTR == -1) {
      printf("*** shmat error (Process) ***\n");
      exit(1);
  }
  printf("Process has attached the shared memory...\n");
	
	
	// Forking.
  printf("Orig Bank Account = %d\n", ShmPTR[1]);
	pid = fork();
	// ERROR.
	if (pid < 0) {
    printf("*** fork error (server) ***\n");
    exit(1);
	}
	// CHILD.
	else if (pid == 0) {
    time_t t;
    srand((unsigned) time(&t));
    ChildProcess(ShmPTR);
	}
	// Parent.
	else{
    srand(3478);
		ParentProcess(ShmPTR);
	}
}

void DepositMoney(int SharedMem[]){
  int balance = random_int(0, 100);
  // Odd number.
  if(balance % 2 == 1){
    SharedMem[1] += balance;
    printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, SharedMem[1]);
  }else{
    printf("Dear old Dad: Doesn't have any money to give\n");
  }
}

void ParentProcess(int SharedMem[])
{
	// Loop endlessly.
	while(1){
    sleep(random_int(0, 5));
    // Wait for our turn.
		while(SharedMem[0] != 0) { }
		// CRITICAL SECTION //
		if(SharedMem[1] <= 100){
			DepositMoney(SharedMem);
		}else{
			printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", SharedMem[1]);
		}
		// GIVE UP TURN //
		SharedMem[0] = 1;
	}
}

void  ChildProcess(int  SharedMem[])
{
	// Loop endlessly.
	while(1){
    sleep(random_int(0, 5));
    // Wait for our turn.
		while(SharedMem[0] != 1) { }
		// CRITICAL SECTION //
		int balance = random_int(0, 100);
		printf("Poor Student needs $%d\n", balance);
		if(balance <= SharedMem[1]){
			SharedMem[1] -= balance;
			printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, SharedMem[1]);
		}else{
			printf("Poor Student: Not Enough Cash ($%d)\n", SharedMem[1]);
		}
		// GIVE UP TURN //
		SharedMem[0] = 0;
	}
}

// https://stackoverflow.com/questions/29381843/generate-random-number-in-range-min-max
int random_int(int min, int max)
{
   return min + rand() % (max+1 - min);
}
