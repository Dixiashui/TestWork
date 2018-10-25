#################################################
#  Includes
#################################################
include Makefile.defines

#################################################
#  Executes subdir makefiles
#################################################
# the line below is incorrect!! variable substitution needed instead of $@

subdirs = common emuProtocolLayers emuStackBuilders

.PHONY: $(subdirs)

#################################################
#  Executes subdir makefiles
#################################################
all clean: $(subdirs)

$(subdirs):
	cd $@ && ${MAKE} $(MAKECMDGOALS)

