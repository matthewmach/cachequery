#ifdef __aarch64__
#include "../include/msrdrv-aarch64.h"

#include <inttypes.h>

// TODO: Check if we need to zero out counter or does PMCR_EL0 reset handle it

static struct MsrInOut msr_start_l3[] = {
    {.op = MSR_WRITE, .reg = PMSELR_EL0, .value = 0x00},
    {.op = MSR_WRITE,
     .reg = PMEVTYPER0_EL0,
     .value = (L3D_CACHE_REFILL | PMU_USER_EVENTS)},
    {.op = MSR_WRITE, .reg = PMCNTENSET_EL0, .value = PMU_ENABLE_COUNTER_0},
    {.op = MSR_STOP, .reg = 0x00, .value = 0x00},
};

static struct MsrInOut msr_start_l2[] = {
    {.op = MSR_WRITE, .reg = PMSELR_EL0, .value = 0x00},
    {.op = MSR_WRITE,
     .reg = PMEVTYPER0_EL0,
     .value = (L2D_CACHE_REFILL | PMU_USER_EVENTS)},
    {.op = MSR_WRITE, .reg = PMCNTENSET_EL0, .value = PMU_ENABLE_COUNTER_0},
    {.op = MSR_STOP, .reg = 0x00, .value = 0x00},
};

static struct MsrInOut msr_start_l1[] = {
    {.op = MSR_WRITE, .reg = PMSELR_EL0, .value = 0x00},
    {.op = MSR_WRITE,
     .reg = PMEVTYPER0_EL0,
     .value = (L1D_CACHE_REFILL | PMU_USER_EVENTS)},
    {.op = MSR_WRITE, .reg = PMCNTENSET_EL0, .value = PMU_ENABLE_COUNTER_0},
    {.op = MSR_STOP, .reg = 0x00, .value = 0x00},
};

// TODO: Check if we need to filter only to userspace cycles
static struct MsrInOut msr_start_cycles[] = {
    {.op = MSR_WRITE, .reg = PMCNTENSET_EL0, .value = PMU_ENABLE_CYCLE_COUNTER},
    {.op = MSR_STOP, .reg = 0x00, .value = 0x00},
};

static struct MsrInOut msr_stop[] = {
    // TODO: Check if anything needs to be done here
    {.op = MSR_STOP, .reg = 0x00, .value = 0x00},
};

inline uint64_t __attribute__((always_inline)) read_msr(enum MsrRegister reg) {
    uint64_t result = 0;

    switch (reg) {
        case PMCR_EL0:
            __asm__ __volatile__("mrs %0, PMCR_EL0" : "=r"(result));
            break;
        case PMCCNTR_EL0:
            __asm__ __volatile__("mrs %0, PMCCNTR_EL0" : "=r"(result));
            break;
        case PMEVTYPER0_EL0:
            __asm__ __volatile__("mrs %0, PMEVTYPER0_EL0" : "=r"(result));
            break;
        case PMEVCNTR0_EL0:
            __asm__ __volatile__("mrs %0, PMEVCNTR0_EL0" : "=r"(result));
            break;
        case PMCNTENSET_EL0:
            __asm__ __volatile__("mrs %0, PMCNTENSET_EL0" : "=r"(result));
            break;
        case PMSELR_EL0:
            __asm__ __volatile__("mrs %0, PMSELR_EL0" : "=r"(result));
            break;
        default:
            // should not execute;
    }

    // printk(
    //     KERN_ALERT
    //     "Module msrdrv: Read 0x%016llx (0x%08x:0x%08x) from MSR
    //     0x%08x\n", result, edx, eax, ecx);
    return result;
}

inline void __attribute__((always_inline)) write_msr(enum MsrRegister reg,
                                                     unsigned long long value) {
    switch (reg) {
        case PMCR_EL0:
            __asm__ __volatile__("msr PMCR_EL0, %0" ::"r"(value));
            break;
        case PMCCNTR_EL0:
            __asm__ __volatile__("msr PMCCNTR_EL0, %0" ::"r"(value));
            break;
        case PMEVTYPER0_EL0:
            __asm__ __volatile__("msr PMEVTYPER0_EL0, %0" ::"r"(value));
            break;
        case PMEVCNTR0_EL0:
            __asm__ __volatile__("msr PMEVCNTR0_EL0, %0" ::"r"(value));
            break;
        case PMCNTENSET_EL0:
            __asm__ __volatile__("msr PMCNTENSET_EL0, %0" ::"r"(value));
            break;
        case PMSELR_EL0:
            __asm__ __volatile__("msr PMSELR_EL0, %0" ::"r"(value));
            break;
        default:
            return;
    }
}

// static inline uint64_t __attribute__((always_inline)) pmu_get_cycle_counter()
// {
//     uint64_t c;
//     __asm__ __volatile__("MRS %0, PMCCNTR_EL0" : "=r"(c));
//     return c;
// }

long msrdrv_run(struct MsrInOut* msrops) {
    int i;
    for (i = 0;; i++) {
        switch (msrops[i].op) {
            case MSR_NOP:
                //	printk(KERN_ALERT "Module msrdrv: seen MSR_NOP
                // command\n");
                break;
            case MSR_STOP:
                //	printk(KERN_ALERT "Module msrdrv: seen MSR_STOP
                // command\n");
                goto label_end;
            case MSR_READ:
                //	printk(KERN_ALERT "Module msrdrv: seen MSR_READ
                // command\n");
                msrops[i].value = read_msr(msrops[i].reg);
                break;
            case MSR_WRITE:
                //	printk(KERN_ALERT "Module msrdrv: seen MSR_WRITE
                // command\n");
                write_msr(msrops[i].reg, msrops[i].value);
                break;
            default:
                //	printk(KERN_ALERT "Module msrdrv: Unknown option
                // 0x%x\n", msrops[i].op);
                return 1;
        }
    }
label_end:

    return 0;
}

void enable() {
    uint64_t val = read_msr(PMCR_EL0);

    // Enable PMU in EL1
    val |= PMU_ENABLE | PMU_RESET;
    write_msr(PMCR_EL0, val);
}

void disable() {
    uint64_t val = read_msr(PMCR_EL0);

    // Disable PMU in EL1
    val &= ~PMU_ENABLE;
    write_msr(PMCR_EL0, val);
}

void prepare_counters(int level) {
    enable();
    switch (level) {
        case 1:
            msrdrv_run(msr_start_l1);
            break;
        case 2:
            msrdrv_run(msr_start_l2);
            break;
        case 3:
            msrdrv_run(msr_start_l3);
            break;
        default:
            msrdrv_run(msr_start_cycles);
            break;
    }
}

void disable_counters(void) {
    msrdrv_run(msr_stop);
    disable();
}
#endif