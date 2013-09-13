CC     := gcc
CXX    := g++
CPP    := gcc
LEX    := flex
YACC   := bison
AR		 := ar

TARGET := unittest
CFILES :=
CXXFILES := $(wildcard *.cc)

# intermedia files
# OBJFILES := $(CFILES:%.c=obj/%.o) $(CXXFILES:%.cc=obj/%.o)
OBJFILES := $(CFILES:%.c=%.o) $(CXXFILES:%.cc=%.o)
DEPFILES := $(OBJFILES:%.o=%.d)

GMOCK_DIR := ../3rd/gmock-1.6.0
GTEST_DIR := ${GMOCK_DIR}/gtest/

CFLAGS := -g \
	-I../ \
	-I../libs/include/ \
	-I${GTEST_DIR}/include \
	-I${GMOCK_DIR}/include

# CFLAGS := -g -D_DEBUG_
LDFLAGS := \
	-L../libzigbee -lzb \
	-L../libbase -lbase \
	-L../libframework -lframework \
	-L../libs/lib -levent_core -ljansson


$(TARGET) : $(OBJFILES) libgmock.a
	$(CXX) -o $@ $^ $(LDFLAGS)

# ------------------------------
# --- BEGIN --- for google mock
libgmock.a: ${GTEST_DIR}/src/gtest-all.cc ${GMOCK_DIR}/src/gmock-all.cc
	$(CXX) -I${GTEST_DIR}/include -I${GTEST_DIR} -I${GMOCK_DIR}/include -I${GMOCK_DIR} -o gtest-all.o -c ${GTEST_DIR}/src/gtest-all.cc
	$(CXX) -I${GTEST_DIR}/include -I${GTEST_DIR} -I${GMOCK_DIR}/include -I${GMOCK_DIR} -o gmock-all.o -c ${GMOCK_DIR}/src/gmock-all.cc
	$(AR) -rv libgmock.a gtest-all.o gmock-all.o
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
	rm -f $(OBJFILES) $(DEPFILES)

rebuild : clean $(TARGET)

test : $(TARGET)
	@clear
	@./ts



