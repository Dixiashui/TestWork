#!/bin/bash


lOGFILE=/dev/null

STATE=`/sbin/lspci | grep "Performance Technologies"`
if [ "$STATE" = "" ];
	then
		exit -1
else
		exit 0 
	fi
	exit 0
esac
