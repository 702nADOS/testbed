#!/bin/bash


if [[ "$1" == "--help" ]]; then
   echo "Please choose as first parameter the location of your MLO (./MLO)"
   echo "Please choose as second parameter the location of your 'u-boot.img'(./u-boot.img)"
   echo "If you dont choose any parameters the default ones(brackets) will be choosen"
   echo -e "\033[31m";
   echo "Use with care no input validation is implemented! This can damage your sd card!"
   echo -e "\033[30m";

exit

fi


DISK=/dev/mmcblk0

echo "zero ${DISK}"
sudo dd if=/dev/zero of=${DISK} bs=1M count=10


#.MLO is now $1
echo "Install bootloader"
if [[ -z "$1" ]]; then
	sudo dd if=./MLO of=${DISK} count=1 seek=1 bs=128k
else
	sudo dd if=$1 of=${DISK} count=1 seek=1 bs=128k
fi

#./u-boot.img is now $2
if [[ -z "$2" ]]; then
	sudo dd if=./u-boot.img of=${DISK} count=2 seek=1 bs=384k
else
	sudo dd if=$2 of=${DISK} count=2 seek=1 bs=384k
fi

echo "Create Partition"
#sudo sfdisk --in-order --Linux --unit M ${DISK} <<-__EOF__
#1,,0x83,*
#__EOF__


sudo sfdisk ${DISK} <<-__EOF__
1M,,0x83,*
__EOF__

echo "Format Partition"
sudo mkfs.vfat ${DISK}p1

echo "Sync"
sudo sync

echo "READY"

if [[ -z "$S1" ]]; then
echo -e "\033[31m";
echo "Caution: Standard parameters selected: if you did not intend to do this please check if everything is ok!"
echo -e "\033[00m";
fi
