#pragma once
#include "common.h"

/*
 * user.h - Header file for user-space programs
 *
 * Purpose:
 *   Declares essential functions and includes shared definitions for user-space programs.
 *
 * Key Components:
 *   - #pragma once: Ensures single inclusion per compilation unit.
 *   - Includes common.h for shared definitions.
 *   - Declares exit() (noreturn) and putchar() for user-space.
 */

__attribute__((noreturn)) void exit(void); // __attribute__((noreturn)) tells the compiler this function will not return.
void putchar(char ch);
