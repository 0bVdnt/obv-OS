#include "kernel.h" // Include kernel-specific definitions
#include "common.h" // Include common utility definitions

// External symbols defined elsewhere, here in the linker script
extern char __bss[], __bss_end[], __stack_top[], __free_ram[], __free_ram_end[];

// Function to make an SBI call with given arguments
struct ret_sbi call_sbi(long arg0, long arg1, long arg2, long arg3, long arg4,
                        long arg5, long fid, long eid) {
  // Register assignments for the SBI call
  register long a0 __asm__("a0") = arg0;
  register long a1 __asm__("a1") = arg1;
  register long a2 __asm__("a2") = arg2;
  register long a3 __asm__("a3") = arg3;
  register long a4 __asm__("a4") = arg4;
  register long a5 __asm__("a5") = arg5;
  register long a6 __asm__("a6") = fid; // Function ID
  register long a7 __asm__("a7") = eid; // Extension ID

  // Inline assembly to perform the ecall instruction
  __asm__ __volatile__("ecall"
                       : "=r"(a0), "=r"(a1) // Output operands
                       : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                         "r"(a6), "r"(a7)        // Input operands
                       : "memory");              // Clobbered registers
  return (struct ret_sbi){.err = a0, .val = a1}; // Return the result
}

// Function to output a character using SBI
void putchar(char ch) {
  call_sbi(ch, 0, 0, 0, 0, 0, 0, 1); // Call SBI to output the character
}

// A function to allocate pages of memory for a program
paddr_t alloc_pages(uint32_t n) {
  static paddr_t next_paddr = (paddr_t)__free_ram;
  paddr_t paddr = next_paddr;
  next_paddr += n * PAGE_SIZE;

  if (next_paddr > (paddr_t)__free_ram_end) {
    PANIC("out of memory for execution");
  }

  memset((void *)paddr, 0, n * PAGE_SIZE);
  return paddr;
}

// Main function of the kernel
void kernel_main(void) {
  // Clear the BSS section
  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

  paddr_t paddr0 = alloc_pages(2);
  paddr_t paddr1 = alloc_pages(1);
  printf("alloc_pages test: paddr0=%x\n", paddr0);
  printf("alloc_pages test: paddr1=%x\n", paddr1);

  PANIC("booted");
}

// Boot function to set up the stack and jump to kernel_main
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
