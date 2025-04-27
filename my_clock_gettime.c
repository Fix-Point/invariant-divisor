
#include "config.h"
#include "my_invdiv.h"
#include "helper_func.h"

#include <time.h>
#include <errno.h>
#include <stdio.h>

#define MY_FREQUENCIES 1996800000u
#define NSEC_PER_SEC   1000000000u

static invdiv64d32_t freq_div;

static uint64_t cycle_off;

static inline void my_clock_gettime_ts(struct timespec *ts)
{
    uint64_t cycle = get_cycle() - cycle_off;

    /* 计算秒部分 (Converting cycle to second.)
     * sec = cycle / freq;
     */
    ts->tv_sec  = invdiv64d32(cycle, &freq_div);

    cycle -= (uint64_t)ts->tv_sec * MY_FREQUENCIES;

    /* 不足一秒的部分 (Converting remaining cycle to nano-second.)
     * rem_cycle = cycle % freq;
     * nsec = rem_cycle * NSEC_PER_SEC / freq;
     */
    ts->tv_nsec = invdiv64d32(cycle * NSEC_PER_SEC, &freq_div);
}

int my_clock_gettime(int clock_id, struct timespec *ts)
{
    /* 参考实现，你可以自行修改。
     * (You can modify this function to implement your own clock_gettime)
     */
    int ret = 0;

    switch (clock_id) {
        case CLOCK_REALTIME:
            my_clock_gettime_ts(ts);
            break;
        case CLOCK_MONOTONIC:
            my_clock_gettime_ts(ts);
            break;
        default:
            ret = -EINVAL;
            break;

    }

    return ret;
}

void my_clock_gettime_init(void)
{
    struct timespec ts1, ts2;
    long diff_cycle;

    /* 初始化除数不变的整数除法的参数 (Initialize the invariant divisor parameter.) */
    invdiv64d32_init(MY_FREQUENCIES, &freq_div);

    diff_cycle = 0;

    for (unsigned int i = 0; i < 64u; i++) {
        clock_gettime(CLOCK_MONOTONIC, &ts1);
        my_clock_gettime(CLOCK_MONOTONIC, &ts2);

        diff_cycle += (uint64_t)ts2.tv_sec * MY_FREQUENCIES + ts2.tv_nsec * NSEC_PER_SEC / MY_FREQUENCIES;
        diff_cycle -= (uint64_t)ts1.tv_sec * MY_FREQUENCIES + ts1.tv_nsec * NSEC_PER_SEC / MY_FREQUENCIES;


        my_clock_gettime(CLOCK_MONOTONIC, &ts2);
        clock_gettime(CLOCK_MONOTONIC, &ts1);

        diff_cycle += (uint64_t)ts2.tv_sec * MY_FREQUENCIES + ts2.tv_nsec * NSEC_PER_SEC / MY_FREQUENCIES;
        diff_cycle -= (uint64_t)ts1.tv_sec * MY_FREQUENCIES + ts1.tv_nsec * NSEC_PER_SEC / MY_FREQUENCIES;
    }

    cycle_off = diff_cycle / (2 * 64);

    printf("Synchronized cycle_off = %ld\n", cycle_off);
}

