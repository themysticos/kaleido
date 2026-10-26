// market.h
#ifndef MARKET_H
#define MARKET_H
#include <stdint.h>

typedef void* (*cap_fn)(void*);

typedef struct {
    const char *name;
    cap_fn      fn;
    void       *ctx;
} capability;

void         market_init(void);
int          market_register(const char *name, cap_fn fn, void *ctx);
cap_fn       market_acquire(const char *name, void **ctx);
#endif
