#!/bin/bash -x

cd $(dirname $0)

option=$1
host=192.168.2.146
user=zero
base_dir=/Volumes/Data/workspaces/monsys

# --dry-run
rsync ${option} --delete -azvv \
  -e ssh ${user}@${host}:${base_dir}/{libbase,libframework,libzigbee,server,web} .



