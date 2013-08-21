CC     := gcc
CXX    := g++
CPP    := gcc
LEX    := flex
YACC   := bison

TARGET := fgw
CFILES :=
CXXFILES := fgw.cc \
	module.cc \
	zserial.cc \
	zwebapi_server.cc \
	zwebapi_handler.cc \
	zwebapi_session.cc \
	zzigbee_handler.cc \
	zformatter.cc \
	unittest_main.cc \
	push_message.cc \
	fgw_client.cc \
	fgw_client_handler.cc

# intermedia files
# OBJFILES := $(CFILES:%.c=obj/%.o) $(CXXFILES:%.cc=obj/%.o)
OBJFILES := $(CFILES:%.c=%.o) $(CXXFILES:%.cc=%.o)
DEPFILES := $(OBJFILES:%.o=%.d)

CFLAGS := -Wall -g \
	-I../ \
	-I../libs/include/ \
	-I/Volumes/Data/workspaces/jansson/out/include/

# CFLAGS := -g -D_DEBUG_
LDFLAGS := \
	-L../libbase -lbase \
	-L../libzigbee -lzb \
	-L../libframework -lframework \
	-L../libs/lib -levent_core \
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



