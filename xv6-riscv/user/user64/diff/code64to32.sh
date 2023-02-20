#!/bin/sh
for filename in ../*
do
	sed -i 's/uint64/uint32/g' ${filename}
done
for filename in ../*
do
	echo '###############################################'
	echo ${filename}
	echo '###############################################'
	diff ${filename} ../${filename}
	echo 
done
