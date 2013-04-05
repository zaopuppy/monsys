#!/bin/bash

./configure --prefix=$(pwd)/out --add-module=$(pwd)/src/monsys \
	--with-cc-opt='-I../../libmonsys/' \
	--with-ld-opt='-L../../libmonsys/ -lmonsys'



