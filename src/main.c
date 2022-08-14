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

void print_header() 
{
	printf("\nsimple cpu benchmark\n");
	printf("freshbit.se\n\n");
	fflush(stdout);
}

int main(int argc, char **argv)
{
	print_header();

	int number_of_threads = get_processor_count();
	
#ifdef _WIN32
	HANDLE threads[number_of_threads];
	unsigned thread_ids[number_of_threads];
#else
	pthread_t threads[number_of_threads];
#endif
	const unsigned long default_number = 100000000;
	unsigned long big_number = default_number;
	unsigned long from_number, to_number;
	int i;
	
	if (argc >1)
		parse_args(argc, argv, &big_number, &number_of_threads);

	if (big_number <= 0)
		big_number = default_number;
	
	/* Number of operations per thread */
	unsigned long numbers_per_thread = ceil( (big_number / number_of_threads) );
	struct range ranges[number_of_threads];
	
	printf("crunching primes from number %ld using %d threads\n", big_number, number_of_threads);
	fflush(stdout);

#ifndef _WIN32
	if (pthread_mutex_init(&_prime_count_lock, NULL) != 0)
		die("pthread_mutex_init failed");
	if (pthread_mutex_init(&_total_count_lock, NULL) != 0)
		die("pthread_mutex_init failed");
#endif
	time_t start_time, end_time, elapsed_time;
	time(&start_time);

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
		printf("f: %ld t: %ld\n", from_number, to_number);
		#endif

		/*
		 *	_beginthreadex returns 0 on error
		 *	pthread returns 0 on success :)
		 */
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
	show_progress(&big_number);

#ifdef _WIN32
	WaitForMultipleObjects(number_of_threads, threads, 1, INFINITE);
#endif

	for (i = 0; i < number_of_threads; i++)
	{
#ifdef _WIN32
		CloseHandle(thread[i]);
#else
		if (pthread_join(threads[i], NULL) != 0)
			die("Error joining thread");
#endif	
	}
	
	time(&end_time);
	elapsed_time = end_time - start_time;
	
	printf("\nFound %ld primes in %ld seconds\n", _prime_count, elapsed_time);
	
#ifndef _WIN32
	pthread_mutex_destroy(&_prime_count_lock);
	pthread_mutex_destroy(&_total_count_lock);
#endif

	return 0;
}