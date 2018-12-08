NAME = nemu
INC_DIR += ./include
BUILD_DIR ?= ./build

ifeq ($(SHARE), 1)
SO = -so
SO_CFLAGS = -fPIC -D_SHARE=1
SO_LDLAGS = -shared -fPIC
endif

OBJ_DIR ?= $(BUILD_DIR)/obj$(SO)
BINARY ?= $(BUILD_DIR)/$(NAME)$(SO)

include Makefile.git

.DEFAULT_GOAL = app

# Compilation flags
CC = gcc
LD = gcc
INCLUDES  = $(addprefix -I, $(INC_DIR))
CFLAGS   += -O2 -MMD -Wall -Werror -ggdb3 $(INCLUDES) -fomit-frame-pointer
CFLAGS   += -DDIFF_TEST_QEMU

# Files to be compiled
SRCS = $(shell find src/ -name "*.c")
OBJS = $(SRCS:src/%.c=$(OBJ_DIR)/%.o)

# Compilation patterns
$(OBJ_DIR)/%.o: src/%.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(SO_CFLAGS) -c -o $@ $<


# Depencies
-include $(OBJS:.o=.d)

# Some convinient rules

.PHONY: app run clean
app: $(BINARY)

# set testcase by yourself
TESTCASE=dummy

TESTBIN=$(AM_HOME)/tests/cputest/build/$(TESTCASE)-x86-nemu.bin
QEMU_SO=$(NEMU_HOME)/tools/qemu-diff/build/qemu-so

$(QEMU_SO): $(wildcard $(abspath $(QEMU_SO)/../../src)/*.c)
	cd $(abspath $(@D)/..) && make

$(TESTBIN): $(abspath $(TESTBIN)/../..)/tests/$(TESTCASE).c
	cd $(abspath $(@D)/..) && make ARCH=x86-nemu ALL=$(TESTCASE)

override ARGS ?= -l $(BUILD_DIR)/nemu-log.txt
override ARGS += -d $(QEMU_SO)
override ARGS += $(TESTBIN)

# Command to execute NEMU
NEMU_EXEC := $(BINARY) $(ARGS)

$(BINARY): $(OBJS)
	$(call git_commit, "compile")
	@echo + LD $@
	@$(LD) -O2 -rdynamic $(SO_LDLAGS) -o $@ $^ -lSDL2 -lreadline -ldl

run: $(BINARY) $(TESTBIN) $(QEMU_SO)
	$(call git_commit, "run")
	$(NEMU_EXEC)

gdb: $(BINARY) $(TESTBIN) $(QEMU_SO)
	$(call git_commit, "gdb")
	gdb -s $(BINARY) --args $(NEMU_EXEC)

clean: 
	-rm -rf $(BUILD_DIR)
	$(MAKE) -C tools/gen-expr clean
	$(MAKE) -C tools/qemu-diff clean
