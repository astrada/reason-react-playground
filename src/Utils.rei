let optionMap: ('a => 'b) => option 'a => option 'b;

let debounce: 'a => wait::float? => 'a;

type compilerResult =
  | OutputCode string
  | ErrorMessage string;

let compileReason: string => (compilerResult => unit) => unit;

let compileOCaml: string => (compilerResult => unit) => unit;
