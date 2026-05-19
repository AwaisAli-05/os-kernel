# os-kernel

> A custom x86 operating system built from scratch in C and Assembly, featuring MLFQ process scheduling — demonstrated via QEMU emulator.

**Built by Muhammad Awais** · Cybersecurity Student · Pakistan

---

## What is this?

This is a bare-metal operating system written entirely from scratch without any existing OS underneath it. It boots directly on x86 hardware (or QEMU emulator), sets up the CPU, handles interrupts, and schedules processes using a **Multi-Level Feedback Queue (MLFQ)** algorithm.

---

## Features

- Custom bootloader written in x86 Assembly
- 32-bit protected mode setup via GDT (Global Descriptor Table)
- Interrupt handling via IDT and ISR routines
- MLFQ (Multi-Level Feedback Queue) process scheduler
- Kernel written in C with Assembly low-level routines
- Runs on QEMU x86 emulator

---

## Project Structure

```
os-kernel/
├── boot.asm          ← Bootloader — first code that runs when OS starts
├── gdt.asm           ← Global Descriptor Table — sets up CPU memory segments
├── interrupt.asm     ← Low-level interrupt service routines (ISR)
├── kernel.c          ← Main kernel — MLFQ scheduler and core logic
├── linker.ld         ← Linker script — controls memory layout
├── Makefile          ← Build system — compiles and links everything
├── boot.bin          ← Compiled bootloader binary
├── kernel.bin        ← Compiled kernel binary
└── os-image.bin      ← Final OS image (boot + kernel combined)
```

---

## How MLFQ Scheduling Works

**Multi-Level Feedback Queue (MLFQ)** is a CPU scheduling algorithm that uses multiple queues with different priority levels.

- New processes start in the **highest priority queue**
- If a process uses its full time slice → it moves **down** to a lower priority queue
- If a process gives up the CPU early (I/O wait) → it **stays** in the same queue
- This automatically separates short interactive tasks (high priority) from long CPU-heavy tasks (low priority)
- Prevents starvation by periodically **boosting** all processes back to the top queue

```
Queue 0 (Highest Priority) → Short time slice (e.g. 8ms)
Queue 1 (Medium Priority)  → Medium time slice (e.g. 16ms)
Queue 2 (Lowest Priority)  → Long time slice (e.g. 32ms)
```

---

## Requirements

- **NASM** — Netwide Assembler (for .asm files)
- **GCC** — GNU C Compiler (cross-compiler for x86)
- **LD** — GNU Linker
- **QEMU** — x86 emulator to run the OS
- **Make** — build automation

Install on Ubuntu/Debian:
```bash
sudo apt install nasm gcc make qemu-system-x86
```

---

## Build & Run

**Clone the repo:**
```bash
git clone https://github.com/AwaisAli-05/os-kernel.git
cd os-kernel
```

**Build the OS:**
```bash
make
```

**Run in QEMU:**
```bash
qemu-system-x86_64 -drive format=raw,file=os-image.bin
```

Or if using 32-bit:
```bash
qemu-system-i386 -drive format=raw,file=os-image.bin
```

---

## What I Learned

- How a CPU boots and transfers control to a bootloader
- x86 Assembly — registers, memory addressing, CPU modes
- Protected mode and the Global Descriptor Table (GDT)
- How interrupts work at the hardware level (IDT, ISR, IRQ)
- Process scheduling algorithms — specifically MLFQ
- Linking and memory layout with a custom linker script
- Low-level C programming without any standard library

---

## Tech Stack

`C` `x86 Assembly` `NASM` `GCC` `QEMU` `Make` `Linker Script`

---

## Disclaimer

This is an educational project built to understand OS internals from the ground up. It is not a production operating system.

---

**Muhammad Awais** · Cybersecurity Student · Pakistan
ISC2 CC Candidate · Open to Work Worldwide
[LinkedIn](https://linkedin.com/in/yourprofile) · [GitHub](https://github.com/AwaisAli-05)
