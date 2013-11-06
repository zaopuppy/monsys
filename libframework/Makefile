
CC  := clang
CXX := clang++
# AR  := ar


CFLAGS  := -g -Wall \
	-I../ -I../libs/include
LDFLAGS := -L../libbase -lbase \
           -L../libzigbee -lzb \
					 -L../libs/lib -levent
# ARFLAGS := -r

TARGET  := libframework.a

CFILES :=

CXXFILES := zclient.cc \
	zdispatcher.cc \
	zserver.cc \
	zserver_handler.cc \
	zutil.cc \
	zbyte_buffer.cc

OBJFILES := $(CFILES:%.c=%.o) $(CXXFILES:%.cc=%.o)
DEPFILES := $(OBJFILES:%.o=%.d)

$(TARGET) : $(OBJFILES)
	rm -f $@
	$(CXX) $(LDFLAGS) -shared -o $@ $^
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

ts: main.cc $(TARGET)
	$(CXX) $< -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(DEPFILES) $(TARGET)

test: build
	./$(TARGET)


