include ../../../make.config
include ../../../inc_list.config

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
INC_FLAG = $(patsubst %,-I %,$(INC))

all: $(SRC)
#	echo $(shell ls ../../..)
#	echo 'mod make start 23'
	$(ECHO)$(TOOL_CHAIN) $(INC_FLAG) $(MY_BUILD_FLAG) -c $(SRC)
	$(ECHO)mv *.o ../../../$(OUTPUT)
