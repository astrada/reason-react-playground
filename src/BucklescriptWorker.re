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

external toCompilerResult : 'a => compilerResult = "%identity";

external ocaml : compiler = "" [@@bs.val];

external compile : compiler => string => string = "" [@@bs.send];

let compileSync code => Js.Json.parseExn (compile ocaml code);
/*
 Worker.importScripts Worker.Script.self "bucklescript.js";

 Worker.importScripts Worker.Script.self "load_module.js";

 Worker.onMessage
   Worker.Script.self
   (
     fun evt => {
       let result = compileCode evt##data;
       Worker.postMessage Worker.Script.self result
     }
   );
 */
