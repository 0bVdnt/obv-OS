ENTRY(boot)

SECTIONS {
    . = 0x80200000;

    .text :{
        KEEP(*(.text.boot));
        *(.text .text.*);
    }

    .rodata : ALIGN(4) {
        *(.rodata .rodata.*);
    }

    .data : ALIGN(4) {
        *(.data .data.*);
    }

    .bss : ALIGN(4) {
        __bss = .;
        *(.bss .bss.* .sbss .sbss.*);
        __bss_end = .;
    }

    . = ALIGN(4);
    . += 128 * 1024; /* 128KB */
    __stack_top = .;
}

/*
    1: ENTRY(boot)
        - Specifies the entry point of the program
        - The boot symbol is defined elsewhere in the code (likely in an assembly file)
        - This is where execution will begin when the kernel starts
    
    2: SECTIONS { ... }
        - Defines how different sections of the program should be laid out in memory
    3: . = 0x80200000;
        - Sets the location counter (.) to address 0x80200000
        - This means the following sections will be placed starting at this physical memory address
        - This is typically used in bare metal/kernel development where specific memory addresses are important
    4: .text :{ ... }
        - Defines the text section which contains executable code
        - The .text section typically holds program instructions
    5: KEEP(*(.text.boot));
        -KEEP prevents the linker from optimizing out this section
        -*(.text.boot) matches any input sections named .text.boot from any input file
        -This is likely where the boot code resides
    6: *(.text .text.*);
        -Includes all other .text sections and any sections starting with .text.
        -The * wildcard means "from any input file"
*/