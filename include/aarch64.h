// implementing the ARMv8.2-A 64-bit instruction set
#include "../config/settings.h"
#include "parser.h"

#ifndef __AARCH64_H
#define __AARCH64_H

#define C(...) (char[]){__VA_ARGS__}

// not exact equivalent due to diff registers
// just replaced with equivalent arm function paralogue
// 0x29 = frame pointer
// 0x30 = Link register
// sp = stack pointer

// rbx = general purpose, use x19? (when else is rbx used?)
// rdi = rax = x0 (both first argument and return value)

// all instructions iin little endian

#define PUSH_RBP()			C(0xFD, 0x7B, 0x3F, 0xA9)   // stp X29, X30, [sp, #-16]
#define MOV_RBP_RSP()		C(0xFD, 0x03, 0x00, 0x91)   // mov x29, sp
#define PUSH_RBX()          C()

// movk = {reg (0-5), imm16 (6-20), hw (21-22), opcode (23-30), sf (31)}
// sf set to 1 for aarch64 

#define LOAD_RAX(Q)         C((((Q) & 0x07) << 5) | 0xA, ((Q>>3) & 0xff), (((Q >> 11) & 0x1f) | 0x80), 0xF2, \          // movk x10, Q[0:15]
                              (((Q>>16) & 0x07) << 5) | 0xA, ((Q>>19) & 0xff), (((Q >> 27) & 0x1f) | 0xA0), 0xF2, \     // movk x10, Q[16:31], lsl #16
                              (((Q>>32) & 0x07) << 5) | 0xA, ((Q>>35) & 0xff), (((Q >> 43) & 0x1f) | 0xC0), 0xF2, \     // movk x10, Q[32:47], lsl #32
                              (((Q>>48) & 0x07) << 5) | 0xA, ((Q>>51) & 0xff), (((Q >> 59) & 0x1f) | 0xE0), 0xF2)       // movk x10, Q[48:63], lsl #48

#define MOV_RAX_CT(Q)        C(((Q) & 0x07) << 5, ((Q>>3) & 0xff), (((Q >> 11) & 0x1f) | 0x80), 0xF2, \          // movk x0, Q[0:15]
                              ((Q>>16) & 0x07) << 5, ((Q>>19) & 0xff), (((Q >> 27) & 0x1f) | 0xA0), 0xF2, \     // movk x0, Q[16:31], lsl #16
                              ((Q>>32) & 0x07) << 5, ((Q>>35) & 0xff), (((Q >> 43) & 0x1f) | 0xC0), 0xF2, \     // movk x0, Q[32:47], lsl #32
                              ((Q>>48) & 0x07) << 5, ((Q>>51) & 0xff), (((Q >> 59) & 0x1f) | 0xE0), 0xF2)       // movk x0, Q[48:63], lsl #48
#define WBINVD()            C(0x4A, 0x7E, 0x08, 0xD5)     // dc cisw, x10

#define CLFLUSH_RAX()       C(0x20, 0x7E, 0x0B, 0xD5)   // dc civac, x0

#define XOR_RAX_RAX()       C(0x00, 0x00, 0x00, 0xCA)   // eor x0


#define CPUID()             C(0x9F, 0x3F, 0x03, 0xD5, \  // dsb sy
                              0xDF, 0x3F, 0x03, 0xD5)   // isb
#define MFENCE()			C(0x9F, 0x3F, 0x03, 0xD5)    // dsb sy

#define POP_RBX()           C()
#define POP_RBP()           C(0xFD, 0x7B, 0xC1, 0xA8)   // ldp x29, x30, [sp], 16
#define RETQ()              C(0xC0, 0x03, 0x5F, 0xD6)   // ret

#define WBINVD()            C(0x4A, 0x7E, 0x08, 0xD5)   // dc cisw, x10

#define SERIALIZE()		CPUID()



// replace with pmu reset, not done
#define MOV_ECX_DWORD(...)	C(0xb9, __VA_ARGS__)
#define XOR_EAX_EAX()		C(0x31, 0xc0)
#define XOR_EDX_EDX()		C(0x31, 0xd2)
#define RESET_PMC0(code)				\
	{									\
		OPCODE(code, MOV_ECX_DWORD(_MSR_IA32_PMC0, 0x00, 0x00, 0x00)); \
		OPCODE(code, XOR_EAX_EAX());	\
		OPCODE(code, XOR_EDX_EDX());	\
		OPCODE(code, SERIALIZE());		\
		OPCODE(code, WRMSR());			\
	}

#endif /* __AARCH64_H */
