
TARGET = hello

# compiler
CC   = clang --target=riscv64
LINK = ld.lld -m elf64lriscv
COPY = ../../tools/riscv64-unknown-elf-objcopy


CFLAGS  = -std=c11
CFLAGS += -Wall -Wextra -Werror -Wshadow -Wunreachable-code -pedantic
#CFLAGS += -march=rv64gc -mabi=lp64 -mcmodel=medany

CFLAGS += -Wno-error=unused-command-line-argument -Wno-gnu-statement-expression

#CFLAGS += -ffreestanding -fno-common -nostdlib -fno-builtin
#CFLAGS += -fno-stack-protector -ffunction-sections -fdata-sections -fno-unwind-tables -fno-asynchronous-unwind-tables
#CFLAGS += -fno-pie
# ----

$(TARGET).app: $(TARGET).bin
	$(LINK) -r -b binary -o $@ $<

%.bin: %.o
	$(LINK) -s --gc-sections -Ttext 0x0 $^ -o $<
	$(COPY) $< -O binary $@

%.o: %.c
	$(CC) $(CFLAGS) -S -o $<.asm $<
	$(CC) $(CFLAGS) -c -o $@ $<
