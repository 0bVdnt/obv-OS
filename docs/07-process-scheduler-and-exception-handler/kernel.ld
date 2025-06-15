/* Define the entry point of the kernel */
ENTRY(boot)

/* Define memory sections and their layout */
SECTIONS {
    /* Set the starting address of the kernel to 0x80200000
     * This is typically where the kernel is loaded in memory */
    . = 0x80200000;

    /* Text section containing executable code */
    .text :{
        KEEP(*(.text.boot));    /* Keep the boot code at the start */
        *(.text .text.*);       /* Include all other text sections */
    }

    /* Read-only data section (constants, strings, etc.) */
    .rodata : ALIGN(4) {
        *(.rodata .rodata.*);
    }

    /* Initialized data section */
    .data : ALIGN(4) {
        *(.data .data.*);
    }

    /* Uninitialized data section (BSS - Block Started by Symbol) */
    .bss : ALIGN(4) {
        __bss = .;              /* Start of BSS section */
        *(.bss .bss.* .sbss .sbss.*);  /* Include all BSS sections */
        __bss_end = .;          /* End of BSS section */
    }

    /* Align to 4-byte boundary */
    . = ALIGN(4);
    /* Allocate 128KB for kernel stack */
    . += 128 * 1024;           /* 128KB */
    __stack_top = .;           /* Define top of kernel stack */

    /* Align to 4KB boundary (page size) */
    . = ALIGN(4096);
    __free_ram = .;            /* Start of free RAM area */
    /* Allocate 64MB for free RAM */
    . += 64 * 1024 * 1024;     /* 64 MB */
    __free_ram_end = .;        /* End of free RAM area */
}


