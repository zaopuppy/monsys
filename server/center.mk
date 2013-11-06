
CC     := clang
CXX    := clang++
CPP    := clang
LEX    := flex
YACC   := bison

TARGET := center
CFILES :=
CXXFILES := center.cc \
	fgw_server.cc \
	fgw_server_handler.cc \
	webapi_msg.cc \
	zwebapi_server.cc \
	zwebapi_handler.cc \
	zwebapi_session.cc

# intermedia files
# OBJFILES := $(CFILES:%.c=obj/%.o) $(CXXFILES:%.cc=obj/%.o)
OBJFILES := $(CFILES:%.c=%.o) $(CXXFILES:%.cc=%.o)
DEPFILES := $(OBJFILES:%.o=%.d)

CFLAGS := -Wall -g \
	-I../ \
	-I../libs/include/

# CFLAGS := -g -D_DEBUG_
LDFLAGS := \
	-L../libbase -lbase \
	-L../libzigbee -lzb \
	-L../libframework -lframework \
	-L../libs/lib -levent_core -ljansson

$(TARGET) : $(OBJFILES)
	$(CXX) -o $@ $(OBJFILES) $(LDFLAGS)

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
	rm -f $(OBJFILES) $(DEPFILES) $(TARGET)

rebuild : clean $(TARGET)

test : $(TARGET)
	@clear
	@./ts



