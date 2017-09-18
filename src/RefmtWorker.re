type compilerResult = Js.t {. ocamlCode : string, errorMessage : string};

external toCompilerResult : 'a => compilerResult = "%identity";

external refmt : string => string => string => string => array string = "" [@@bs.val];

external get : array string => int => string = "" [@@bs.get_index];

Worker.importScripts Worker.Script.self "refmt.js";

Worker.onMessage
  Worker.Script.self
  (
    fun evt => {
      let code = evt##data;
      let converted = refmt code "RE" "implementation" "ML";
      let result =
        if (get converted 0 == "REtoML") {
          {"ocamlCode": get converted 1, "errorMessage": ""}
        } else if (
          get converted 1 == ""
        ) {
          {"ocamlCode": "", "errorMessage": "Syntax error"}
        } else {
          {"ocamlCode": "", "errorMessage": get converted 1}
        };
      Worker.postMessage Worker.Script.self result
    }
  );
