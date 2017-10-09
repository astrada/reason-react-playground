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

let refmtRE2ML code => {
  let result = Refmt.refmtRE2ML code;
  switch result.errorMessage {
  | Some errorMessage => ErrorMessage errorMessage
  | None => OutputCode (Js.Option.getWithDefault "" result.ocamlCode)
  }
};

let refmtML2RE code => {
  let result = Refmt.refmtML2RE code;
  switch result.errorMessage {
  | Some errorMessage => ErrorMessage errorMessage
  | None => OutputCode (Js.Option.getWithDefault "" result.reasonCode)
  }
};

let jsxv2Rewrite code => {
  let result = JsxV2.rewrite code;
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

external location : Dom.location = "" [@@bs.val] [@@bs.scope "window"];

external setLocationHref : Dom.location => string => unit = "href" [@@bs.set];

external openWindow : string => string => unit = "open" [@@bs.val] [@@bs.scope "window"];

let redirect url => setLocationHref location url;

let openWindow ::target="_blank" url => openWindow url target;
