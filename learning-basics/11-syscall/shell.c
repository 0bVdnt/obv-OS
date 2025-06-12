/*
 * shell.c - Example user program demonstrating system call usage
 *
 * This program prints a message using printf, which internally calls putchar.
 * In 11-syscall, putchar is implemented as a system call (ECALL) to the kernel.
 *
 * Key syntax:
 *   - printf("Hello World from shell!\n");
 *     This triggers a series of putchar syscalls, each issuing an ECALL.
 *
 *   - #include "user.h"
 *     Brings in the syscall interface and putchar declaration for user programs.
 */
#include "user.h"

void main(void) { printf("Hello World from shell!\n"); }
