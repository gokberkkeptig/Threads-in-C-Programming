/*Yusuf Gokberk Keptig 2079366*/
/* 
*First in the main we initilize the code we need main thread kept going while other threads are running .
*In philosopher function we have an infinite for loop for all philosophers from different threads
*Also in this function where the philosophers eat and think. We have a function called getForks this is called by the function philosophers whenever a philosopher hungry this 
*function is called. Inside we check if they are any available forks or not if not then it waits for the signal.
*Also we have dropForks function which is called by again philosopher function this changes philosopher state into think. Also calls
*their neigbours to see if there are free forks and if there is available fork in the box. 
*Lastly we print result in result function. Get all the waiting times compare for largest one to find id. And calculate the avareage waiting time. Also I used mutex to solve synchronization problems
*/


#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<time.h>


#define RIGHT (x+1)%n
#define LEFT  (x-1+n)%n

void *philosopher(void *arg);
void getForks(int x);
void dropForks(int x);
void check(int x);

int n;
int forksLeftInTheBox;

pthread_mutex_t lock;
/*T represents Think State, E represents Eating , H represents Hungry*/
char *state;
pthread_cond_t *waitCond;
double *stateDuration;
clock_t *waitTime;
int maxEat = 50;
int foodNumber = 0;


int main()
{
    int i;
    printf("Enter the numbers of philosophers:");
    scanf("%d",&n);

    state =(char *)malloc(sizeof(char)*n); 
    waitCond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t)*n);
    stateDuration = (double *)malloc(sizeof(double)*n);
    waitTime = (clock_t *)malloc(sizeof(clock_t)*n);
    forksLeftInTheBox = n/3;
    printf("\nNumber of the forks starting: %d",forksLeftInTheBox);
    pthread_t th[n];
    pthread_mutex_init(&lock, NULL);
    for ( i = 0; i < n; i++)
    {
        int *index = malloc(sizeof(int));
        *index = i;
        state[i] = 'T';
        stateDuration[i] = 0;
        waitTime[i] = 0;
        if(pthread_create(&th[i], NULL, &philosopher, index)){
          perror("Failed to create thread!");
        }
    }

    for (i = 0; i < n; i++)
    {
      if(pthread_join(th[i],NULL) != 0){
        perror("Failed to join thread!");
      }
    }
}


void *philosopher(void *arg)
{
    int phID = *(int *)arg;
    for(;;){
        srand(time(NULL) + phID);
        int eatingTime = (rand() % 5) + 1;
        int thinkingTime = (rand() % 5) + 1;
        state[phID] = 'T';
        printf("\n%d. philosopher thinks %d secs", phID, thinkingTime);
        sleep(thinkingTime);
        stateDuration[phID] = stateDuration[phID] + thinkingTime;
        getForks(phID);
        printf("\n%d. philosopher eats %d secs", phID, eatingTime);
        sleep(eatingTime);
        dropForks(phID);
    }
}

void result()
{
    double avgTime = 0;
    int i;
    double largest=0.0;
    int ID = 0;

    for ( i = 0; i < n; i++)
    {
        if (largest < stateDuration[i]) {
            largest = stateDuration[i];
            ID = i;
        }
        avgTime = avgTime + stateDuration[i];
    }
    printf("\n Avg hungry time = %lf",  avgTime/n );
  
    printf("\n Largest time = %lf ID:%d ", largest ,ID );

    exit(0);
}

void check(int x)
{
    printf("\n%d. philosopher tries to find forks", x); 
        if (state[x] == 'H' && (state[RIGHT] != 'E' && state[LEFT] != 'E') && forksLeftInTheBox > 0) 
            {
                state[x] = 'E';
                forksLeftInTheBox--;
                if(forksLeftInTheBox == 1){
                    printf("\nThere is only 1 fork left int the box!");
                }
                
                clock_t end = clock();
                double timeSpent = (double)(end - waitTime[x]) / CLOCKS_PER_SEC ;
                stateDuration[x] =  stateDuration[x] + timeSpent;

                pthread_cond_signal(&waitCond[x]);
            }
    
}

void dropForks(int x)
{
   
    pthread_mutex_lock(&lock);
    forksLeftInTheBox++;
    state[x] = 'T';
    printf("\n%d. philosopher finishes eating\n", x);

    foodNumber++;
    printf("\n%d times philosophers ate so far.\n", foodNumber);
    if (foodNumber == maxEat)
    {
        result();
    }

    check(LEFT);
    check(RIGHT);
 
    pthread_mutex_unlock(&lock);
}


void getForks(int x)
{
    pthread_mutex_lock(&lock);
    state[x] = 'H';
    printf("\n%d. philosopher is now hungry state\n", x);

    
    waitTime[x] = clock(); 
    
    check(x);
    if (state[x] != 'E')
    {
        pthread_cond_wait(&waitCond[n], &lock);   
    }

    pthread_mutex_unlock(&lock);
}

