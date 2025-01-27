#include "common.h"

// Function prototype for putchar, which outputs a single character
void putchar(char c);

// Custom implementation of printf function
void printf(const char* fmt, ...) {
    var_list args; // Declare a variable list to handle the variable arguments
    var_start(args, fmt); // Initialize the variable argument list

    // Loop through each character in the format string
    while(*fmt) {
        if(*fmt == '%') { // Check for format specifier
            fmt++; // Move to the specifier character
            switch(*fmt) {
                case '\0': // Handle end of string
                    putchar('%'); // Output a literal '%'
                    goto end; // Exit the loop`
                case '%': // Handle '%%' to output a single '%'
                    putchar('%');
                    break;
                case 's': { // Handle string format specifier
                    const char * s = var_arg(args, const char *); // Get the string argument
                    while(*s) { // Output each character of the string
                        putchar(*s);
                        s++;
                    }
                    break;
                }
                case 'd': { // Handle integer format specifier
                    int val = var_arg(args, int); // Get the integer argument
                    if(val < 0) { // Handle negative numbers
                        putchar('-');
                        val = -val;
                    }
                    int divisor = 1;
                    // Calculate the divisor to extract each digit
                    while(val / divisor >= 10)
                        divisor *= 10;
                    // Output each digit
                    while(divisor > 0) {
                        putchar('0' + val / divisor);
                        val %= divisor;
                        divisor /= 10;
                    }
                    break;
                }
                case 'x': { // Handle hexadecimal format specifier
                    int val = var_arg(args, int); // Get the integer argument
                    // Output each hex digit
                    for(int i = 7; i >= 0; i--) {
                        putchar("0123456789abcdef"[val >> (i * 4) & 0xF]);
                    }
                    break;
                }
                default: // Handle unknown specifiers
                    putchar(*fmt);
                    break;
            }
        } else { // Output regular characters
            putchar(*fmt);
        }
        fmt++; // Move to the next character in the format string
    }
end:
    var_end(args); // Clean up the variable argument list
}