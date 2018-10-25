#!/bin/bash -x

if [[ "$1" == "" ]]
then
    echo "Usage: $0 spec-file <list of files>"
    exit 1
fi

PREFIX=$(cat $1  | awk '/Name:/{ print $2 }')
VERSION=$(awk '/^Version:/ { print $2 }' $1)

echo "Creating package for $PREFIX $VERSION..."


# Create the directory
rm -Rf /tmp/$PREFIX-$VERSION
mkdir -p /tmp/$PREFIX-$VERSION

# Copy the files.
FIRSTARG=true;
for i in $*
do
    if [[ "$FIRSTARG" == "true" ]]
    then
        FIRSTARG=false;
    else
        cp $i /tmp/$PREFIX-$VERSION
    fi
done


# Create the package
rm -f ~/rpmbuild/SOURCES/$PREFIX-$VERSION.tar.gz
rm -f ~/rpmbuild/RPMS/x86_64/$PREFIX-$VERSION*
echo "HH*************"
cd /tmp && tar czvf ~/rpmbuild/SOURCES/$PREFIX-$VERSION.tar.gz --exclude .svn $PREFIX-$VERSION && cd -
echo "HH end*********"
rpmbuild -bb $1
cp ~/rpmbuild/RPMS/x86_64/$PREFIX-$VERSION* packages

