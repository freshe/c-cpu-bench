/* 
 * Simple cpu test
 * Fredrik B (info@freshbit.se)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifdef WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
extern pthread_mutex_t primelock;
extern pthread_mutex_t totlock;
#endif

extern unsigned long _primecount;
extern unsigned long _totalcount;

struct ft
{
	unsigned long f;
	unsigned long t;
};

void showprogress(unsigned long *);
int getprocessors();
int isprime(unsigned long);
void increment_primecount();
void increment_totalcount();
void numbercruncher(unsigned long, unsigned long);
#ifdef WINDOWS
unsigned __stdcall cputhread(void *);
LPSYSTEM_INFO getsysteminfo();
#else
void * cputhread(void *);
#endif
void parseargs(int argc, char **, unsigned long *, int *);
void die(char * msg);
