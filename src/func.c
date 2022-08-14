/* 
 * Simple cpu bench
 * Fredrik B (info@freshbit.se)
 */

#include "cpu.h"

void show_progress(unsigned long *n)
{
	double progress = 0;
	
	while(_total_count < (*n))
	{
		progress = 100.0 * _total_count / (*n);
		
		printf("\r");
		printf("%d%% (%lu/%lu)", (int)progress, _total_count, *n);
		fflush(stdout);
#ifdef _WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}
}

int get_processor_count()
{
	int count = 1;

#ifdef _WIN32
	LPSYSTEM_INFO info = malloc(sizeof(SYSTEM_INFO));
	if (info == NULL) 
	{
		die("Error allocating memory");
	}
	GetNativeSystemInfo(info);
	count = (int)info->dwNumberOfProcessors;
	free(info);
#else
	count = (int)sysconf(_SC_NPROCESSORS_ONLN);
#endif
	if (count < 1)
		count = 1;

	return count;
}

int is_prime(unsigned long number)
{
	if (number <= 1) 
		return 0;
	
	unsigned long j, start = ceil(sqrt(number));
	int is_prime = 1;

	for (j = start; j >= 2; j--)
	{
		if (number % j == 0) 
		{
			is_prime = 0;
			break;
		}
	}

	return is_prime;
}

void increment_prime_count()
{
#ifdef _WIN32
	InterlockedExchangeAdd(&_prime_count, 1);
#else
	pthread_mutex_lock(&_prime_count_lock);
	_prime_count++;
	pthread_mutex_unlock(&_prime_count_lock);
#endif
}

void increment_total_count()
{
#ifdef _WIN32
	InterlockedExchangeAdd(&_total_count, 1);
#else
	pthread_mutex_lock(&_total_count_lock);
	_total_count++;
	pthread_mutex_unlock(&_total_count_lock);
#endif
}

void crunch_range(unsigned long from, unsigned long to)
{
	unsigned long i;

	for (i = from; i <= to; i++)
	{
		if (is_prime(i))
			increment_prime_count();
		
		increment_total_count();
	}
}

#ifdef _WIN32
unsigned __stdcall crunch_range_on_thread(void *args)
{
	struct range *thread_range = (struct range *)args;
	crunch_range(thread_range->from, thread_range->to);
	_endthreadex(0);
	return 0;
}
#else
void *crunch_range_on_thread(void *args)
{
	struct range *thread_range = (struct range *)args;
	crunch_range(thread_range->from, thread_range->to);
	pthread_exit(0);
	return NULL;
}
#endif

void parse_args(int argc, char *argv[], unsigned long *n, int *t)
{
	int i;

	for(i = 1; i < argc; i++)
	{
		if (argv[i][0] != '-')
		{
			continue;
		}

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

void die(char *msg)
{
	printf("\n%s\n", msg);
	exit(1);
}