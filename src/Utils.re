let optionMap fn option =>
  switch option {
  | Some value => Some (fn value)
  | None => None
  };

external lodashDebounce : 'a => Js.Nullable.t float => 'a = "lodash.debounce" [@@bs.module];

let debounce func ::wait=0.0 => lodashDebounce func (Js.Nullable.return wait);

type compilerResult =
  | OutputCode string
  | ErrorMessage string;

let compileOCaml code onDone => {
  let worker = Worker.make "BucklescriptWorker.js";
  Worker.onMessage
    worker
    (
      fun messageEvent => {
        let result = BucklescriptWorker.toCompilerResult messageEvent##data;
        let jsCode = Js.Nullable.to_opt result##js_code;
        let errorMessage =
          Js.Option.firstSome
            (Js.Nullable.to_opt result##js_error_msg) (Js.Nullable.to_opt result##text)
          |> Js.Option.getWithDefault "";
        let compilerResult =
          if (Js.Option.isSome jsCode) {
            OutputCode (Js.Option.getExn jsCode)
          } else {
            ErrorMessage errorMessage
          };
        onDone compilerResult
      }
    );
  Worker.postMessage worker code
};

let compileReason code onDone => {
  let worker = Worker.make "RefmtWorker.js";
  Worker.onMessage
    worker
    (
      fun messageEvent => {
        let result = RefmtWorker.toCompilerResult messageEvent##data;
        let ocamlCode = result##ocamlCode;
        let errorMessage = result##errorMessage;
        let compilerResult =
          if (errorMessage == "") {
            OutputCode ocamlCode
          } else {
            ErrorMessage errorMessage
          };
        onDone compilerResult
      }
    );
  Worker.postMessage worker code
};

type evalResult =
  | ReactElement ReasonReact.reactElement
  | ErrorMessage string;

let evalJs code onDone => {
  let worker = Worker.make "EvalJsWorker.js";
  Worker.onMessage
    worker
    (
      fun messageEvent => {
        let result = EvalJsWorker.toEvalResult messageEvent##data;
        let reactElement = Js.Nullable.to_opt result##reactElement;
        let errorMessage = Js.Nullable.to_opt result##errorMessage;
        let evalResult =
          switch reactElement {
          | Some re => ReactElement re
          | None => ErrorMessage (Js.Option.getWithDefault "" errorMessage)
          };
        onDone evalResult
      }
    );
  Worker.postMessage worker code
};
