#!/bin/bash

n=$(basename $PWD)
echo $n

cd src

for h in *.h
do
 b=$(basename $h .h)
 if grep -q Q_OBJECT $h
 then
 # Change this to point to your SDK installation directory
 #
 # VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV  
  "/Applications/bbndk/host_10_0_10_536/darwin/x86/usr/bin/moc" $h -o moc_${b}.cpp
 fi

 if [ -f $b ]
 then
  cp $b $h ../public/$n
 fi
done
