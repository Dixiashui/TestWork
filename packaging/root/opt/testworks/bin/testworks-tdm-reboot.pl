#!/bin/bash

LOGFILE=/dev/null


case "$1" in
  start)
        echo "Starting TDM Driver. This can take some time..." >> $LOGFILE
        date >> $LOGFILE
        sudo /usr/pti/drivers/mps/load_mps
        while
            true
        do
            STATE=`/usr/pti/bin/commstats -i | grep "Controller .: PT" | cut -d " " -f 6`
            echo $STATE
            if [ "$STATE" = "Functional" ];
                then
                for controler in $( /usr/pti/bin/commstats -i | grep "Controller .: PT" | cut -d " " -f 2 | cut -d ":" -f 1 ) ;
                         do
                        /usr/pti/bin/nwc -c $controler -f /usr/pti/util/nwc/wcp384TransparentFullChanE1 2>&1 >> $LOGFILE
                        done
            break
            else
                echo "Initial TDM Driver. Wait another 10s"
                sleep 10
                continue
            fi
        done
;;
  *)
        echo $"Usage: $0 {start}"
        exit 0 
esac
