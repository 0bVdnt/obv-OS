typedef int bool;                        // Define boolean type as an integer
typedef unsigned char uint8_t;           // Define 8-bit unsigned integer type
typedef unsigned short uint16_t;         // Define 16-bit unsigned integer type
typedef unsigned int uint32_t;           // Define 32-bit unsigned integer type
typedef unsigned long long uint64_t;     // Define 64-bit unsigned integer type
typedef uint32_t size_t;                 // Define size_t type for memory sizes and array indices
typedef uint32_t paddr_t;                // Define physical address type (32-bit)
typedef uint32_t vaddr_t;                // Define virtual address type (32-bit)

#define PAGE_SIZE 4096                   // Define memory page size as 4KB (4096 bytes)
#define true 1                           // Define true as 1 for boolean operations
#define false 0                          // Define false as 0 for boolean operations
#define NULL ((void *)0)                 // Define NULL as a void pointer to address 0
#define align_up(value, align) __builtin_align_up(value, align)       // Macro to align a value up to the next alignment boundary
#define is_aligned(value, align) __builtin_is_aligned(value, align)   // Macro to check if a value is aligned to the specified boundary
#define offsetof(type, member) __builtin_offsetof(type, member)       // Macro to get the byte offset of a member within a struct
#define var_list __builtin_va_list                                    // Type for handling variable argument lists
#define var_start __builtin_va_start                                  // Macro to initialize variable argument processing
#define var_arg __builtin_va_arg                                      // Macro to access the next argument in a variable argument list
#define var_end __builtin_va_end                                      // Macro to clean up after variable argument processing

void *memset(void *buf, char c, size_t n);   // Function to fill memory block with a specific value
void printf(const char *fmt, ...);           // Function for formatted output, similar to C standard printf
