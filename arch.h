#ifndef ARCH_H
#define ARCH_H

#include <stdint.h>

struct excepframe {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;
};

typedef struct excepframe ExceptionFrame_t;

union exc_return {
    uint32_t v;
    struct {
        unsigned ES     :1;
        unsigned        :1;
        unsigned SPSEL  :1;
        unsigned Mode   :1;
        unsigned FType  :1;
        unsigned DCRS   :1;
        unsigned S      :1;
        unsigned        :17;
        unsigned PREFIX :8;
    };
};

typedef union exc_return ExceptionReturn_t;

#define EXC_RETURN_MAGIC    0xFF


#endif /* ARCH_H */