/* 
 * Simple cpu bench
 * Fredrik B (info@freshbit.se)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
extern pthread_mutex_t _prime_count_lock;
extern pthread_mutex_t _total_count_lock;
#endif

#define MAX_NUMBER_OF_THREADS 1024
#define DEFAULT_NUMBER 100000000
#define PROGRESS_BAR_LENGTH 30

extern unsigned long _prime_count;
extern unsigned long _total_count;

struct range {
    unsigned long from;
    unsigned long to;
};

void print_progress(unsigned long);
void print_progress_until_complete(unsigned long);
int get_processor_count();
int is_prime(unsigned long);
void increment_prime_count();
void increment_total_count();
void crunch_range(unsigned long, unsigned long);
void parse_args(int, char**, unsigned long *, int *);
void die(char *);

struct range *get_ranges(unsigned long, int);
void free_ranges(struct range *); 

#ifdef _WIN32
unsigned __stdcall crunch_range_on_thread(void *);
LPSYSTEM_INFO get_systeminfo();
#else
void *crunch_range_on_thread(void *);
#endif