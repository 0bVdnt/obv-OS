#pragma once

// Structure to hold the return values from an SBI (Supervisor Binary Interface) call
struct ret_sbi {
    long err; // Error code returned by the SBI call, if any
    long val; // Value returned by the SBI call, typically used for successful results
};

// Function to make an SBI call with given arguments
struct ret_sbi call_sbi(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long fid, long eid);

// Function to output a character using SBI
void putchar(char ch);

// Main function of the kernel
void kernel_main(void);


// PANIC macro to print an error message and halt the system
#define PANIC(fmt, ...)                                                        \
    do {                                                                       \
        printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);  \
        while (1) {}                                                           \
    } while (0)
