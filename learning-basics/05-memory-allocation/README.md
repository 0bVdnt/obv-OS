# RISC-V Memory Allocation Components

This folder contains a basic implementation of memory allocation mechanisms for the RISC-V operating system (obv-OS). As part of the learning-basics series, this project demonstrates how dynamic memory management can be integrated into a kernel environment.

---

## Overview

In this project, we implement fundamental memory allocation routines to support dynamic memory management within the kernel. The kernel sets up memory spaces (including a heap), initializes the BSS section, and includes a basic allocator to manage dynamic storage for kernel operations. This minimal design is intended for educational purposes and lays the groundwork for more advanced memory management strategies.

---

## Project Structure

- **kernel.c**  
  Contains the main kernel entry point and initialization routines. This file sets up essential memory regions and, potentially, tests memory allocation functionality.

- **kernel.h**  
  Provides declarations for kernel functions and memory allocation interfaces.

- **common.c / common.h**  
  Implements shared utility functions, which may include routines for memory management and other low-level operations.

- **kernel.ld**  
  The linker script that defines the memory layout of the kernel, including sections for code, data, BSS, and the heap.

- **opensbi-riscv32-generic-fw_dynamic.bin**  
  The OpenSBI firmware binary necessary for bootstrapping and handling SBI calls in supervisor mode.

- **run.sh**  
  Shell script to build and run the kernel on a RISC-V emulator (QEMU).

- **kernel.elf & kernel.map**  
  The output executable and memory map generated during the build process.

---

## Key Functions and Concepts

### Kernel Initialization (`kernel_main`)
- **Purpose:**  
  Sets up the kernel environment.
- **Tasks Performed:**
  - Clears the BSS section to initialize all uninitialized global variables.
  - Configures the memory layout by setting up segments, including the heap.
  - Invokes memory allocation routines (if any) to test dynamic memory management.

### Memory Allocation Routines
- **Overview:**  
  The project includes early-stage dynamic memory allocation functions that manage the kernel heap.
- **Details:**
  - Implements simple allocation strategies (e.g., first-fit or best-fit) for educational purposes.
  - Interfaces for these routines are declared in `kernel.h` and implemented in `common.c`.

### Linker Script (`kernel.ld`)
- **Role:**  
  Defines the memory layout for the kernel.
- **Key Points:**
  - Specifies memory regions for text, data, BSS, and the heap.
  - Ensures proper alignment and sizing for all segments, which is critical for correct operation on RISC-V.

---

## Running Instructions

### Prerequisites
- **RISC-V GNU Toolchain:**  
  Ensure you have a working RISC-V compiler and associated utilities.
- **QEMU RISC-V Emulator:**  
  Used to emulate RISC-V hardware.
- **OpenSBI Firmware:**  
  Included in this project to provide SBI (Supervisor Binary Interface) support.

### Building and Running the Kernel
Compile the project and launch the kernel in QEMU using the provided script:
```bash
./run.sh
```
This command generates the `kernel.elf` executable and `kernel.map` file then the kernel should boot, and any memory allocation tests will be executed.

---

## Memory Layout Inspection & Debugging

After running the kernel using the `run.sh` script, you can further inspect the memory layout via QEMU's debug interface and the `llvm-nm` tool.

### Using QEMU Debug Interface

1. **Accessing Debug Mode:**  
   While QEMU is running, access the debug console by pressing:
   - `Ctrl-A` then `C`  
   This opens the QEMU monitor/debug interface.

2. **Inspect Memory Layout with llvm-nm:**  
   On your host machine, run:
   ```bash
   llvm-nm kernel.elf
   ```
   **Expected Sample Output:**
   ```plaintext
   0000XXXXXXXXXXXX T _start         # Kernel entry point
   0000XXXXXXXXXXXX T kernel_main    # Kernel main function
   0000XXXXXXXXXXXX D __data_start   # Start of the .data section
   0000XXXXXXXXXXXX D __data_end     # End of the .data section
   0000XXXXXXXXXXXX B __bss_start    # Start of the .bss section
   0000XXXXXXXXXXXX B __bss_end      # End of the .bss section
   0000XXXXXXXXXXXX D __heap_start   # Start of the heap
   0000XXXXXXXXXXXX D __heap_end     # End of the heap
   ```
   This output verifies that the memory sections (code, data, BSS, and heap) have been correctly set up as defined in the linker script.

---

## Additional Notes

- **Current Implementation:**  
  The memory allocation mechanism is a simplified, early-stage implementation designed for learning purposes. It may not handle all edge cases. The algorithm used here is known as Bump allocator or Linear allocator.
  
- **Future Enhancements:**  
  Potential improvements include implementing a deallocation function (`free`), optimizing allocation algorithms, and adding features to reduce memory fragmentation.
  
- **Memory Layout:**  
  The memory layout defined in `kernel.ld` is critical. It sets the boundaries for various segments (text, data, BSS, and heap) and ensures alignment requirements are met.

### Additional Details on the Bump Allocator

This project employs one of the world's simplest dynamic allocation algorithms: the **bump allocator** (or **linear allocator**).

- **Mechanism:**  
  A static pointer (`next_paddr`) is initialized to the start of the free memory area (`__free_ram`), which is defined and aligned in the linker script (typically on a 4KB boundary).
  
- **Allocation Process:**  
  When memory is allocated via the function (e.g., `alloc_pages`), the allocator:
  - Returns the current value of `next_paddr`.
  - Increments `next_paddr` by the number of pages requested multiplied by the page size (4KB).
  - Zeroes out the newly allocated pages to prevent issues with uninitialized memory.
  
- **Limitations:**  
  This simple algorithm does not support deallocation. If `next_paddr` exceeds `__free_ram_end`, the system triggers a kernel panic indicating an out-of-memory condition.

This approach is highly efficient for scenarios where memory is allocated sequentially and freed only when the system halts or is reset.

---