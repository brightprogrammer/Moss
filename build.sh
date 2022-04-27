#!/usr/bin/env bash

# create System directory structure
# root of our operating system
mkdir -pv System
# contains bootloader related files
mkdir -pv System/Boot

# remove previous kernel
rm System/Boot/Kernel

# build os
mkdir -pv Build
cd Build
# take any changes in account
cmake .. -DCMAKE_BUILD_TYPE=Debug
# build
make -j8
# install will keep the build files in build directory only
# we will need to copy those files from here to System
make install
cd ..

# remove previous moss.hdd
rm moss.hdd

# Create an empty zeroed out 64MB moss.hdd file.
# block size is 1 MB (read/write bytes at once)
dd if=/dev/zero bs=1M count=0 seek=64 of=moss.hdd

# Create a GUID partition table.
# -s is --script : this never prompts for user intervention
parted -s moss.hdd mklabel gpt

# Create an EFI System Partition (ESP) that spans the whole disk.
parted -s moss.hdd mkpart ESP fat32 2048s 100%
# set ESP flag "on" for partition 1 on moss.hdd
parted -s moss.hdd set 1 esp on

# Build limine-install.
make -C limine

# Install the Limine BIOS stages onto moss
limine/limine-deploy moss.hdd

# Mount the loopback device.
# The loop device is a block device that maps its data blocks
# not to a physical device such as a hard disk or optical disk
# drive, but to the blocks of a regular file in a filesystem
# or to another block device
#
# partscan forces the kernel to scan the partition table for newly
# created loop device.
# find the first unused loop device and if a file argument is present
# (in our case moss.hdd) use it as loop device
# show the name of the assigned loop device (--find [file] must be present)
USED_LOOPBACK=$(sudo losetup --partscan --find moss.hdd --show)

# Format the ESP partition (partition 1 : p1) as FAT32.
sudo mkfs.fat -F 32 ${USED_LOOPBACK}p1

# Create directory for mounting moss.hdd
mkdir -p Disk
# mount hdd so that we can directly copy/paste files into it
sudo mount ${USED_LOOPBACK}p1 Disk

# copy kernel
cp -v Build/Kernel/Kernel System/Boot/Kernel
# copy bootloader related file
mkdir -pv System/Boot/EFI
mkdir -pv System/Boot/EFI/BOOT
cp -v limine/BOOTX64.EFI System/Boot/EFI/BOOT/
# copy limine config
cp -v limine.cfg limine/limine.sys System/Boot/

# Copy the relevant files over
sudo cp -v System/* Disk/ -r

# Sync system cache and unmount partition and loopback device.
sync
sudo umount Disk
sudo losetup -d ${USED_LOOPBACK}
