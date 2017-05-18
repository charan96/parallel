#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>

int NowYear;		// 2017 - 2022
int NowMonth;		// 0 - 11

float NowPrecip;		// inches of rain per month
float NowTemp;		// temperature this month
float NowHeight;		// grain height in inches
int NowNumDeer;		// number of deer in the current population
int locust;

const float GRAIN_GROWS_PER_MONTH = 8.0;
const float ONE_DEER_EATS_PER_MONTH = 0.5;

const float AVG_PRECIP_PER_MONTH = 6.0;	// average
const float AMP_PRECIP_PER_MONTH = 6.0;	// plus or minus
const float RANDOM_PRECIP = 2.0;	// plus or minus noise

const float AVG_TEMP = 50.0;	// average
const float AMP_TEMP = 20.0;	// plus or minus
const float RANDOM_TEMP = 10.0;	// plus or minus noise

const float MIDTEMP = 40.0;
const float MIDPRECIP = 10.0;

unsigned int seed = 0;  // a thread-private variable

float SQR(float x);
float Ranf(unsigned int *seedp, float low, float high);
int Ranf(unsigned int *seedp, int ilow, int ihigh );
float calculateAngle();
float calculateTemp();
float calculatePrecip();
void GrainDeer();
void Grain();
void Watcher();
void locusts();
float convertCM(float height);
float convertC(float temp);

std::ofstream outfile;

int main( int argc, char *argv[ ] )
{
#ifndef _OPENMP
	fprintf( stderr, "OpenMP no supported\n");
	return 1;
#endif

	outfile.open("output.csv");
	outfile << "Date , MonthCtr , Temp , Precip , Deer , Grain , Alien\n";

	NowMonth = 0;
	NowYear = 2017;

	NowNumDeer = 1;
	NowHeight = 1.;

	omp_set_num_threads(4);
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			GrainDeer( );
		}

		#pragma omp section
		{
			Grain( );
		}

		#pragma omp section
		{
			Watcher( );
		}

		#pragma omp section
		{
			locusts( );	// your own
		}
	}       // implied barrier -- all functions must return in order
			// to allow any of them to get past here
	outfile.close();
	return 0;
}

float calculateAngle()
{
	float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );
	return ang;
}

float calculateTemp()
{
	float temp = AVG_TEMP - AMP_TEMP * cos( calculateAngle() );
	temp += Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );
	return temp;
}

float calculatePrecip()
{
	float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( calculateAngle() );
	precip += Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
	if( precip < 0. )
		precip = 0.;
	return precip;
}

float convertC(float temp){
	temp = (5./9.)*(temp-32);
	return temp;
}

float convertCM(float height){
	height *= 2.54;
	return height;
}

float SQR( float x )
{
        return x*x;
}

float Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

int Ranf( unsigned int *seedp, int ilow, int ihigh )
{
        float low = (float)ilow;
        float high = (float)ihigh + 0.9999f;

        return (int)(  Ranf(seedp, low,high) );
}

void GrainDeer()
{
	int currentDeer;
	int ripDeer;
	
	while(NowYear <= 2023)
	{
		currentDeer = 0;
		ripDeer = 0;

		if(locusts)
			ripDeer = Ranf(&seed, 1, NowNumDeer-1);

		if(NowNumDeer <= NowHeight)
			ripDeer++;
		else
			currentDeer += 2;

		// DoneComputing barrier:
		#pragma omp barrier
		NowNumDeer += currentDeer;
		NowNumDeer -= ripDeer;

		if(NowNumDeer <= 0)
			NowNumDeer = 1;

		// DoneAssigning barrier:
		#pragma omp barrier

		// DonePrinting barrier:
		#pragma omp barrier
	}
}

void Grain()
{
	float height;
	while(NowYear <= 2023)
	{
		float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
		float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );
		if(locusts)
			height = NowHeight - Ranf(&seed, (float)0., (float)NowHeight-1.);
		else
			height = 0.;

		// DoneComputing barrier:
		#pragma omp barrier
		NowHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
		NowHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
		NowHeight -= height;
		
		if(NowHeight < 0.)
			NowHeight = 0.1;

		// DoneAssigning barrier:
		#pragma omp barrier

		// DonePrinting barrier:
		#pragma omp barrier
	}
}

void locusts()
{
	while(NowYear <= 2023)
	{
		int tempLocust;

		if(calculateTemp() > AVG_TEMP && calculatePrecip() > AVG_PRECIP_PER_MONTH)
			tempLocust = 1;
		else
			tempLocust = 0;

		// DoneComputing barrier:
		#pragma omp barrier
		locust = tempLocust;

		// DoneAssigning barrier:
		#pragma omp barrier

		// DonePrinting barrier:
		#pragma omp barrier
	}
}

void Watcher()
{
	float newTemp;
	float newPrecip;

	int ctr = 0;

	while(NowYear <= 2023)
	{
		newTemp = calculateTemp();
		newPrecip = calculatePrecip();

		// DoneComputing barrier:
		#pragma omp barrier
		NowTemp = newTemp;
		NowPrecip = newPrecip;
		ctr++;

		// DoneAssigning barrier:
		#pragma omp barrier
		outfile << NowMonth+1 << "/" << NowYear << " , " << ctr << " , " << convertC(NowTemp) << " , " << convertCM(NowPrecip) << " , " << NowNumDeer << " , " << convertCM(NowHeight) << " , " << locust << "\n";
		NowMonth++;
		
		if(NowMonth > 11)
		{
			NowYear++;
			NowMonth = 0;
		}
		// DonePrinting barrier:
		#pragma omp barrier
	}
}
