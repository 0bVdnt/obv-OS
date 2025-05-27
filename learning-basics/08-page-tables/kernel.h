/*
 * Kernel Header File
 * 
 * This header defines kernel-specific types, structures, and macros:
 * 1. Process Management
 *    - Process states and limits
 *    - Process control structure
 *    - Process stack layout
 * 
 * 2. Memory Management
 *    - Page table flags and permissions
 *    - Virtual memory configuration
 *    - Memory protection bits
 * 
 * 3. System Interface
 *    - CSR (Control and Status Register) operations
 *    - SBI (Supervisor Binary Interface) structures
 *    - System call interface
 * 
 * 4. System Control
 *    - Trap handling structures
 *    - System panic handling
 *    - Kernel initialization
 */

#pragma once
#include "common.h"

// Process Management
#define PROCS_MAX 8                // Maximum number of processes supported
#define PROC_UNUSED 0              // Process slot is free
#define PROC_RUNNABLE 1            // Process is ready to run

// System Interface
// CSR (Control and Status Register) operations
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

// Memory Management
// Page table configuration
#define SATP_SV32 (1u << 31)      // Enable Sv32 paging mode
#define PAGE_V (1 << 0)           // Page table entry valid bit
#define PAGE_R (1 << 1)           // Page is readable
#define PAGE_W (1 << 2)           // Page is writable
#define PAGE_X (1 << 3)           // Page is executable
#define PAGE_U (1 << 4)           // Page is user-accessible

// Page table index masks
#define TEN_ON_BITS 0x3ff         // Mask for 10-bit page table indices

// System Control
// Trap handling structure - matches the register save order in kernel_entry
struct trap_frame {
  uint32_t ra;    // Return address
  uint32_t gp;    // Global pointer
  uint32_t tp;    // Thread pointer
  uint32_t t0;    // Temporary registers
  uint32_t t1;
  uint32_t t2;
  uint32_t t3;
  uint32_t t4;
  uint32_t t5;
  uint32_t t6;
  uint32_t a0;    // Argument registers
  uint32_t a1;
  uint32_t a2;
  uint32_t a3;
  uint32_t a4;
  uint32_t a5;
  uint32_t a6;
  uint32_t a7;
  uint32_t s0;    // Saved registers
  uint32_t s1;
  uint32_t s2;
  uint32_t s3;
  uint32_t s4;
  uint32_t s5;
  uint32_t s6;
  uint32_t s7;
  uint32_t s8;
  uint32_t s9;
  uint32_t s10;
  uint32_t s11;
  uint32_t sp;    // Stack pointer
} __attribute__((packed));

// Process Management
// Process control structure
struct process {
  int pid;                    // Process identifier
  int state;                  // Current process state
  vaddr_t sp;                 // Stack pointer
  uint32_t *page_table;       // Process page table
  uint8_t stack[8192];        // Process kernel stack
};

// System Interface
// Return values from SBI (Supervisor Binary Interface) calls
struct ret_sbi {
  long err;                   // Error code from SBI call
  long val;                   // Return value from SBI call
};

// Function Declarations
// System Interface
struct ret_sbi call_sbi(long arg0, long arg1, long arg2, long arg3, long arg4,
                        long arg5, long fid, long eid);  // Make SBI call
void putchar(char ch);                                  // Output character

// System Control
void kernel_main(void);                                // Kernel entry point

// Error Handling
// PANIC macro for system errors - prints message and halts
#define PANIC(fmt, ...)                                                        \
  do {                                                                         \
    printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);      \
    while (1) {                                                                \
    }                                                                          \
  } while (0)
