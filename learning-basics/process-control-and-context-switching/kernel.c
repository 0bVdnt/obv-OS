#include "kernel.h" // Include kernel-specific definitions
#include "common.h" // Include common utility definitions

// RISC-V Process Control and Context Switching Implementation
// This file demonstrates basic process management and context switching between processes

// External symbols defined in the linker script (kernel.ld)
// These symbols mark different memory regions
extern char __bss[], __bss_end[], __stack_top[], __free_ram[], __free_ram_end[];

// Function to make an SBI call with given arguments
// SBI (Supervisor Binary Interface) provides a way for the OS to interact with the machine
struct ret_sbi call_sbi(long arg0, long arg1, long arg2, long arg3, long arg4,
                        long arg5, long fid, long eid) {
  // Register assignments for the SBI call (RISC-V ABI registers)
  register long a0 __asm__("a0") = arg0;
  register long a1 __asm__("a1") = arg1;
  register long a2 __asm__("a2") = arg2;
  register long a3 __asm__("a3") = arg3;
  register long a4 __asm__("a4") = arg4;
  register long a5 __asm__("a5") = arg5;
  register long a6 __asm__("a6") = fid; // Function ID for the SBI call
  register long a7 __asm__("a7") = eid; // Extension ID for the SBI call

  // Inline assembly to perform the ecall instruction
  // ecall is used to make a system call to a higher privilege level
  __asm__ __volatile__("ecall"
                       : "=r"(a0), "=r"(a1) // Output operands - return values
                       : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                         "r"(a6), "r"(a7)        // Input operands
                       : "memory");              // Clobbered registers
  return (struct ret_sbi){.err = a0, .val = a1}; // Return the SBI call result
}

// Function to output a character using SBI
// Uses SBI console putchar extension (EID=1, FID=0)
void putchar(char ch) {
  call_sbi(ch, -1, 0, 0, 0, 0, 0, 1); // Call SBI to output the character
}

// Trap entry point - called when a trap/exception occurs
// naked attribute means no prologue/epilogue code is generated
// aligned(4) ensures proper instruction alignment
__attribute__((naked)) __attribute__((aligned(4))) void kernel_entry(void) {
  // Save all registers to the stack to preserve the execution context
  __asm__ __volatile__("csrw sscratch, sp\n"          // Save original sp to sscratch
                       "addi sp, sp, -4 * 31\n"       // Allocate stack space for 31 registers
                       "sw ra,  4 * 0(sp)\n"          // Save return address
                       "sw gp,  4 * 1(sp)\n"          // Save global pointer
                       "sw tp,  4 * 2(sp)\n"          // Save thread pointer
                       "sw t0,  4 * 3(sp)\n"          // Save temporary registers
                       "sw t1,  4 * 4(sp)\n"
                       "sw t2,  4 * 5(sp)\n"
                       "sw t3,  4 * 6(sp)\n"
                       "sw t4,  4 * 7(sp)\n"
                       "sw t5,  4 * 8(sp)\n"
                       "sw t6,  4 * 9(sp)\n"
                       "sw a0,  4 * 10(sp)\n"         // Save argument registers
                       "sw a1,  4 * 11(sp)\n"
                       "sw a2,  4 * 12(sp)\n"
                       "sw a3,  4 * 13(sp)\n"
                       "sw a4,  4 * 14(sp)\n"
                       "sw a5,  4 * 15(sp)\n"
                       "sw a6,  4 * 16(sp)\n"
                       "sw a7,  4 * 17(sp)\n"
                       "sw s0,  4 * 18(sp)\n"         // Save saved registers
                       "sw s1,  4 * 19(sp)\n"
                       "sw s2,  4 * 20(sp)\n"
                       "sw s3,  4 * 21(sp)\n"
                       "sw s4,  4 * 22(sp)\n"
                       "sw s5,  4 * 23(sp)\n"
                       "sw s6,  4 * 24(sp)\n"
                       "sw s7,  4 * 25(sp)\n"
                       "sw s8,  4 * 26(sp)\n"
                       "sw s9,  4 * 27(sp)\n"
                       "sw s10, 4 * 28(sp)\n"
                       "sw s11, 4 * 29(sp)\n"

                       "csrr a0, sscratch\n"          // Retrieve original sp from sscratch
                       "sw a0, 4 * 30(sp)\n"          // Save original sp to stack

                       "mv a0, sp\n"                  // Pass stack pointer as argument to handle_trap
                       "call handle_trap\n"           // Call the trap handler function

                       // Restore all registers from the stack
                       "lw ra,  4 * 0(sp)\n"          // Restore return address
                       "lw gp,  4 * 1(sp)\n"          // Restore global pointer
                       "lw tp,  4 * 2(sp)\n"          // Restore thread pointer
                       "lw t0,  4 * 3(sp)\n"          // Restore temporary registers
                       "lw t1,  4 * 4(sp)\n"
                       "lw t2,  4 * 5(sp)\n"
                       "lw t3,  4 * 6(sp)\n"
                       "lw t4,  4 * 7(sp)\n"
                       "lw t5,  4 * 8(sp)\n"
                       "lw t6,  4 * 9(sp)\n"
                       "lw a0,  4 * 10(sp)\n"         // Restore argument registers
                       "lw a1,  4 * 11(sp)\n"
                       "lw a2,  4 * 12(sp)\n"
                       "lw a3,  4 * 13(sp)\n"
                       "lw a4,  4 * 14(sp)\n"
                       "lw a5,  4 * 15(sp)\n"
                       "lw a6,  4 * 16(sp)\n"
                       "lw a7,  4 * 17(sp)\n"
                       "lw s0,  4 * 18(sp)\n"         // Restore saved registers
                       "lw s1,  4 * 19(sp)\n"
                       "lw s2,  4 * 20(sp)\n"
                       "lw s3,  4 * 21(sp)\n"
                       "lw s4,  4 * 22(sp)\n"
                       "lw s5,  4 * 23(sp)\n"
                       "lw s6,  4 * 24(sp)\n"
                       "lw s7,  4 * 25(sp)\n"
                       "lw s8,  4 * 26(sp)\n"
                       "lw s9,  4 * 27(sp)\n"
                       "lw s10, 4 * 28(sp)\n"
                       "lw s11, 4 * 29(sp)\n"
                       "lw sp,  4 * 30(sp)\n"         // Restore original stack pointer
                       "sret\n");                     // Return from supervisor mode trap
}

// Context switch function - switches execution from one process to another
// naked attribute means no prologue/epilogue code is generated
__attribute__((naked)) void switch_context(uint32_t *prev_sp,
                                           uint32_t *next_sp) {
  __asm__ __volatile__(
      // Save callee-saved registers of the current process
      "addi sp, sp, -13 * 4\n"           // Allocate stack space for 13 registers
      "sw ra,  0  * 4(sp)\n"             // Save return address
      "sw s0,  1  * 4(sp)\n"             // Save callee-saved registers (s0-s11)
      "sw s1,  2  * 4(sp)\n"             // These must be preserved across function calls
      "sw s2,  3  * 4(sp)\n"             // per RISC-V calling convention
      "sw s3,  4  * 4(sp)\n"
      "sw s4,  5  * 4(sp)\n"
      "sw s5,  6  * 4(sp)\n"
      "sw s6,  7  * 4(sp)\n"
      "sw s7,  8  * 4(sp)\n"
      "sw s8,  9  * 4(sp)\n"
      "sw s9,  10 * 4(sp)\n"
      "sw s10, 11 * 4(sp)\n"
      "sw s11, 12 * 4(sp)\n"

      // Save current stack pointer to prev_sp and load next_sp
      "sw sp, (a0)\n"                    // Save current SP to location pointed by prev_sp
      "lw sp, (a1)\n"                    // Load SP from location pointed by next_sp

      // Restore callee-saved registers of the next process
      "lw ra,  0  * 4(sp)\n"             // Restore return address
      "lw s0,  1  * 4(sp)\n"             // Restore callee-saved registers (s0-s11)
      "lw s1,  2  * 4(sp)\n"
      "lw s2,  3  * 4(sp)\n"
      "lw s3,  4  * 4(sp)\n"
      "lw s4,  5  * 4(sp)\n"
      "lw s5,  6  * 4(sp)\n"
      "lw s6,  7  * 4(sp)\n"
      "lw s7,  8  * 4(sp)\n"
      "lw s8,  9  * 4(sp)\n"
      "lw s9,  10 * 4(sp)\n"
      "lw s10, 11 * 4(sp)\n"
      "lw s11, 12 * 4(sp)\n"
      "addi sp, sp, 13 * 4\n"            // Adjust stack pointer back
      "ret\n");                          // Return to the new process
}

// Trap handler function - handles exceptions and interrupts
void handle_trap(void) {
  // Read control and status registers to get trap information
  uint32_t scause = READ_CSR(scause);    // Read cause of trap
  uint32_t stval = READ_CSR(stval);      // Read trap value
  uint32_t user_pc = READ_CSR(sepc);     // Read program counter where trap occurred

  // Print error message and halt the system
  PANIC("unexpected trap scause=%x, stval=%x, sepc=%x\n", scause, stval,
        user_pc);
}

// Simple delay function to slow down execution for demonstration purposes
void delay(void) {
  for (int i = 0; i < 100000000; i++)
    __asm__ __volatile__("nop");         // No operation instruction
}

struct process procs[PROCS_MAX]; // Array of process control structures

// Create a new process with the given entry point
struct process *create_process(uint32_t pc) {
  // Find an unused process control structure
  struct process *proc = NULL;
  int i;
  for (i = 0; i < PROCS_MAX; i++) {
    if (procs[i].state == PROC_UNUSED) {
      proc = &procs[i];
      break;
    }
  }

  // If no free process slot is found, panic
  if (!proc)
    PANIC("no free process slots");

  // Initialize the process stack with callee-saved registers
  // These register values will be restored during the first context switch
  uint32_t *sp = (uint32_t *)&proc->stack[sizeof(proc->stack)];  // Start at the top of stack
  *--sp = 0;            // s11 - Initialize all saved registers to zero
  *--sp = 0;            // s10
  *--sp = 0;            // s9
  *--sp = 0;            // s8
  *--sp = 0;            // s7
  *--sp = 0;            // s6
  *--sp = 0;            // s5
  *--sp = 0;            // s4
  *--sp = 0;            // s3
  *--sp = 0;            // s2
  *--sp = 0;            // s1
  *--sp = 0;            // s0
  *--sp = (uint32_t)pc; // ra - Return address set to the process entry point

  // Initialize process control block fields
  proc->pid = i + 1;                 // Assign process ID (1-based)
  proc->state = PROC_RUNNABLE;       // Mark process as runnable
  proc->sp = (uint32_t)sp;           // Save stack pointer
  return proc;
}

// Process control blocks for our two demo processes
struct process *proc_a;
struct process *proc_b;

// Entry point function for process A
void proc_a_entry(void) {
  printf("starting process A\n");
  while (true) {
    putchar('A');                           // Print 'A'
    switch_context(&proc_a->sp, &proc_b->sp); // Switch to process B
    delay();                                 // Wait a while before printing again
  }
}

// Entry point function for process B
void proc_b_entry(void) {
  printf("starting process B\n");
  while (true) {
    putchar('B');                           // Print 'B'
    switch_context(&proc_b->sp, &proc_a->sp); // Switch to process A
    delay();                                 // Wait a while before printing again
  }
}

// Main function of the kernel - initialization and setup
void kernel_main(void) {
  // Clear the BSS section (uninitialized data)
  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

  // Set up trap vector - configure where traps will be handled
  WRITE_CSR(stvec, (uint32_t)kernel_entry);

  // Create two processes
  proc_a = create_process((uint32_t)proc_a_entry);
  proc_b = create_process((uint32_t)proc_b_entry);
  
  // Start execution with process B
  // This function never returns as it enters the process B code
  proc_b_entry();

  // We should never reach here
  PANIC("unreachable here");
}

// Boot function - entry point from the bootloader
// section attribute places this function first in the binary
// naked attribute means no prologue/epilogue code is generated
__attribute__((section(".text.boot")))
__attribute__((naked))
void boot(void) {
  // Set up the stack pointer and jump to the kernel main function
  __asm__ __volatile__(
      "mv sp, %[stack_top]\n"        // Initialize stack pointer to __stack_top
      "j kernel_main\n"              // Jump to kernel_main
      :
      : [stack_top] "r"(__stack_top) // Input operand - stack top address
  );
}