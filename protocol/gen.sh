#!/bin/bash


main()
{
  /Volumes/Data/source/google/protobuf-2.5.0/out/bin/protoc \
    --java_out=../server/java/MonsysServer/src \
    --java_out=../client/android/src \
    --cpp_out=../server \
    *.proto
}

cd $(dirname $0)
main "$@"

