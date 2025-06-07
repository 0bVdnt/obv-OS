/* Kernel Linker Script
 * 
 * This script defines the memory layout of the kernel:
 * 1. Entry Point
 *    - Sets boot as the entry point
 *    - Kernel starts at 0x80200000 (standard RISC-V boot address)
 * 
 * 2. Code Sections
 *    - .text: Contains executable code
 *    - .rodata: Read-only data
 *    - .data: Initialized data
 *    - .bss: Uninitialized data
 * 
 * 3. Memory Regions
 *    - Kernel Stack: 128KB for kernel operations
 *    - Free RAM: 64MB for process memory
 * 
 * 4. Memory Boundaries
 *    - __kernel_base: Start of kernel code
 *    - __bss: Start of uninitialized data
 *    - __bss_end: End of uninitialized data
 *    - __stack_top: Top of kernel stack
 *    - __free_ram: Start of free memory
 *    - __free_ram_end: End of free memory
 */

ENTRY(boot)  /* Set boot as the entry point */

SECTIONS {
    /* Start at standard RISC-V boot address */
    . = 0x80200000;

    /* Define kernel base address */
    __kernel_base = .;

    /* Code section - contains executable instructions */
    .text :{
        KEEP(*(.text.boot));  /* Ensure boot code is first */
        *(.text .text.*);     /* All other code sections */
    }

    /* Read-only data section */
    .rodata : ALIGN(4) {
        *(.rodata .rodata.*);  /* Constants and read-only data */
    }

    /* Initialized data section */
    .data : ALIGN(4) {
        *(.data .data.*);      /* Global/static variables with initial values */
    }

    /* Uninitialized data section (BSS - Block Started by Symbol) */
    .bss : ALIGN(4) {
        __bss = .;             /* Start of BSS section */
        *(.bss .bss.* .sbss .sbss.*);  /* All uninitialized data */
        __bss_end = .;         /* End of BSS section */
    }

    /* Kernel Stack */
    . = ALIGN(4);
    . += 128 * 1024;          /* Allocate 128KB for kernel stack */
    __stack_top = .;          /* Top of kernel stack */

    /* Free RAM Region */
    . = ALIGN(4096);          /* Align to page boundary */
    __free_ram = .;           /* Start of free memory */
    . += 64 * 1024 * 1024;    /* Allocate 64MB for process memory */
    __free_ram_end = .;       /* End of free memory */
}
