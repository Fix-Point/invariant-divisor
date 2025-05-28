/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <assert.h>

#include "config.h"
#include "helper_func.h"
#include "my_invdiv.h"

#include "3rdlib/libdivide.h"

#define cycle_hash(cycle) ((cycle) * 0x9e37fffffffc0001ul)

extern int printf(const char *__restrict __format, ...);
extern const uint32_t invariant_divisor32[DATA_SET_NR];
extern const uint64_t invariant_divisor64[DATA_SET_NR];

static invdiv32d32_t d32d32;
static invdiv64d32_t d64d32;
static invdiv64d64_t d64d64;

static void function_test(void)
{
    for (unsigned i = 0u; i < DATA_SET_NR; i++) {
        uint32_t divisor32 = invariant_divisor32[i];
        uint64_t divisor64 = invariant_divisor64[i];

        invdiv32d32_init(divisor32, &d32d32);
        invdiv64d32_init(divisor32, &d64d32);
        invdiv64d64_init(divisor64, &d64d64);

        for (unsigned j = 0u; j < DATA_SET_NR; j++) {
            uint64_t cycle = cycle_hash(get_cycle());
            uint32_t res32;
            uint64_t res64_32;
            uint64_t res64_64;

            res32 = invdiv32d32(cycle, &d32d32);
            res64_32 = invdiv64d32(cycle, &d64d32);
            res64_64 = invdiv64d64(cycle, &d64d64);

            /* 验证是否等于整数除法 
            * (Validating if the invdiv is equal to the 
            *  unsigned integer division)
            */
            assert(res32 == (uint32_t)cycle / divisor32);
            assert(res64_32 == (uint64_t)cycle / divisor32);
            assert(res64_64 == (uint64_t)cycle / divisor64);
        }
    }
}

static void throughput_test(void)
{
    uint64_t total_cyclediv    = 0;
    uint64_t total_cyclelibdiv = 0;
    uint64_t total_cycle32d32  = 0;
    uint64_t total_cycle64d32  = 0;
    uint64_t total_cycle64d64  = 0;

    printf("\n---------Thoughput Test--------\n");

    for (int i = 0; i < 100; i++) {
        uint32_t divisor32 = invariant_divisor32[i];
        uint64_t divisor64 = invariant_divisor64[i];
        struct libdivide_u64_t libdiv = libdivide_u64_gen(divisor64);
        uint64_t start_cycle;
        uint64_t end_cycle;

        uint64_t cycle;
        invdiv32d32_init(divisor32, &d32d32);
        invdiv64d32_init(divisor32, &d64d32);
        invdiv64d64_init(divisor64, &d64d64);

        asm volatile ("":::"memory");

        start_cycle = get_cycle();
        for (unsigned j = 0u; j < TEST_NR; j++) {
            volatile uint32_t res64;
            cycle = cycle_hash(start_cycle + j);
            res64 = cycle / divisor64;
        }
        end_cycle = get_cycle();
        total_cyclediv += end_cycle - start_cycle;

        start_cycle = get_cycle();
        for (unsigned j = 0u; j < TEST_NR; j++) {
            volatile uint64_t res64;
            cycle = cycle_hash(start_cycle + j);
            res64 = libdivide_u64_do(cycle, &libdiv);
        }
        end_cycle = get_cycle();
        total_cyclelibdiv += end_cycle - start_cycle;

        start_cycle = get_cycle();
        for (unsigned j = 0u; j < TEST_NR; j++) {
            volatile uint32_t res32;
            cycle = cycle_hash(start_cycle + j);
            res32 = invdiv32d32(cycle, &d32d32);
        }
        end_cycle = get_cycle();
        total_cycle32d32 += end_cycle - start_cycle;

        start_cycle = get_cycle();
        for (unsigned j = 0u; j < TEST_NR; j++) {
            volatile uint32_t res32;
            cycle = cycle_hash(start_cycle + j);
            res32 = invdiv64d32(cycle, &d64d32);
        }
        end_cycle = get_cycle();
        total_cycle64d32 += end_cycle - start_cycle;

        start_cycle = get_cycle();
        for (unsigned j = 0u; j < TEST_NR; j++) {
            volatile uint64_t res64;
            cycle = cycle_hash(start_cycle + j);
            res64 = invdiv64d64(cycle, &d64d64);
        }
        end_cycle = get_cycle();

        total_cycle64d64 += end_cycle - start_cycle;
    }

    printf("divide: %.3lf cycle/op\n", (double)total_cyclediv/((uint64_t)100 * TEST_NR));
    printf("libdivide: %.3lf cycle/op\n", (double)total_cyclelibdiv/((uint64_t)100 * TEST_NR));
    printf("invdiv32d32: %.3lf cycle/op\n", (double)total_cycle32d32/((uint64_t)100 * TEST_NR));
    printf("invdiv64d32: %.3lf cycle/op\n", (double)total_cycle64d32/((uint64_t)100 * TEST_NR));
    printf("invdiv64d64: %.3lf cycle/op\n", (double)total_cycle64d64/((uint64_t)100 * TEST_NR));
}

#define HISTOGRAM_CYCLES 128
static void latency_test(void)
{
    uint32_t hidx;
    uint64_t histogramdivide[HISTOGRAM_CYCLES]    = {0};
    uint64_t histogramlibdivide[HISTOGRAM_CYCLES] = {0};
    uint64_t histogram32d32[HISTOGRAM_CYCLES]     = {0};
    uint64_t histogram64d32[HISTOGRAM_CYCLES]     = {0};
    uint64_t histogram64d64[HISTOGRAM_CYCLES]     = {0};
  
    printf("\n---------Latency Test--------\n");

    for (int i = 0; i < 1; i++) {
        uint32_t divisor32 = invariant_divisor32[i];
        uint64_t divisor64 = invariant_divisor64[i];
        struct libdivide_u64_t libdiv = libdivide_u64_gen(divisor64);
        uint64_t start_cycle;
        uint64_t end_cycle;

        uint64_t cycle;

        invdiv32d32_init(divisor32, &d32d32);
        invdiv64d32_init(divisor32, &d64d32);
        invdiv64d64_init(divisor64, &d64d64);

        asm volatile ("":::"memory");

        for (unsigned j = 0u; j < TEST_NR; j++) {
            volatile uint32_t res64;

            start_cycle = get_cycle();
            cycle = cycle_hash(start_cycle);
            res64 = cycle / divisor64;
            end_cycle = get_cycle();

            hidx = end_cycle - start_cycle;
            hidx = hidx >= HISTOGRAM_CYCLES ? HISTOGRAM_CYCLES - 1 : hidx;
            histogramdivide[hidx]++;
        }

        for (unsigned j = 0u; j < TEST_NR; j++) {
            volatile uint32_t res64;

            start_cycle = get_cycle();
            cycle = cycle_hash(start_cycle);
            res64 = libdivide_u64_do(cycle, &libdiv);
            end_cycle = get_cycle();

            hidx = end_cycle - start_cycle;
            hidx = hidx >= HISTOGRAM_CYCLES ? HISTOGRAM_CYCLES - 1 : hidx;
            histogramlibdivide[hidx]++;
        }

        for (unsigned j = 0u; j < TEST_NR; j++) {
            volatile uint32_t res32;

            start_cycle = get_cycle();
            cycle = cycle_hash(start_cycle);
            res32 = invdiv32d32(cycle, &d32d32);
            end_cycle = get_cycle();

            hidx = end_cycle - start_cycle;
            hidx = hidx >= HISTOGRAM_CYCLES ? HISTOGRAM_CYCLES - 1 : hidx;
            histogram32d32[hidx]++;
        }

        for (unsigned j = 0u; j < TEST_NR; j++) {
            volatile uint32_t res32;

            start_cycle = get_cycle();
            cycle = cycle_hash(start_cycle);
            res32 = invdiv64d32(cycle, &d64d32);
            end_cycle = get_cycle();

            hidx = end_cycle - start_cycle;
            hidx = hidx >= HISTOGRAM_CYCLES ? HISTOGRAM_CYCLES - 1 : hidx;
            histogram64d32[hidx]++;
        }

        for (unsigned j = 0u; j < TEST_NR; j++) {
            volatile uint32_t res32;

            start_cycle = get_cycle();
            cycle = cycle_hash(start_cycle);
            res32 = invdiv64d64(cycle, &d64d64);
            end_cycle = get_cycle();

            hidx = end_cycle - start_cycle;
            hidx = hidx >= HISTOGRAM_CYCLES ? HISTOGRAM_CYCLES - 1 : hidx;
            histogram64d64[hidx]++;
        }
    }

    output_to_file("./log/divide_histogram.log", histogramdivide, HISTOGRAM_CYCLES);
    output_to_file("./log/libdivide_histogram.log", histogramlibdivide, HISTOGRAM_CYCLES);
    output_to_file("./log/invdiv32d32_histogram.log", histogram32d32, HISTOGRAM_CYCLES);
    output_to_file("./log/invdiv64d32_histogram.log", histogram64d32, HISTOGRAM_CYCLES);
    output_to_file("./log/invdiv64d64_histogram.log", histogram64d64, HISTOGRAM_CYCLES);
}

int main(int argc, char *argv[])
{

    /* 功能正确性测试(Functional Correctness Test) */
    function_test();

    /* 吞吐率测试(Throughput Test) */
    throughput_test();

    /* 延迟采样测试(Latency Sampling Test) */
    latency_test();

    return 0;
}
