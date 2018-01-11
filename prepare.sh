#!/bin/sh

set -e

cp index.html index.html.bak
sed -ne 's/"\//"/gp' index.html.bak > index.html
rm index.html.bak
