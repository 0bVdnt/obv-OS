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

__attribute__((noreturn)) void
exit(void) { // __attribute__((noreturn)) tells the compiler this function will
             // not return.
  for (;;)
    ;
}

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

void putchar(char ch) { syscall(SYS_PUTCHAR, ch, 0, 0); }
