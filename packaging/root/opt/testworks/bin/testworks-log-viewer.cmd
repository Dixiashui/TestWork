@ECHO OFF

SET JARLIST=	

FOR %%I in (..\lib\*.jar) DO CALL SET JARLIST=%%~fI;%%JARLIST%%

start "TW" javaw -splash:..\images\testworks-logo.png -cp "%JARLIST%" pegasus.utilities.logviewer.SelectFileFrame
