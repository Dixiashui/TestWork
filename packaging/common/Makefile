#################################################
#  Includes
#################################################
include ../Makefile.defines


#objects = TwLog.o
objects = InterprocessMutex.o SemaphoreGuard.o MutexGuard.o Mutex.o BitArray.o Semaphore.o \
	TimerThread.o Util.o UtilGUILog.o Socket.o Socket4.o Socket6.o JsiTwConnection.o \
	ActiveObject.o DataBuffer.o Crc.o Thread.o ServerSocket.o Tuntap.o \
	NamedPipe.o CommonBuffer.o RTTimer.o ReadWriteMutex.o ReadWriteMutexGuard.o \
	CommonException.o MultiTimerThread.o AnotherMultiTimerThread.o LogToFile.o \
	DynamicFactory.o
	
objectsLog = TwLogDebug.o TwLog.o

packagename = common.a

#################################################
#  Executes subdir makefiles
#################################################
all: $(objects) $(objectsLog) package

$(packagename): all

package: $(objects)
	ar rvs $(packagename) $(objects)

##############################
# Generic Rules
##############################
%.o: %.cpp %.h
	$(CPP) $(CPPFLAGS) $(INCLUDE_PATH) -c $*.cpp -o $@

%.o: %.c %.h
	$(CC) $(CPPFLAGS) $(INCLUDE_PATH) -c $*.c -o $@

clean:
	rm -f *.o
	rm -f $(objectsLog)
	rm -f $(packagename)
