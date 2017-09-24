#!/usr/bin/env node

var p = require('child_process');
var fs = require('fs');
var path = require('path');

var config = {
  cwd: __dirname,
  encoding: 'utf8',
  stdio: [0, 1, 2],
  shell: true
};

function e(cmd) {
  console.log(`>>>>>> running command: ${cmd}`)
  //p.execSync(cmd, config)
  console.log(`<<<<<<`)
}

var node_modules = '../node_modules';

var includes = ['reason-react'].map(x => path.join(__dirname, node_modules, x, 'lib/ocaml')).map(x => `-I ${x}`).join(' ');

var cmi_files =
  [
  'reactDOMRe', 'reactDOMServerRe', 'reactEventRe', 'reasonReact'
].map(x => `${x}.cmi:/cmis/${x}.cmi`).map(x => `--file ${x}`).join(' ');

e(`js_of_ocaml --toplevel +weak.js ./polyfill.js jsc.byte ${includes} ${cmi_files} -o ./libs.js`);

