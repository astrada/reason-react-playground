let optionMap: ('a => 'b, option('a)) => option('b);

let debounce: ('a, ~wait: float=?) => 'a;

type compilerResult =
  | OutputCode(string, option(string))
  | ErrorMessage(string);

let refmtRE2ML: string => compilerResult;

let refmtML2RE: string => compilerResult;

let jsxv2Rewrite: string => compilerResult;

let compileOCaml: string => compilerResult;

type evalResult =
  | Success
  | ErrorMessage(string);

let evalJs: string => evalResult;

let redirect: string => unit;

let openWindow: (~target: string=?, string) => unit;
