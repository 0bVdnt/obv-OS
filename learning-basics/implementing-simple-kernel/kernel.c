typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];

//A simple implementation of memset function
void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *) buf;
    while (n--)
        *p++ = c;
    return buf;
}

void kernel_main(void) {
    //Initialize the .bss section to 0
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);
    //Keep the kernel running indefinitely
    for (;;);
}

__attribute__((section(".text.boot")))
__attribute__((naked))
void boot(void) {
    // Set the stack pointer and jump to kernel_main
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n"
        "j kernel_main\n"
        :
        : [stack_top] "r" (__stack_top)
    );
}

// Kernel Entry Point
//      The kernel execution starts from the boot function, which is specified as the entry point in the linker script. The boot function sets the stack pointer (sp) to the end address of the stack area and then jumps to the kernel_main function.

// Boot Function Attributes
//      The boot function has two special attributes:

// __attribute__((naked)): Ensures the compiler does not generate unnecessary code before and after the function body.
// __attribute__((section(".text.boot"))): Places the function in the .text.boot section, ensuring it is located at the correct address (e.g., 0x80200000).

// External Symbols
//      Symbols defined in the linker script are declared using extern char. This allows the code to obtain the addresses of these symbols.

// .bss Section Initialization
//      In the kernel_main function, the .bss section is initialized to zero using the memset function. This ensures that any uninitialized global or static variables are set to zero.
