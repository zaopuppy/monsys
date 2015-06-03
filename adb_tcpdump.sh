#!/bin/bash

# adb shell tcpdump -i any -s 0 -w /sdcard/zhaoyi/t.pcap host 192.168.2.105
adb shell tcpdump -i any -s 0 -A host 192.168.2.105


