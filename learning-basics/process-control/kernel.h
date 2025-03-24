#pragma once
#include "common.h"

// Process management constants
#define PROCS_MAX 8

// Process states
#define PROC_UNUSED 0
#define PROC_RUNNABLE 1

// Trap frame structure - holds saved registers during trap handling
// This represents the complete CPU context at the time of an exception
struct trap_frame {
  uint32_t ra;                // x1: Return Address - holds return location after function call
  uint32_t gp;                // x3: Global Pointer - points to global data segment
  uint32_t tp;                // x4: Thread Pointer - points to thread-local storage
  uint32_t t0;                // x5: Temporary register 0 - caller-saved registers used for 
  uint32_t t1;                // x6: Temporary register 1 - temporary storage of values
  uint32_t t2;                // x7: Temporary register 2 - not preserved across function calls
  uint32_t t3;                // x28: Temporary register 3
  uint32_t t4;                // x29: Temporary register 4
  uint32_t t5;                // x30: Temporary register 5
  uint32_t t6;                // x31: Temporary register 6
  uint32_t a0;                // x10: Argument/return value register 0 - function arguments and return values
  uint32_t a1;                // x11: Argument/return value register 1
  uint32_t a2;                // x12: Argument register 2 - function arguments
  uint32_t a3;                // x13: Argument register 3
  uint32_t a4;                // x14: Argument register 4
  uint32_t a5;                // x15: Argument register 5
  uint32_t a6;                // x16: Argument register 6
  uint32_t a7;                // x17: Argument register 7
  uint32_t s0;                // x8/fp: Saved register 0/Frame pointer - callee-saved registers
  uint32_t s1;                // x9: Saved register 1 - preserved across function calls
  uint32_t s2;                // x18: Saved register 2
  uint32_t s3;                // x19: Saved register 3
  uint32_t s4;                // x20: Saved register 4
  uint32_t s5;                // x21: Saved register 5
  uint32_t s6;                // x22: Saved register 6
  uint32_t s7;                // x23: Saved register 7
  uint32_t s8;                // x24: Saved register 8
  uint32_t s9;                // x25: Saved register 9
  uint32_t s10;               // x26: Saved register 10
  uint32_t s11;               // x27: Saved register 11
  uint32_t sp;                // x2: Stack Pointer - points to current stack position
} __attribute__((packed));

// Macro to read a Control and Status Register (CSR)
// CSRs are special RISC-V registers that control processor behavior
#define READ_CSR(reg)                                                          \
  ({                                                                           \
    unsigned long __tmp;                                                       \
    __asm__ __volatile__("csrr %0, " #reg : "=r"(__tmp));                      \
    __tmp;                                                                     \
  })

#define WRITE_CSR(reg, value)                                                  \
  do {                                                                         \
    uint32_t __tmp = (value);                                                  \
    __asm__ __volatile__("csrw " #reg ", %0" ::"r"(__tmp));                    \
  } while (0)

// Process Control Block (PCB) structure - represents a process in the kernel
struct process {
  int pid;                    // Process ID - unique identifier for each process
  int state;                  // Current state of the process (UNUSED or RUNNABLE)
  vaddr_t sp;                 // Saved stack pointer of the process
  uint8_t stack[8192];        // 8KB kernel stack for the process
};

// Structure to hold the return values from an SBI (Supervisor Binary Interface)
// call
struct ret_sbi {
  long err; // Error code returned by the SBI call, if any
  long val; // Value returned by the SBI call, typically used for successful
            // results
};

// Function to make an SBI call with given arguments
// SBI provides a way for the OS to interact with the hardware/firmware
struct ret_sbi call_sbi(long arg0, long arg1, long arg2, long arg3, long arg4,
                        long arg5, long fid, long eid);

// Function to output a character using SBI console putchar extension
void putchar(char ch);

// Main function of the kernel - initializes the system and starts processes
void kernel_main(void);

// PANIC macro to print an error message and halt the system
// Used for unrecoverable error conditions in the kernel
#define PANIC(fmt, ...)                                                        \
  do {                                                                         \
    printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);      \
    while (1) {                                                                \
    }                                                                          \
  } while (0)
