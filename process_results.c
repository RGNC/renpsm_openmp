#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define THREADS 8
#define ITERATIONS 30

int starts_with(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}



int main(int argc, char* argv[])
{
	
	
	if (argc!=2) {
		printf("Error: you should include the benchmark output file\n");
		exit(1);
	}

	FILE* fp = fopen(argv[1],"r");
	
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    double times[THREADS][ITERATIONS];
	
	int aux,threads=0;
	int iteration=0;
	
	int i=-1;
	int j=0;
	
	if (fp==NULL) {
		printf("Error opening %s\n",argv[1]);
		exit(1);
	}
	
	 while ((read = getline(&line, &len, fp)) != -1) {
       if (starts_with(line,"Threads")) {
		   sscanf(line,"Threads: %d Iteration: %d",&aux,&iteration);
		   if (aux!=threads) {
			   threads=aux;
			   i++;
		   }
		   j = iteration-1;
	   } else if (starts_with(line,"Wall")) {
		   sscanf(line,"Wall time: %lf seconds",&times[i][j]);
		  
	   }
    }
	
	double average[THREADS];
	double sd[THREADS];
	int t=1;
	for (i=0;i<THREADS;i++) {
		average[i]=0;
		for (j=0;j<ITERATIONS;j++) {
			average[i] += times[i][j];
		}
		average[i] /= (double)ITERATIONS;
		sd[i]=0;
		for (j=0;j<ITERATIONS;j++) {
			sd[i] += (times[i][j] - average[i])*(times[i][j] - average[i]);
		}
		sd[i] /= (double)(ITERATIONS - 1);
		sd[i] = sqrt(sd[i]);
		printf("Threads: %d. Average time: %lf seconds. Sd: %lf. Speed-up %lf\n",t,average[i], sd[i], average[0]/average[i]);
		t<<=1;
	}
	
	fclose(fp);
	
	
	return 0;
	
}


