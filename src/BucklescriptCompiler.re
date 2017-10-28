type compiler;

type compilerResult =
  Js.t {
    .
    js_code : Js.nullable string,
    js_error_msg : Js.nullable string,
    row : Js.nullable int,
    column : Js.nullable int,
    endRow : Js.nullable int,
    endColumn : Js.nullable int,
    text : Js.nullable string,
    _type : Js.nullable string
  };

external ocaml : compiler = "" [@@bs.val];

external compile : compiler => string => string = "" [@@bs.send];

external parse : string => compilerResult = "" [@@bs.val] [@@bs.scope "JSON"];

let compile code => parse (compile ocaml code);