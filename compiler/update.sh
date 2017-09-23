#!/bin/sh

set -e

wget \
  "https://github.com/BuckleScript/bucklescript-playground/raw/master/exports.js" \
  --output-document=bucklescript.js
wget \
  "https://github.com/reasonml-community/refmt-js/raw/master/refmt.js" \
  --output-document=refmt.js
wget \
  "https://github.com/reasonml/reasonml.github.io/raw/source/static/dummy.js" \
  --output-document=dummy.js
./download-stdlib.sh
node ./regenerateStdlibForBrowser.js

