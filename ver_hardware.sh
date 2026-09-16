#!/bin/sh
echo "=== Topologia de cache ==="
lscpu | grep -E "L1|L2|L3|Model name" || true
echo
echo "=== Tamano de linea L1 (getconf) ==="
getconf LEVEL1_DCACHE_LINESIZE
echo
echo "=== Coherency line size (sysfs) ==="
cat /sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size
