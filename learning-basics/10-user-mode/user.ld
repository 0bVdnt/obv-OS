/*
 * user.ld - Linker script for user-space programs
 *
 * Purpose:
 *   Controls how the user program is laid out in memory and defines key symbols like the entry point and stack location.
 *
 * Key Components:
 *   - ENTRY(start): Sets the entry point to the start function (user.c).
 *   - . = 0x1000000: Loads program at 16MB.
 *   - .text: Executable code, keeps start at the beginning.
 *   - .rodata: Read-only data, 4-byte aligned.
 *   - .data: Initialized data, 4-byte aligned.
 *   - .bss: Zero-initialized data, 4-byte aligned, allocates 64KB stack, defines __stack_top.
 *   - Asserts program size does not exceed 8MB.
 */
ENTRY(start)

SECTIONS {
    . = 0x1000000;

    /* machine code */
    .text :{
        KEEP(*(.text.start)); // KEEP ensures this section is included even if not explicitly referenced.
        *(.text .text.*);
    }

    /* read-only data */
    .rodata : ALIGN(4) {
        *(.rodata .rodata.*);
    }

    /* data with initial values */
    .data : ALIGN(4) {
        *(.data .data.*);
    }

    /* data that should be zero-filled at startup */
    .bss : ALIGN(4) {
        *(.bss .bss.* .sbss .sbss.*);

        . = ALIGN(16);
        . += 64 * 1024; /* 64KB */
        __stack_top = .;

       ASSERT(. < 0x1800000, "executable is too large");
    }
}
