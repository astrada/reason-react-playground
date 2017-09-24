#! /bin/bash

# Based on https://github.com/ojbc/docker/blob/master/java8-karaf3/files/git-download.sh

set -e

DIR="stdlib"
URL="https://api.github.com/repos/Bucklescript/bucklescript-playground/contents/stdlib"

if [[ ! -d $DIR ]]; then
  mkdir $DIR
fi

FILES=$(curl -s $URL | grep download_url | sed "s/.*\(https.*\)\".*/\1/g")

cd $DIR

for f in $FILES; do
  FILENAME=$(basename $f)
  curl -s $f -o $FILENAME
done

cd ..

