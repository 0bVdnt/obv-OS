/*
 * user.c - Minimal runtime and entry point for user-space programs
 *
 * Purpose:
 *   Provides the minimal runtime and entry point for user-space programs in the
 * OS. Defines how a user program starts, how it exits, and sets up the stack
 * for execution.
 *
 * Key Components:
 *   - Includes user.h for user-space function declarations and dependencies.
 *   - Uses extern char __stack_top[] defined in the linker script (user.ld) to
 * initialize the stack pointer.
 *   - exit(): Infinite loop as a placeholder for program termination. Marked
 * noreturn.
 *   - start(): Entry point for user programs, placed in .text.start section,
 * naked attribute for precise control. Sets stack pointer, calls main, then
 * exit. Ensures clean stack and program termination.
 *   - Commented-out putchar(): Placeholder for user-space output, not
 * implemented here.
 */
#include "user.h"

extern char __stack_top[];


// __attribute__((section(".text.start"))) places this function in the
// .text.start section.
// __attribute__((naked)) prevents the compiler from adding function
// prologue/epilogue.
__attribute__((section(".text.start"))) __attribute__((naked)) void
start(void) {
  __asm__ __volatile__("mv sp, %[stack_top] \n"
    "call main           \n"
    "call exit           \n" ::[stack_top] "r"(__stack_top));
}

/*
 * System call interface for user programs
 *
 * syscall() issues an ECALL instruction to request a service from the kernel.
 * - sysno: system call number (e.g., SYS_PUTCHAR)
 * - arg0, arg1, arg2: arguments to the syscall (placed in a0, a1, a2)
 *
 * The ECALL instruction triggers a trap to the kernel, which handles the
 * syscall. The return value from the kernel is returned in a0.
 *
 * Syntax explanation:
 *   register int a0 __asm__("a0") = arg0; // Place arg0 in register a0
 *   __asm__ __volatile__("ecall" ...);     // Inline assembly to trigger ECALL
 */
int syscall(int sysno, int arg0, int arg1, int arg2) {
  register int a0 __asm__("a0") = arg0;
  register int a1 __asm__("a1") = arg1;
  register int a2 __asm__("a2") = arg2;
  register int a3 __asm__("a3") = sysno;
  
  __asm__ __volatile__("ecall"
    : "=r"(a0)
    : "r"(a0), "r"(a1), "r"(a2), "r"(a3)
    : "memory");
    
    return a0;
  }
  
  /*
  * putchar - user-level function to print a character to the console
  *
  * Issues a SYS_PUTCHAR system call (ECALL) to the kernel.
  *   - The character to print is passed as the first argument (a0).
  *   - The kernel receives the request, performs the output (e.g., via SBI), and returns control to the user program.
  *
  * This ensures user programs cannot access hardware directly and must use the kernel for I/O.
  */
 void putchar(char ch) { syscall(SYS_PUTCHAR, ch, 0, 0); }
 
 /*
 * getchar - user-level function to input a character from the console
 *
 * Issues a SYS_GETCHAR system call (ECALL) to the kernel.
 *   - No arguments are needed.
 *   - The kernel waits for input and returns the character in a0.
 *
 * This allows user programs to read input in a safe, controlled way, with the kernel mediating all device access.
 *
 * Example usage:
 *   char c = getchar(); // reads a character from the console via a syscall
 */
int getchar(void) { return syscall(SYS_GETCHAR, 0, 0, 0); }

/*
 * exit - user-level function to terminate the current process
 *
 * Issues a SYS_EXIT system call (ECALL) to the kernel.
 *   - No arguments are needed.
 *   - The kernel marks the process as exited and schedules another process.
 *
 * This is the standard way for a user program to terminate itself in a controlled manner.
 *
 * Example usage:
 *   exit(); // terminates the current process via a syscall
 */
__attribute__((noreturn)) void
exit(void) { // __attribute__((noreturn)) tells the compiler this function will not return.
  syscall(SYS_EXIT, 0, 0, 0);
  for (;;); // Should be generally unreachable.
}