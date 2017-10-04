type jsxv2;

type compilerResult =
  Js.t {
    .
    ocaml_code : Js.nullable string,
    ppx_error_msg : Js.nullable string,
    row : Js.nullable int,
    column : Js.nullable int,
    endRow : Js.nullable int,
    endColumn : Js.nullable int,
    text : Js.nullable string,
    _type : Js.nullable string
  };

external jsxv2 : jsxv2 = "" [@@bs.val];

external rewrite : jsxv2 => string => string = "" [@@bs.send];

external parse : string => compilerResult = "" [@@bs.val] [@@bs.scope "JSON"];

let rewrite code => parse (rewrite jsxv2 code);
