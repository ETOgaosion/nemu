# NEMU Documentation

We built nemu for riscv64 architecture.

## Implementation

### pa2

#### Decode

There is RISCV-G for both 32 bits and 64 bits instruction format, but RISCV-64 has some difference operation, read RISCV-Chinese and RISCV-64I manual to check every instruction behavior like sign-extension.

#### Infrastructure

We suggest not use llvm decoder to generate itrace, but use our own decoder information instead. Though decoder infomation, we can not only get instruction details, but also can know register and memory value as we like, thus sacrifice some performance but does great for debugging.

`mtrace` only gather information in `pread/pwrite`.

`ftrace` is also in decode stage, we judge whether the operation is function call and ret. Riscv64 has some tricks, but due to our observation, they have these traits:

- `call`: `jalr` or `jal` operation, `dnpc` is function address base
- `ret`: `jalr rd, rs1, [addr], [addr]` is in function address scale, function depth > 0

#### Device

AM_UP_TIME bug: in `nemu/src/device/timer.c`, only when register offset is 4, nemu would update timers, but at that time, lower bits have been read, if the time goes ahead, lower bits will not be valid. So we change the offset to 0, only first time query register will modify the timers, and we can make sure the concurrency been promised.

#### Benchmarks

see [benchmarks_results](./benchmarks.md).

#### FCEUX

```C
// fceux-am/src/fceu.cpp
#define FUNC_IDX_MAX256
```

##### WSL sound

To enable WSL sound, [this](https://x410.dev/cookbook/wsl/enabling-sound-in-wsl-ubuntu-let-it-sing/) is helpful, but line 60-61 should be:

```
load-module module-native-protocol-tcp auth-ip-acl=0.0.0.0 auth-anonymous=1
load-module module-esound-protocol-tcp auth-ip-acl=0.0.0.0 auth-anonymous=1
```

or you can use wsl ip to promise safety.

In wsl, exportation would be:

```sh
export HOST_IP=$(ip route | grep default | awk '{print $3}')
export PULSE_SERVER=tcp:$HOST_IP
```

### pa3

#### ELF

Use `riscv64-unknown-elf-readelf -h xxx.elf` can read header and find Magic is always: `Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00`

### pa4

#### VME

This is the **most difficult part** in the whole pa, for reasons below:

- Lack of debug tools (Infrastructures)
    - Difftests not working anymore, data in fs is wrongly set
    - Test our experience to find real reason which causes the bug
    - Log and Assert is the most sufficient tool, `mtrace` and `itrace` does a lot of help
- System becomes complex, Failure is too far from Fault
    - Think about reason (This method is tough for beginners, but is very useful to improve your understanding of the whole system)
    - Use debug tools to check possible Faults

Here are some holes (坑) that you can pre-read to avoid falling in:

- Notice the bit masks, set them correctly
- AM's `malloc` should be set carefully, separated from nanos' `new_page`. There is an easy way to resolve this: divide the free memory space. I've tried to mix them up, but failed, still have misunderstandable bugs.
- Things that the lecture notes not mention
    - [This one mentioned in latter notes] context switch when trapping should change: **`sp` should not be the one used in user stack, but shall be set to kernel stack**. So not only `sp` needs to be used, but also **`tp` as trap pointer, pointing to a fixed context location of each process**. When user program trap in, the sp should be set to tp as context base location, and kernel tasks should execute with this kernel stack.
    - Due to the complexity, we are not able to support nested interrupt:
        - There are not only user processes, but also kernel threads.
        - Thus we should implement 2 different trap mechanisms: context fixed or flexible.
        - But it is difficult for tasks to be loaded at first time:
            - kernel threads return way is different from user processes
            - mtvec is hard to manage

Here are bug recordings and solutions, maybe helpful:

You may encounter:

- Multiple times of page walk error - some vpn cannot be found on PTE page:
    - Failure: Page should be mapped
        - Fault: Page not map or map arguments wrong
            - Sol: Think/Log to find where the map should be and fix it
    - Failure: Page should not be mapped
        - Fault: Map is wrongly implemented
            - Sol: Check vme.c and mmu.c, make sure they are consistent
    - Failure: Instruction is incorrent, not the same as binary dumped file when use pc to search
        - Fault: vme or mmu is incorrect
            - Sol: Check vme.c and mmu.c, make sure they are consistent, Notice the bitmarks
- Incorrect program flow:
    - Failure: Strange behavior after trap
        - Fault: Trap is incorrect, kernel stack and context was written
            - Sol: You can try what I mentioned above