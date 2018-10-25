#################################################
#  Includes
#################################################
include ../Makefile.defines

subdirs = Sctp


.PHONY: $(subdirs)

#################################################
#  Executes subdir makefiles
#################################################
all clean: $(subdirs)

$(subdirs):
	cd $@ && ${MAKE} $(MAKECMDGOALS)

