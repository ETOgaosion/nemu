.section .data
.global ramdisk_start, ramdisk_end
ramdisk_start:
# .incbin "build/ramdisk_native.img"
.incbin "build/ramdisk_riscv64.img"
ramdisk_end:

.section .rodata
.globl logo
logo:
.incbin "resources/logo.txt"
.byte 0
