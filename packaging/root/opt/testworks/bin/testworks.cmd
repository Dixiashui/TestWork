@ECHO OFF

SET JARLIST=	

FOR %%I in (..\lib\*.jar) DO CALL SET JARLIST=%%I;%%JARLIST%%

start "TW" javaw -Xmx512M -Dsun.java2d.pmoffscreen=false -splash:..\images\testworks-logo.png -cp "%JARLIST%" pegasus.PegasusMain
