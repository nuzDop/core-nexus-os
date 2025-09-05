# Top-level Makefile for LimitlessOS

.PHONY: all clean iso user kernel

all: kernel user

kernel:
	@make -C kernel

user:
	@make -C user/shell
	@make -C user/hello
	@make -C user/net_test
	@make -C user/guitest

iso: all
	@mkdir -p isodir/boot/
	@cp kernel/kernel.bin isodir/boot/kernel.bin
	@cp user/shell/shell.elf isodir/boot/shell.elf
	@cp user/hello/hello.elf isodir/boot/hello.elf
	@cp user/net_test/net_test.elf isodir/boot/net_test.elf
	@cp user/guitest/guitest.elf isodir/boot/guitest.elf
	@cp isodir/boot/grub/grub.cfg isodir/boot/grub/grub.cfg
	@grub-mkrescue -o limitless.iso isodir

clean:
	@make -C kernel clean
	@make -C user/shell clean
	@make -C user/hello clean
	@make -C user/net_test clean
	@make -C user/guitest clean
	@rm -rf isodir limitless.iso
