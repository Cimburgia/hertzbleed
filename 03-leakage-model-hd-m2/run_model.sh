#!/usr/bin/env bash

TOTAL_PHYSICAL_CORES=`sysctl -n hw.physicalcpu`
TOTAL_LOGICAL_CORES=`sysctl -n hw.logicalcpu `

# Load MSR module
#sudo modprobe msr

# Setup
samples=10000	# 10 seconds
outer=30
num_thread=$TOTAL_LOGICAL_CORES
date=`date +"%m%d-%H%M"`

# Alert
echo "This script will take about $((((10)*$outer*(16+56+48+32+16+56+48+32+16+16)/60+10)/60)) hours. Reduce 'outer' if you want a shorter run."

### Warm Up ###
stress-ng -q --cpu $TOTAL_LOGICAL_CORES -t 10m

sudo rm -rf out
mkdir out
sudo rm -rf input.txt

for selector in `seq 0 16`; do
	echo $selector >> input.txt
done

sudo ./bin/driver-input ${num_thread} ${samples} ${outer}
cp -r out data/out-input-${date}
cd ../scripts