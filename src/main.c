/* 
 * Simple cpu bench
 * Fredrik B (info@freshbit.se)
 */

#include "cpu.h"

unsigned long _prime_count = 0;
unsigned long _total_count = 0;

#ifndef _WIN32
pthread_mutex_t _prime_count_lock;
pthread_mutex_t _total_count_lock;
#endif

#define MAX_NUMBER_OF_THREADS 1024

void print_header() 
{
	printf("\nsimple cpu benchmark\n");
	printf("freshbit.se\n\n");
	fflush(stdout);
}

int main(int argc, char* argv[])
{
	print_header();

	const unsigned long default_number = 100000000;
	unsigned long big_number = default_number;
	
	int number_of_threads = get_processor_count();

	if (argc > 1)
		parse_args(argc, argv, &big_number, &number_of_threads);

	if (number_of_threads > MAX_NUMBER_OF_THREADS)
		die("Too many threads");

	if (big_number <= 0)
		big_number = default_number;

	unsigned long numbers_per_thread = ceil( (big_number / number_of_threads) );

	if (big_number < 100)
	{
		/* don't even bother splitting up the work */
		number_of_threads = 1;
		numbers_per_thread = big_number;
	}

#ifdef _WIN32
	HANDLE threads[MAX_NUMBER_OF_THREADS];
	unsigned thread_ids[MAX_NUMBER_OF_THREADS];
#else
	pthread_t threads[MAX_NUMBER_OF_THREADS];
#endif
	struct range ranges[MAX_NUMBER_OF_THREADS];

#ifndef _WIN32
	if (pthread_mutex_init(&_prime_count_lock, NULL) != 0)
		die("pthread_mutex_init failed");
	if (pthread_mutex_init(&_total_count_lock, NULL) != 0)
		die("pthread_mutex_init failed");
#endif
	printf("crunching primes from number %lu using %d threads\n", big_number, number_of_threads);
	fflush(stdout);

	time_t start_time, end_time, elapsed_time;
	time(&start_time);

	int i;
	unsigned long from_number, to_number;
	
	from_number = 1;

	for (i = 0; i < number_of_threads; i++)
	{
		struct range thread_range;
		to_number = from_number + numbers_per_thread;
		if (to_number > big_number) to_number = big_number;
		
		thread_range.from = from_number;
		thread_range.to = to_number;
		ranges[i] = thread_range;
				
#ifdef DEBUG
		printf("f: %lu t: %lu\n", from_number, to_number);
#endif

		/* _beginthreadex returns 0 on error. pthread returns 0 on success :) */
#ifdef _WIN32
		threads[i] = (HANDLE)(intptr_t)_beginthreadex(NULL, 0, &crunch_range_on_thread, &ranges[i], 0, &thread_ids[i]);
		if (threads[i] == 0)
			die("Error creating thread");
#else	
		if (pthread_create(&threads[i], NULL, &crunch_range_on_thread, &ranges[i]) != 0)
			die("Error creating thread");
#endif
		from_number += numbers_per_thread + 1;
	}

	printf("\n");
	print_progress_until_complete(&big_number);

#ifdef _WIN32
	WaitForMultipleObjects(number_of_threads, threads, 1, INFINITE);
#endif

	for (i = 0; i < number_of_threads; i++)
	{
#ifdef _WIN32
		CloseHandle(threads[i]);
#else
		if (pthread_join(threads[i], NULL) != 0)
			die("Error joining thread");
#endif	
	}
	
	time(&end_time);
	elapsed_time = end_time - start_time;
	
	print_progress(&big_number);
	printf("\nFound %lu primes in %ld seconds\n", _prime_count, elapsed_time);
	
#ifndef _WIN32
	pthread_mutex_destroy(&_prime_count_lock);
	pthread_mutex_destroy(&_total_count_lock);
#endif

	return 0;
}