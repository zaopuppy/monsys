CC     := gcc
CXX    := g++
CPP    := gcc
LEX    := flex
YACC   := bison

TARGET := ts
CFILES :=
CXXFILES := main.cc \
	zutil.cc \
	zmodule_ex.cc \
	zapi_module.cc \
	zzigbee_module.cc \
	zserial.cc \
	zwebapi_server.cc \
	zwebapi_handler.cc \
	zapi_server.cc \
	zwebapi_session.cc \
	zzigbee_handler.cc \
	zdevice.cc \
	zformatter.cc \
	unittest_main.cc

CXXFILES += \
	framework/zdispatcher.cc \
	framework/zclient.cc \
	framework/zserver.cc \
	framework/zserver_handler.cc

# intermedia files
# OBJFILES := $(CFILES:%.c=obj/%.o) $(CXXFILES:%.cc=obj/%.o)
OBJFILES := $(CFILES:%.c=%.o) $(CXXFILES:%.cc=%.o)
DEPFILES := $(OBJFILES:%.o=%.d)

CFLAGS := -Wall -g \
	-I../libs/include/ \
	-I../libmonsys/ \
	-I/Volumes/Data/workspaces/jansson/out/include/

# CFLAGS := -g -D_DEBUG_
LDFLAGS := \
	-L../libs/lib -levent_core \
	-L../libmonsys/ -lmonsys \
	-L/Volumes/Data/workspaces/jansson/out/lib -ljansson

$(TARGET) : $(OBJFILES)
	$(CXX) $(LDFLAGS) -o $@ $(OBJFILES)

%.d : %.cc
	$(CXX) $(CFLAGS) -MT $(@:%.d=%.o) -MM -o $@ $<

%.d : %.c
	$(CC) $(CFLAGS) -MT $(@:%.d=%.o) -MM -o $@ $<

%.o : %.cc
	$(CXX) $(CFLAGS) -c -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

include $(DEPFILES)

.PHONY : clean rebuild test gdb

# check :
# 	if [ ! -d obj ]; then rm -rf obj; mkdir obj; fi

clean :
	rm -f $(OBJFILES) $(DEPFILES)

rebuild : clean $(TARGET)

test : $(TARGET)
	@clear
	@./ts



