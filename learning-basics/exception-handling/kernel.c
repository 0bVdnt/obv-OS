#include "kernel.h"  // Include kernel-specific definitions
#include "common.h"  // Include common utility definitions

// External symbols defined elsewhere, here in the linker script
extern char __bss[], __bss_end[], __stack_top[];

// Function to make an SBI call with given arguments
struct ret_sbi call_sbi(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long fid, long eid) {
    // Register assignments for the SBI call
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a6 __asm__("a6") = fid;  // Function ID
    register long a7 __asm__("a7") = eid;  // Extension ID

    // Inline assembly to perform the ecall instruction
    __asm__ __volatile__("ecall"
                        : "=r"(a0), "=r"(a1)    // Output operands
                        : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)  // Input operands
                        : "memory");  // Clobbered registers
    return (struct ret_sbi){.err = a0, .val = a1};  // Return the result
}

// Function to output a character using SBI
void putchar(char ch) {
    call_sbi(ch, 0, 0, 0, 0, 0, 0, 1);  // Call SBI to output the character
}

// Main function of the kernel
void kernel_main(void) {
    // Clear the BSS section
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);
    // Trigger a panic to indicate boot
    PANIC("booted!");
    // This line should never be reached
    printf("unreachable here!\n");
}

__attribute__((naked))     // No compiler-generated prologue/epilogue code
__attribute__((aligned(4))) // Ensure function is 4-byte aligned in memory
void kernel_entry(void) {
    __asm__ __volatile__(
        // Save the current stack pointer to sscratch CSR (Control and Status Register)
        "csrw sscratch, sp\n"
        
        // Allocate space on stack for 31 registers (31 * 4 bytes), here 4 is negative because the stack is supposed to be grown downwards.
        "addi sp, sp, -4 * 31\n"
        
        // Save all general purpose registers to stack
        // Format: "sw reg, offset(sp)" - Store Word instruction
        "sw ra,  4 * 0(sp)\n"   // Return Address
        "sw gp,  4 * 1(sp)\n"   // Global Pointer
        "sw tp,  4 * 2(sp)\n"   // Thread Pointer
        "sw t0,  4 * 3(sp)\n"   // Temporary registers
        "sw t1,  4 * 4(sp)\n"
        "sw t2,  4 * 5(sp)\n"
        "sw t3,  4 * 6(sp)\n"
        "sw t4,  4 * 7(sp)\n"
        "sw t5,  4 * 8(sp)\n"
        "sw t6,  4 * 9(sp)\n"
        "sw a0,  4 * 10(sp)\n"  // Argument registers
        "sw a1,  4 * 11(sp)\n"
        "sw a2,  4 * 12(sp)\n"
        "sw a3,  4 * 13(sp)\n"
        "sw a4,  4 * 14(sp)\n"
        "sw a5,  4 * 15(sp)\n"
        "sw a6,  4 * 16(sp)\n"
        "sw a7,  4 * 17(sp)\n"
        "sw s0,  4 * 18(sp)\n"  // Saved registers
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

        // Read original sp from sscratch and save it
        "csrr a0, sscratch\n"   // Read from sscratch into a0
        "sw a0, 4 * 30(sp)\n"   // Store original sp

        // Prepare for trap handler call
        "mv a0, sp\n"           // Move stack pointer to a0 (first argument)
        "call handle_trap\n"     // Call the C trap handler

        // Restore all registers from stack
        // Format: "lw reg, offset(sp)" - Load Word instruction
        "lw ra,  4 * 0(sp)\n"   // Return Address
        "lw gp,  4 * 1(sp)\n"   // Global Pointer
        "lw tp,  4 * 2(sp)\n"   // Thread Pointer
        "lw t0,  4 * 3(sp)\n"   // Temporary registers
        // ... (similar pattern for all registers)
        "lw s11, 4 * 29(sp)\n"
        "lw sp,  4 * 30(sp)\n"  // Restore original stack pointer

        "sret\n"                // Return from supervisor mode trap
    );
}

void handle_trap(struct trap_frame *f) {
    uint32_t scause = READ_CSR(scause);   // Read the cause of the trap
    uint32_t stval = READ_CSR(stval);     // Read the trap value
    uint32_t user_pc = READ_CSR(sepc);    // Read the program counter where trap occurred

    // Print error and halt the system
    PANIC("unexpected trap scause=%x, stval=%x, sepc=%x\n", scause, stval, user_pc);
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
