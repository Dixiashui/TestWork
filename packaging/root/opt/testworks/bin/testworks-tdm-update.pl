#!/bin/bash

LOGFILE=/dev/null
        echo "Starting TDM Driver. This can take some time..." >> $LOGFILE
        date >> $LOGFILE
        #/usr/pti/drivers/mps/load_mps
        STATE=`/usr/pti/bin/commstats -i | grep "Controller .: PT" | cut -d " " -f 6`
        echo $STATE
        if [ "$STATE" = "Functional" ];
                then
                    echo $STATE
        else
                echo "error1"
                exit -1
        fi

        if [ "$1" = "method1" ];
            then
            echo "select method1"
            /usr/pti/bin/wcpLoad -c "$2" -k 0 -E -f "$3" #>> $LOGFILE
            if [ "$?" != "0" ]
                    then
                    exit -1
            fi
        fi
        if [ "$1" = "method2" ];
            then
            echo "select method2"
            /usr/pti/bin/nwslCli -c "$2" "mkdev flash /dev/kernel0 0x00EF"
            /usr/pti/bin/wcpLoad -c "$2" -k 0 -E -f "$3"
            if [ "$?" != "0" ]
                    then
                    exit -1
            fi
        fi
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
        exit 0 
esac
