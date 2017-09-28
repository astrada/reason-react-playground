#!/usr/bin/env node

// Based on:
// https://github.com/reasonml/reasonml.github.io/blob/source/regenerateStdlibForBrowser.js

const fs = require('fs');
const path = require('path');
const browserify = require('browserify');

const b = browserify();

const stdlibDir = path.join(__dirname, 'stdlib');

let stdlibDirFiles;
try {
  stdlibDirFiles = fs.readdirSync(stdlibDir);
} catch (e) {
  console.log(`
** There's an error while reading ${stdlibDir}.`);
  process.exit(1);
};

stdlibDirFiles.forEach(file => {
  const exposedRequireName = path.join('stdlib', path.basename(file, '.js'));
  // map `require('stdlib/list')` from the playground to `require('./stdlib/list.js')`
  b.require(path.join(stdlibDir, file), {expose: exposedRequireName});
});

b
  .transform('deamdify')
  .transform('uglifyify', { global: true })
  .require(path.join(__dirname, 'dummy.js'), {expose: 'fs'})
  .bundle()
  .pipe(fs.createWriteStream(path.join(__dirname, 'stdlibBundle.js')));

