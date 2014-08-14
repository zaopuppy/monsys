#!/bin/bash

add_nic()
{
  ip=$1
  sudo ifconfig en0 add $ip netmask 255.255.255.0 up
}

add_nic 10.10.10.5 && \
  add_nic 10.10.10.10 && \
  add_nic 10.10.10.11 && \
  add_nic 10.10.10.12 && \
  add_nic 10.10.10.13 && \
  add_nic 10.10.10.14 && \
  add_nic 10.10.10.15



