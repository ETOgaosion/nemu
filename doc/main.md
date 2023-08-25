# NEMU Documentary

We built nemu for riscv64 architecture.

## Implementation

We suggest not use llvm decoder to generate itrace, but use our own decoder information instead. Though decoder infomation, we can not only get instruction details, but also can know register and memory value as we like, thus does great for debugging.