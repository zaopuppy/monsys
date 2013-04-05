CC     := gcc
CXX    := g++
CPP    := gcc
LEX    := flex
YACC   := bison

TARGET := ts
CFILES :=
CXXFILES := main.cc ztask.cc zclient.cc zserver.cc \
	zapi_module.cc zzigbee_module.cc zdispatcher.cc zserial.cc \
	zzigbee_message.cc

# intermedia files
OBJFILES := $(CFILES:%.c=obj/%.o) $(CXXFILES:%.cc=obj/%.o)
DEPFILES := $(OBJFILES:%.o=%.d)

CFLAGS := -Wall -g \
	-I../libs/include/ \
	-I../libmonsys/

# CFLAGS := -g -D_DEBUG_
LDFLAGS := \
	-L../libs/lib -levent_core \
	-L../libmonsys/ -lmonsys

$(TARGET) : $(OBJFILES)
	$(CXX) $(LDFLAGS) -o $@ $(OBJFILES)

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

check :
	if [ ! -d obj ]; then rm -rf obj; mkdir obj; fi

clean :
	rm -f $(OBJFILES) $(DEPFILES)

rebuild : clean $(TARGET)

test : $(TARGET)
	@clear
	@./ts



