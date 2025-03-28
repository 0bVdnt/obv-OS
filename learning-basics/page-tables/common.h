/*
 * Common Header File
 * 
 * This header defines the fundamental types and utilities used throughout the kernel:
 * 1. Basic Types
 *    - Integer types (uint8_t, uint16_t, uint32_t, uint64_t)
 *    - Boolean type
 *    - Address types (paddr_t, vaddr_t)
 *    - Size type (size_t)
 * 
 * 2. System Constants
 *    - Page size (PAGE_SIZE)
 *    - Boolean constants
 *    - NULL pointer
 * 
 * 3. Compiler Built-ins
 *    - Memory alignment functions
 *    - Structure offset calculation
 *    - Variable argument handling
 * 
 * 4. Function Declarations
 *    - Memory operations
 *    - String operations
 *    - I/O operations
 */

// Basic Types
typedef int bool;                    // Boolean type
typedef unsigned char uint8_t;       // 8-bit unsigned integer
typedef unsigned short uint16_t;     // 16-bit unsigned integer
typedef unsigned int uint32_t;       // 32-bit unsigned integer
typedef unsigned long long uint64_t; // 64-bit unsigned integer
typedef uint32_t size_t;            // Size type for memory operations
typedef uint32_t paddr_t;           // Physical address type
typedef uint32_t vaddr_t;           // Virtual address type

// System Constants
#define PAGE_SIZE 4096              // Size of a memory page in bytes
#define true 1                      // Boolean true value
#define false 0                     // Boolean false value
#define NULL ((void *)0)            // Null pointer definition

// Compiler Built-ins
// Memory alignment utilities
#define align_up(value, align) __builtin_align_up(value, align)     // Align value up to boundary
#define is_aligned(value, align) __builtin_is_aligned(value, align) // Check if value is aligned
#define offsetof(type, member) __builtin_offsetof(type, member)     // Get offset of struct member

// Variable argument handling
#define var_list __builtin_va_list   // Type for variable arguments
#define var_start __builtin_va_start // Initialize variable arguments
#define var_arg __builtin_va_arg     // Get next variable argument
#define var_end __builtin_va_end     // Clean up variable arguments

// Function Declarations
// Memory Operations
void *memset(void *buf, char c, size_t n);    // Set memory to value
void *memcpy(void *dest, const void *src, size_t n);  // Copy memory

// String Operations
char *strcpy(char *dest, const char *src);     // Copy string
int strcmp(const char *s1, const char *s2);    // Compare strings

// I/O Operations
void printf(const char *fmt, ...);             // Formatted output
