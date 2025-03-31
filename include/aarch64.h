// implementing the ARMv8.2-A 64-bit instruction set
#ifndef __AARCH64_H
#define __AARCH64_H
#include "../config/settings.h"
#include "parser.h"

#define C(...) (char[]){__VA_ARGS__}

// not exact equivalent due to diff registers
// just replaced with equivalent arm function paralogue
// 0x29 = frame pointer
// 0x30 = Link register
// sp = stack pointer

// rbx = general purpose, use x19? (when else is rbx used?)
// rdi = rax = x0 (both first argument and return value)

// for load rax, we use x10 now
// thus wbinvd clears x10
// move_rax still uses x0


/*
Register Mapping we'll use

RAX => X0
RBX => X3
RCX => X1
RDX => X2
RSI => X4
RDI => X5
RBP => X29
RSP => X31

*/

// all instructions iin little endian


#define PUSH_RBP()			C(0xFD, 0x7B, 0x3F, 0xA9)       // stp X29, X30, [sp, #-16]
#define MOV_RBP_RSP()		C(0xFD, 0x03, 0x00, 0x91)       // mov x29, sp
// #define PUSH_RBX()          C()
#define XOR_RSI_RSI()       C(0x00, 0x00, 0x00, 0xCA)       // eor x0
// movk = {reg (0-5), imm16 (6-20), hw (21-22), opcode (23-30), sf (31)}
// sf set to 1 for aarch64 
#define LOAD_RAX(Q) \
    C( (((Q) & 0x07) << 5) | 0xA, ((Q >> 3) & 0xFF), (((Q >> 11) & 0x1F) | 0x80), 0xF2, \
       (((Q >> 16) & 0x07) << 5) | 0xA, ((Q >> 19) & 0xFF), (((Q >> 27) & 0x1F) | 0xA0), 0xF2, \
       (((Q >> 32) & 0x07) << 5) | 0xA, ((Q >> 35) & 0xFF), (((Q >> 43) & 0x1F) | 0xC0), 0xF2, \
       (((Q >> 48) & 0x07) << 5) | 0xA, ((Q >> 51) & 0xFF), (((Q >> 59) & 0x1F) | 0xE0), 0xF2 )

#define MOV_RAX_CT(Q) \
    C( (((Q) & 0x07) << 5), ((Q >> 3) & 0xFF), (((Q >> 11) & 0x1F) | 0x80), 0xF2, \
       (((Q >> 16) & 0x07) << 5), ((Q >> 19) & 0xFF), (((Q >> 27) & 0x1F) | 0xA0), 0xF2, \
       (((Q >> 32) & 0x07) << 5), ((Q >> 35) & 0xFF), (((Q >> 43) & 0x1F) | 0xC0), 0xF2, \
       (((Q >> 48) & 0x07) << 5), ((Q >> 51) & 0xFF), (((Q >> 59) & 0x1F) | 0xE0), 0xF2 )

#define WBINVD()            C(0x4A, 0x7E, 0x08, 0xD5)       // dc cisw, x10

#define CLFLUSH_RAX()       C(0x20, 0x7E, 0x0B, 0xD5)       // dc civac, x0

#define XOR_RAX_RAX()       C(0x00, 0x00, 0x00, 0xCA)       // eor x0

// #define XOR_RDX_RDX()       C()
// #define XOR_RDI_RDI()       C()

#define CPUID() \
    C( 0x9F, 0x3F, 0x03, 0xD5, /* dsb sy */ \
       0xDF, 0x3F, 0x03, 0xD5  /* isb */ )

// #define LFENCE()
#define MFENCE()			C(0x9F, 0x3F, 0x03, 0xD5)       // dsb sy
// #define SFENCE()

// #define RDPMC()

// #define POP_RBX()           C()
#define POP_RBP()           C(0xFD, 0x7B, 0xC1, 0xA8)       // ldp x29, x30, [sp], 16
#define RETQ()              C(0xC0, 0x03, 0x5F, 0xD6)       // ret
//#define MOV_RAX_RDI()
#define JMP_SHORT(_0) C(((_0 >> 2) & 0xFF), ((_0 >> 10) & 0xFF), ((_0 >> 18) & 0xFF), 0x14)


#define MOVNTDQA_RAX() \
    C( 0x20, 0x7E, 0x0B, 0xD5, /* dc civac, x0 */ \
       0x9F, 0x3F, 0x03, 0xD5, /* dsb sy */ \
       0xDF, 0x3F, 0x03, 0xD5, /* isb */ \
       0x00, 0x00, 0x40, 0xF8  /* ldr x0, [x0] */ )

// #define MOV_RAX_CR0()
// #define MOV_CR0_RAX()
#define WBINVD()            C(0x4A, 0x7E, 0x08, 0xD5)   // dc cisw, x10

#define SERIALIZE()		    CPUID()

#define MOV_ECX_DWORD(...)	C(0xb9, __VA_ARGS__)
#define XOR_EAX_EAX()		C(0x31, 0xc0)
#define XOR_EDX_EDX()		C(0x31, 0xd2)

// mov x1 1
#define MOV_RDI_DWORD(...) C(0x21, 0x00, 0x80, 0xd2)




#define ISB()          C(0xDF, 0x3F, 0x03, 0xD5)   // isb

#define MSR_EVCNTR0_X0() C(0x00, 0xE8, 0x1B, 0xD5) // msr PMEVCNTR0_EL0, x0
#define MRS_X0_EVCNTR0() C(0x00, 0xE8, 0x3B, 0xD5) // mrs x0, PMEVCNTR0_EL0

#define MSR_CCNTR_X0() C(0x00, 0x9D, 0x1B, 0xD5) // msr PMCCNTR_EL0, x0
#define MRS_X0_CCNTR() C(0x00, 0x9D, 0x3B, 0xD5) // mrs x0, PMCCNTR_EL0

#define MSR_PMSELR_EL0_X0() C(0xA0, 0x9C, 0x1B, 0xD5) // msr PMSELR_EL0, x0

/*
Reset the PMU counter

dsb sy
isb

eor x0, x0, x0
msr PMEVCNTR0_EL0, x0

isb
*/
#define RESET_PMC0(code)  \
    do {  \
        OPCODE(code, CPUID());        /* dsb sy; isb */  \
        OPCODE(code, XOR_RAX_RAX());  /* eor x0, x0, x0 */  \
        OPCODE(code, MSR_EVCNTR0_X0()); /* msr PMEVCNTR0_EL0, x0 */  \
        OPCODE(code, ISB());          /* isb */  \
    } while (0)

/*
Puts value of PMEVCNTR0_EL0 into x0
Counter 0 should have been selected by now

dsb sy
isb

mrs x0, PMEVCNTR0_EL0

isb

*/
#define MEASURE_POST_CORE(code)  \
    do {  \
        OPCODE(code, CPUID());        /* dsb sy; isb */  \
        OPCODE(code, MRS_X0_EVCNTR0()); /* mrs x0, PMEVCNTR0_EL0 */  \
        OPCODE(code, ISB());          /* isb */  \
    } while (0)


/*
dsb sy
isb

eor x0, x0, x0
msr PMCCNTR_EL0, x0

isb
*/
#define MEASURE_PRE_TSC(code)  \
    do {  \
        OPCODE(code, CPUID());        /* dsb sy; isb */  \
        OPCODE(code, XOR_RAX_RAX());  /* eor x0, x0, x0 */  \
        OPCODE(code, MSR_CCNTR_X0()); /* msr PMCCNTR_EL0, x0 */  \
        OPCODE(code, ISB());          /* isb */  \
    } while (0)

/*
dsb sy
isb

mrs x0, PMCCNTR_EL0

isb
*/
#define MEASURE_POST_TSC(code)  \
    do {  \
        OPCODE(code, CPUID());      /* dsb sy; isb */  \
        OPCODE(code, MRS_X0_CCNTR()); /* mrs x0, PMCCNTR_EL0 */  \
        OPCODE(code, ISB());        /* isb */  \
    } while (0)

#endif /* __AARCH64_H */
