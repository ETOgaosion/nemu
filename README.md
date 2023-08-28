# ICS2022 Programming Assignment

This project is the programming assignment of the class ICS (Introduction to Computer System)
in Department of Computer Science and Technology, Nanjing University.

For the guide of this programming assignment,
refer to https://nju-projectn.github.io/ics-pa-gitbook/ics2022/

This repo use riscv64 arch.

Progress:

- [x] pa1: sdb
- [x] pa2: simple machine
    - [x] pa2-1: decoder
    - [x] pa2-2: infrastructure(2), all tess pass
        - [x] advanced itrace: show register value
        - [x] mtrace
        - [x] ftrace
        - [x] diff test
    - [x] pa2-3: IOE
        - [x] serial
        - [x] timer
        - [x] dtrace
        - [x] keyboard
        - [x] VGA
        - [x] audio
- [ ] pa3: OS
    - [x] pa3-1: hardware support
        - [x] csr registers and operations
        - [x] trap and context switch
        - [x] etrace
    - [x] pa3-2: user programs and syscalls
        - [x] load user programs
        - [x] syscall: exit, yield, write(0/1), brk


Beyond normal contents, TODOs:

- [ ] Optimization:
    - [ ] rv64 program instruction analysis, adjust decoding order
    - [ ] multi-launch cpu emulation, use pthread or sth. else
    - [ ] branch prediction, use AI
- [ ] Feature Requests:
    - [ ] floating points support

Documentation is in [./doc](./doc/main.md)