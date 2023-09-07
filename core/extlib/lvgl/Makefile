include ../../../make.config
LVGL_DIR = $(shell pwd)
LVGL_DIR_NAME = .
export LVGL_DIR LVGL_DIR_NAME

include lvgl.mk
CC = $(ECHO)$(CROSS_COMPILE)gcc
OBJ = $(patsubst %.c,%.o,$(CSRCS))
CFLAGS += "-I."
CFLAGS += "-I./src"

MYFLAGS = -g

all:$(OBJ)
	$(CROSS_COMPILE)ar rcu liblvgl.a build/*.o

%.o:%.c
	$(CC) -g $(CFLAGS) -c $< -o build/$(notdir $@)

.PHONY:clean

OBJ = $(shell find . -name '*.o')
clean:
	@rm -f $(OBJ)
	rm liblvgl.a