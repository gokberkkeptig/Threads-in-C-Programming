/*Yusuf Gokberk Keptig 2079366*/

#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

/*Function Prototypes*/
void displayArr(int **arr,int m);
void* shiftingOperation(void *arg); 
int** shiftingRows(int **arr,int n,int m);
int** shiftingColumns(int **arr,int n, int m);

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int **arr;
int everyThread = 0 ,lastThread = 0;
int d,m,s;
pthread_mutex_t mutex;
int counter=0;

int main(int argc, char* argv[]){
	FILE *inputFile;
	int i,j=0;
	
	for(;;){
		inputFile = fopen("input.txt","r");
		fscanf(inputFile, "%d",&m);
		printf("Enter d Value:");
		scanf("%d",&d);
		if(d>m || d <= 0){
			printf("\nThe value you entered must be smaller than m value and it must be bigger than 0.\n");
			continue;
		}
		everyThread = m/d;
		if(m % d != 0){
			lastThread = (m/d) + (m%d);	
		}
		printf("\nEvery Th: %d, last Th: %d",everyThread,lastThread);
		printf("\nEnter s value:");
		scanf("%d",&s);
		
		arr = (int **)malloc(sizeof(int *) * m);
		
    		for (i=0; i<m; i++)
         		arr[i] = (int *)malloc(sizeof(int) * m);
         		
		int readNumber;
		i=0;
		while(fscanf(inputFile, "%d",&readNumber)==1){
			arr[i][j]=readNumber;
			j++;
			if(j==m){
				j=0;
				i++;
			}	
		}
		
		printf("Initial Matrix");
		displayArr(arr,m);
		
		
		pthread_t *th = malloc(sizeof(pthread_t)*d);
		
		pthread_mutex_init(&mutex,NULL);

		for(i=0;i<d;i++){
			int *index = malloc(sizeof(int)); 
			*index = i;
			if((pthread_create(&th[i],NULL,&shiftingOperation, index)) != 0){
				perror("Failed to create thread!");
			}
				
		}
		for(i=0;i<d;i++){
			if(pthread_join(th[i],NULL) != 0){
				perror("Failed to join thread!");
			}
		
		}
	
		printf("\n----------FINAL-----------");
		displayArr(arr,m);
		fclose(inputFile);
		
		break;
	}
	
				
	pthread_mutex_destroy(&mutex);


	free(arr);

	return 0;	
}

void displayArr(int **arr,int m){
 	printf("\n    ");
        int x,j,y;
        for ( x = 1; x <= m; x++)
        {
            printf("%d   ",x);
        }
        printf("  \n");
        for ( x = 0; x < m; x++)
        {
            printf("  ");
            for ( j = 0; j < m; j++)
            {
                printf("+---");
            }
            printf("+\n");
            printf("%d",x+1);

            for ( y = 0; y < m; y++)
            {
                    printf(" | %d", arr[x][y]);
            }
            printf(" | \n");
        }
        printf("  ");
        for ( j = 0; j < m; j++)
        {
            printf("+---");
        }
        printf("+\n");
}


void* shiftingOperation(void *arg){

	int thIndex = *(int*)arg;
	int begins,ends,i,k;
	
	begins = thIndex*everyThread;
	
	ends = begins+everyThread-1;
	if(m%d != 0){
		if(thIndex ==(d-1)){
			ends = begins+lastThread-1;
		}
	}
	

	for(i =0 ; i<s ; i++ ){
		for(k=begins;k<=ends;k++){
			
			arr = shiftingRows(arr,k,m);	
			displayArr(arr,m);
			
			
		}
		/*This loop makes threads wait until row shifts finish*/
		for(;;){
			if(!(counter < d))
				break;
		}
		sleep(1);
		
		
		for(k=begins;k<=ends;k++){
			
			arr = shiftingColumns(arr,k,m);	

			displayArr(arr,m);
		}
		/*This loop makes threads wait until column shifts finish*/
		for(;;){
			if(!(counter > 0))
				break;
		}
		sleep(1);
		
		
	} 	


}


int** shiftingRows(int **arr,int n,int m){
	
	int temp,k;
	temp = arr[n][m-1];
	for(k=m-1;k>0;k--)
	{
	    arr[n][k] = arr[n][k-1];
	}
	arr[n][0] = temp;
	pthread_mutex_lock(&mutex);	
	counter++;
	pthread_mutex_unlock(&mutex);
	
	return arr;
}

int** shiftingColumns(int **arr,int n,int m){

	int k;
	int temp = arr[0][n];
	for(k=0;k<m-1;k++){
		arr[k][n] = arr[k+1][n];
	}
	arr[m-1][n] = temp;

	pthread_mutex_lock(&mutex);
	counter--;
	pthread_mutex_unlock(&mutex);

	return arr;
}


