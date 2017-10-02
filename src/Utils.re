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

let jsonToCompilerResult codeFieldName errorFieldName json => {
  let result = Js.Json.classify json;
  switch result {
  | Js.Json.JSONObject resultDict =>
    switch (Js.Dict.get resultDict codeFieldName) {
    | Some json =>
      let outputCode =
        switch (Js.Json.classify json) {
        | Js.Json.JSONString c => c
        | _ => ""
        };
      OutputCode outputCode
    | None =>
      switch (Js.Dict.get resultDict errorFieldName) {
      | Some json =>
        let errorMessage =
          switch (Js.Json.classify json) {
          | Js.Json.JSONString em => em
          | _ => ""
          };
        ErrorMessage errorMessage
      | None => assert false
      }
    }
  | _ => ErrorMessage "Unknown error"
  }
};

let jsxv3Rewrite code onDone => {
  let resultJson = JsxV3.rewrite code;
  let compilerResult = jsonToCompilerResult "ocaml_code" "ppx_error_msg" resultJson;
  onDone compilerResult
};

let compileOCaml code onDone => {
  let resultJson = BucklescriptCompiler.compile code;
  let compilerResult = jsonToCompilerResult "js_code" "js_error_msg" resultJson;
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
