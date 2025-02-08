# Kernel Panic Mode

## Overview
Kernel panic mode is a critical state where the operating system encounters a fatal error from which it cannot safely recover. When a panic occurs, the system halts to prevent further damage or data corruption.

## PANIC Macro
The `PANIC` macro is used to print an error message and halt the system. It provides a formatted output that includes the file name and line number where the panic occurred.

### Definition
```c
#define PANIC(fmt, ...)                                                        \
    do {                                                                       \
        printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);  \
        while (1) {}                                                           \
    } while (0)
```

### Usage
To trigger a panic, use the `PANIC` macro with a formatted message:
```c
PANIC("Unexpected error occurred: %d", error_code);
```

## Functions

### `call_sbi`
Makes an SBI (Supervisor Binary Interface) call with the given arguments.
```c
struct ret_sbi call_sbi(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long fid, long eid);
```

### `putchar`
Outputs a character using SBI.
```c
void putchar(char ch);
```

### `kernel_main`
The main function of the kernel.
```c
void kernel_main(void);
```

## Structure

### `ret_sbi`
Holds the return values from an SBI call.
```c
struct ret_sbi {
    long err; // Error code returned by the SBI call, if any
    long val; // Value returned by the SBI call, typically used for successful results
};
```

## Conclusion
Kernel panic mode is an essential mechanism for handling unrecoverable errors in the operating system. The `PANIC` macro provides a straightforward way to log the error and halt the system to prevent further issues.
