#!/bin/bash

#
# Script for building Testworks package
#

# Check JAVA_HOME
echo $JAVA_HOME | grep 1.7 > /dev/null 2>&1
if [[ "$?" != "0" ]]
then
	echo "Environment variable JAVA_HOME should be set to Java 1.7 JDK"
	exit -1
fi	


# Collect Version
VERSION=$(awk '/^Version:/ { print $2 }' testworks.spec)
RELEASE=$(awk '/^Release:/ { print $2 }' testworks.spec)

ANT=`pwd`/apache-ant-1.7.0/bin/ant
BUILDHOME=`pwd`
# Clean 
cd ../gui/packaging && $ANT clean && cd $BUILDHOME
rm -f testworks-[0-9]*.rpm
rm -f TestworksWindowsInstaller-*.jar

# Create version file
echo "version=$VERSION-$RELEASE" > ../gui/src/pegasus/version.prop

# Build 
echo "Ant begin ************************************************************"
cd ../gui/packaging && $ANT jar && cd $BUILDHOME
echo "Ant end **************************************************************"

# Remove version file, so private builds should not have it
rm -f ../gui/src/pegasus/version.prop

# Create directory for package
rm -Rf /tmp/testworks-$VERSION
mkdir -p /tmp/testworks-$VERSION

# Copy jars
cp -r root/* /tmp/testworks-$VERSION
cp ../gui/jar/* /tmp/testworks-$VERSION/opt/testworks/lib
cp ../gui/dist/Testworks.jar /tmp/testworks-$VERSION/opt/testworks/lib 

# Copy stacks
cp        ../binaries/SbSctpIp.bin \
        /tmp/testworks-$VERSION/opt/testworks/bin

# Copy SDP Converter script
cp  ../gui/src/pegasus/utilities/versionManager/SDPconversion.py\
    /tmp/testworks-$VERSION/opt/testworks/bin

# Copy extra testworks files
tar xzv -C /tmp/testworks-$VERSION/ -f testworks-extra.tar.gz
rm -f /tmp/testworks-$VERSION/etc/rc.d/rc.local
cp ../gui/src/pegasus/utilities/sysConfEditor/supportedProtocols.xml /tmp/testworks-$VERSION/opt/testworks/protocolDescription/

cp ./rclocal.sh \
   /tmp/testworks-$VERSION/opt/testworks/bin/
    
# Build tar.gz for package
rm -f ~/rpmbuild/SOURCES/testworks-$VERSION.tar.gz
echo "ready to tar *************************************************************************"
cd /tmp && tar czvf ~/rpmbuild/SOURCES/testworks-$VERSION.tar.gz testworks-$VERSION && cd -
echo "end to tar ***************************************************************************"
echo "ready to rpmbuild *******************************"
rpmbuild -bb testworks$1.spec 
echo "end *******************************************"
cp ~/rpmbuild/RPMS/x86_64/testworks-$VERSION* packages
rm -f ~/rpmbuild/RPMS/x86_64/testworks-$VERSION* .

# Build .zip for Windows package
cd ../gui/packaging
rm -f /tmp/testworks.zip
cd /tmp && zip -r /tmp/testworks.zip testworks-$VERSION && cd - && mv /tmp/testworks.zip WindowsInstaller/src/

# Build Windows Package
$ANT -f WindowsInstaller/autobuild.xml clean
$ANT -f WindowsInstaller/autobuild.xml jar
cd $BUILDHOME

# Copy Windows Package
mv ../gui/packaging/WindowsInstaller/dist/TestworksInstaller.jar packages/TestworksWindowsInstaller-$VERSION-$RELEASE.jar

# Copy jar file (just for development purposes)
#cp ../dist/Testworks.jar .

# Clean up Windows
cd ../gui/packaging
$ANT -f WindowsInstaller/autobuild.xml clean
rm -f WindowsInstaller/src/testworks.zip

# Clean up linux
rm -Rf /tmp/testworks-$VERSION
$ANT clean
cd $BUILDHOME

