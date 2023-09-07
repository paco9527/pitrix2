include make.config
include inc_list.config

PWD = $(shell pwd)
INC = 

#注意分号
mod_dir_list = $(PWD)/$(Mod_dir)/$(1)
cp_mod_makefile = $(1)/Makefile;
mod_make = $(MAKE) -C $(1);

sys_dir_list = $(PWD)/$(SYS_Dir)/$(1)
cp_sys_makefile = $(1)/Makefile;
sys_make = $(MAKE) -C $(1);

export mod_dir_list sys_dir_list

all: $(TARGET)

get_mod_list:
# 执行MODBUILD_DIR := $(PWD)/core/modules/log等...
	$(eval MOD_BUILD_DIR := \
		$(foreach n,$(ModLIB),\
		$(call mod_dir_list,$(n))\
		)\
	)

get_sys_list:
# SYS_BUILD_DIR := $(PWD)/core/sys/main等...
	$(eval SYS_BUILD_DIR := \
		$(foreach n,$(SYS),\
		$(call sys_dir_list,$(n))\
		)\
	)

make_mod: get_mod_list
# 执行：cp mod_makefile $(PWD)/core/modules/log; make -C $(PWD)/core/modules/log; 等...
	$(ECHO)$(foreach n,$(MOD_BUILD_DIR), \
		$(shell cp mod_makefile $(call cp_mod_makefile,$(n)))\
		$(call mod_make,$(n))\
	)

make_extlib:
	cp extlib_makefile core/extlib/Makefile
	make -C core/extlib
	
make_sys: get_sys_list
	$(ECHO)$(foreach n,$(SYS_BUILD_DIR), \
		$(shell cp mod_makefile $(call cp_sys_makefile,$(n)))\
		$(call sys_make,$(n))\
	)

$(TARGET): make_mod make_sys make_extlib
	$(ECHO)cd $(OUTPUT)
	$(ECHO)cp link_makefile $(OUTPUT)/makefile
	$(MAKE) -C $(OUTPUT);


.PHONY:clean

clean:
	find . -name '*.o' -exec rm {} \;
	find core/modules -name 'Makefile' -exec rm {} \;
	find core/sys -name 'Makefile' -exec rm {} \;
	rm -f core/extlib/Makefile
	rm -f output/*
	rm -f inc_list.config
	rm -f pitrix
