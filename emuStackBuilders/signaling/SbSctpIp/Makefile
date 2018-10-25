#################################################
#  Includes
#################################################
include ../../../Makefile.defines

binaries = SbSctpIp.bin

#################################################
#  Executes subdir makefiles
#################################################
all: $(binaries)

##############################
# Generic Rules
##############################
%.bin: %.o $(LIB_SCTP)
	$(CPP) $*.o $(LIB_SCTP) $(INCLUDE_LIB) $(LIB_COMMON) $(OBJ_LOG) -lpthread -lsctp -o $(TWR20)/binaries/$@

%.o: %.cpp 
	$(CPP) $(CPPFLAGS) $(INCLUDE_PATH) -c $*.cpp -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(INCLUDE_PATH) -c $*.c -o $@

clean:
	rm -f *.o
	rm -f $(TWR20)/binaries/$(binaries)

$(LIB_SCTP):
	${MAKE} $(LIB_SCTP_DIR)
