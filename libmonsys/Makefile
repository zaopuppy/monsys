
CC  := gcc
CXX := g++
AR  := ar


CFLAGS  := -g -Wall
LDFLAGS :=
ARFLAGS := -r

TARGET  := libmonsys.a

CFILES := main.c zlog.c \
	zmessage_codec.c z_codec.c zb_msg.c

CXXFILES := zzigbee_message.cc

OBJFILES := $(CFILES:%.c=%.o) $(CXXFILES:%.cc=%.o)
DEPFILES := $(OBJFILES:%.o=%.d)

$(TARGET) : $(OBJFILES)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $^

%.d : %.c
	rm -f $@
	$(CC) $(CFLAGS) -MM -o $@ -c $<

%.d : %.cc
	rm -f $@
	$(CXX) $(CFLAGS) -MM -o $@ -c $<

%.o : %.c
	rm -f $@
	$(CC) $(CFLAGS) -o $@ -c $<

%.o : %.cc
	rm -f $@
	$(CXX) $(CFLAGS) -o $@ -c $<

include $(DEPFILES)

.PHONY : build clean test

build : $(TARGET)

clean :
	rm -f $(OBJFILES) $(DEPFILES)

test: build
	./$(TARGET)


