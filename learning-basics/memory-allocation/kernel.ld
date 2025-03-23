/* Specify the entry point of the program - the boot function defined in kernel.c */
ENTRY(boot)

SECTIONS {
    . = 0x80200000;                      /* Set the location counter to this memory address (program's starting address) */

    .text :{                             /* Start of code section (contains executable instructions) */
        KEEP(*(.text.boot));             /* Ensure boot code isn't removed by garbage collection or optimizations */
        *(.text .text.*);                /* Include all other code sections */
    }

    .rodata : ALIGN(4) {                 /* Read-only data section (constants), aligned to 4 bytes */
        *(.rodata .rodata.*);            /* Include all read-only data sections */
    }

    .data : ALIGN(4) {                   /* Initialized data section, aligned to 4 bytes */
        *(.data .data.*);                /* Include all initialized data sections */
    }

    .bss : ALIGN(4) {                    /* Uninitialized data section, aligned to 4 bytes */
        __bss = .;                       /* Define symbol for start of BSS section */
        *(.bss .bss.* .sbss .sbss.*);    /* Include all BSS sections (including small BSS) */
        __bss_end = .;                   /* Define symbol for end of BSS section */
    }

    . = ALIGN(4);                        /* Align to 4-byte boundary */
    . += 128 * 1024;                     /* Reserve 128KB of space for the stack */
    __stack_top = .;                     /* Define symbol marking the top of the stack */

    . = ALIGN(4096);                     /* Align to 4KB page boundary */
    __free_ram = .;                      /* Define symbol marking the start of free memory */
    . += 64 * 1024 * 1024;               /* Reserve 64MB for free memory */
    __free_ram_end = .;                  /* Define symbol marking the end of free memory */
}

