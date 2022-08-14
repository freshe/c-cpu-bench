/* 
 * Simple cpu test
 * Fredrik B (info@freshbit.se)
 */

#include "cpu.h"

unsigned long _primecount = 0;
unsigned long _totalcount = 0;

#ifndef WINDOWS
pthread_mutex_t primelock;
pthread_mutex_t totlock;
#endif

int main(int argc, char **argv)
{
	//Get number of cpus
	int nThreads = getprocessors();
#ifdef WINDOWS
	HANDLE thread[nThreads];
#else
	pthread_t thread[nThreads];
#endif

	//Standard values
	const unsigned long defaultNumber = 100000000;
	unsigned long bigNumber = defaultNumber, from = 1, tom;
	int i;
	
	//If arguments was passed
	if (argc >1)
		parseargs(argc, argv, &bigNumber, &nThreads);
	
	//No use unless a decent number size
	if (bigNumber <= 0)
		bigNumber = defaultNumber;
	
	//Calculate number of operations per thread
	unsigned long per_thread = ceil( (bigNumber / nThreads) );
	struct ft ftptr[nThreads];
	unsigned threadId[nThreads];
	
	printf("Crunching primes from number %ld using %d threads", bigNumber, nThreads);
	fflush(stdout);

#ifndef WINDOWS
	if (pthread_mutex_init(&primelock, NULL) != 0)
		die("pthread_mutex_init failed");
	if (pthread_mutex_init(&totlock, NULL) != 0)
		die("pthread_mutex_init failed");
#endif

	//Start time
	time_t start, end, diff;
	time(&start);
	
	for(i = 0; i < nThreads; i++)
	{
		struct ft fromtom;
		tom = from + per_thread;
		if (tom > bigNumber) tom = bigNumber;
		
		fromtom.f = from;
		fromtom.t = tom;
		ftptr[i] = fromtom;
				
		/*
		 *	_beginthreadex returns 0 on error
		 *	pthread returns 0 on success :)
		 */
		 
#ifdef WINDOWS
		thread[i] = (HANDLE)(intptr_t)_beginthreadex(NULL, 0, &cputhread, &ftptr[i], 0, &threadId[i]);
		if (thread[i] == 0)
			die("Error creating thread");
#else	
		if (pthread_create(&thread[i], NULL, &cputhread, &ftptr[i]) != 0)
			die("Error creating thread");
#endif
		from += per_thread + 1;
	}

	printf("\n");
	showprogress(&bigNumber);

#ifdef WINDOWS
	WaitForMultipleObjects(nThreads, thread, 1, INFINITE);
#endif

	for(i = 0; i < nThreads; i++)
	{
#ifdef WINDOWS
		CloseHandle(thread[i]);
#else
		if (pthread_join(thread[i], NULL))
			die("Error joining thread");
#endif	
	}
	
	//End time
	time(&end);
	diff = end - start;
	
	printf("\nFound %ld primes in %ld seconds\n", _primecount, diff);
	
#ifndef WINDOWS
	pthread_mutex_destroy(&primelock);
	pthread_mutex_destroy(&totlock);
#endif
	return 0;
}