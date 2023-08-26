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
- ret: jalr zero, ra, [addr], [addr] is in function address scale

#### Device

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