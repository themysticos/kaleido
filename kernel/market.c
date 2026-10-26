// market.c
#include "market.h"

#define MAX_CAPS 32

static capability caps[MAX_CAPS];
static int cap_count = 0;

void market_init(void) { cap_count = 0; }

int market_register(const char *name, cap_fn fn, void *ctx) {
    if (cap_count >= MAX_CAPS) return -1;
    caps[cap_count].name = name;
    caps[cap_count].fn   = fn;
    caps[cap_count].ctx  = ctx;
    return cap_count++;
}

static int str_eq(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *a == *b;
}

cap_fn market_acquire(const char *name, void **ctx) {
    for (int i = 0; i < cap_count; i++)
        if (str_eq(caps[i].name, name)) { if (ctx) *ctx = caps[i].ctx; return caps[i].fn; }
    return 0;
}
