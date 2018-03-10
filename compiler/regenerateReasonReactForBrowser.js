#!/usr/bin/env node

// Based on:
// https://github.com/reasonml/reasonml.github.io/blob/source/regenerateStdlibForBrowser.js

const fs = require('fs');
const path = require('path');
const browserify = require('browserify');

const b = browserify();

function uncapitalize(s) {
  return s.charAt(0).toLowerCase() + s.substr(1);
}

const reactFile = path.join(__dirname, '..', 'node_modules', 'react', 'index.js');
b.require(reactFile, {expose: "react"});

const reasonReactDir = path.join(__dirname, '..', 'node_modules', 'reason-react', 'lib', 'js', 'src');
let reasonReactDirFiles;
try {
  reasonReactDirFiles = fs.readdirSync(reasonReactDir);
} catch (e) {
  console.log(`
** There's an error while reading ${reasonReactDir}.`);
  process.exit(1);
};
reasonReactDirFiles.forEach(file => {
  //const exposedRequireName = path.join('reason-react', 'lib', 'js', 'src', path.basename(file, '.js'));
  const exposedRequireName = path.join('stdlib', uncapitalize(path.basename(file, '.js')));
  b.require(path.join(reasonReactDir, file), {expose: exposedRequireName});
});

b
  .transform('deamdify')
  .transform('uglifyify', { global: true })
  .bundle()
  .pipe(fs.createWriteStream(path.join(__dirname, 'reasonReactBundle.js')));

