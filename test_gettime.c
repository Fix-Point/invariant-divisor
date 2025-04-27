#include <stdio.h>
#include <time.h>
// #include <linux/time.h>

#include <assert.h>

#define TOLEARANT_ERROR_NSEC (1000000000u)

extern int my_clock_gettime(int clock_id, struct timespec *ts);
extern void my_clock_gettime_init(void);

/* 功能正确性测试(Functional Correctness Test) */
static void function_test(void)
{
    struct timespec ts1;
    struct timespec ts2;
    long diff_ns;

    /* 验证获取时间的准确性
     * (Validating the correctness of the time)
     */

    for (unsigned i = 0u; i < 1024u; i++) {
        diff_ns = 0;
        clock_gettime(CLOCK_MONOTONIC, &ts1);
        my_clock_gettime(CLOCK_MONOTONIC, &ts2);

        diff_ns += (ts2.tv_sec - ts1.tv_sec) * 1000000000 +
                   (ts2.tv_nsec - ts1.tv_nsec);

        my_clock_gettime(CLOCK_MONOTONIC, &ts2);
        clock_gettime(CLOCK_MONOTONIC, &ts1);

        diff_ns += (ts2.tv_sec - ts1.tv_sec) * 1000000000 +
                   (ts2.tv_nsec - ts1.tv_nsec);
        diff_ns  = diff_ns / 2;

        assert(diff_ns < TOLEARANT_ERROR_NSEC);
    }
}

/* 吞吐率测试(Throughput Test) */
static void throughput_test(void)
{
    /* 请参考test_div.c实现 */
}

/* 延迟采样测试(Latency Sampling Test) */
static void latency_test(void)
{
    /* 请参考test_div.c实现 */
}

int main(int argc, char *argv[])
{
    my_clock_gettime_init();

    /* 功能正确性测试(Functional Correctness Test) */
    function_test();

    printf("\n---------Functional Test Passed --------\n");

    /* 吞吐率测试(Throughput Test) */
    throughput_test();

    /* 延迟采样测试(Latency Sampling Test) */
    latency_test();

    return 0;
}