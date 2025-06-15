# RISC-V Exception Handling Components

This document describes the key components used for exception/trap handling in the RISC-V operating system.

## Core Structures

### Trap Frame (`struct trap_frame`)
Stores CPU state when a trap occurs. Contains all general-purpose registers:
``` c
    struct trap_frame {
    uint32_t ra; // Return Address
    uint32_t gp; // Global Pointer
    uint32_t tp; // Thread Pointer
    // Temporary registers
    uint32_t t0-t6; // t0-t6
    // Argument registers
    uint32_t a0-a7; // a0-a7
    // Saved registers
    uint32_t s0-s11;// s0-s11
    uint32_t sp; // Stack Pointer
    } attribute((packed));
```

## Key Functions

### `kernel_entry(void)`
Entry point for all traps/exceptions.
- Attributes: `naked`, `aligned(4)`
- Saves all CPU registers to trap frame
- Calls `handle_trap()`
- Restores registers and returns from trap
- Uses `sscratch` CSR for stack switching

### `handle_trap(struct trap_frame *f)`
C-language trap handler.
- Parameters: Pointer to saved trap frame
- Reads trap-related CSRs:
  - `scause`: Cause of the trap
  - `stval`: Additional trap information
  - `sepc`: Program counter at trap
- Currently panics on all traps with diagnostic info

### `kernel_main(void)`
Kernel initialization including trap setup:
- Clears BSS section using `memset()`
- Sets up trap vector (`stvec`) to point to `kernel_entry`
- Triggers test trap with unimplemented instruction

## CSR Macros

### `READ_CSR(reg)`
Reads from a Control and Status Register
```c
uint32_t cause = READ_CSR(scause);
```

### `WRITE_CSR(reg, value)`
Writes to a Control and Status Register
```c
WRITE_CSR(stvec, (uint32_t) kernel_entry);
```
## Important CSRs (Control and Status Registers)

- `stvec`: Supervisor Trap Vector - points to trap handler
- `scause`: Supervisor Trap Cause - indicates why trap occurred
- `stval`: Supervisor Trap Value - additional trap info
- `sepc`: Supervisor Exception Program Counter - instruction that trapped
- `sscratch`: Supervisor Scratch Register - used for stack switching

## Error Handling

### `PANIC(fmt, ...)`
Macro for fatal error handling:
- Prints error message with file/line information
- Includes variable arguments for detailed info
- Halts the system in infinite loop

```c
PANIC("unexpected trap scause=%x, stval=%x, sepc=%x\n",
scause, stval, user_pc);
```
## Exception Flow

1. Exception/trap occurs
2. CPU jumps to address in `stvec` (`kernel_entry`)
3. `kernel_entry`:
   - Saves current stack pointer to `sscratch`
   - Allocates stack space for registers
   - Saves all registers to stack
   - Calls `handle_trap`
4. `handle_trap`:
   - Reads trap info from CSRs
   - Prints diagnostic info
   - Panics (in current implementation)
5. `kernel_entry`:
   - Restores all registers
   - Returns from trap with `sret`

## External Dependencies

- `common.h`: Common utility definitions
- Linker script symbols:
  - `__bss`: Start of BSS section
  - `__bss_end`: End of BSS section
  - `__stack_top`: Top of kernel stack

## Running Instructions

### Prerequisites
- RISC-V GNU Toolchain
- QEMU RISC-V Emulator
- OpenSBI Firmware

### Run the exception handler
```bash
$ ./run.sh
```

Expected output:
```
...
PANIC: kernel.c:XX: unexpected trap scause=00000002, stval=00000000, sepc=XXXXXXXX
```
This indicates successful trap handling of the test illegal instruction.

### Debugging
To run with `llvm-addr2line`:
```bash
# Terminal 1: Start QEMU
$ ./ run.sh

# Terminal 2: Use the command below check the location where the value of the sepc=XXXXXXXX points to
$ llvm-addr2line -e kernel.elf XXXXXXXX
```
### Expected result:
```bash 
# Here the kernel.c:XXX should point to the line where the exception has occured. 
/Users/.../kernel.c:XXX
```
## Notes

- Current implementation is minimal - only reports and halts
- Stack grows downward in memory
- Register save/restore order must match `trap_frame` structure
- Future implementations should handle different trap types appropriately