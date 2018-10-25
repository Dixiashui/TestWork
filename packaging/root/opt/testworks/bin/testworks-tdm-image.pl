#!/bin/bash
STATE=`strings "$1" | grep "810[pP]088" | cut -d " " -f 6 | cut -c 8-10`
if [ "$STATE" = "" ]
        then
    exit -1
fi
    echo $STATE
    exit 0
echo $STATE
