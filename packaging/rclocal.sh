strFileName="/etc/rc.d/rc.local"
strLineOne="su tester \"-c /etc/rc.modules\""
strLineTwo="su tester \"-c vncserver :1 -name tester -geometry 1280x800;\""
strLineThree="touch /var/lock/subsys/local"
iLineOne=1
iLineTwo=1
iLineThree=1

if [[ -f $strFileName ]]
then
	while read LINE
	do
		if [ "$LINE" = "$strLineOne" ]
		then
			iLineOne=0
		elif [ "$LINE" = "$strLineTwo" ]
		then
			iLineTwo=0
		elif [ "$LINE" = "$strLineThree" ]
		then
			iLineThree=0
		fi
		
	done < $strFileName
fi

if [ "$iLineOne" -eq "1" ]
then
	echo $strLineOne >> $strFileName
fi
	
if [ "$iLineTwo" -eq "1" ]
then
	echo $strLineTwo >> $strFileName
fi
	
if [ "$iLineThree" -eq "1" ]
then
	echo $strLineThree >> $strFileName
fi