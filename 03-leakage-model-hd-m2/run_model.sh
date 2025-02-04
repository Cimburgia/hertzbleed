#!/usr/bin/env bash

TOTAL_PHYSICAL_CORES=`sysctl -n hw.physicalcpu`
TOTAL_LOGICAL_CORES=`sysctl -n hw.logicalcpu `

# Load MSR module
#sudo modprobe msr

# Check for entering a comment to run
if [ "$#" -ne 1 ]; then
    echo "Single Comment needed..."
    exit 1
fi

# Create and save new run folder
date=`date +"%m%d-%H%M"`
new_dir="data/out-$date"
mkdir -p "$new_dir"

# Compile and execute program, save stdout to log file
echo "$1" > "$new_dir/log.txt"

# Setup
samples=2000	# 10 seconds
outer=2
num_thread=$TOTAL_LOGICAL_CORES
echo "${num_thread} ${samples} ${outer}" >> "$new_dir/log.txt"

# Alert
echo "This script will take about $((((10)*$outer*(16+56+48+32+16+56+48+32+16+16)/60+10)/60)) hours. Reduce 'outer' if you want a shorter run."

### Warm Up ###
#stress-ng -q --cpu $TOTAL_LOGICAL_CORES -t 10m

sudo rm -rf out
mkdir out
sudo rm -rf input.txt

for selector in `seq 0 16`; do
	echo $selector >> input.txt
done


echo "Starting experiment 1 at ${date} - no BG work." >> "$new_dir/log.txt"
sudo ./bin/driver ${num_thread} ${samples} ${outer} >> "$new_dir/log.txt"
cp -r out $new_dir/out-nobg-${date}

sudo rm -rf out
mkdir out
sudo rm -rf input.txt

for selector in `seq 0 16`; do
	echo $selector >> input.txt
done

# echo "Starting experiment 2 at ${date} - BG work, matrix." >> "$new_dir/log.txt"
# stress-ng --cpu $TOTAL_LOGICAL_CORES --cpu-method matrixprod &
# BG_PID=$!
# sudo ./bin/driver ${num_thread} ${samples} ${outer} >> "$new_dir/log.txt"
# kill $BG_PID

# cp -r out $new_dir/out-bg-matrix-${date}

# sudo rm -rf out
# mkdir out
# sudo rm -rf input.txt

# for selector in `seq 0 16`; do
# 	echo $selector >> input.txt
# done

# echo "Starting experiment 3 at ${date} - BG work, cpu." >> "$new_dir/log.txt"
# stress-ng -q --cpu $TOTAL_LOGICAL_CORES &
# BG_PID=$!
# sudo ./bin/driver ${num_thread} ${samples} ${outer} >> "$new_dir/log.txt"
# kill $BG_PID

# cp -r out $new_dir/out-bg-cpu-${date}