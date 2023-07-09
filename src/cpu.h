/*
 MIT License
 
 Copyright (c) Fredrik B
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
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

extern unsigned long long _prime_count;
extern unsigned long long _total_count;

struct range {
    unsigned long long from;
    unsigned long long to;
};

void print_progress(unsigned long long);
void print_progress_until_complete(unsigned long long);
int get_processor_count();
int is_prime(unsigned long long);
void increment_prime_count(unsigned long long);
void increment_total_count(unsigned long long);
void crunch_range(unsigned long long, unsigned long long);
void parse_args(int, char**, unsigned long long *, int *);
void die(char *);

struct range *get_ranges(unsigned long long, int);
void free_ranges(struct range *); 

#ifdef _WIN32
unsigned __stdcall crunch_range_on_thread(void *);
LPSYSTEM_INFO get_systeminfo();
#else
void *crunch_range_on_thread(void *);
#endif