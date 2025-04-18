/*
 * RISC-V Operating System Kernel with Page Tables
 * 
 * This kernel implements:
 * 1. Virtual Memory Management
 *    - Two-level page tables (Sv32 mode)
 *    - Page allocation and mapping
 *    - Memory protection and isolation
 * 
 * 2. Process Management
 *    - Process creation and scheduling
 *    - Context switching
 *    - Process isolation through separate page tables
 * 
 * 3. System Interface
 *    - SBI (Supervisor Binary Interface) for hardware operations
 *    - Console output
 *    - Trap handling
 * 
 * 4. Boot Process
 *    - Memory initialization
 *    - Process setup
 *    - Trap vector configuration
 */

#include "kernel.h" // Include kernel-specific definitions
#include "common.h" // Include common utility definitions

// External symbols defined in the linker script
// These define important memory regions:
// __bss: Uninitialized data section
// __bss_end: End of uninitialized data section
// __stack_top: Top of the kernel stack
// __free_ram: Start of free RAM region
// __free_ram_end: End of free RAM region - marks the boundary between kernel space and user space
//                This is where the kernel's identity mapping ends and where user processes
//                can start allocating their own memory. The kernel maps all memory from
//                __kernel_base to __free_ram_end with read/write/execute permissions.
// __kernel_base: Base address of kernel code
extern char __bss[], __bss_end[], __stack_top[], __free_ram[], __free_ram_end[],
    __kernel_base[];

// Forward declarations of functions in order of use
__attribute__((naked)) __attribute__((aligned(4))) void kernel_entry(void);
__attribute__((section(".text.boot"))) __attribute__((naked)) void boot(void);
__attribute__((naked)) void switch_context(uint32_t *prev_sp, uint32_t *next_sp);
void kernel_main(void);
void handle_trap(void);
void yeild(void);
void proc_a_entry(void);
void proc_b_entry(void);

// Process management variables
struct process *curr_proc, *idle_proc;  // Current and idle processes
struct process procs[PROCS_MAX];        // Array of all processes
struct process *proc_a, *proc_b;        // User processes

// Memory Management
// Allocates n pages of physical memory
// Returns the physical address of the first page
// Pages are aligned to PAGE_SIZE (4KB) boundary
// This is the core memory allocation function used by both kernel and processes
paddr_t alloc_pages(uint32_t n) {
  // Keep track of the next available physical address
  static paddr_t next_paddr = (paddr_t)__free_ram;
  paddr_t paddr = next_paddr;
  next_paddr += n * PAGE_SIZE;

  // Check if we've exceeded available memory
  if (next_paddr > (paddr_t)__free_ram_end) {
    PANIC("out of memory for execution");
  }

  // Zero out the allocated pages
  memset((void *)paddr, 0, n * PAGE_SIZE);
  return paddr;
}

// System Interface
// Makes a call to the SBI (Supervisor Binary Interface)
// This is used for low-level hardware operations like console output
// SBI provides the interface between supervisor mode (kernel) and machine mode (firmware)
struct ret_sbi call_sbi(long arg0, long arg1, long arg2, long arg3, long arg4,
                        long arg5, long fid, long eid) {
  // Set up arguments in registers a0-a7
  register long a0 __asm__("a0") = arg0;
  register long a1 __asm__("a1") = arg1;
  register long a2 __asm__("a2") = arg2;
  register long a3 __asm__("a3") = arg3;
  register long a4 __asm__("a4") = arg4;
  register long a5 __asm__("a5") = arg5;
  register long a6 __asm__("a6") = fid; // Function ID
  register long a7 __asm__("a7") = eid; // Extension ID

  // Perform the ecall instruction to enter SBI
  __asm__ __volatile__("ecall"
                       : "=r"(a0), "=r"(a1) // Output operands
                       : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                         "r"(a6), "r"(a7)        // Input operands
                       : "memory");              // Clobbered registers
  return (struct ret_sbi){.err = a0, .val = a1}; // Return the result
}

// Output a single character using SBI console
void putchar(char ch) {
  call_sbi(ch, -1, 0, 0, 0, 0, 0, 1);
}

// Kernel entry point for handling traps/exceptions
// This function is called when a trap occurs
// It saves all registers and then calls handle_trap
__attribute__((naked)) __attribute__((aligned(4))) void kernel_entry(void) {
  __asm__ __volatile__(
    "csrrw sp, sscratch, sp\n"
    "addi sp, sp, -4 * 31\n"
    // Save all registers to stack
    "sw ra,  4 * 0(sp)\n"
    "sw gp,  4 * 1(sp)\n"
    "sw tp,  4 * 2(sp)\n"
    "sw t0,  4 * 3(sp)\n"
    "sw t1,  4 * 4(sp)\n"
    "sw t2,  4 * 5(sp)\n"
    "sw t3,  4 * 6(sp)\n"
    "sw t4,  4 * 7(sp)\n"
    "sw t5,  4 * 8(sp)\n"
    "sw t6,  4 * 9(sp)\n"
    "sw a0,  4 * 10(sp)\n"
    "sw a1,  4 * 11(sp)\n"
    "sw a2,  4 * 12(sp)\n"
    "sw a3,  4 * 13(sp)\n"
    "sw a4,  4 * 14(sp)\n"
    "sw a5,  4 * 15(sp)\n"
    "sw a6,  4 * 16(sp)\n"
    "sw a7,  4 * 17(sp)\n"
    "sw s0,  4 * 18(sp)\n"
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

    // Save original sp from sscratch
    "csrr a0, sscratch\n"
    "sw a0, 4 * 30(sp)\n"

    // Set up new sscratch value
    "addi a0, sp, 4 * 31\n"
    "csrw sscratch, a0\n"

    // Call C trap handler
    "mv a0, sp\n"
    "call handle_trap\n"

    // Restore all registers from stack
    "lw ra,  4 * 0(sp)\n"
    "lw gp,  4 * 1(sp)\n"
    "lw tp,  4 * 2(sp)\n"
    "lw t0,  4 * 3(sp)\n"
    "lw t1,  4 * 4(sp)\n"
    "lw t2,  4 * 5(sp)\n"
    "lw t3,  4 * 6(sp)\n"
    "lw t4,  4 * 7(sp)\n"
    "lw t5,  4 * 8(sp)\n"
    "lw t6,  4 * 9(sp)\n"
    "lw a0,  4 * 10(sp)\n"
    "lw a1,  4 * 11(sp)\n"
    "lw a2,  4 * 12(sp)\n"
    "lw a3,  4 * 13(sp)\n"
    "lw a4,  4 * 14(sp)\n"
    "lw a5,  4 * 15(sp)\n"
    "lw a6,  4 * 16(sp)\n"
    "lw a7,  4 * 17(sp)\n"
    "lw s0,  4 * 18(sp)\n"
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
    "lw sp,  4 * 30(sp)\n"
    "sret\n"
  );
}

// Process Management
// Context switch between processes
// Saves callee-saved registers and switches stack pointers
// This is a critical function for process scheduling and switching
__attribute__((naked)) void switch_context(uint32_t *prev_sp,
                                           uint32_t *next_sp) {
  __asm__ __volatile__(
      "addi sp, sp, -13 * 4\n"
      // Save callee-saved registers
      "sw ra,  0  * 4(sp)\n"
      "sw s0,  1  * 4(sp)\n"
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

      // Switch stack pointers
      "sw sp, (a0)\n"
      "lw sp, (a1)\n"

      // Restore callee-saved registers
      "lw ra,  0  * 4(sp)\n"
      "lw s0,  1  * 4(sp)\n"
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
      "addi sp, sp, 13 * 4\n"
      "ret\n");
}

// System Interface
// Handle traps/exceptions
// Currently just panics with trap information
// This is where page faults and other exceptions would be handled
void handle_trap(void) {
  uint32_t scause = READ_CSR(scause);  // Cause of the trap
  uint32_t stval = READ_CSR(stval);    // Trap value
  uint32_t user_pc = READ_CSR(sepc);   // Program counter at trap

  PANIC("unexpected trap scause=%x, stval=%x, sepc=%x\n", scause, stval,
        user_pc);
}

// Virtual Memory Management
// Map a virtual page to a physical page in the page table
// Implements the two-level page table structure (Sv32 mode)
// table1: First level page table
// vaddr: Virtual address to map
// paddr: Physical address to map to
// flags: Page permissions (read/write/execute)
void map_page(uint32_t *table1, uint32_t vaddr, paddr_t paddr, uint32_t flags) {
  // Check alignment
  if (!is_aligned(vaddr, PAGE_SIZE))
    PANIC("unaligned vaddr %x", vaddr);

  if (!is_aligned(paddr, PAGE_SIZE))
    PANIC("unaligned paddr %x", paddr);

  // Extract VPN[1] (first level page table index)
  uint32_t vpn1 = (vaddr >> 22) & TEN_ON_BITS;
  // If second level page table doesn't exist, create it
  if ((table1[vpn1] & PAGE_V) == 0) {
    uint32_t pt_paddr = alloc_pages(1);
    table1[vpn1] = ((pt_paddr / PAGE_SIZE) << 10) | PAGE_V;
  }

  // Extract VPN[0] (second level page table index)
  uint32_t vpn0 = (vaddr >> 12) & TEN_ON_BITS;
  // Get pointer to second level page table
  uint32_t *table0 = (uint32_t *)((table1[vpn1] >> 10) * PAGE_SIZE);
  // Create page table entry
  table0[vpn0] = ((paddr / PAGE_SIZE) << 10) | flags | PAGE_V;
}

// Process Management
// Create a new process with its own page table and stack
// Sets up initial process state including:
// - Page table with kernel space mapping
// - Stack with initial register values
// - Process control structure
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

  // Set up initial stack with callee-saved registers
  uint32_t *sp = (uint32_t *)&proc->stack[sizeof(proc->stack)];
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
  *--sp = (uint32_t)pc; // ra (return address = program counter)

  // Create page table for the process
  uint32_t *page_table = (uint32_t *)alloc_pages(1);
  // Map kernel space (identity mapping)
  for (paddr_t paddr = (paddr_t)__kernel_base; paddr < (paddr_t)__free_ram_end;
       paddr += PAGE_SIZE)
    map_page(page_table, paddr, paddr, PAGE_R | PAGE_W | PAGE_X);

  // Initialize process fields
  proc->pid = i + 1;
  proc->state = PROC_RUNNABLE;
  proc->sp = (uint32_t)sp;
  proc->page_table = page_table;
  return proc;
}

// Process Management
// Yield to next runnable process
// Implements round-robin scheduling
// Handles page table switching and context switching
void yeild(void) {
  struct process *next = idle_proc;
  // Find next runnable process
  for (int i = 0; i < PROCS_MAX; i++) {
    struct process *proc = &procs[(curr_proc->pid + i) % PROCS_MAX];
    if (proc->state == PROC_RUNNABLE && proc->pid > 0) {
      next = proc;
      break;
    }
  }
  if (next == curr_proc) {
    return;
  }

  // Switch page tables and stack pointers
  __asm__ __volatile__(
      "sfence.vma\n"  // Flush TLB
      "csrw satp, %[satp]\n"  // Set new page table
      "sfence.vma\n"  // Flush TLB again
      "csrw sscratch, %[sscratch]\n"  // Set new stack pointer
      :
      : [satp] "r"(SATP_SV32 | ((uint32_t)next->page_table / PAGE_SIZE)),
        [sscratch] "r"((uint32_t)&next->stack[sizeof(next->stack)]));

  // Perform context switch
  struct process *prev_proc = curr_proc;
  curr_proc = next;
  switch_context(&prev_proc->sp, &next->sp);
}

// A function to simulate work.
void delay(void) {
  for (int i = 0; i < 500000000; i++) {
    // Using "nop" instruction makes sure that the compiler does not 
    // optimize and remove the loop which does not perform any work
    __asm__ __volatile__("nop");
  }
}

// User Process Entry Points
// Process A: Prints 'A' repeatedly
void proc_a_entry(void) {
  printf("starting process A\n");
  while (true) {
    putchar('A');
    yeild();
    delay();    
  }
}

// Process B: Prints 'B' repeatedly
void proc_b_entry(void) {
  printf("starting process B\n");
  while (true) {
    putchar('B');
    yeild();
    delay();    
  }
}

// Boot Process
// Main kernel function
// Initializes the system and starts process scheduling
void kernel_main(void) {
  // Clear BSS section
  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

  printf("\n\n");

  // Set up trap vector
  WRITE_CSR(stvec, (uint32_t)kernel_entry);
  
  // Create processes
  idle_proc = create_process((uint32_t)NULL);
  idle_proc->pid = 0;
  curr_proc = idle_proc;
  proc_a = create_process((uint32_t)proc_a_entry);
  proc_b = create_process((uint32_t)proc_b_entry);
  
  // Start scheduling
  yeild();
  PANIC("unreachable here");
}

// Boot Process
// First code to run
// Sets up initial stack and jumps to kernel_main
__attribute__((section(".text.boot")))
__attribute__((naked))
void boot(void) {
  // Set up stack and jump to kernel_main
  __asm__ __volatile__(
      "mv sp, %[stack_top]\n"
      "j kernel_main\n"
      :
      : [stack_top] "r"(__stack_top)
  );
}

