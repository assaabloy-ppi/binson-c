#!/bin/bash 

echo $1

BSONDIR="$(dirname $1)"
FAILDIR=$BSONDIR/fail
mkdir $FAILDIR

./fuzz_test $1
RES=$?

if [ $RES -eq 0 ]
then
  rm $1
else
  mv $1 $FAILDIR
fi

exit $RES
