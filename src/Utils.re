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

let compileReason code => {
  let result = Refmt.refmt code;
  let outputCode = result##ocamlCode;
  let errorMessage = result##errorMessage;
  if (errorMessage == "") {
    OutputCode outputCode
  } else {
    ErrorMessage errorMessage
  }
};

let jsxv3Rewrite code => {
  let result = JsxV3.rewrite code;
  let outputCode = Js.Nullable.to_opt result##ocaml_code;
  let errorMessage = Js.Nullable.to_opt result##ppx_error_msg;
  switch outputCode {
  | Some ""
  | None => ErrorMessage (Js.Option.getWithDefault "" errorMessage)
  | Some c => OutputCode c
  }
};

let compileOCaml code => {
  let result = BucklescriptCompiler.compile code;
  let outputCode = Js.Nullable.to_opt result##js_code;
  let errorMessage = Js.Nullable.to_opt result##js_error_msg;
  switch outputCode {
  | Some ""
  | None => ErrorMessage (Js.Option.getWithDefault "" errorMessage)
  | Some c => OutputCode c
  }
};

type evalResult =
  | Success
  | ErrorMessage string;

let evalJs code => {
  let result = EvalJs.evalJs code;
  let errorMessage = Js.Nullable.to_opt result##errorMessage;
  switch errorMessage {
  | None => Success
  | Some em => ErrorMessage em
  }
};
