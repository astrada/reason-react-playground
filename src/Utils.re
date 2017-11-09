let optionMap = (fn, option) =>
  switch option {
  | Some(value) => Some(fn(value))
  | None => None
  };

[@bs.module] external lodashDebounce : ('a, Js.nullable(float)) => 'a = "lodash.debounce";

let debounce = (func, ~wait=0.0) => lodashDebounce(func, Js.Nullable.return(wait));

type compilerResult =
  | OutputCode(string, option(string))
  | ErrorMessage(string);

let refmtRE2ML = (code) => {
  let result = RefmtJs.refmtRE2ML(code);
  switch result.errorMessage {
  | Some(errorMessage) => ErrorMessage(errorMessage)
  | None => OutputCode(Js.Option.getWithDefault("", result.ocamlCode), None)
  }
};

let refmtML2RE = (code) => {
  let result = RefmtJs.refmtML2RE(code);
  switch result.errorMessage {
  | Some(errorMessage) => ErrorMessage(errorMessage)
  | None => OutputCode(Js.Option.getWithDefault("", result.reasonCode), None)
  }
};

let jsxv2Rewrite = (code) => {
  let result = JsxV2.rewrite(code);
  let outputCode = Js.Nullable.to_opt(result##ocaml_code);
  let errorMessage = Js.Nullable.to_opt(result##ppx_error_msg);
  switch outputCode {
  | Some("")
  | None => ErrorMessage(Js.Option.getWithDefault("", errorMessage))
  | Some(c) => OutputCode(c, None)
  }
};

let compileOCaml = (code) => {
  let (result, warnings) = BucklescriptCompiler.compile(code);
  let outputCode = Js.Nullable.to_opt(result##js_code);
  let errorMessage = Js.Nullable.to_opt(result##js_error_msg);
  switch outputCode {
  | Some("")
  | None => ErrorMessage(Js.Option.getWithDefault("", errorMessage))
  | Some(c) => OutputCode(c, if (warnings == "") None else Some(warnings))
  }
};

type evalResult =
  | Success
  | ErrorMessage(string);

let evalJs = (code) => {
  let result = EvalJs.evalJs(code);
  let errorMessage = Js.Nullable.to_opt(result##errorMessage);
  switch errorMessage {
  | None => Success
  | Some(em) => ErrorMessage(em)
  }
};

[@bs.val] [@bs.scope "window"] external location : Dom.location = "";

[@bs.set] external setLocationHref : (Dom.location, string) => unit = "href";

[@bs.val] [@bs.scope "window"] external openWindow : (string, string) => unit = "open";

let redirect = (url) => setLocationHref(location, url);

let openWindow = (~target="_blank", url) => openWindow(url, target);
