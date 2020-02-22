PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))


ifneq  ($(OS:Windows%=WIN) ,WIN)

#  Not Windows
EXE = sim6730

else

#  WINDOWs
EXE = sim6730.exe

ifeq  (,$(findstring cygwin,$(PATH)))
#haven't install cygwin
RM=del

endif
endif

OBJS =  pq.o engine.o application.o

#ifeq ($(BUILD_MODE),debug)
 	CFLAGS += -g  -Wall -lm
#else ifeq ($(BUILD_MODE),run)
#	CFLAGS += -O2 -Wall -g
#else
#	CFLAGS += -O2 -Wall -g
	#@echo  Build mode $(BUILD_MODE) not supported by this Makefile
#endif

all:$(EXE)

$(EXE):	$(OBJS)
	gcc $(CFLAGS) -o $@   $^ 
	
	


%.o:$(PROJECT_ROOT)%.c
	gcc -std=c99 -c $(CFLAGS)  -o $@  $<

clean:

		$(RM)  $(OBJS) $(EXE)
		
#	@echo  $(OS:Windows%=WIN) $(EXE)  $(patsubst  Win%, win,$(OS))
