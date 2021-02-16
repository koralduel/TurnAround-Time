#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <linux/random.h>

#define FCFS 1
#define LCFSnotP 2
#define LCFS_P 3
#define RR 4
#define SJF 5

struct Stack { //mahsanit
	int top;
	unsigned capacity;
	int* array;
};

struct Stack* createStack(unsigned capacity)
{
	struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));
	stack->capacity = capacity;
	stack->top = -1;
	stack->array = (int*)malloc(stack->capacity * sizeof(int));
	return stack;
}

int isEmpty(struct Stack* stack){	return stack->top == -1;	}
void push(struct Stack* stack, int item){	stack->array[++stack->top] = item;	}

int pop(struct Stack* stack)
{
	if (isEmpty(stack))
		return -1;
	return stack->array[stack->top--];
}

//return the length of the char
int my_strlen(const char * str)
{
        int counter=0;
        while(str[counter]!='\0')
                counter++;
        return counter;
}

//Threads between strings of char
char* my_strcat(char* destination, const char* source)
{
    char* ptr = destination + my_strlen(destination);
    while (*source != '\0')
        *ptr++ = *source++;
    *ptr = '\0';
    return destination;
}

//Copies a string of char  from one variable to another
char* my_strcpy(char* destination, const char* source)
{
    if (destination == NULL)
        return NULL;
    char *ptr = destination;
    while (*source != '\0')
    {
		*destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
    return ptr;
}

//swap between 2 char
void swap(int* x, int* y) {
        int t = *x;
        *x = *y;
        *y = t;
}

int readLine(char* buf1,char*buf2,int start)
{
        int counter=0;
        while(buf1[start]!='\n')
                buf2[counter++]=buf1[start++];

        buf2[counter++]=buf1[start++];
        return start;
}

//Insert data to array
void data(int index, char *file, int *arrive, int * cpu, int size)
{
	for(int i=0; i< size; i++)
	{
		char line[100];
		index = readLine(file,line,index);
		char buff[100];
		char buff2[100];
		int k = 0;
		while(line[k]!=',')
		{
			buff[k] = line[k];
			k++;
		}
		k++;
		int index = 0;
		while(line[k]!= '\n')
		{
			buff2[index] = line[k];
			k++;
			index++;
		}
		arrive[i] = atoi(buff);
		cpu[i] = atoi(buff2);
	}
}

void bSort(int *arrA,int *arrB, int n)
{
   int i, j;
   for (i = 0; i < n-1; i++)
       	for (j = 0; j < n-i-1; j++)
           	if (arrA[j] > arrA[j+1])
			{
             	swap(&arrA[j], &arrA[j+1]);
				swap(&arrB[j], &arrB[j+1]);
			}
}

void findWaitingTimeFCFS(int* arriveTime,int length,int* cputime, int waitTime[])
{
    waitTime[0] = 0;
    for (int  i = 1; i < length ; i++)
	{
		if(waitTime[i-1] + cputime[i-1] + arriveTime[i-1] < arriveTime[i])
			waitTime[i]=0;
		else
			waitTime[i] = cputime[i-1]+waitTime[i-1]-(arriveTime[i]-arriveTime[i-1]);
	}
}
void findWaitingTimeLCFSnp(int* arriveTime,int length,int* cputime, int waitTime[])
{
	waitTime[0] = 0;
	int q[length];
	int index_q = 1 ;
	struct Stack* m = createStack(length);
	int indexAT = 1;
	int indexWT = 1;
 	struct Stack* mCPU = createStack(length);
	int cpu[length]; // q cpu
	q[0] = arriveTime[0];
	cpu[0]=cputime[0];

	while (index_q < length-1)
	{
		if (indexAT < length)
		{
			if(arriveTime[indexAT] <= waitTime[indexWT-1]+cpu[index_q-1]+q[index_q-1])
			{
				push(m,arriveTime[indexAT]);
				push(mCPU,cputime[indexAT]);
				indexAT++;
			}
			else
			{
				if(isEmpty(m))
				{
					q[index_q] = arriveTime[indexAT];
					cpu[index_q] =cputime[indexAT];
					index_q++;
					indexAT++;
				}
				else
				{
					q[index_q] = pop(m);
					cpu[index_q] = pop(mCPU);
					waitTime[index_q] = waitTime[index_q-1]+cpu[index_q-1]+q[index_q-1]-q[index_q];
					if(waitTime[index_q]<=0)
						waitTime[index_q]=0;
					index_q++;
				}
			}
		}
		else
		{
			while(!isEmpty(m))
			{
				q[index_q] = pop(m);
                                cpu[index_q] = pop(mCPU);
                                waitTime[index_q] = waitTime[index_q-1]+cpu[index_q-1]+q[index_q-1]-q[index_q];
                                if(waitTime[index_q]<=0)
                                        waitTime[index_q]=0;
                                index_q++;
			}
		}
	}
}
void findWaitingTimeLCFSp(int* arriveTime,int length,int* cputime, int waitTime[])
{
	waitTime[length-1] = 0;
	for(int i=length-2; i>=0; i--)
	{
		if ( arriveTime[i] + cputime[i]<arriveTime[i+1])
			waitTime[i] = 0;
		else
			waitTime[i] = waitTime[i+1] + cputime[i+1];
	}
}
void findWaitingTimeRR(int* arriveTime, int length, int* cputime, int waitTime[])
{
	int quantum = 2;
	int i=0;
	int totalE=0;
	int totalW =0;
	int flag=0;
	int remainProcess=length;
	int remainTime[length];
	int index=0;
	int tempflag=0;

	for(int i=0;i<length;i++)
		remainTime[i]=cputime[i];
	 while(remainTime[index]==0)
        {
            if(totalE==0)
                totalE++;
            totalE+=arriveTime[index+1]-arriveTime[index];
            index++;
            remainProcess--;
            waitTime[index]=0;
        }
    for(i=index;remainProcess!=0;)
    {
        tempflag=0;
        if(remainTime[i]!=0)
        {
     	   if(remainTime[i]<=quantum && remainTime[i]>0){
        	totalE+=remainTime[i];
            	tempflag=1;
            	remainTime[i]=0;
            	flag=1;
        	}
		else if(remainTime[i]>0){
           		remainTime[i]-=quantum;
            		totalE+=quantum;
            		tempflag=1;
        	}

        	if(flag==1 && remainTime[i]==0)
		{
            		if(totalE-arriveTime[i]-cputime[i] <=0){
                		waitTime[i] = 0;
                		totalW+=0;
            		}
            		else
            		{
                		waitTime[i]=totalE-arriveTime[i]-cputime[i];
                		totalW+=totalE-arriveTime[i]-cputime[i];
            		}
            		flag=0;
            		remainProcess--;
       	 	}
   	 }
        if(i==length-1)
            i=0;
        else if(arriveTime[i+1]<=totalE)
            i++;
        else if(tempflag==0)
            totalE+=1;
        else
            i=0;
    }
}
void findWaitingTimeSJF(int* arriveTime, int length, int* cputime, int waitTime[])
{
	int i, smallest, count = 0, time=0;
	int end;
	int remainTime[length+1];
	for(int i=0;i<length;i++)
                remainTime[i]=cputime[i];
	remainTime[length]=9999;
      while(cputime[time]==0)
      {
          count++;
          time++;
      }
      for(time; count != length; time++)
      {
            smallest = length;
            for(i = 0; i < length; i++)
            {
                  if(arriveTime[i] <= time && remainTime[i] < remainTime[smallest] && remainTime[i] > 0)
                  {
                        smallest = i;
                  }
            }
            remainTime[smallest]--;
            if(remainTime[smallest] == 0)
            {
                  count++;
                  end = time+1;
                  waitTime[smallest] =end - arriveTime[smallest] - cputime[smallest];
            }
      }
}
void findTurnAroundTime( int *arraiveTime, int n,int *cputime, int waitTime[], int turnAround[]) 
{
    for (int  i = 0; i < n ; i++)
        turnAround[i] = cputime[i] + waitTime[i];
}

void turnAroundTimeRR (int* arriveTime, int length, int* cputime, int waitTime[],int tr[])
{
	int sumW=0,sumC=0;;
	for(int i=0;i<length;i++)
	{
		sumW+= waitTime[i];
		sumC+= cputime[i];
	}
	tr[0]=sumW+sumC;
}

float find_avg(int *aroundArr,int length)
{
	float total_tat = 0;
	for (int i = 0; i < length; i++)
		total_tat = total_tat + aroundArr[i];
	float avg = (float)total_tat / (float)length;
	return avg;
}

void find_operation(int *arriveTime, int *cpuTime,int length,int op)
{
	int waitArr[length];
	int aroundArr[length];
	if (op == FCFS)
	{
		findWaitingTimeFCFS(arriveTime, length, cpuTime, waitArr);
		findTurnAroundTime(arriveTime, length, cpuTime, waitArr, aroundArr);
		float avg = find_avg(aroundArr, length);
		printf("FCFS : %f\n", avg);
	}
	if (op == LCFSnotP)
	{
		findWaitingTimeLCFSnp(arriveTime, length, cpuTime, waitArr);
		findTurnAroundTime(arriveTime, length, cpuTime, waitArr, aroundArr);
		float avg = find_avg(aroundArr, length);
		printf("LCFS NOT PREEMPTIVE :%f\n",avg);
	}
	if (op == LCFS_P)
	{
		findWaitingTimeLCFSp(arriveTime, length, cpuTime, waitArr);
		findTurnAroundTime(arriveTime, length, cpuTime, waitArr, aroundArr);
		float avg = find_avg(aroundArr, length);
		printf("LCFS PREEMPTIVE : %f\n", avg);
	}
	if (op == RR)
	{
		findWaitingTimeRR(arriveTime, length, cpuTime, waitArr);
		turnAroundTimeRR(arriveTime, length, cpuTime, waitArr, aroundArr);
		float avg =(float)aroundArr[0]/(float)length;
		printf("Round Robin :%f \n",avg);
	}
	if (op == SJF)
	{
		findWaitingTimeSJF(arriveTime, length, cpuTime, waitArr);
		findTurnAroundTime(arriveTime, length, cpuTime, waitArr, aroundArr);
		float avg = find_avg(aroundArr, length);
		printf("SJF :%f \n",avg);
	}
}


int main(int argc,char *argv[])
{
	if(argc!=2)
	{
		printf("Not enough arguments");
		exit(-1);
	}
	int inputFd = open(argv[1],O_RDONLY);
	if(inputFd < 0)
	{
		printf("Reading from file error");
		exit(-1);
	}
	char text[1000];
	char line[1000];
	read(inputFd,text,1000);
	close(inputFd);
	int index = readLine(text,line,0);
	int length  = atoi(line);
	int *arrivelTimePtr = (int*)malloc(length*sizeof(int));
	int *cpuTimePtr =(int*)malloc(length*sizeof(int)); 
	if(arrivelTimePtr == NULL || cpuTimePtr == NULL)
	{
		printf("Error in memory allocation");
		exit(-1);
	}
	data(index,text,arrivelTimePtr,cpuTimePtr,length);
	bSort(arrivelTimePtr,cpuTimePtr,length);

	find_operation(arrivelTimePtr, cpuTimePtr, length, FCFS);
	find_operation(arrivelTimePtr, cpuTimePtr, length, LCFSnotP);
	find_operation(arrivelTimePtr, cpuTimePtr, length, LCFS_P);
	find_operation(arrivelTimePtr, cpuTimePtr, length, RR);
	find_operation(arrivelTimePtr, cpuTimePtr, length, SJF);

	printf("\n\n");
	free(arrivelTimePtr);
	free(cpuTimePtr);
}
