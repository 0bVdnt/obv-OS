#pragma once
#include "common.h"

/*
 * user.h - Header file for user-space programs
 *
 * Purpose:
 *   Declares essential functions and includes shared definitions for user-space programs.
 *   In 11-syscall, this header provides the syscall interface for user programs.
 *
 * Key additions in 11-syscall:
 *   - putchar(char ch): Now implemented as a system call (ECALL) to the kernel.
 *   - SYS_PUTCHAR: System call number for outputting a character (see common.h).
 *   - exit(): Terminates the user program (infinite loop for now).
 *
 * Syntax notes:
 *   - __attribute__((noreturn)) tells the compiler that exit() never returns.
 */
__attribute__((noreturn)) void exit(void); // __attribute__((noreturn)) tells the compiler this function will not return.
void putchar(char ch);
