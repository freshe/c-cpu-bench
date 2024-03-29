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

#include "cpu.h"

struct range *get_ranges(unsigned long long n, int c) 
{
    unsigned long long size = floor(n / c);
    unsigned long long remainder = n % c;
    unsigned long long f, t;
    int i;

    struct range *ranges = malloc(c * sizeof(*ranges));

    if (ranges == NULL)
        die("Error allocating memory");

    f = 1;
    t = size + remainder;

    for (i = 0; i < c; i++)
    {
        struct range item;
        item.from = f;
        item.to = t;
        ranges[i] = item;

        f = t + 1;
        t = f + size;

        if (t > n)
            t = n;
    }

    return ranges;
}

void free_ranges(struct range *ranges) 
{
    free(ranges);
}

void print_progress(unsigned long long n)
{
    char buf[PROGRESS_BAR_LENGTH + 1];

    int progress = ceil(100.0 * _total_count / n);
    int progress_bar_count = ceil(progress / 100.0 * PROGRESS_BAR_LENGTH);
    int i;
    
    printf("\r[");
    
    for (i = 0; i < PROGRESS_BAR_LENGTH; i++)
    {
        if ((i + 1) <= progress_bar_count)
            buf[i] = '=';
        else
            buf[i] = ' ';
    }
    
    buf[i] = '\0';
    printf("%s", buf);
    printf("] %d%% (%llu/%llu)", progress, _total_count, n);

    fflush(stdout);
}

void print_progress_until_complete(unsigned long long n)
{
    while (_total_count < n)
    {
        print_progress(n);
#ifdef _WIN32
        Sleep(10);
#else
        usleep(10000);
#endif
    }
}

int get_processor_count()
{
    int count = 1;

#ifdef _WIN32
    LPSYSTEM_INFO info = malloc(sizeof(SYSTEM_INFO));

    if (info == NULL)
        die("Error allocating memory");
    
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

int is_prime(unsigned long long number)
{
    if (number < 2) 
        return 0;

    if (number == 2 || number == 3)
        return 1;

    if (number % 2 == 0 || number % 3 == 0)
        return 0;
    
    unsigned long long j, end = ceil(sqrt(number));

    for (j = 5; j <= end; j++) {
        if (number % j == 0)
            return 0;
    }

    return 1;
}

void increment_prime_count(unsigned long long n)
{
#ifdef _WIN32
    InterlockedExchangeAdd64(&_prime_count, n);
#else
    pthread_mutex_lock(&_prime_count_lock);
    _prime_count += n;
    pthread_mutex_unlock(&_prime_count_lock);
#endif
}

void increment_total_count(unsigned long long n)
{
#ifdef _WIN32
    InterlockedExchangeAdd64(&_total_count, n);
#else
    pthread_mutex_lock(&_total_count_lock);
    _total_count += n;
    pthread_mutex_unlock(&_total_count_lock);
#endif
}

void crunch_range(unsigned long long from, unsigned long long to)
{
    unsigned long long i;
    unsigned long long prime_count = 0;
    unsigned long long total_count = 0;

    for (i = from; i <= to; i++)
    {
        if (is_prime(i))
            prime_count++;

        total_count++;

        if (i % 100000 == 0) {
            increment_prime_count(prime_count);
            increment_total_count(total_count);
            prime_count = 0;
            total_count = 0;
        }
    }

    increment_prime_count(prime_count);
    increment_total_count(total_count);
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

void parse_args(int argc, char** argv, unsigned long long *n, int *t)
{
    int i;

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
            continue;
        
        if (argc > i + 1)
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