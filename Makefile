
include config.mk

.PHONY: all rebuild clean

# 3rd party library
.PHONY: libevent jansson

.PHONY: libframework

.PHONY: server

all: libframework server

libevent:
	cd 3rd/libevent-2.0/ && \
		./configure --prefix=$(PWD)/libs && \
		make -j4 && make install

zlog:
	cd 3rd/zlog/ && \
		make PREFIX=$(PWD)/libs && \
		make PREFIX=$(PWD)/libs install

# autoreconf -i
jansson:
	cd 3rd/jansson && \
		autoreconf -i && \
		./configure --prefix=$(PWD)/libs && \
		make -j4 && make install

libyaml-cpp:
	cd 3rd/yaml-cpp-0.5.1/ && \
		if [ ! -d build ]; then rm -rf build; mkdir build; fi && \
		cd build && cmake -DCMAKE_INSTALL_PREFIX=$(PWD)/libs .. && \
		make && make install

libframework:
	make -C $@

server: libframework
	make -C $@

# stub:
# 	make -C $@

unittest:
	make -C $@

clean:
	make -C libframework clean
	make -C server clean
#	make -C unittest clean


