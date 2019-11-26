#!/bin/bash

# folder where all files will be stored
folder=./tmp

# wipe
rm -rf $folder

# creation
mkdir $folder
touch $folder/read-only
touch $folder/write-only

# go into folder for chmods
cd $folder
# 444 is read only for all users
chmod 444 read-only
# 222 is write only for all users
chmod 222 write-only
# go back up
cd ..
