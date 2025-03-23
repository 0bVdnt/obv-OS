#pragma once

// Function to output a character using SBI
void putchar(char ch);

// Main function of the kernel
void kernel_main(void);

// PANIC macro to print an error message and halt the system
#define PANIC(fmt, ...)                                                        \
  do {                                                                         \
    printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);      \
    while (1) {                                                                \
    }                                                                          \
  } while (0)
