#ifndef __TIMING_H_
#define __TIMING_H_

// time calculations
#include <stdint.h> // <cstdint> is preferred in C++, but stdint.h works.

#ifdef _MSC_VER
#include <time.h>
#include <Windows.h>
#include <intrin.h>
#else
#include <sys/time.h>
#include <x86intrin.h>
#endif

#define TIME_IT(expr, var_name) (expr)

#endif
