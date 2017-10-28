type jsxv2;

type compilerResult = {
  .
  "ocaml_code": Js.nullable(string),
  "ppx_error_msg": Js.nullable(string),
  "row": Js.nullable(int),
  "column": Js.nullable(int),
  "endRow": Js.nullable(int),
  "endColumn": Js.nullable(int),
  "text": Js.nullable(string),
  "_type": Js.nullable(string)
};

[@bs.val] external jsxv2 : jsxv2 = "";

[@bs.send] external rewrite : (jsxv2, string) => string = "";

[@bs.val] [@bs.scope "JSON"] external parse : string => compilerResult = "";

let rewrite = (code) => parse(rewrite(jsxv2, code));
