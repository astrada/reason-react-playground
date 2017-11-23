#!/usr/bin/env node

const fs = require('fs');
const path = require('path');
const bin2js = require('./bin2js');

const reasonReactDir = path.join('..', 'node_modules', 'reason-react', 'lib', 'bs', 'src');
const modules = [
  {
    cmi: path.join(reasonReactDir, 'ReactDOMRe.cmi'),
    cmj: path.join(reasonReactDir, 'ReactDOMRe.cmj')
  },
  {
    cmi: path.join(reasonReactDir, 'ReactDOMServerRe.cmi'),
    cmj: path.join(reasonReactDir, 'ReactDOMServerRe.cmj'),
  },
  {
    cmi: path.join(reasonReactDir, 'ReactEventRe.cmi'),
    cmj: path.join(reasonReactDir, 'ReactEventRe.cmj'),
  },
  {
    cmi: path.join(reasonReactDir, 'ReasonReact.cmi'),
    cmj: path.join(reasonReactDir, 'ReasonReact.cmj'),
  },
  {
    cmi: path.join(reasonReactDir, 'ReasonReactOptimizedCreateClass.cmi'),
    cmj: path.join(reasonReactDir, 'ReasonReactOptimizedCreateClass.cmj'),
  },
];

const jsName = './externalModules.js';
fs.truncateSync(jsName);
modules.forEach(module => {
  const cmi = module.cmi
  const cmiBasename = path.basename(cmi);
  fs.writeFileSync(jsName,
                   `ocaml.load_module("/static/cmis/${cmiBasename}", `,
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

