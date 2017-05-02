#include <iostream>
#include <stdlib.h>
#include <omp.h>
#include <stdio.h>

#define ARRAYSIZE 32*1024

float ARRAY[ARRAYSIZE];

float Ranf( float low, float high )
{
	float r = (float) rand();		// 0 - RAND_MAX
	return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

int main(int argc, char *argv[])
{
	for(int k=0;k<ARRAYSIZE;k++)
		ARRAY[k] = Ranf( -1.f, 1.f );

	omp_set_num_threads(NUMT);
	float prod;

	double time0 = omp_get_wtime();
	
	#pragma omp parallel for schedule(SCHEDULING,CHUNK_SIZE),private(prod)
	for (int i=0;i<ARRAYSIZE;i++)
	{
		prod = 1;
		for (int j=0;j<i;j++)
		{
			prod *= ARRAY[j];
		}
	}

	double time1 = omp_get_wtime();

	long int numMuled = (long int)ARRAYSIZE * (long int)(ARRAYSIZE+1) / 2;
	fprintf(stderr, "MegaMults/sec = %10.2lf\n\n", (double)numMuled/(time1 - time0)/1000000.);
}
