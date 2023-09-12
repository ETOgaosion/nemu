# ICS2022 Programming Assignment

This project is the programming assignment of the class ICS (Introduction to Computer System)
in Department of Computer Science and Technology, Nanjing University.

For the guide of this programming assignment,
refer to https://nju-projectn.github.io/ics-pa-gitbook/ics2022/

This repo use **riscv64** arch.

Progress (2023):

- [x] pa1: sdb [8.7]
- [x] pa2: simple machine [8.26]
    - [x] pa2-1: decoder [8.11]
    - [x] pa2-2: infrastructure(2), all tess pass [8.25]
        - [x] advanced itrace: show register value
        - [x] mtrace
        - [x] ftrace
        - [x] diff test
    - [x] pa2-3: IOE [8.26]
        - [x] serial
        - [x] timer
        - [x] dtrace
        - [x] keyboard
        - [x] VGA
        - [x] audio
- [x] pa3: batch OS [8.30]
    - [x] pa3-1: hardware support [8.27]
        - [x] csr registers and operations
        - [x] trap and context switch
        - [x] etrace
    - [x] pa3-2: user programs and syscalls [8.28]
        - [x] load user programs
        - [x] syscall: exit, yield, write(0/1), brk
    - [x] pa3-3: FS, VFS, devices [8.30]
        - [x] FS operations
        - [x] IOE: serial, timer, keyboard, vedio
        - [x] TODO: other VFS directories
- [x] pa4: TDM OS (Time-Division Multiplexing) [9.12]
    - [x] pa4-1: multiplexing [9.8]
        - [x] context switch
        - [x] kernel threads and user processes
        - [x] all tests pass
    - [x] pa4-2: VME [9.11]
        - [x] VME Support
            - **THIS IS THE MOST DIFFICULT PART**, for more info like suggestions and bug record, check [./doc](./doc/main.md#VME)
        - [x] S/U mode support, stack context switch
    - [x] pa4-3: TIE [9.12]
        - [x] time-division multiplexing
        - [x] preempt multiplexing

Beyond normal contents, TODOs:

- [x] Optimization:
    - [x] rv64 program instruction analysis, adjust decoding order [+11 pts]
    - [x] refactor decode operation with hash decoder and force jump table [+10 pts]
        - this is as far as we can do, cannot be equal to 1

Documentation is in [./doc](./doc/main.md)