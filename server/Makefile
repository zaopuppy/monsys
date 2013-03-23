CC     := gcc
CXX    := g++
CPP    := gcc
LEX    := flex
YACC   := bison

TARGET := ts
CFILES :=
CXXFILES := main.cc ztask.cc zclient.cc zserver.cc \
	zapi_module.cc zzigbee_module.cc zdispatcher.cc

# intermedia files
OBJFILES := $(CFILES:%.c=obj/%.o) $(CXXFILES:%.cc=obj/%.o)
DEPFILES := $(OBJFILES:%.o=%.d)

CFLAGS := -Wall -g \
	-I/Volumes/Data/source/libevent/libevent-2.0.21-stable/out/include/ \
	-Ilibmonsys/

# CFLAGS := -g -D_DEBUG_
LDFLAGS := \
	-L/Volumes/Data/source/libevent/libevent-2.0.21-stable/out/lib/ -levent_core \
	-Llibmonsys/ -lmonsys

$(TARGET) : $(OBJFILES)
	$(CXX) $(LDFLAGS) -o $@ $^

obj/%.d : %.cc
	$(CXX) $(CFLAGS) -MT $(@:%.d=%.o) -MM -o $@ $<

obj/%.d : %.c
	$(CC) $(CFLAGS) -MT $(@:%.d=%.o) -MM -o $@ $<

obj/%.o : %.cc
	$(CXX) $(CFLAGS) -c -o $@ $<

obj/%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

include $(DEPFILES)

.PHONY : clean rebuild test gdb

clean :
	rm -f $(OBJFILES) $(DEPFILES)

rebuild : clean $(TARGET)

test : $(TARGET)
	@clear
	@./ts



