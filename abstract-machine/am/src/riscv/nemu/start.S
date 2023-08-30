.section entry, "ax"
.globl _start
.type _start, @function

_start:
  li t0, 0xa00001800
  csrw mstatus, t0
  mv s0, zero
  la sp, _stack_pointer
  jal _trm_init
