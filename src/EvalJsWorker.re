type evalResult = Js.t {. errorMessage : Js.Nullable.t string};

external toEvalResult : 'a => evalResult = "%identity";

external eval : string => unit = "" [@@bs.val];

let evalJsSync code =>
  try {
    eval code;
    {"errorMessage": Js.Nullable.null}
  } {
  | Js.Exn.Error e => {"errorMessage": Js.Nullable.from_opt (Js.Exn.message e)}
  };
/*
 Worker.importScripts Worker.Script.self "stdlibBundle.js";

 [%%bs.raw
   {|
     self.onmessage = (function(evt) {
         var result;
         var code = evt.data;
         try {
             var reactElement = eval(code);
             result = {
                 reactElement: reactElement,
                 errorMessage: null
             };
         }
         catch (ex) {
             result = {
                 reactElement: null,
                 errorMessage: ex.toString()
             };
         };
         self.postMessage(result);
     });
   |}
 ];
 */
