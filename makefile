include make.config
include inc_list.config

PWD = $(shell pwd)
INC = 

#注意分号
mod_dir_list = $(PWD)/$(Mod_dir)/$(1)
cp_mod_makefile = $(1)/makefile;
mod_make = $(MAKE) -C $(1);

sys_dir_list = $(PWD)/$(SYS_Dir)/$(1)
cp_sys_makefile = $(1)/makefile;
sys_make = $(MAKE) -C $(1);

extlib_dir_list = $(PWD)/$(HW_Dir)/$(1)
#cp_hw_makefile = $(1)/makefile;
extlib_make = $(MAKE) -C $(1);

export mod_dir_list sys_dir_list extlib_dir_list

all: $(TARGET)

prebuild:
	-rm inc_list.config

#make_hwlib:

get_mod_list:
	$(eval MOD_BUILD_DIR := \
		$(foreach n,$(ModLIB),\
		$(call mod_dir_list,$(n))\
		)\
	)

get_sys_list:
	$(eval SYS_BUILD_DIR := \
		$(foreach n,$(SYS),\
		$(call sys_dir_list,$(n))\
		)\
	)

get_extlib_list:
	$(eval EXT_BUILD_DIR := $(foreach n,$(ExtLIB),$(call extlib_dir_list,$(n))))

make_mod: get_mod_list
	$(ECHO)$(foreach n,$(MOD_BUILD_DIR), \
		$(shell cp mod_makefile $(call cp_mod_makefile,$(n)))\
		$(call mod_make,$(n))\
	)

#make_extlib: get_extlib_list
#	$(ECHO)$(foreach n,$(EXT_BUILD_DIR), \
#		$(call extlib_make,$(n))\
#	)
make_extlib:
	make -C core/extlib
	
make_sys: get_sys_list
	$(ECHO)$(foreach n,$(SYS_BUILD_DIR), \
		$(shell cp sys_makefile $(call cp_sys_makefile,$(n)))\
		$(call sys_make,$(n))\
	)

$(TARGET): make_mod make_sys make_extlib make_extlib
	$(ECHO)cd $(OUTPUT)
	$(ECHO)cp link_makefile $(OUTPUT)/makefile
	$(MAKE) -C $(OUTPUT);


.PHONY:clean

clean:
	find . -name '*.o' -exec rm {} \;
