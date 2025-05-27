// Basic type definitions for the kernel - providing standardized data types
typedef int bool;                  // Boolean type for true/false values
typedef unsigned char uint8_t;     // 8-bit unsigned integer (1 byte)
typedef unsigned short uint16_t;   // 16-bit unsigned integer (2 bytes)
typedef unsigned int uint32_t;     // 32-bit unsigned integer (4 bytes)
typedef unsigned long long uint64_t; // 64-bit unsigned integer (8 bytes)
typedef uint32_t size_t;           // Type for sizes of objects (memory operations)
typedef uint32_t paddr_t;          // Physical memory address type (32-bit)
typedef uint32_t vaddr_t;          // Virtual memory address type (32-bit)

// Memory and utility constants/macros
#define PAGE_SIZE 4096             // Memory page size in bytes (4KB)
#define true 1                     // Boolean true value
#define false 0                    // Boolean false value
#define NULL ((void *)0)           // Null pointer constant

// Memory alignment and offset calculation utilities
#define align_up(value, align) __builtin_align_up(value, align)       // Round up to the nearest multiple of align
#define is_aligned(value, align) __builtin_is_aligned(value, align)   // Check if a value is aligned
#define offsetof(type, member) __builtin_offsetof(type, member)       // Get byte offset of a struct member

// Variable argument handling macros (for printf implementation)
#define var_list __builtin_va_list       // List type for variable arguments
#define var_start __builtin_va_start     // Initialize variable argument list
#define var_arg __builtin_va_arg         // Extract next argument from list
#define var_end __builtin_va_end         // Clean up the variable argument list

// Memory and I/O utility function declarations
void *memset(void *buf, char c, size_t n);   // Set a block of memory to a specific value
void printf(const char *fmt, ...);           // Format and print a string to the console
