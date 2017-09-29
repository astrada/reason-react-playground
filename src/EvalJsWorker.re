type evalResult =
  Js.t {
    .
    reactElement : Js.Nullable.t ReasonReact.reactElement, errorMessage : Js.Nullable.t string
  };

external toEvalResult : 'a => evalResult = "%identity";

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
