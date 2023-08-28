# NEMU Documentary

We built nemu for riscv64 architecture.

## Implementation

### pa2

#### Decode

There is RISCV-G for both 32 bits and 64 bits instruction format, but RISCV-64 has some difference operation, read RISCV-Chinese and RISCV-64I manual to check every instruction behavior like sign-extension.

#### Infrastructure

We suggest not use llvm decoder to generate itrace, but use our own decoder information instead. Though decoder infomation, we can not only get instruction details, but also can know register and memory value as we like, thus sacrifice some performance but does great for debugging.

mtrace only gather information in pread/pwrite.

ftrace is also in decode stage, we judge whether the operation is function call and ret. Riscv64 has some tricks, but due to our observation, they have these traits:

- call: jalr or jal operation, dnpc is function address base
- ret: jalr rd, rs1, [addr], [addr] is in function address scale

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