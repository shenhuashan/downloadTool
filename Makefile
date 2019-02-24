CX 	    = g++
LD      = ld

# ------------  set compiler flags  -------------------------------------------
BUG   	=yes
LIBS    = -lpthread -lcurl
DEBUG  	= -DDEBUG -DMEMWATCH -DMEMWATCH_STDIO -Wall -O0 -g -pg
RELEASE	= -Wall -O2 -march=i686 -mtune=i686

# ------------  set predefined objects  ---------------------------------------
EXEC 	= downloadTool

#===============================================================================
# Do some pre-processing
#===============================================================================
ifeq (${BUG}, yes)
	CFLAGS = $(DEBUG) $(DIRS)
	OBJS   =  main.o Download.o HttpDownload.o
else
	CFLAGS = $(RELEASE) $(DIRS)
	OBJS   =  main.o Download.o HttpDownload.o

endif

#===============================================================================
# Start the goals
#===============================================================================
# ------------  make the objects (specified rules)  ----------------------------
$(EXEC): $(OBJS)
	$(CX) $(CFLAGS) $(LIBS) -o $@ $^ 

# ------------  make the objects (default rules)  ------------------------------
%.o: %.cpp %.h
	$(CX) $(CFLAGS) -c -std=c++11 $<

# ------------  remove generated files  ----------------------------------------
clean:
	rm -rf $(OBJS) $(EXEC)

# ------------  declare phony targets  -----------------------------------------
.PHONY: all clean

# ==============================================================================
# vim:ft=make:ts=4:sw=4:



