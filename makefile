
NAME    = kernel
VERSION = alpha

SRC_DIRS = src/kernel src/libc
INC_DIRS = src/include

APPS = src/apps/hello.app

TMP_DIR  = .tmp
BIN_DIR  = bin

# compiler
CC   = clang --target=riscv64 -mno-relax
LINK = ld.lld -m elf64lriscv -s --gc-sections -z max-page-size=4096
COPY = tools/riscv64-unknown-elf-objcopy

LINKER_SCRIPT = src/virt.ld


CFLAGS  = -std=c11 -O2
CFLAGS += -Wall -Wextra -Werror -Wshadow -Wunreachable-code -pedantic
CFLAGS += -march=rv64gc -mabi=lp64 -mcmodel=medany

CFLAGS += -Wno-error=unused-command-line-argument -Wno-gnu-statement-expression

CFLAGS += -ffreestanding -fno-common -nostdlib -fno-builtin
CFLAGS += -fno-stack-protector -ffunction-sections -fdata-sections -fno-unwind-tables -fno-asynchronous-unwind-tables
CFLAGS += -fno-pie

DEP_FLAGS = -MT $@ -MMD -MP -MF $(TMP_DIR)/$*.d
# ----

TARGET   := $(BIN_DIR)/$(NAME)
INCLUDES := $(addprefix -I, $(INC_DIRS))

ASM_FILES := $(foreach sdir, $(SRC_DIRS), $(wildcard $(sdir)/*.s))
C_FILES   := $(foreach sdir, $(SRC_DIRS), $(wildcard $(sdir)/*.c))

OBJ_FILES := $(C_FILES:%.c=$(TMP_DIR)/%.o) $(ASM_FILES:%.s=$(TMP_DIR)/%.o)
DEP_FILES := $(OBJ_FILES:.o=.d)


all: apps $(TARGET)


$(TARGET): $(OBJ_FILES) $(LINKER_SCRIPT) | $(TMP_DIR)/$(BIN_DIR)
	@echo "link  $@"
	@$(LINK) -T$(LINKER_SCRIPT) $(APPS) $^ -o $(TMP_DIR)/$@.elf
	@$(COPY) $(TMP_DIR)/$@.elf -O binary $@

$(TMP_DIR)/%.o: %.c $(TMP_DIR)/%.d | $(TMP_DIR)
	@echo "cc    $<"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(DEP_FLAGS) $(INCLUDES) -c -o $@ $<

$(TMP_DIR)/%.o: %.s | $(TMP_DIR)
	@echo "as    $<"
	@mkdir -p $(@D)
	@$(CC) -c -o $@ $<


apps:
	@(cd src/apps && make -s -f makefile)


clean:
	@rm -f $(TARGET)
	@find $(TMP_DIR) -type f -name '*.o' -delete
	@find $(TMP_DIR) -type f -name '*.d' -delete
	@find $(TMP_DIR) -type f -name '*.elf' -delete


$(DEP_FILES):
include $(wildcard $(DEP_FILES))

