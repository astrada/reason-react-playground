#!/usr/bin/env node

// Based on:
// https://github.com/reasonml/reasonml.github.io/blob/source/regenerateStdlibForBrowser.js

const fs = require('fs');
const path = require('path');
const browserify = require('browserify');

const b = browserify();

const reactDir = path.join(__dirname, '../node_modules/react/lib');

let reactDirFiles;
try {
  reactDirFiles = fs.readdirSync(reactDir);
} catch (e) {
  console.log(`
** There's an error while reading ${reactDir}.`);
  process.exit(1);
};

reactDirFiles.forEach(file => {
  const exposedRequireName = path.join('stdlib', path.basename(file, '.js'));
  // map `require('stdlib/list')` from the playground to `require('./stdlib/list.js')`
  b.require(path.join(reactDir, file), {expose: exposedRequireName});
});

b
  .transform('deamdify')
  .transform('uglifyify', { global: true })
  .require(path.join(__dirname, 'dummy.js'), {expose: 'fs'})
  .bundle()
  .pipe(fs.createWriteStream(path.join(__dirname, 'libBundle.js')));

