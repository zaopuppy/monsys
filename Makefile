
include config.mk

.PHONY: all rebuild clean

# 3rd party library
.PHONY: libevent

.PHONY: libbase libzigbee libframework

.PHONY: server stub

all: libbase libzigbee libframework server stub unittest

libevent:
	cd 3rd/libevent-2.0/ && \
		./configure --prefix=$(PWD)/libs && \
		make -j4 && make install

libbase:
	make -C $@

libzigbee: libbase
	make -C $@

libframework: libbase
	make -C $@

server: libzigbee libbase
	make -C $@

stub:
	make -C $@

unittest:
	make -C $@

clean:
	make -C libbase clean
	make -C libzigbee clean
	make -C libframework clean
	make -C server clean
	make -C stub clean
	make -C unittest clean


