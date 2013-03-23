#!/bin/bash

cd $(dirname $0)

case $1 in
	start)
		echo "stopping...done"
		./out/sbin/nginx -s stop
		echo "starting...done"
		./out/sbin/nginx
		;;
	stop)
		echo "stopping...done"
		./out/sbin/nginx -s stop
		;;
	status)
		ps -ef | grep nginx
		;;
	test)
		wget -q -O - --server-response http://127.0.0.1:8082/monsys
		;;
	*)
		./out/sbin/nginx -h
		;;
esac


