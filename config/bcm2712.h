/* Broadcom BCM2712 in RPi 5*/
// afaik no slices in any of the caches
/*
Sources:
https://en.wikichip.org/wiki/arm_holdings/microarchitectures/cortex-a76
https://www.7-cpu.com/cpu/Cortex-A76.html
https://developer.arm.com/documentation/100798/0401/L2-memory-system/About-the-L2-memory-system
*/

//// L3 shared cache
// 2 MiB
// ??? 16 way associative
// ??? 64B cache lines 
// Not in arm technical reference manual lol
#define L3_CACHE_WAYS 16
#define L3_WAYS_BITS 4
#define L3_SET_BITS 11      // log2(2097152/64/16)
#define L3_LINE_BITS 6
#define L3_SLICE_BITS 0

//// L2 Cache
// 512 KiB cache size
// 8 way associative
// 64B cache lines
// Strictly inclusive of the L1 data cache & non-inclusive of the L1 instruction cache?
#define L2_CACHE_WAYS 8
#define L2_WAYS_BITS 3
#define L2_SET_BITS 10      // log2(524288/64/8)
#define L2_LINE_BITS 6
#define L2_SLICE_BITS 0

//// L1 cache
// 64 KiB cache size 
// 4-way associative
// 64B cache lines
#define L1_CACHE_WAYS 4
#define L1_WAYS_BITS 2
#define L1_SET_BITS 8       // log2(65536/64/4)
#define L1_LINE_BITS 6
#define L1_SLICE_BITS 0