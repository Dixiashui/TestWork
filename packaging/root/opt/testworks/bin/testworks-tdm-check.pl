#!/bin/bash

LOGFILE=/dev/null
numbers=0
        echo "Starting TDM Driver. This can take some time..." >> $LOGFILE
            STATE=`/usr/pti/bin/commstats -i | grep "Controller .: PT" | cut -d " " -f 6`
            if [ "$STATE" = "Functional" ];
                then
                for controler in $( /usr/pti/bin/commstats -i | grep "Controller .: PT" | cut -d " " -f 2 | cut -d ":" -f 1 ) ;
                         do
                                 VERSION=`/usr/pti/bin/commstats -c $controler | grep "PTI" | cut -d " " -f 9 | cut -c 8-10`
                                 echo "$VERSION $controler"
                                 let numbers++
                        done
            echo $"controller-numbers $numbers" >> $LOGFILE
            else
                    echo "RESET"
                    exit -1 
           fi
        exit 0 
esac
