#include "kernel.h"  // Include kernel-specific definitions
#include "common.h"  // Include common utility definitions

// Define some basic types for convenience
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

// External symbols defined elsewhere, here in the linker script
extern char __bss[], __bss_end[], __stack_top[];

//A simple implementation of memset function to set a block of memory to a specific value
void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *) buf;
    while (n--) // Set each byte to the specified character
        *p++ = c;
    return buf; // Return the pointer to the buffer
}

// Function to make an SBI call with given arguments
struct ret_sbi call_sbi(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long fid, long eid) {
    // Register assignments for the SBI call
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a6 __asm__("a6") = fid;  //Function ID
    register long a7 __asm__("a7") = eid;  //Extension ID

   // Inline assembly to perform the ecall instruction
    __asm__ __volatile__("ecall"
                        : "=r"(a0), "=r"(a1)    //Output operands
                        : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)  //Input operands
                        : "memory");  //Clobbered registers
    return (struct ret_sbi){.err = a0, .val = a1};  //Return the result
}

// Function to output a character using SBI
void putchar(char ch) {
    call_sbi(ch, 0, 0, 0, 0, 0, 0, 1);  // Call SBI to output the character
}

// Main function of the kernel
void kernel_main(void) {
    printf("Hello from obv-OS!\n");
    printf("Running on %s\n", "RISC-V.");    
    printf("1 + 2 = %d\n", 1 + 2);
    //Infinite loop to keep the kernel running
    for(;;) {
        __asm__ __volatile__("wfi");  // Wait for interrupt
    }
}

// Boot function to set up the stack and jump to kernel_main
__attribute__((section(".text.boot")))  // Place in the .text.boot section
__attribute__((naked))  // No prologue/epilogue
void boot(void) {
    // Set the stack pointer and jump to kernel_main
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n"  // Move stack top address to stack pointer 
        "j kernel_main\n"  // Jump to kernel_main
        :
        : [stack_top] "r" (__stack_top)  // Input operand
    );
}
// After preparing the arguments, the ecall instruction is executed in inline assembly.
// When this is called, the CPU's execution mode switches from kernel mode (S-Mode) to 
// OpenSBI mode (M-Mode), and OpenSBI's processing handler is invoked. Once it's done, 
// it switches back to kernel mode, and execution resumes after the ecall instruction.
