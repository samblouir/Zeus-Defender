#!/bin/bash

# This script attempts to run all of the programs, in order.
# Filter -> Receiver -> Data Analytics -> Pitcher
inner_folder=ZD
sleep_dur=0.5 # Delays between starting each program
port_to_use=4444


###############################################################################
## Print a welcome message
# Grab our IP
long_form=$(ip a | grep "inet" | grep "192" | awk {'print $2'})
readarray -d "/" -t splitArr <<< "$long_form"
my_ip=${splitArr[0]}
echo "Using $my_ip:$port_to_use..."

###############################################################################
## Start the filter
echo "Starting the filter!"
#sudo rm -r /tmp/zeus
mkdir -p /tmp/zeus # Prepares the filter directory, in case it's missing

gnome-terminal -- $inner_folder/filter/filter
###############################################################################
## Prepare the receiver
sleep $sleep_dur
echo "Starting the receiver!"

gnome-terminal -- $inner_folder/receiver/receiver $my_ip $port_to_use
#echo "send data!"
#sleep 10
###############################################################################
## Run the dummy Data Analytics
sleep $sleep_dur
echo "Starting Data Analytics!"

filter_path="$inner_folder/filter"
cd $filter_path
gnome-terminal -- python send_schema.py # schema.xsd
cd ../..
###############################################################################
## Run the Pitcher
sleep $sleep_dur
echo "Starting the Pitcher!"
pitcher_path="$inner_folder/pitcher"
cmd="$pitcher_path/pitcher $inner_folder/example_packets.xml $my_ip $port_to_use"
echo "Running $cmd..."
$cmd
###############################################################################
