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
 *     Brings in the syscall interface and putchar declaration for user
 * programs.
 */
#include "user.h"

// void main(void) { printf("Hello World from the shell!\n"); }

void main(void) {
  while (1) {
  prompt:
    printf("> ");
    char cmdline[128];
    for (int i = 0;; i++) {
      char ch = getchar();
      putchar(ch);
      if (i == sizeof(cmdline) - 1) {
        printf("command line too long\n");
        goto prompt;
      } else if (ch == '\r') {
        // NOTE: On the debug console, the newline character is ('\r').
        printf("\n");
        cmdline[i] = '\0';
        break;
      } else {
        cmdline[i] = ch;
      }
    }
    if (strcmp(cmdline, "hello") == 0)
      printf("Hello World form the shell!\n");
    else
      printf("unknown command: %s\n", cmdline);
  }
}
