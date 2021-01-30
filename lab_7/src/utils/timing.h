#ifndef __TIMING_H_
#define __TIMING_H_

#include <stdint.h>

#ifdef _MSC_VER
#include <time.h>
#include <Windows.h>
#include <intrin.h>
#else
#include <sys/time.h>
#include <x86intrin.h>
#endif

#define BEGIN_TIMER                            \
    struct timeval __timer_tv_1, __timer_tv_2; \
    unsigned long long __timer_real;           \
    gettimeofday(&__timer_tv_1, NULL);         \
    unsigned long long __timer_ticks = __rdtsc();

#define END_TIMER                                                                                                       \
    __timer_ticks = __rdtsc() - __timer_ticks;                                                                          \
    gettimeofday(&__timer_tv_2, NULL);                                                                                  \
    __timer_real = 1000000 * (__timer_tv_2.tv_sec - __timer_tv_1.tv_sec) + __timer_tv_2.tv_usec - __timer_tv_1.tv_usec; \
    (void)__timer_real;

#define TIMER_TICKS __timer_ticks
#define TIMER_MICROSECONDS ((float)__timer_real)
#define TIMER_MILISECONDS ((float)__timer_real / 1e3)
#define TIMER_SECONDS ((float)__timer_real / 1e6)

#endif
