#pragma once                  // Ensure header is only included once
#include "common.h"          // Include common type definitions and utilities

// Maximum number of processes supported by the scheduler
#define PROCS_MAX 8

// Process state definitions
#define PROC_UNUSED 0    // Process slot is free and can be allocated
#define PROC_RUNNABLE 1  // Process is ready to be scheduled

// CPU register state saved during context switches and interrupts
struct trap_frame {
    // Caller-saved registers
    uint32_t ra;    // Return address for function calls
    uint32_t gp;    // Global pointer for static data access
    uint32_t tp;    // Thread pointer register
    uint32_t t0;    // Temporary register 0
    uint32_t t1;    // Temporary register 1
    uint32_t t2;    // Temporary register 2
    uint32_t t3;    // Temporary register 3
    uint32_t t4;    // Temporary register 4
    uint32_t t5;    // Temporary register 5
    uint32_t t6;    // Temporary register 6
    uint32_t a0;    // Function argument/return value 0
    uint32_t a1;    // Function argument/return value 1
    uint32_t a2;    // Function argument 2
    uint32_t a3;    // Function argument 3
    uint32_t a4;    // Function argument 4
    uint32_t a5;    // Function argument 5
    uint32_t a6;    // Function argument 6
    uint32_t a7;    // Function argument 7
    uint32_t s0;    // Saved register 0 (frame pointer)
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
    uint32_t sp;    // Stack pointer register
} __attribute__((packed));   // Ensure tight packing of structure

// Process Control Block - represents a process in the system
struct process {
    int pid;                // Unique process identifier
    int state;             // Current state (UNUSED/RUNNABLE)
    vaddr_t sp;            // Stack pointer for this process context
    uint8_t stack[8192];   // Dedicated kernel stack for the process
};

// CSR access macros for privileged register operations
#define READ_CSR(reg)                                                         \
    ({                                                                        \
        unsigned long __tmp;                                                  \
        __asm__ __volatile__("csrr %0, " #reg : "=r"(__tmp));                 \
        __tmp;                                                                \
    })

// Macro to write to a Control and Status Register (CSR)
#define WRITE_CSR(reg, value)                                                \
    do {                                                                     \
        uint32_t __tmp = (value);                                            \
        __asm__ __volatile__("csrw " #reg ", %0" ::"r"(__tmp));              \
    } while (0)

// Structure to hold the return values from an SBI (Supervisor Binary Interface)
// call
struct ret_sbi {
  long err; // Error code returned by the SBI call, if any
  long val; // Value returned by the SBI call, typically used for successful
            // results
};

// Function to make an SBI call with given arguments
struct ret_sbi call_sbi(long arg0, long arg1, long arg2, long arg3, long arg4,
                        long arg5, long fid, long eid);

// Function to output a character using SBI
void putchar(char ch);

// Main function of the kernel
void kernel_main(void);

// PANIC macro to print an error message and halt the system
#define PANIC(fmt, ...)                                                        \
  do {                                                                         \
    printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);      \
    while (1) {                                                                \
    }                                                                          \
  } while (0)
