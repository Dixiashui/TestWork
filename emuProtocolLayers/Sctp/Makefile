#################################################
#  Includes
#################################################
include ../../Makefile.defines


objects = SctpClient.o SctpCnx.o Sctp.o SctpServer.o
packagename = Sctp.a
 
#################################################
#  Executes subdir makefiles
#################################################
all: $(objects) package

package: $(objects)
	ar rv $(packagename) $(objects)

##############################
# Generic Rules
##############################
%.o: %.cpp %.h
	$(CPP) $(CPPFLAGS) $(INCLUDE_PATH) -c $*.cpp -o $@

%.o: %.c %.h
	$(CC) $(CPPFLAGS) $(INCLUDE_PATH) -c $*.c -o $@

clean:
	rm -f $(objects)
	rm -f $(packagename)

