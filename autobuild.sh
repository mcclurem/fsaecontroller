#!/bin/bash

#This script automatically builds a new initrd based on the contents of $CODEDIR/target

TARGETS='main'
BUILDDIR='/root/'

DEV=/dev/usbstick

if [ -e $DEV ]; then
	mount $DEV /mnt/usbstick
	#We sucessfully mounted a usbdrive
	if [ $? == 0 ]; then
		cp -rf /mnt/usbstick/fsaecontroller $BUILDDIR
		cd $BUILDDIR/fsaecontroller
		#Lets check that we have a makefile
		[ !-e ./Makefile ] && exit 1
		#We need to verify that the target file exists(our arguments)
		[ !-e ./target ] &&  exit 1
		#Now lets make all the binarys
		for TARGET in ./target
		do
			TARGETS=$TARGETS + $TARGET
			make $TARGET
			[ $? != 0 ] && exit 1
		done
		./mkird $TARGETS
		#Check that mkird succedded
		[ $? != 0 ] && exit 1
		exit 0
	#Couldn't mount
	else
		exit 1
	fi
else
	exit 1
fi


