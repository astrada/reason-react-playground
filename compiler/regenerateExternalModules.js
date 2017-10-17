#!/usr/file/env node

const fs = require('fs');
const path = require('path');
const bin2js = require('./bin2js');

const reasonReactDir = path.join('..', 'node_modules', 'reason-react', 'lib', 'bs', 'src');
const files = [
  path.join(reasonReactDir, 'reactDOMRe.cmi'),
  path.join(reasonReactDir, 'reactDOMServerRe.cmi'),
  path.join(reasonReactDir, 'reactEventRe.cmi'),
  path.join(reasonReactDir, 'reasonReact.cmi'),
  path.join(reasonReactDir, 'reasonReactOptimizedCreateClass.cmi'),
];

const jsName = './cmiBundle.js';
fs.truncateSync(jsName);
files.forEach(file => {
  const basename = path.basename(file);
  fs.writeFileSync(jsName, `ocaml.load_module("/cmis/${basename}", `, {flag: 'a'});
  bin2js.serializeBinary(file, jsName);
  fs.writeFileSync(jsName, ');\n', {flag: 'a'});
});

