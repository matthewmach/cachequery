#ifndef __MSRDRV_AARCH64_H
#define __MSRDRV_AARCH64_H

#include <linux/kernel.h>
#include <linux/types.h>

enum MsrOperation {
    MSR_NOP = 0,
    MSR_READ = 1,
    MSR_WRITE = 2,
    MSR_STOP = 3,
    MSR_RDTSC = 4
};

// Registers used for PMU
enum MsrRegister {
    PMCR_EL0,        // PMU Control Register
    PMCCNTR_EL0,     // PMU Cycle Count Register
    PMEVTYPER0_EL0,  // PMU Event Type Register for Counter 0
    PMEVCNTR0_EL0,   // PMU Event Count Register for Counter 0
    PMCNTENSET_EL0,  // PMU Counter Enable Set Register
    PMSELR_EL0,      // PMU Event Counter Select Register
};  // Modify [read_msr] and [write_msr] when adding new registers

// use to read/write MSR
struct MsrInOut {
    enum MsrOperation op;  // MSR operation
    enum MsrRegister reg;  // msr identifier

    unsigned long long value;  // value to write or read
};

// PMU Control Register
#define PMU_ENABLE BIT(0)
#define PMU_RESET BIT(2)

// Enable PMU Counters
#define PMU_ENABLE_COUNTER_0 (1 << 0)
#define PMU_ENABLE_CYCLE_COUNTER (1 << 31)

// Only track User space PMU events
#define PMU_USER_EVENTS (1 << 31)

// PMU Events
#define L1D_CACHE_REFILL 0x03
#define L2D_CACHE_REFILL 0x17
#define L3D_CACHE_REFILL 0x2A

void prepare_counters(int lvl);
void disable_counters(void);

#endif