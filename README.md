# obv-OS - A Minimal RISC-V Operating System

**obv-OS** is a minimal operating system designed for learning purposes, built from scratch for the RISC-V architecture. This project uses the QEMU `virt` machine and is intended to demonstrate fundamental OS concepts in a simplified environment. Inline assembly (`__asm__`) instructions are used in C in order to instruct the machine.

## Learning Basics Series

This repository is structured as a learning series, with each subdirectory in `learning-basics/` focusing on a specific OS concept.  The current modules include:

- **`implementing-simple-kernel`**:  Introduces the basic structure of a kernel, including setting up the `.bss` section and the kernel entry point. This module demonstrates the minimal setup required to get a RISC-V kernel running. [README](learning-basics/implementing-simple-kernel/README.md)
- **`output-from-the-kernel`**:  Expands on the simple kernel by adding output capabilities. It implements a basic `putchar` function using SBI calls and a `printf` function for formatted output, allowing the kernel to communicate with the console. [README](learning-basics/output-from-the-kernel/README.md)
- **`memory-allocation`**:  Focuses on memory management within the kernel. It implements a simple page allocator (`alloc_pages`) and demonstrates how to manage kernel memory using a bump allocator. [README](learning-basics/memory-allocation/README.md)
- **`exception-handling`**:  Demonstrates exception and trap handling in a RISC-V kernel. It sets up a trap vector, saves and restores CPU registers during exceptions, and implements a basic panic handler. [README](learning-basics/exception-handling/README.md)
- **`process-control-and-context-switching`**:  Introduces process control and context switching. It implements Process Control Blocks (PCBs), a context switching function (`switch_context`), and demonstrates basic multi-tasking between two simple processes. [README](learning-basics/process-control-and-context-switching/README.md)

Each module builds upon the previous one, gradually introducing more complex OS functionalities.

## Prerequisites

To build and run **obv-OS**, you will need the following tools installed on your system:

- **RISC-V Toolchain**:  A RISC-V compiler (like `clang`) and associated tools are necessary to compile the kernel source code for the RISC-V architecture.
- **QEMU**: QEMU (`qemu-system-riscv32`) is used to emulate the RISC-V `virt` machine, providing a platform to run the kernel.
- **LLVM Tools**:  `llvm-nm` and `llvm-objdump` are helpful for debugging and inspecting the compiled kernel binary.
- **OpenSBI Firmware**: The `opensbi-riscv32-generic-fw_dynamic.bin` firmware binary is required for booting the kernel in QEMU and handling SBI calls. This is included in the repository.

## Getting Started

To explore and run the examples in **obv-OS**:

1. **Navigate to the module directory**:
   ```bash
   cd learning-basics/<module-name>
   ```
   Replace `<module-name>` with the directory of the module you want to explore (e.g., `implementing-simple-kernel`, `output-from-the-kernel`, etc.).

2. **Build and run the kernel**:
   Each module directory contains a `run.sh` script. Make it executable and run it:
   ```bash
   chmod +x run.sh
   ./run.sh
   ```
   This script will compile the kernel and launch it in QEMU.

3. **Explore the code and README**:
   Refer to the `README.md` file in each module directory for a detailed explanation of the module's concepts, code structure, and expected output.

## Debugging

For debugging, you can use tools like `llvm-nm` and `llvm-objdump` to inspect the kernel binary. QEMU also provides a monitor interface that can be accessed during runtime (typically by pressing `Ctrl+A C`) to inspect registers and memory.  See the `README.md` in each module for specific debugging instructions related to that module.

## Project Status

This project is actively under development. New modules are being added regularly to cover more OS concepts. Check the Issues tab for planned features and known issues.

## Example Output

When running the `output-from-the-kernel` module, you should see something like this:

```
Hello from obv-OS!
Running on RISC-V
1 + 2 = 3
```

## Contributing

Contributions are welcome! If you'd like to contribute:

1. Fork the repository
2. Create a new branch for your feature
3. Submit a Pull Request with a clear description of your changes

Please ensure your code follows the existing style and includes appropriate documentation.