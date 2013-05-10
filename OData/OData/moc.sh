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
 # VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV  
  "C:\bbndk\host_10_1_0_212\win32\x86\usr\bin\moc.exe" $h -o moc_${b}.cpp
 fi

 if [ -f $b ]
 then
  cp $b $h ../public/$n
 fi
done