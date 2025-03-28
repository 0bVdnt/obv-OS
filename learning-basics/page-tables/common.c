/*
 * Common Utility Functions
 * 
 * This file provides essential utility functions used throughout the kernel:
 * 1. I/O Operations
 *    - printf implementation for formatted output
 *    - putchar for basic character output
 * 
 * 2. Memory Operations
 *    - memcpy for memory copying
 *    - memset for memory initialization
 * 
 * 3. String Operations
 *    - strcpy for string copying
 *    - strcmp for string comparison
 * 
 * These functions provide the basic building blocks for:
 * - Debug output and logging
 * - Memory management
 * - String handling
 * - System initialization
 */

#include "common.h"

// I/O Operations
// Function prototype for putchar, which outputs a single character
// This is the lowest level output function, used by printf
void putchar(char c);

// I/O Operations
// Custom implementation of printf function
// Supports basic format specifiers: %s (strings), %d (integers), %x (hex)
// Used for kernel debugging and status reporting
void printf(const char *format, ...) {
  var_list args; // Declare a variable list to handle the variable arguments
  var_start(args, format); // Initialize the variable argument list

  // Loop through each character in the format string
  while (*format) {
    if (*format == '%') { // Check for format specifier
      format++;           // Move to the specifier character
      switch (*format) {
      case '\0':      // Handle end of string
        putchar('%'); // Output a literal '%'
        goto end;     // Exit the loop
      case '%':       // Handle '%%' to output a single '%'
        putchar('%');
        break;
      case 's': { // Handle string format specifier
        const char *s = var_arg(args, const char *); // Get the string argument
        while (*s) { // Output each character of the string
          putchar(*s);
          s++;
        }
        break;
      }
      case 'd': {                     // Handle integer format specifier
        int val = var_arg(args, int); // Get the integer argument
        if (val < 0) {                // Handle negative numbers
          putchar('-');
          val = -val;
        }
        int divisor = 1;
        // Calculate the divisor to extract each digit
        while (val / divisor >= 10)
          divisor *= 10;
        // Output each digit
        while (divisor > 0) {
          putchar('0' + val / divisor);
          val %= divisor;
          divisor /= 10;
        }
        break;
      }
      case 'x': {                     // Handle hexadecimal format specifier
        int val = var_arg(args, int); // Get the integer argument
        // Output each hex digit
        for (int i = 7; i >= 0; i--) {
          putchar("0123456789abcdef"[val >> (i * 4) & 0xF]);
        }
        break;
      }
      default: // Handle unknown specifiers
        putchar(*format);
        break;
      }
    } else { // Output regular characters
      putchar(*format);
    }
    format++; // Move to the next character in the format string
  }
end:
  var_end(args); // Clean up the variable argument list
}

// Memory Operations
// Copy n bytes from src to dest
// Used for memory management and data movement
void *memcpy(void *dest, const void *src, size_t n) {
  uint8_t *d = (uint8_t *)dest;
  const uint8_t *s = (const uint8_t *)src;
  while (n--)
    *d++ = *s++;

  return dest;
}

// Memory Operations
// Set n bytes of memory starting at buf to value c
// Used for memory initialization and zeroing
void *memset(void *buf, char c, size_t n) {
  uint8_t *p = (uint8_t *)buf;
  while (n--) // Set each byte to the specified character
    *p++ = c;
  return buf; // Return the pointer to the buffer
}

// String Operations
// Copy string from src to dest
// Used for string manipulation and message handling
char *strcpy(char *dest, const char *src) {
  char *p = dest;

  while (*src)
    *p++ = *src++;

  *p = '\0';
  return dest;
}

// String Operations
// Compare two strings
// Returns negative if s1 < s2, positive if s1 > s2, 0 if equal
// Used for string comparison and command parsing
int strcmp(const char *s1, const char *s2) {
  while (*s1 && *s2) {
    if (*s1 != *s2)
      break;
    s1++;
    s2++;
  }

  return *(unsigned char *)s1 - *(unsigned char *)s2;
}
