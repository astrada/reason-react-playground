let optionMap fn option =>
  switch option {
  | Some value => Some (fn value)
  | None => None
  };

external lodashDebounce : 'a => Js.Nullable.t float => 'a = "lodash.debounce" [@@bs.module];

let debounce func ::wait=0.0 => lodashDebounce func (Js.Nullable.return wait);

type jsxv3;

external jsxv3 : jsxv3 = "" [@@bs.val];

external rewrite : jsxv3 => string => string = "" [@@bs.send];

type compilerResult =
  | OutputCode string
  | ErrorMessage string;

let compileOCamlSync code onDone => {
  let jsxTransformed = rewrite jsxv3 code;
  let resultJson = BucklescriptWorker.compileSync jsxTransformed;
  let result = Js.Json.classify resultJson;
  let compilerResult =
    switch result {
    | Js.Json.JSONObject resultDict =>
      switch (Js.Dict.get resultDict "js_code") {
      | Some json =>
        let jsCode =
          switch (Js.Json.classify json) {
          | Js.Json.JSONString jsCode => jsCode
          | _ => ""
          };
        OutputCode jsCode
      | None =>
        switch (Js.Dict.get resultDict "js_error_msg") {
        | Some json =>
          let errorMessage =
            switch (Js.Json.classify json) {
            | Js.Json.JSONString errorMessage => errorMessage
            | _ => ""
            };
          ErrorMessage errorMessage
        | None => assert false
        }
      }
    | _ => ErrorMessage "Unknown error"
    };
  onDone compilerResult
};

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
  | Success
  | ErrorMessage string;

let evalJsSync code onDone => {
  let result = EvalJsWorker.evalJsSync code;
  let errorMessage = Js.Nullable.to_opt result##errorMessage;
  let evalResult =
    switch errorMessage {
    | None => Success
    | Some em => ErrorMessage em
    };
  onDone evalResult
};

let evalJs code onDone => {
  let worker = Worker.make "EvalJsWorker.js";
  Worker.onMessage
    worker
    (
      fun messageEvent => {
        let result = EvalJsWorker.toEvalResult messageEvent##data;
        let errorMessage = Js.Nullable.to_opt result##errorMessage;
        let evalResult =
          switch errorMessage {
          | None => Success
          | Some em => ErrorMessage em
          };
        onDone evalResult
      }
    );
  Worker.postMessage worker code
};
