#pragma once  // Modern header guard - ensures this file is only included once

#include "common.h"  // Include common definitions

// Structure to store all CPU registers during a trap/exception
struct trap_frame {
    // General Purpose Registers (GPRs)
    uint32_t ra;    // Return Address
    uint32_t gp;    // Global Pointer
    uint32_t tp;    // Thread Pointer
    
    // Temporary Registers
    uint32_t t0;    // Temporary/Alternate link register
    uint32_t t1;    // Temporary register 1
    uint32_t t2;    // Temporary register 2
    uint32_t t3;    // Temporary register 3
    uint32_t t4;    // Temporary register 4
    uint32_t t5;    // Temporary register 5
    uint32_t t6;    // Temporary register 6
    
    // Argument Registers
    uint32_t a0;    // Argument/Return value 0
    uint32_t a1;    // Argument/Return value 1
    uint32_t a2;    // Argument 2
    uint32_t a3;    // Argument 3
    uint32_t a4;    // Argument 4
    uint32_t a5;    // Argument 5
    uint32_t a6;    // Argument 6
    uint32_t a7;    // Argument 7
    
    // Saved Registers
    uint32_t s0;    // Saved register 0 / Frame pointer
    uint32_t s1;    // Saved register 1
    uint32_t s2;    // Saved register 2
    uint32_t s3;    // Saved register 3
    uint32_t s4;    // Saved register 4
    uint32_t s5;    // Saved register 5
    uint32_t s6;    // Saved register 6
    uint32_t s7;    // Saved register 7
    uint32_t s8;    // Saved register 8
    uint32_t s9;    // Saved register 9
    uint32_t s10;   // Saved register 10
    uint32_t s11;   // Saved register 11
    uint32_t sp;    // Stack Pointer
} __attribute__((packed));  // No padding between members

// Structure to hold the return values from an SBI (Supervisor Binary Interface) call
struct ret_sbi {
    long err; // Error code returned by the SBI call, if any
    long val; // Value returned by the SBI call, typically used for successful results
};

// Function to make an SBI call with given arguments
struct ret_sbi call_sbi(long arg0, long arg1, long arg2, long arg3,
                        long arg4, long arg5, long fid, long eid);

// Function to output a character using SBI
void putchar(char ch);
// Main function of the kernel
void kernel_main(void);

// Macro for system panic - prints error and halts
#define PANIC(fmt, ...)                                                         \
    do {                                                                        \
        printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);   \
        while (1) {}                                                            \
    } while (0)

// Macro to read from a Control and Status Register (CSR)
#define READ_CSR(reg)                                                           \
    ({                                                                          \
        unsigned long __tmp;                                                    \
        __asm__ __volatile__("csrr %0, " #reg : "=r"(__tmp));                   \
        __tmp;                                                                  \
    })

// Macro to write to a Control and Status Register (CSR)
#define WRITE_CSR(reg, value)                                                   \
    do {                                                                        \
        uint32_t __tmp = (value);                                               \
        __asm__ __volatile__("csrw " #reg ", %0" ::"r"(__tmp));                 \
    } while (0)