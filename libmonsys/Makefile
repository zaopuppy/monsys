
CC  := /usr/bin/gcc
AR  := /usr/bin/ar


CFLAGS  := -g -Wall
LDFLAGS :=
ARFLAGS := -r

TARGET  := libmonsys.a

CFILES := main.c zlog.c zmessage_codec.c z_codec.c zb_msg.c

OBJFILES := $(CFILES:.c=.o)
DEPFILES := $(OBJFILES:.o=.d)

$(TARGET) : $(OBJFILES)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $^

%.d : %.c
	rm -f $@
	$(CC) $(CFLAGS) -MM -o $@ -c $<

%.o : %.c
	rm -f $@
	$(CC) $(CFLAGS) -o $@ -c $<

include $(DEPFILES)

.PHONY : build clean test

build : $(TARGET)

clean :
	rm -f $(OBJFILES) $(DEPFILES)

test: build
	./$(TARGET)


