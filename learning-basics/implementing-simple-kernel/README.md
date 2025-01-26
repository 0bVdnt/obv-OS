# Simple Kernel Project

This project demonstrates a simple kernel written in C for a RISC-V architecture. The kernel initializes the `.bss` section and enters an infinite loop.

## Project Structure

- `kernel.c`: Contains the kernel code.
- `kernel.ld`: Linker script defining the memory layout.
- `run.sh`: Script to build and run the kernel using QEMU.
- `kernel.elf`: The compiled kernel binary (generated after running `run.sh`).

## Prerequisites

- Clang compiler
- QEMU emulator
- LLVM tools (`llvm-nm`, `llvm-objdump`)

## Building and Running the Kernel

1. Ensure you have Clang, QEMU, and LLVM tools installed on your system.
2. Make the `run.sh` script executable:
    ```sh
    chmod +x run.sh
    ```
3. Run the `run.sh` script to build and run the kernel:
    ```sh
    ./run.sh
    ```

## Debugging

To debug the kernel using `llvm-nm`, `llvm-objdump`, and QEMU monitor, follow these steps:

1. List the symbols in the kernel binary using `llvm-nm`:
    ```sh
    llvm-nm kernel.elf
    ```

2. Disassemble the kernel binary using `llvm-objdump`:
    ```sh
    llvm-objdump -d kernel.elf
    ```

3. Start QEMU with the kernel and enter the QEMU monitor(Press Shift+A and then c to enter the QEMU monitor). In the QEMU monitor, you can inspect the CPU registers by using `info registers`:
    ```sh
    (qemu) info registers
    ```

4. You can also stop the process and continue execution in the QEMU monitor:
    ```sh
    (qemu) stop               # The process stops
    (qemu) info registers     # Shows the current state
    (qemu) cont               # The process resumes
    ```
