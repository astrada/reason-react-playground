#!/usr/bin/env node

// Based on:
// https://github.com/reasonml/reasonml.github.io/blob/source/regenerateStdlibForBrowser.js

const fs = require('fs');
const path = require('path');
const browserify = require('browserify');

const b = browserify();

const refmtFile = path.join(__dirname, '..', 'node_modules', 'reason', 'refmt.js');
b.require(refmtFile, {expose: "refmt"});

b
  .transform('deamdify')
  .transform('uglifyify', { global: true })
  .bundle()
  .pipe(fs.createWriteStream(path.join(__dirname, 'refmt.js')));

