# lvgl.mk文件使用的条件变量，用于确定源码目录
LVGL_DIR = $(shell pwd)
LVGL_DIR_NAME = lvgl

include lvgl/lvgl.mk

include ../../make.config

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
CFLAGS += "-I."
CFLAGS += "-I./lua-5.4.6/src"
CFLAGS += "-I./lvgl"
CFLAGS += "-I./lua-cjson"
CFLAGS += "-I./luavgl/src"

all:
	make -C lua-5.4.6/src
	make makelib -C lua-cjson
	make -C luavgl
	make -C lvgl
	# mv luavgl/libluavgl.a build
	# mv lua-cjson/libluacjson.a build
	# mv lvgl/liblvgl.a build
	# mv lua-5.4.6/src/liblua.a build

	mv luavgl/libluavgl.a ../../output
	mv lua-cjson/libluacjson.a ../../output
	mv lvgl/liblvgl.a ../../output
	mv lua-5.4.6/src/liblua.a ../../output

	$(CC) -I lua-5.4.6/src -c lua_import.c -o ../../output/lua_import.o
	# $(AR) x build/libluavgl.a
	# $(AR) x build/libluacjson.a
	# $(AR) x build/liblvgl.a
	# $(AR) x build/liblua.a

	# mv ./*.o build

	# $(AR) rc build/liblua_env.a build/*.o
	# mv build/liblua_env.a ../../output/


.PHONY: clean

clean:
	find . -name '*.o' -exec rm {} \;
