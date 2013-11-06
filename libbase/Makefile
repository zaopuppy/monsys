
CC  := clang
CXX := clang++
# CC  := $(CROSS_CHAIN)gcc
# CXX := $(CROSS_CHAIN)g++
# AR  := $(CROSS_CHAIN)ar
# AR  := ar


CFLAGS  := -g -Wall
LDFLAGS :=
# ARFLAGS := -r

TARGET  := libbase.so

CFILES := $(wildcard *.c)

CXXFILES := $(wildcard *.cc) $(wildcard algorithm/*.cc)

OBJFILES := $(CFILES:%.c=%.o) $(CXXFILES:%.cc=%.o)
DEPFILES := $(OBJFILES:%.o=%.d)

$(TARGET) : $(OBJFILES)
	rm -f $@
	$(CXX) -shared -o $@ $^
# $(AR) $(ARFLAGS) $@ $^

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

all: $(TARGET)

clean:
	rm -f $(OBJFILES) $(DEPFILES) $(TARGET)

test: build
	./$(TARGET)


