// Define boolean type as integer
typedef int bool;

// Define standard integer types with specific sizes
typedef unsigned char uint8_t;      // 8-bit unsigned integer
typedef unsigned short uint16_t;    // 16-bit unsigned integer
typedef unsigned int uint32_t;      // 32-bit unsigned integer
typedef unsigned long long uint64_t; // 64-bit unsigned integer

// Define size and address types
typedef uint32_t size_t;            // Type for sizes and counts
typedef uint32_t paddr_t;           // Type for physical addresses
typedef uint32_t vaddr_t;           // Type for virtual addresses

// Define system constants
#define PAGE_SIZE 4096              // Standard page size in bytes
#define true 1                      // Boolean true value
#define false 0                     // Boolean false value
#define NULL ((void *)0)            // Null pointer definition

// Memory alignment macros
#define align_up(value, align) __builtin_align_up(value, align)           // Align value up to specified alignment
#define is_aligned(value, align) __builtin_is_aligned(value, align)       // Check if value is aligned to specified alignment
#define offsetof(type, member) __builtin_offsetof(type, member)           // Get offset of member within struct

// Variable argument list macros
#define var_list __builtin_va_list  // Type for variable argument lists
#define var_start __builtin_va_start // Initialize variable argument list
#define var_arg __builtin_va_arg    // Get next argument from variable list
#define var_end __builtin_va_end    // End variable argument list processing

// Function declarations
void printf(const char *fmt, ...);  // Print formatted string to output
void *memset(void *buf, char c, size_t n);  // Fill memory with specified byte value
