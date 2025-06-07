/*
 * NOTES:
 * - Now intentionally triggers a memory access fault to demonstrate user mode
 * isolation:
 *   *((volatile int *)0x80500000) = 0x69420;
 *   (This address is outside the user program's allowed range, so this should
 * cause a trap.)
 *
 * SYNTAX/CONCEPT NOTES:
 * - volatile: tells the compiler not to optimize away the memory access.
 * - Infinite loop: placeholder for a more complex shell or user program.
 */
#include "user.h"

void main(void) { printf("Hello World from shell!\n"); }
