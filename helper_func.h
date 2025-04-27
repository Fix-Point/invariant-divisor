#pragma once

#include <stdint.h>

/* 获取当前周期计数 (Get current CPU cycle count) */

#ifdef __amd64__
static inline uint64_t rdtsc(void)
{
    uint32_t lo;
    uint32_t hi;

    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return (uint64_t)lo | (((uint64_t)hi) << 32);
}

static inline uint64_t rdtscp(void)
{
    uint32_t lo;
    uint32_t hi;

    __asm__ volatile("rdtscp" : "=a"(lo), "=d"(hi)::"ecx");
    return (uint64_t)lo | (((uint64_t)hi) << 32);
}

static inline uint64_t get_cycle(void)
{
    return rdtscp();
}
#elif defined(__aarch64__)
static inline uint64_t get_cycle(void)
{
    uint64_t cycle;
    __asm__ volatile("isb\n\t"
                     "mrs %0, cntvct_el0"
                     : "=r"(cycle));
  return cycle;
}
#elif defined(__arm__)
static inline uint64_t get_cycle(void)
{
    uint64_t cycle;
    __asm__ volatile("isb\n\t"
                     "mrrc p15, 0, %Q0, %R0, c14"
                     : "=r"(cycle));
    return cycle;
}
#else
#   error "不支持的体系结构，请在此处添加体系结构支持 (Unsupported Architecture, Please Add support for specific architecture here)"
#endif