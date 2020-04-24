

#include "includes.h"

#define TASK_STK_SIZE 512 // Size of each task's stacks
#define N_TASKS 2		  // Number of identical tasks

OS_STK TaskStk[N_TASKS][TASK_STK_SIZE]; // Tasks stacks
OS_STK TaskStartStk[TASK_STK_SIZE];
char a;
OS_EVENT *RandomSem;

void Task1(void *data);
void Task2(void *data);
void TaskStart(void *data);

void main(void)
{
	PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK); // Clear the screen

	OSInit(); // Initialize uC/OS-II

	PC_DOSSaveReturn();		   // Save environment to return to DOS
	PC_VectSet(uCOS, OSCtxSw); // Install uC/OS-II's context switch vector

	RandomSem = OSSemCreate(1); // Random number semaphore

	OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0); // Create Task
	OSStart(); // Start multitasking
}

void TaskStart(void *pdata) // Manage the task
{
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR cpu_sr;
#endif
	char s[100];
	INT16S key;

	pdata = pdata; // Prevent compiler warning

	OS_ENTER_CRITICAL(); // To disable interrupt
	PC_VectSet(0x08, OSTickISR); // Context Switcher handler
	PC_SetTickRate(OS_TICKS_PER_SEC);
	OS_EXIT_CRITICAL(); // Reenable interrupt

	OSStatInit(); // Can be used to determine speed of CPU

	OSTaskCreate(Task1, (void *)&a, &TaskStk[0][TASK_STK_SIZE - 1], 1); // Creating Task1
	OSTaskCreate(Task2, (void *)&a, &TaskStk[1][TASK_STK_SIZE - 1], 2); // Creating Task2

	for (;;) // Just for exit the program using Escape key
	{

		if (PC_GetKey(&key) == TRUE)
		{
			if (key == 0x1B)
			{
				PC_DOSReturn();
			}
		}

		OSCtxSwCtr = 0;
		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}

void display(char c) // Display character
{
	putchar(c);
}

void Task1(void *pdata) // Task 1
{
	char ch = '1';
	INT8U err;
	for (;;)
	{
		OSSemPend(RandomSem, 0, &err);
		display(ch);
		OSSemPost(RandomSem);
		OSTimeDlyHMSM(0, 0, 0, 500);
	}
}

void Task2(void *pdata) // Task 2
{
	char ch = '2';
	INT8U err;
	for (;;)
	{
		OSSemPend(RandomSem, 0, &err);
		display(ch);
		OSSemPost(RandomSem);
		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}
