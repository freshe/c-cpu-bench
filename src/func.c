/* 
 * Simple cpu test
 * Fredrik B (info@freshbit.se)
 */

#include "cpu.h"

void showprogress(unsigned long *n)
{
	double progress = 0;
	while(_totalcount < (*n))
	{
		progress = 100.0 * _totalcount / (*n);
		
		printf("\r");
		printf("%d%% (%lu/%lu)", (int)progress, _totalcount, *n);
		fflush(stdout);
#ifdef WINDOWS
		Sleep(1000);
#else
		sleep(1);
#endif
	}
}

int getprocessors()
{
	int nCpus = 1;
#ifdef WINDOWS
	LPSYSTEM_INFO info = malloc(sizeof(SYSTEM_INFO));
	if (info == NULL) 
	{
		die("Error allocating memory");
	}
	GetNativeSystemInfo(info);
	nCpus = (int)info->dwNumberOfProcessors;
	free(info);
#else
	nCpus = (int)sysconf(_SC_NPROCESSORS_ONLN);
#endif
	if (nCpus < 1) nCpus = 1;
	return nCpus;
}

int isprime(unsigned long n)
{
	if (n <= 1) return 0;
	
	unsigned long j, start = ceil(sqrt(n));
	int ispr = 1;

	for (j = start; j >= 2; j--)
	{
		if (n % j == 0) 
		{
			ispr = 0;
			break;
		}
	}
	return ispr;
}

void increment_primecount()
{
#ifdef WINDOWS
	InterlockedExchangeAdd(&_primecount, 1);
#else
	pthread_mutex_lock(&primelock);
	_primecount++;
	pthread_mutex_unlock(&primelock);
#endif
}

void increment_totalcount()
{
#ifdef WINDOWS
	InterlockedExchangeAdd(&_totalcount, 1);
#else
	pthread_mutex_lock(&totlock);
	_totalcount++;
	pthread_mutex_unlock(&totlock);
#endif
}

void numbercruncher(unsigned long f, unsigned long t)
{
	unsigned long i;
	for (i = f; i <= t; i++)
	{
		if (isprime(i))
			increment_primecount();
		increment_totalcount();
	}
}

#ifdef WINDOWS
unsigned __stdcall cputhread(void *args)
{
	struct ft *ftptr = (struct ft *)args;
	numbercruncher(ftptr->f, ftptr->t);
	_endthreadex(0);
	return 0;
}
#else
void *cputhread(void *args)
{
	struct ft *ftptr = (struct ft *)args;
	numbercruncher(ftptr->f, ftptr->t);
	pthread_exit(0);
	return NULL;
}
#endif

void parseargs(int argc, char *argv[], unsigned long *n, int *t)
{
	int i;
	for(i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (argc > i+1)
			{
				switch(argv[i][1])
				{
					case 't': 
						*t = atol(argv[i+1]);
						break;
					case 'n':
						*n = atol(argv[i+1]);
						break;
					default:
						break;
				}
			}
			else 
			{
				die("\nInvalid arguments! -t [number of threads] -n [number to crunch]\n");
			}
		}
	}
}

void die(char * msg)
{
	printf("\n%s\n", msg);
	exit(1);
}