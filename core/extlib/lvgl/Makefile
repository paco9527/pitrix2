include ../../../make.config
LVGL_DIR = $(shell pwd)
LVGL_DIR_NAME = .
export LVGL_DIR LVGL_DIR_NAME

include lvgl.mk
CC = $(ECHO)$(CROSS_COMPILE)gcc
OBJ = $(patsubst %.c,%.o,$(CSRCS))
CFLAGS += "-I."
CFLAGS += "-I./src"

WORKING_DIR = $(shell pwd)
isexist = $(shell if [ -d $(WORKING_DIR)/build ]; then echo "true"; else echo "false"; fi)

ifeq ("$(isexist)", "false")
all:checkdir $(OBJ)
else
all:$(OBJ)
endif
	$(CROSS_COMPILE)ar rcu liblvgl.a build/*.o

%.o:%.c
	$(CC) -g $(CFLAGS) -c $< -o build/$(notdir $@)

checkdir:
	$(ECHO)mkdir build

.PHONY:clean

clean:
	$(ECHO)rm -rf build
	$(ECHO)rm liblvgl.a