type compiler;

type compilerResult = {
  .
  "js_code": Js.nullable(string),
  "js_error_msg": Js.nullable(string),
  "row": Js.nullable(int),
  "column": Js.nullable(int),
  "endRow": Js.nullable(int),
  "endColumn": Js.nullable(int),
  "text": Js.nullable(string),
  "_type": Js.nullable(string)
};

[@bs.val] external ocaml : compiler = "";

[@bs.send] external compile : (compiler, string) => string = "";

[@bs.val] [@bs.scope "JSON"] external parse : string => compilerResult = "";

let compile = (code) => parse(compile(ocaml, code));
