#pragma once

#include <stdint.h>

/* 请修改以下实现， */

typedef struct {
    uint32_t divisor;
} invdiv32d32_t;

static inline void invdiv32d32_init(uint32_t divisor, invdiv32d32_t *invdiv)
{
    invdiv->divisor = divisor;
}

static inline uint32_t invdiv32d32(uint32_t a, invdiv32d32_t *invdiv)
{
    uint32_t res = a / invdiv->divisor;
    return res;
}

typedef struct {
    uint32_t divisor;
} invdiv64d32_t;

static inline void invdiv64d32_init(uint32_t divisor, invdiv64d32_t *invdiv)
{
    invdiv->divisor = divisor;
}

static inline uint64_t invdiv64d32(uint64_t a, invdiv64d32_t *invdiv)
{
    uint64_t res = a / invdiv->divisor;

    return res;
}

typedef struct {
    uint64_t divisor;
} invdiv64d64_t;

static inline void invdiv64d64_init(uint64_t divisor, invdiv64d64_t *invdiv)
{
    invdiv->divisor = divisor;
}

static inline uint64_t invdiv64d64(uint64_t a, invdiv64d64_t *invdiv)
{
    uint64_t res = a / invdiv->divisor;

    return res;
}
