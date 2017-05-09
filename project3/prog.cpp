#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

struct s
{
	float value;
	int pad[NUMPAD];
} Array[4];

void fix1()
{
	omp_set_num_threads( NUMT );
	const int SomeBigNumber = 100000000;	// keep < 2B
	double startTime = omp_get_wtime();

	#pragma omp parallel for
	for( int i = 0; i < 4; i++ )
	{
		unsigned int seed = 0;		// automatically private
		for( unsigned int j = 0; j < SomeBigNumber; j++ )
			Array[ i ].value = Array[ i ].value + (float)rand_r( &seed );
	}

	double endTime = omp_get_wtime();
	fprintf(stdout, "FIX 1 | Perf: %8.2lf\n", (double) (4 * SomeBigNumber)/(endTime - startTime)/1000000.);

	return;
}

void fix2()
{
	omp_set_num_threads( NUMT );
	const int SomeBigNumber = 100000000;
	double startTime = omp_get_wtime();

	#pragma omp parallel for
	for( int i = 0; i < 4; i++ )
	{
		unsigned int seed = 0;
		float temp = Array[i].value;

		for( int j = 0; j < SomeBigNumber; j++)
			temp = temp + (float)rand_r(&seed);

		Array[i].value = temp;
	}

	double endTime = omp_get_wtime();
	fprintf(stdout, "FIX 2 | Perf: %8.2lf\n", (double) (4 * SomeBigNumber)/(endTime - startTime)/1000000.);

	return;
}

int main()
{
	fix1();
	//fix2();
	
	return 0;
}
