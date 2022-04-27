#!/usr/bin/env bash

./build.sh
qemu-system-x86_64           \
    moss.hdd                 \
    -m 128                   \
    -no-reboot               \
    -no-shutdown             \
    -M smm=off               \
    -d int                   \
#    -s -S
