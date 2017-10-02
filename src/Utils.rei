let optionMap: ('a => 'b) => option 'a => option 'b;

let debounce: 'a => wait::float? => 'a;

type compilerResult =
  | OutputCode string
  | ErrorMessage string;

let compileReason: string => (compilerResult => unit) => unit;

let jsxv3Rewrite: string => (compilerResult => unit) => unit;

let compileOCaml: string => (compilerResult => unit) => unit;

type evalResult =
  | Success
  | ErrorMessage string;

let evalJs: string => (evalResult => unit) => unit;
