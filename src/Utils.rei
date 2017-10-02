let optionMap: ('a => 'b) => option 'a => option 'b;

let debounce: 'a => wait::float? => 'a;

type compilerResult =
  | OutputCode string
  | ErrorMessage string;

let compileReason: string => compilerResult;

let jsxv3Rewrite: string => compilerResult;

let compileOCaml: string => compilerResult;

type evalResult =
  | Success
  | ErrorMessage string;

let evalJs: string => evalResult;
