#include "kernel.h"         // Include kernel declarations
#include "common.h"         // Include common utilities

// External symbols defined in linker script
extern char __bss[], __bss_end[], __stack_top[], __free_ram[], __free_ram_end[];

// Function to make Supervisor Binary Interface (SBI) calls
// This is the interface between the kernel and the SBI firmware
struct ret_sbi call_sbi(long arg0, long arg1, long arg2, long arg3, long arg4,
                        long arg5, long fid, long eid) {
    // Map arguments to specific registers as per SBI calling convention
    register long a0 __asm__("a0") = arg0;    // First argument
    register long a1 __asm__("a1") = arg1;    // Second argument
    register long a2 __asm__("a2") = arg2;    // Third argument
    register long a3 __asm__("a3") = arg3;    // Fourth argument
    register long a4 __asm__("a4") = arg4;    // Fifth argument
    register long a5 __asm__("a5") = arg5;    // Sixth argument
    register long a6 __asm__("a6") = fid;     // Function ID
    register long a7 __asm__("a7") = eid;     // Extension ID

    // Execute the SBI call and return results
    __asm__ __volatile__("ecall"
                       : "=r"(a0), "=r"(a1)   // Output: error code and return value
                       : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                         "r"(a6), "r"(a7)     // Input: all arguments
                       : "memory");            // Clobbers memory
    return (struct ret_sbi){.err = a0, .val = a1};
}

// Function to output a character using SBI console
void putchar(char ch) {
  call_sbi(ch, -1, 0, 0, 0, 0, 0, 1); // Call SBI to output the character
}

// Entry point for handling traps/exceptions
// Must be naked to handle its own stack frame
__attribute__((naked)) __attribute__((aligned(4)))
void kernel_entry(void) {
    __asm__ __volatile__(
        // Save process stack pointer and switch to kernel stack
        "csrrw sp, sscratch, sp\n"           // Swap sp and sscratch
        "addi sp, sp, -4 * 31\n"             // Allocate space for registers
        
        // Save all registers to kernel stack
        "sw ra,  4 * 0(sp)\n"                // Save return address
        "sw gp,  4 * 1(sp)\n"                // Save global pointer
        "sw tp,  4 * 2(sp)\n"                // Save thread pointer
        "sw t0,  4 * 3(sp)\n"                // Save temporary registers
        "sw t1,  4 * 4(sp)\n"
        "sw t2,  4 * 5(sp)\n"
        "sw t3,  4 * 6(sp)\n"
        "sw t4,  4 * 7(sp)\n"
        "sw t5,  4 * 8(sp)\n"
        "sw t6,  4 * 9(sp)\n"
        "sw a0,  4 * 10(sp)\n"               // Save argument registers
        "sw a1,  4 * 11(sp)\n"
        "sw a2,  4 * 12(sp)\n"
        "sw a3,  4 * 13(sp)\n"
        "sw a4,  4 * 14(sp)\n"
        "sw a5,  4 * 15(sp)\n"
        "sw a6,  4 * 16(sp)\n"
        "sw a7,  4 * 17(sp)\n"
        "sw s0,  4 * 18(sp)\n"               // Save saved registers
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

        // Save user stack pointer
        "csrr a0, sscratch\n"
        "sw a0, 4 * 30(sp)\n"

        // Update sscratch with kernel stack
        "addi a0, sp, 4 * 31\n"
        "csrw sscratch, a0\n"

        // Call C trap handler
        "mv a0, sp\n"                        // Pass stack pointer as argument
        "call handle_trap\n"                 // Call C handler
        
        // Restore all registers and return
        "lw ra,  4 * 0(sp)\n"               // Restore return address
        "lw gp,  4 * 1(sp)\n"               // Restore global pointer
        "lw tp,  4 * 2(sp)\n"               // Restore thread pointer
        "lw t0,  4 * 3(sp)\n"               // Restore temporary registers
        "lw t1,  4 * 4(sp)\n"
        "lw t2,  4 * 5(sp)\n"
        "lw t3,  4 * 6(sp)\n"
        "lw t4,  4 * 7(sp)\n"
        "lw t5,  4 * 8(sp)\n"
        "lw t6,  4 * 9(sp)\n"
        "lw a0,  4 * 10(sp)\n"              // Restore argument registers
        "lw a1,  4 * 11(sp)\n"
        "lw a2,  4 * 12(sp)\n"
        "lw a3,  4 * 13(sp)\n"
        "lw a4,  4 * 14(sp)\n"
        "lw a5,  4 * 15(sp)\n"
        "lw a6,  4 * 16(sp)\n"
        "lw a7,  4 * 17(sp)\n"
        "lw s0,  4 * 18(sp)\n"              // Restore saved registers
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
        "lw sp,  4 * 30(sp)\n"              // Restore user stack pointer
        "sret\n"                            // Return from supervisor mode
    );
}

// Function to switch execution context between processes
__attribute__((naked))
void switch_context(uint32_t *prev_sp, uint32_t *next_sp) {
    __asm__ __volatile__(
        // Save current context
        "addi sp, sp, -13 * 4\n"            // Allocate stack space
        "sw ra,  0  * 4(sp)\n"              // Save return address
        "sw s0,  1  * 4(sp)\n"              // Save saved registers
        "sw s1,  2  * 4(sp)\n"
        "sw s2,  3  * 4(sp)\n"
        "sw s3,  4  * 4(sp)\n"
        "sw s4,  5  * 4(sp)\n"
        "sw s5,  6  * 4(sp)\n"
        "sw s6,  7  * 4(sp)\n"
        "sw s7,  8  * 4(sp)\n"
        "sw s8,  9  * 4(sp)\n"
        "sw s9,  10 * 4(sp)\n"
        "sw s10, 11 * 4(sp)\n"
        "sw s11, 12 * 4(sp)\n"

        // Switch stacks
        "sw sp, (a0)\n"                     // Save current stack pointer
        "lw sp, (a1)\n"                     // Load new stack pointer

        // Restore new context and return
        "lw ra,  0  * 4(sp)\n"              // Restore return address
        "lw s0,  1  * 4(sp)\n"              // Restore saved registers
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
        "addi sp, sp, 13 * 4\n"             // Restore stack pointer
        "ret\n"                             // Return to new context
    );
}

// Trap handler function to process exceptions and interrupts
void handle_trap() {
  uint32_t scause = READ_CSR(scause);       // Get cause of trap
  uint32_t stval = READ_CSR(stval);         // Get trap value
  uint32_t user_pc = READ_CSR(sepc);        // Get user program counter

  PANIC("unexpected trap scause=%x, stval=%x, sepc=%x\n", scause, stval,
        user_pc);
}

// Global process management variables
struct process *curr_proc, *idle_proc;       // Current and idle process pointers
struct process procs[PROCS_MAX];             // Array of all process control structures

// Function to yield CPU to another process
void yeild(void) {
    // Find next runnable process
    struct process *next = idle_proc;        // Default to idle process
    for (int i = 0; i < PROCS_MAX; i++) {
        struct process *proc = &procs[(curr_proc->pid + i) % PROCS_MAX];
        if (proc->state == PROC_RUNNABLE && proc->pid > 0) {
            next = proc;
            break;
        }
    }

    // If no other process is runnable, return
    if (next == curr_proc) {
        return;
    }

    // Update kernel stack pointer in sscratch
    __asm__ __volatile__(
        "csrw sscratch, %[sscratch]\n"
        :
        : [sscratch] "r"((uint32_t)&next->stack[sizeof(next->stack)])
    );

    // Switch to next process
    struct process *prev_proc = curr_proc;
    curr_proc = next;
    switch_context(&prev_proc->sp, &next->sp);
}

// Function to create a new process
struct process *create_process(uint32_t pc) {
    // Find unused process slot
    struct process *proc = NULL;
    int i;
    for (i = 0; i < PROCS_MAX; i++) {
        if (procs[i].state == PROC_UNUSED) {
            proc = &procs[i];
            break;
        }
    }

    if (!proc)
        PANIC("no free process slots");

    // Initialize process stack frame
    uint32_t *sp = (uint32_t *)&proc->stack[sizeof(proc->stack)];
    // Zero out saved registers
    *--sp = 0;            // s11
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
    *--sp = (uint32_t)pc; // Set return address to entry point

    // Initialize process control block
    proc->pid = i + 1;                    // Set process ID
    proc->state = PROC_RUNNABLE;          // Set initial state
    proc->sp = (uint32_t)sp;              // Save stack pointer
    return proc;
}

// Simple delay function for timing purposes
void delay(void) {
  for (int i = 0; i < 500000000; i++)
    __asm__ __volatile__("nop");
}

// Global process pointers for demonstration
struct process *proc_a;
struct process *proc_b;

void proc_a_entry(void) {
  printf("starting process A\n");
  while (true) {
    putchar('A');
    yeild();
    delay();
  }
}

void proc_b_entry(void) {
  printf("starting process B\n");
  while (true) {
    putchar('B');
    yeild();
    delay();
  }
}

void kernel_main(void) {
  // Clear the BSS section
  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

  printf("\n\n");

  WRITE_CSR(stvec, (uint32_t)kernel_entry);

  idle_proc = create_process((uint32_t)NULL);
  idle_proc->pid = 0;
  curr_proc = idle_proc;

  proc_a = create_process((uint32_t)proc_a_entry);
  proc_b = create_process((uint32_t)proc_b_entry);
  yeild();

  PANIC("unreachable here");
}

__attribute__((section(".text.boot")))  // Place in the .text.boot section
__attribute__((naked))  // No prologue/epilogue
void boot(void) {
  // Set the stack pointer and jump to kernel_main
  __asm__ __volatile__(
      "mv sp, %[stack_top]\n" // Move stack top address to stack pointer
      "j kernel_main\n"       // Jump to kernel_main
      :
      : [stack_top] "r"(__stack_top) // Input operand
  );
}
