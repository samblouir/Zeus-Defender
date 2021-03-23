#!/bin/bash

# Running this script should create a folder called ZD, and 
# inside of it, download all of the branches, and make them.

inner_folder="ZD"
## Downloads and makes all the branches of this repository!
mkdir -p $inner_folder
cd $inner_folder


path=branches.txt
perma_path=branch_list.txt
### Part 0 (Make a tmp folder so we don't accidentally deleted anything important)
mkdir tmp_saved
#sudo mv Zeus-Defender tmp_saved/Zeus-Defender || exit 1 # Backup, important exit

### Part 1 (Get the branch names)
git clone https://github.com/samblouir/Zeus-Defender && \
cd Zeus-Defender && \
git branch -r | awk '{ print $1 }' | tail -n+2 > ../$path \
|| exit 1

cd ..
sudo rm -r Zeus-Defender


### Part 2 (Clone each branch individually)
file=$(cat $path)

# clear perma_path's contents
printf "" > $perma_path

while read line; do
readarray -d "/" -t splitArr <<< "$line"
curr_branch="${splitArr[1]}"
git clone --branch $curr_branch https://github.com/samblouir/Zeus-Defender $curr_branch
echo $curr_branch >> $perma_path
done <$path
sudo rm $path

### Part 3 (Run Make on each branch)

while read line; do
cd $line && make
cd ..
done <$perma_path
cd ..

# Prepare the filter directory
mkdir -p /tmp/zeus

# Moves run_in_order.sh into our currect directory
mv $PWD/$inner_folder/python-branch/run_in_order.sh $PWD/
chmod +x run_in_order.sh


### Part 4 (Finished)
echo "Ready! Now call ./run_in_order.sh!"


