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

let compileReason code onDone => {
  let result = Refmt.refmt code;
  let outputCode = result##ocamlCode;
  let errorMessage = result##errorMessage;
  let compilerResult =
    if (errorMessage == "") {
      OutputCode outputCode
    } else {
      ErrorMessage errorMessage
    };
  onDone compilerResult
};

let jsxv3Rewrite code onDone => {
  let result = JsxV3.rewrite code;
  let outputCode = Js.Nullable.to_opt result##ocaml_code;
  let errorMessage = Js.Nullable.to_opt result##ppx_error_msg;
  let compilerResult =
    switch outputCode {
    | Some ""
    | None => ErrorMessage (Js.Option.getWithDefault "" errorMessage)
    | Some c => OutputCode c
    };
  onDone compilerResult
};

let compileOCaml code onDone => {
  let result = BucklescriptCompiler.compile code;
  let outputCode = Js.Nullable.to_opt result##js_code;
  let errorMessage = Js.Nullable.to_opt result##js_error_msg;
  let compilerResult =
    switch outputCode {
    | Some ""
    | None => ErrorMessage (Js.Option.getWithDefault "" errorMessage)
    | Some c => OutputCode c
    };
  onDone compilerResult
};

type evalResult =
  | Success
  | ErrorMessage string;

let evalJs code onDone => {
  let result = EvalJs.evalJs code;
  let errorMessage = Js.Nullable.to_opt result##errorMessage;
  let evalResult =
    switch errorMessage {
    | None => Success
    | Some em => ErrorMessage em
    };
  onDone evalResult
};
