#!/usr/bin/env node

const fs = require('fs');
const path = require('path');
const bin2js = require('./bin2js');

const reasonReactDir = path.join('..', 'node_modules', 'reason-react', 'lib', 'bs', 'src');
const modules = [
  {
    cmi: path.join(reasonReactDir, 'reactDOMRe.cmi'),
    cmj: path.join(reasonReactDir, 'reactDOMRe.cmj')
  },
  {
    cmi: path.join(reasonReactDir, 'reactDOMServerRe.cmi'),
    cmj: path.join(reasonReactDir, 'reactDOMServerRe.cmj'),
  },
  {
    cmi: path.join(reasonReactDir, 'reactEventRe.cmi'),
    cmj: path.join(reasonReactDir, 'reactEventRe.cmj'),
  },
  {
    cmi: path.join(reasonReactDir, 'reasonReact.cmi'),
    cmj: path.join(reasonReactDir, 'reasonReact.cmj'),
  },
  {
    cmi: path.join(reasonReactDir, 'reasonReactOptimizedCreateClass.cmi'),
    cmj: path.join(reasonReactDir, 'reasonReactOptimizedCreateClass.cmj'),
  },
];

const jsName = './externalModules.js';
fs.truncateSync(jsName);
modules.forEach(module => {
  const cmi = module.cmi
  const cmiBasename = path.basename(cmi);
  fs.writeFileSync(jsName,
                   `ocaml.load_module("/cmis/${cmiBasename}", `,
                   {flag: 'a'});
  bin2js.serializeBinary(cmi, jsName);
  const cmj = module.cmj
  const cmjBasename = path.basename(cmj);
  fs.writeFileSync(jsName,
                   `, "${cmjBasename}", `,
                   {flag: 'a'});
  bin2js.serializeBinary(cmj, jsName);
  fs.writeFileSync(jsName, ');\n', {flag: 'a'});
});

