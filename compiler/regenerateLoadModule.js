#!/usr/bin/env node

const fs = require('fs');
const path = require('path');
const cmi2js = require('./cmi2js');

const reasonReactDir = path.join('..', 'node_modules', 'reason-react', 'lib', 'bs', 'src');
const cmis = [
  path.join(reasonReactDir, 'reactDOMRe.cmi'),
  path.join(reasonReactDir, 'reactDOMServerRe.cmi'),
  path.join(reasonReactDir, 'reactEventRe.cmi'),
  path.join(reasonReactDir, 'reasonReact.cmi'),
  path.join(reasonReactDir, 'reasonReactOptimizedCreateClass.cmi'),
];

const jsName = './cmiBundle.js';
fs.truncateSync(jsName);
cmis.forEach(cmi => {
  const basename = path.basename(cmi);
  fs.writeFileSync(jsName, `ocaml.load_module("/cmis/${basename}", `, {flag: 'a'});
  cmi2js.convertCmi(cmi, jsName);
  fs.writeFileSync(jsName, ');\n', {flag: 'a'});
});

