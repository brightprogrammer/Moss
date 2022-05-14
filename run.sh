#!/usr/bin/env bash

./build.sh
qemu-system-x86_64           \
    moss.hdd                 \
    -cpu core2duo            \
    -m 512M                  \
    -no-reboot               \
    -no-shutdown             \
    -M smm=off               \
    -d int                   \
#   -s -S
