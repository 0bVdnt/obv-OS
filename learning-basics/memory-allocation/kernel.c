#include "kernel.h" // Include kernel-specific definitions
#include "common.h" // Include common utility definitions

// External symbols defined elsewhere, here in the linker script
extern char __bss[], __bss_end[], __stack_top[], __free_ram[], __free_ram_end[];

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
