# Lab0

## Install Ubuntu on Vbox

Version: Ubuntu LTS 24.04.3

1. install it using ISO
2. install Guest additional addons(Vbox plugin)
3. enable 3D accelerator(to fix vbox display bug on resizing) and set display memory from 16MB to 128MB(to make 2k resoultion possible)

## Lab0

1. sudo apt install gcc-riscv64-unknown-elf qemu-system-riscv64 u-boot-tools gdb-multiarch
2. vim linker.ld (linker)
3. vim a.S (assembly)
4. riscv64-unknown-elf-gcc -c a.S (output: a.o)
5. riscv64-unknown-elf-ld -T linker.ld -o kernel.elf a.o (output: kernel.elf)
6. riscv64-unknown-elf-objcopy -O binary kernel.elf kernel.bin (output: kernel.bin)
7. qemu-system-riscv64 -M virt -kernel kernel.bin -display none -d in_asm (see many machine codes)

Prepare for deploying on real RV2

1. kernel.its (from [link](https://github.com/chiahsuantw/osc-riscv/blob/opirv2/src/kernel.its))
2. dtb file (from [link](https://github.com/chiahsuantw/osc-riscv/blob/opirv2/res/x1_orangepi-rv2.dtb))
3. mkimage -f src/kernel.its kernel.fit
4. sudo dd if=opirv2-sdcard.img of=/dev/sdb
5. replace kernel.fit in sdcard with mine.
6. test(ok)
