RISCVGUN ?= riscv64-unknown-elf

SRC_FILES := $(wildcard *.S) $(wildcard *.c)

OBJ_FILES = $(addprefix build/, $(addsuffix .o, \
            $(basename $(notdir $(SRC_FILES)))))

CFLAGS = -march=rv64gc_zicbom -ffreestanding -mcmodel=medany -Wall -g

all: clean kernel

clean:
	rm -rf build *.fit *.o initramfs.cpio

build:
	cd rootfs && find . | cpio -o -H newc > ../initramfs.cpio
	$(RISCVGUN)-gcc $(CFLAGS) -c $(SRC_FILES)
	@mkdir -p build
	@mv *.o build

kernel: build
	$(RISCVGUN)-gcc -T linker.ld -nostdlib -o build/kernel.elf $(OBJ_FILES)
	$(RISCVGUN)-objcopy -O binary build/kernel.elf build/kernel.bin
	mkimage -f kernel.its build/kernel.fit >> /dev/null

qemu: CFLAGS += -D __QEMU__
qemu: all
	qemu-system-riscv64 -M virt -serial stdio -kernel build/kernel.bin
