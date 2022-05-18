
# compiler
CC   = clang --target=riscv64 -mno-relax
LINK = ld.lld -m elf64lriscv -s --gc-sections -z max-page-size=4096
COPY = ../../tools/riscv64-unknown-elf-objcopy


CFLAGS  = -std=c11 -O2
CFLAGS += -Wall -Wextra -Werror -Wshadow -Wunreachable-code -pedantic
CFLAGS += -march=rv64gc -mabi=lp64 -mcmodel=medany

CFLAGS += -Wno-error=unused-command-line-argument -Wno-gnu-statement-expression

CFLAGS += -ffreestanding -fno-common -nostdlib -fno-builtin
CFLAGS += -fno-stack-protector -ffunction-sections -fdata-sections -fno-unwind-tables -fno-asynchronous-unwind-tables
CFLAGS += -fno-pie
# ----


test: test.o
	$(LINK) -Ttext 0x0 $^ -o $@.elf
	$(COPY) $@.elf -O binary $@

%.o: %.c
	$(CC) $(CFLAGS) -S -o test.asm $<
	$(CC) $(CFLAGS) -c -o $@ $<
