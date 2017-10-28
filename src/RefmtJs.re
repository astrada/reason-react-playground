type refmtResult = {
  ocamlCode: option(string),
  reasonCode: option(string),
  errorMessage: option(string)
};

type jsModule;

[@bs.val] [@bs.scope "window"] external requireBrowser : string => jsModule = "require";

type astAndComments;

[@bs.send] external parseRE : (jsModule, string) => astAndComments = "";

[@bs.send] external parseML : (jsModule, string) => astAndComments = "";

[@bs.send] external printRE : (jsModule, astAndComments) => string = "";

[@bs.send] external printML : (jsModule, astAndComments) => string = "";

let refmt = requireBrowser("refmt");

let refmtRE2ML = (code) =>
  try {
    let astAndComments = parseRE(refmt, code);
    let ocamlCode = printML(refmt, astAndComments);
    {ocamlCode: Some(ocamlCode), reasonCode: None, errorMessage: None}
  } {
  | Js.Exn.Error(e) => {ocamlCode: None, reasonCode: None, errorMessage: Js.Exn.message(e)}
  };

let refmtML2RE = (code) =>
  try {
    let astAndComments = parseML(refmt, code);
    let reasonCode = printRE(refmt, astAndComments);
    {ocamlCode: None, reasonCode: Some(reasonCode), errorMessage: None}
  } {
  | Js.Exn.Error(e) => {ocamlCode: None, reasonCode: None, errorMessage: Js.Exn.message(e)}
  };
