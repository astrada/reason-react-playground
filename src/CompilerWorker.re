type compiler;

type compilerResult =
  Js.t {
    .
    js_code : Js.nullable string, js_error_msg : Js.nullable string, message : Js.nullable string
  };

external toCompilerResult : 'a => compilerResult = "%identity";

external ocaml : compiler = "" [@@bs.val];

external compile : compiler => string => string = "" [@@bs.send];

Worker.importScripts Worker.Script.self "bucklescript.js";

Worker.onMessage
  Worker.Script.self
  (
    fun evt => {
      let code = evt##data;
      let result = Js.Json.parseExn (compile ocaml code);
      Worker.postMessage Worker.Script.self result
    }
  );
