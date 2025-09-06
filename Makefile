# Top-level Makefile for LimitlessOS

.PHONY: all clean iso user kernel limine

# Variables
LIMINE_REPO := https://github.com/limine-bootloader/limine.git
LIMINE_DIR := limine
LIMINE_BIN := $(LIMINE_DIR)/limine-bios.sys

all: kernel user

kernel:
	@make -C kernel

user:
	@make -C user/shell
	@make -C user/hello
	@make -C user/net_test
	@make -C user/guitest
	@make -C libc

limine:
	@if [ ! -d "$(LIMINE_DIR)" ]; then \
		git clone $(LIMINE_REPO) --branch=v7.x-binary --depth=1; \
	fi
	@make -C $(LIMINE_DIR)

iso: all limine
	@mkdir -p isodir/boot/
	@cp kernel/bin/kernel.bin isodir/boot/kernel.bin
	@cp user/shell/shell.elf isodir/boot/shell.elf
	@cp user/hello/hello.elf isodir/boot/hello.elf
	@cp user/net_test/net_test.elf isodir/boot/net_test.elf
	@cp user/guitest/guitest.elf isodir/boot/guitest.elf
	@cp limine.cfg isodir/boot/limine.cfg
	@cp $(LIMINE_BIN) isodir/boot/limine-bios.sys
	@cp $(LIMINE_DIR)/limine-bios-cd.bin isodir/boot/
	@cp $(LIMINE_DIR)/limine-uefi-cd.bin isodir/boot/
	@xorriso -as mkisofs -b boot/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		isodir -o limitless.iso
	@./$(LIMINE_DIR)/limine bios-install limitless.iso


clean:
	@make -C kernel clean
	@make -C user/shell clean
	@make -C user/hello clean
	@make -C user/net_test clean
	@make -C user/guitest clean
	@make -C libc clean
	@rm -rf isodir limitless.iso
