CC     := clang
CXX    := clang++
CPP    := clang
LEX    := flex
YACC   := bison

ifeq ($(ut), 1)
TARGET := fgw_ut
else
TARGET := fgw
endif

CFILES :=
CXXFILES := \
	module.cc \
	zserial.cc \
	zzigbee_handler.cc \
	zformatter.cc \
	webapi_msg.cc \
	fgw_client.cc \
	fgw_client_handler.cc \
	msg_factory.cc \
	zb_stream.cc \
	push_msg.cc

ifeq ($(ut), 1)
CXXFILES += test_zbstream.cc \
						test_push_msg.cc
CXXFILES += unittest.cc
else
CXXFILES += fgw.cc
endif

GMOCK_DIR := ../3rd/gmock-1.6.0
GTEST_DIR := ${GMOCK_DIR}/gtest/

# intermedia files
# OBJFILES := $(CFILES:%.c=obj/%.o) $(CXXFILES:%.cc=obj/%.o)
OBJFILES := $(CFILES:%.c=%.o) $(CXXFILES:%.cc=%.o)
DEPFILES := $(OBJFILES:%.o=%.d)

CFLAGS := -g -fPIC -Wall \
	-I../ \
	-I../libs/include/ \
	-I/opt/local/include \
	-I/opt/local/include/boost/tr1

ifeq ($(ut), 1)
CFLAGS += -I${GTEST_DIR}/include
	-I${GMOCK_DIR}/include \
else
CFLAGS += -Wall
endif

# CFLAGS := -g -D_DEBUG_
LDFLAGS := \
	-L../libzigbee -lzb \
	-L../libbase -lbase \
	-L../libframework -lframework \
	-L../libs/lib -levent_core -ljansson

ifeq ($(ut), 1)
$(TARGET) : $(OBJFILES) libgmock.so
	$(CXX) -o $@ $^ $(LDFLAGS)
else
$(TARGET) : $(OBJFILES)
	$(CXX) -o $@ $^ $(LDFLAGS)
endif
# ------------------------------
# --- BEGIN --- for google mock
libgmock.so: ${GTEST_DIR}/src/gtest-all.cc ${GMOCK_DIR}/src/gmock-all.cc
	$(CXX) -g -fPIC -I${GTEST_DIR}/include -I${GTEST_DIR} -I${GMOCK_DIR}/include -I${GMOCK_DIR} -I/opt/local/include -I/opt/local/include/boost/tr1 -o gtest-all.o -c ${GTEST_DIR}/src/gtest-all.cc
	$(CXX) -g -fPIC -I${GTEST_DIR}/include -I${GTEST_DIR} -I${GMOCK_DIR}/include -I${GMOCK_DIR} -I/opt/local/include -I/opt/local/include/boost/tr1 -o gmock-all.o -c ${GMOCK_DIR}/src/gmock-all.cc
	$(CXX) -shared -o $@ gtest-all.o gmock-all.o

# $(AR) -rv libgmock.a gtest-all.o gmock-all.o
# ---  END  --- for google mock
# ------------------------------

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



